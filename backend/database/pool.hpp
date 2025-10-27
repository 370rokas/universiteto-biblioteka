#ifndef DB_POOL_HPP
#define DB_POOL_HPP

// Originalus kodas paimtas iš:
// https://github.com/Hanzla-Double3/pqxx-connection-pool

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <pqxx/pqxx>
#include <queue>
#include <string>

#include "database/dbModelis.hpp"
#include "utils/logger.hpp"

#define retries 5

template <size_t N> class ConnectionPool {
      public:
	class ConnectionGuard {
	      public:
		ConnectionGuard(std::shared_ptr<pqxx::connection> conn,
				ConnectionPool &pool)
		    : conn_(std::move(conn)), pool_(pool) {}

		~ConnectionGuard() {
			std::lock_guard lock(pool_.mutex_);
			pool_.idle_connections_.push(conn_);
			pool_.condition_.notify_one();
			--pool_.active_count_;
		}

		pqxx::connection &operator*() const { return *conn_; }

		pqxx::connection *operator->() const { return conn_.get(); }

	      private:
		std::shared_ptr<pqxx::connection> conn_;
		ConnectionPool &pool_;
	};

	size_t connections;
	std::chrono::milliseconds timeout;
	std::queue<std::shared_ptr<pqxx::connection>> idle_connections_;
	std::atomic_size_t active_count_{0};
	mutable std::mutex mutex_;
	std::condition_variable condition_;
	std::array<std::pair<const char *, const char *>, N> statements;
	const char *conn_str_;

      public:
	// constr P
	constexpr ConnectionPool(
	    const char *conn_str, size_t connections,
	    std::chrono::milliseconds timeout,
	    const std::array<std::pair<const char *, const char *>, N>
		&statements)
	    : conn_str_(conn_str), connections(connections), timeout(timeout),
	      statements(statements) {};
	// init P
	void init() {
		size_t failed = 0;
		for (size_t conn_idx = 0; conn_idx < connections; ++conn_idx) {

			for (int attempt = 0; attempt < retries; attempt++) {
				try {
					idle_connections_.push(
					    create_connection(conn_idx == 0));
					break;
				} catch (const pqxx::broken_connection &e) {
					logger::get()->error(
					    "Nepavyko prisijungti prie duomenų "
					    "bazės (bandymas {}/{}): {}",
					    attempt + 1, retries, e.what());
					
					std::this_thread::sleep_for(
					    std::chrono::seconds(3));

					if (attempt == retries - 1)
						++failed;
					
					continue;
				}
			}
		}
		if (failed == connections) {
			logger::get()->error(
			    "Nepavyko prisijungti prie duomenų bazės. "
			    "Išeinama iš programos.");
			exit(-1);
		}
	}
	// get_con P
	ConnectionGuard get_connection() {
		std::unique_lock lock(mutex_);

		if (!condition_.wait_for(lock, timeout, [this] {
			    return !idle_connections_.empty();
		    })) {
			throw std::runtime_error("Connection pool timeout");
		}

		auto conn = idle_connections_.front();
		idle_connections_.pop();

		if (conn && conn->is_open()) {
			++active_count_;
			return ConnectionGuard(conn, *this);
		}
		protocol_con_failed();
		throw std::runtime_error("Connection failed");
	}
	// drain P
	void drain() {
		std::lock_guard lock(mutex_);
		while (idle_connections_.size() + active_count_) {
			idle_connections_.pop();
		}
		active_count_ = 0;
	}

      private:
	// reqby init
	std::shared_ptr<pqxx::connection>
	create_connection(bool isFirst = false) {
		auto conn = std::make_shared<pqxx::connection>(conn_str_);
		if (!conn->is_open())
			throw pqxx::broken_connection("Connection failed");

		if (isFirst == true) {
			pqxx::work txn(*conn);
			txn.exec(DB_MODELIS);
			txn.commit();
			logger::get()->info("Užkrautas DB modelis.");
		}

		for (const auto &[name, sql] : statements) {
			conn->prepare(name, sql);
		}
		return conn;
	}

	void protocol_con_failed() { connections--; }

      public:
	bool increase_connection() {

		try {
			auto conn = create_connection(false);
			std::unique_lock lock(mutex_);
			idle_connections_.push(conn);
			++connections;
			return 1;
		} catch (pqxx::broken_connection &e) {
			return 0;
		}
	}

	bool decrease_connection() {
		std::unique_lock<std::mutex> lock(mutex_);
		// Wait for 10 seconds or until the condition is met (e.g.,
		// resource available)
		if (!condition_.wait_for(
			lock, std::chrono::seconds(10),
			[this] { return !idle_connections_.empty(); })) {
			return false; // Timeout occurred
		}
		idle_connections_.pop();
		--connections;
		return true;
	}
	size_t get_current_connections() { return connections; }
};

#endif // DB_POOL_HPP