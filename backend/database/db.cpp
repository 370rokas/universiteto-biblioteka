#include "db.hpp"

#include "utils/logger.hpp"

std::shared_ptr<Database> dbGlobalus;

Database::Database(const std::string &databaseUrl)
	: _pool(databaseUrl.c_str(), DATABASE_POOL_SIZE, std::chrono::seconds(10),
			sqlStatements) {
	try {
		_pool.init();

		logger::get()->info("Prisijungta prie duombazės.");
	} catch (const std::exception &e) {
		logger::get()->error("Klaida prisijungiant prie duombazės: {}",
							 e.what());
		throw;
	}
}

Database::~Database() {
	logger::get()->info("Atsijungta nuo duombazės.");
}