#include "database/db.hpp"

#include "database/types.hpp"
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

Egzempliorius parseRowToEgzempliorius(const pqxx::row &row) {
	Egzempliorius egz;
	egz.id = row["id"].as<std::string>();
	egz.knygos_id = row["knygos_id"].as<std::string>();
	egz.statusas = row["statusas"].as<std::string>();
	egz.bukle = row["bukle"].as<std::string>();
	egz.isigyta = row["isigyta"].as<std::string>();
	return egz;
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

pqxx::result Database::executeSql(const std::string &sql, const pqxx::params &params) {
	auto conn = _pool.get_connection();
	pqxx::work txn(*conn);
	pqxx::result r = txn.exec(sql, params);
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

std::optional<std::vector<Egzempliorius>> Database::getEgzemplioriaiByBookId(const std::string &id) {
	try {
		auto r = executeStatement("getEgzemplioriaiByBookId", pqxx::params{id});

		std::vector<Egzempliorius> egzemplioriai;

		for (const auto &row : r) {
			egzemplioriai.push_back(parseRowToEgzempliorius(row));
		}

		return egzemplioriai;

	} catch (const std::exception &e) {
		logger::get()->error(
			"Nepavyko gauti egzempliorių pagal knygos ID (id: '{}'): {}",
			id, e.what());
		return std::nullopt;
	}
}

std::optional<Egzempliorius> Database::getEgzemplioriusById(const std::string &id) {
	try {
		auto r = executeStatement("getEgzemplioriusById", pqxx::params{id});

		if (r.size() == 0) {
			return std::nullopt;
		}

		return parseRowToEgzempliorius(r[0]);

	} catch (const std::exception &e) {
		logger::get()->error(
			"Nepavyko gauti egzemplioriaus pagal ID (id: '{}'): {}",
			id, e.what());
		return std::nullopt;
	}
}

bool Database::updateEgzemplioriusStatusas(const std::string &id, const std::string &statusas) {
	try {
		executeStatement("updateEgzemplioriausStatusa", pqxx::params{id, statusas});
		return true;
	} catch (const std::exception &e) {
		logger::get()->error(
			"Nepavyko atnaujinti egzemplioriaus statuso (id: '{}', statusas: '{}'): {}",
			id, statusas, e.what());
		return false;
	}
}

bool Database::sukurtiNuomosIrasa(const std::string &egzId, const std::string &userId) {
	try {
		executeStatement("sukurtiNuomosIrasa", pqxx::params{egzId, userId});
		return true;
	} catch (const std::exception &e) {
		logger::get()->error(
			"Nepavyko sukurti nuomos įrašo (egzId: '{}', userId: '{}'): {}",
			egzId, userId, e.what());
		return false;
	}
}

std::vector<SkolinimoIstorijosIrasas> Database::gautiNuomosIstorija(const std::string &userId) {
	try {
		auto r = executeStatement("gautiNuomosIstorija", pqxx::params{userId});

		std::vector<SkolinimoIstorijosIrasas> istorija;

		for (const auto &row : r) {
			SkolinimoIstorijosIrasas irasas;
			irasas.id = row["id"].as<std::string>();
			irasas.pavadinimas = row["pavadinimas"].as<std::string>();
			irasas.autoriai = row["autoriai"].as<std::string>();
			irasas.nuoma_nuo = row["nuoma_nuo"].as<std::string>();
			irasas.nuoma_iki = row["nuoma_iki"].as<std::string>();
			irasas.grazinimo_laikas = row["grazinimo_laikas"].is_null() ? "" : row["grazinimo_laikas"].as<std::string>();
			irasas.suma = row["suma"].is_null() ? "" : row["suma"].as<std::string>();
			irasas.sumoketa = row["sumoketa"].is_null() ? true : row["sumoketa"].as<bool>();

			istorija.push_back(irasas);
		}

		return istorija;

	} catch (const std::exception &e) {
		logger::get()->error(
			"Nepavyko gauti nuomos istorijos (userId: '{}'): {}",
			userId, e.what());
		return {};
	}
}

std::vector<SkolosDuomenysAtnaujinimui> Database::gautiSkolasAtnaujinimui() {
	try {
		auto r = executeStatement("gautiSkolasAtnaujinimui", pqxx::params{});

		std::vector<SkolosDuomenysAtnaujinimui> skolos;

		for (const auto &row : r) {
			SkolosDuomenysAtnaujinimui skola;
			skola.skolos_id = row["skolos_id"].as<std::string>();
			skola.skolos_daugiklis = row["skolos_daugiklis"].as<double>();
			skola.nuoma_nuo = row["nuoma_nuo"].as<std::string>();
			skola.nuoma_iki = row["nuoma_iki"].as<std::string>();
			skola.kaina = row["kaina"].as<double>();

			skolos.push_back(skola);
		}

		return skolos;

	} catch (const std::exception &e) {
		logger::get()->error(
			"Nepavyko gauti skolų atnaujinimui: {}",
			e.what());
		return {};
	}
}

std::optional<AktyviosNuomosData> Database::gautiAktyviaNuomaPagalEgzId(const std::string &egzId) {
	try {
		auto r = executeStatement("gautiAktyviaSkolaPagalEgzemplioriausId", pqxx::params{egzId});

		if (r.size() == 0) {
			return std::nullopt;
		}

		AktyviosNuomosData skola;
		skola.nuoma_id = r[0]["id"].as<std::string>();
		skola.vartotojo_id = r[0]["vartotojo_id"].as<std::string>();
		skola.egzemplioriaus_id = r[0]["egzemplioriaus_id"].as<std::string>();
		skola.nuoma_nuo = r[0]["nuoma_nuo"].as<std::string>();
		skola.nuoma_iki = r[0]["nuoma_iki"].as<std::string>();
		skola.skolos_suma = r[0]["suma"].is_null() ? 0.0 : r[0]["suma"].as<double>();
		skola.skola_sumoketa = r[0]["sumoketa"].is_null() ? true : r[0]["sumoketa"].as<bool>();

		return skola;

	} catch (const std::exception &e) {
		logger::get()->error(
			"Nepavyko gauti aktyvios skolos pagal egzemplioriaus ID (egzId: '{}'): {}",
			egzId, e.what());
		return std::nullopt;
	}
}

std::optional<AktyviosNuomosData> Database::gautiNuomaPagalNuomosId(const std::string &skolosId) {
	try {
		auto r = executeStatement("gautiAktyviaSkolaPagalSkolosId", pqxx::params{skolosId});

		if (r.size() == 0) {
			return std::nullopt;
		}

		AktyviosNuomosData skola;
		skola.nuoma_id = r[0]["id"].as<std::string>();
		skola.vartotojo_id = r[0]["vartotojo_id"].as<std::string>();
		skola.egzemplioriaus_id = r[0]["egzemplioriaus_id"].as<std::string>();
		skola.nuoma_nuo = r[0]["nuoma_nuo"].as<std::string>();
		skola.nuoma_iki = r[0]["nuoma_iki"].as<std::string>();
		skola.skolos_suma = r[0]["suma"].is_null() ? 0.0 : r[0]["suma"].as<double>();
		skola.skola_sumoketa = r[0]["sumoketa"].is_null() ? true : r[0]["sumoketa"].as<bool>();

		return skola;

	} catch (const std::exception &e) {
		logger::get()->error(
			"Nepavyko gauti aktyvios skolos pagal skolos ID (skolosId: '{}'): {}",
			skolosId, e.what());
		return std::nullopt;
	}
}