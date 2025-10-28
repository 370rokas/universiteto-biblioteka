#include "database/db.hpp"

#include "utils/logger.hpp"

#include <bcrypt.h>
#include <pqxx/prepared_statement.hxx>
#include <string>

Knyga parseRowToKnyga(const pqxx::row &row) {
	Knyga book;
	book.id = row["id"].as<std::string>();
	book.isbn = row["isbn"].as<std::string>();
	book.pavadinimas = row["pavadinimas"].as<std::string>();
	book.zanras = row["zanras"].as<std::string>();
	book.kaina = row["kaina"].as<double>();
	book.autoriai = row["autoriai"].as<std::string>();

	if (!row["leidykla"].is_null()) {
		book.leidykla = row["leidykla"].as<std::string>();
	} else {
		book.leidykla = "";
	}

	if (!row["leidimo_metai"].is_null()) {
		book.leidimo_metai = row["leidimo_metai"].as<std::string>();
	} else {
		book.leidimo_metai = "";
	}

	return book;
}

Vartotojas parseRowToVartotojas(const pqxx::row &row) {
	Vartotojas user;
	user.id = row["id"].as<std::string>();
	user.username = row["username"].as<std::string>();
	user.password_hash = row["password"].as<std::string>();
	user.role = row["role"].as<std::string>();
	user.pareigos = row["pareigos"].as<std::string>();
	return user;
}

pqxx::result Database::executeStatement(const std::string &statementName,
										const pqxx::params &params) {
	auto conn = _pool.get_connection();
	pqxx::work txn(*conn);
	pqxx::result r = txn.exec(pqxx::prepped{statementName.c_str()}, params);
	txn.commit();

	return r;
}

std::vector<Knyga> Database::searchBooks(const std::string &query) {
	try {
		logger::get()->info("Ieškoma knygų su užklausa: {}", query);
		auto r = executeStatement("searchBook", pqxx::params{query});

		std::vector<Knyga> books;

		for (const auto &row : r) {
			books.push_back(parseRowToKnyga(row));
		}

		return books;

	} catch (const std::exception &e) {
		logger::get()->error("Nepavyko ieskoti knygos (q: '{}'): {}",
							 query, e.what());
		return {};
	}
}

std::optional<Knyga> Database::getBookById(const std::string &id) {
	try {
		auto r = executeStatement("getBookById", pqxx::params{id});

		if (r.size() == 0) {
			return std::nullopt;
		}

		return parseRowToKnyga(r[0]);

	} catch (const std::exception &e) {
		logger::get()->error(
			"Nepavyko gauti knygos pagal ID (id: '{}'): {}", id,
			e.what());
		return std::nullopt;
	}
}

std::pair<bool, std::string> Database::createUser(const std::string &username,
												  const std::string &password) {
	try {
		std::string passwordHash = bcrypt::generateHash(password);

		auto r = executeStatement("createUser",
								  pqxx::params{username, passwordHash});

		return {true, ""};

	} catch (pqxx::unique_violation &e) {
		return {false, "Toks vartotojas jau egzistuoja."};
	} catch (const std::exception &e) {
		logger::get()->error(
			"Nepavyko sukurti vartotojo (username: '{}'): {}", username,
			e.what());

		return {false, "Įvyko serverio klaida."};
	}
}

std::optional<Vartotojas> Database::getUserByUsername(const std::string &username) {
	try {
		auto r = executeStatement("getUserByUsername", pqxx::params{username});
		if (r.size() == 0) {
			return std::nullopt;
		}

		return parseRowToVartotojas(r[0]);
	} catch (const std::exception &e) {
		logger::get()->error(
			"Nepavyko gauti vartotojo pagal username (username: '{}'): {}",
			username, e.what());

		return std::nullopt;
	}
}

std::optional<Vartotojas> Database::getUserById(const std::string &id) {
	try {
		auto r = executeStatement("getUserById", pqxx::params{id});
		if (r.size() == 0) {
			return std::nullopt;
		}

		return parseRowToVartotojas(r[0]);
	} catch (const std::exception &e) {
		logger::get()->error(
			"Nepavyko gauti vartotojo pagal ID (id: '{}'): {}", id,
			e.what());

		return std::nullopt;
	}
}