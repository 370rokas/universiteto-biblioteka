#include "database/db.hpp"

#include "utils/logger.hpp"
#include <pqxx/prepared_statement.hxx>

std::vector<Knyga> Database::searchBooks(const std::string &query) {
	try {
		auto conn = _pool.get_connection();
		pqxx::work txn(*conn);
		pqxx::result r =
		    txn.exec(pqxx::prepped{"searchBook"}, pqxx::params{query});

		std::vector<Knyga> books;

		for (const auto &row : r) {
			Knyga book;
			book.id = row["id"].as<std::string>();
			book.isbn = row["isbn"].as<std::string>();
			book.pavadinimas = row["pavadinimas"].as<std::string>();
			book.zanras = row["zanras"].as<std::string>();
			book.kaina = row["kaina"].as<double>();
			book.autoriai = row["autoriai"].as<std::string>();

			if (!row["leidykla"].is_null()) {
				book.leidykla =
				    row["leidykla"].as<std::string>();
			} else {
				book.leidykla = "";
			}

			if (!row["leidimo_metai"].is_null()) {
				book.leidimo_metai =
				    row["leidimo_metai"].as<std::string>();
			} else {
				book.leidimo_metai = "";
			}

			books.push_back(book);
		}

		return books;

	} catch (const std::exception &e) {
		logger::get()->error("Nepavyko ieskoti knygos (q: '{}'): {}",
				     query, e.what());
		return {};
	}
}