#include "isdavimas.hpp"
#include "../database/db.hpp"
#include "utils/logger.hpp"
#include "valdymas/skolos.hpp"
#include <stdexcept>

IsdavimoStatusas valdymas::isduotiEgzemplioriu(const std::string &egzId, const std::string &userId) {
	try {
		if (egzId.empty() || userId.empty()) {
			throw std::invalid_argument("egzId arba userId negali būti tušti");
		};

		auto egzOpt = dbGlobalus->getEgzemplioriusById(egzId);
		if (!egzOpt.has_value()) {
			logger::get()->warn("Egzempliorius nerastas (e: {}; u: {})", egzId, userId);
			return IsdavimoStatusas::EGZEMPLIORIUS_NERASTAS;
		}

		const auto &egz = egzOpt.value();
		if (egz.statusas != "laisva") {
			logger::get()->warn("Egzempliorius nėra laisvas (e: {}; u: {})", egzId, userId);
			return IsdavimoStatusas::JAU_ISDUOTAS;
		}

		// Atnaujinam egzemplioriaus statusą
		if (!dbGlobalus->updateEgzemplioriusStatusas(egzId, "skolinama")) {
			logger::get()->error("Nepavyko atnaujinti egzemplioriaus statuso (e: {}; u: {})", egzId, userId);
			return IsdavimoStatusas::KLAIDA;
		}

		// Sukurti irasa nuomos lenteleje
		if (!dbGlobalus->sukurtiNuomosIrasa(egzId, userId)) {
			logger::get()->error("Nepavyko sukurti nuomos įrašo (e: {}; u: {})", egzId, userId);
			dbGlobalus->updateEgzemplioriusStatusas(egzId, "laisvas"); // Atstatom statusa
			return IsdavimoStatusas::KLAIDA;
		}

		return IsdavimoStatusas::SEKMINGAI;

	} catch (const std::exception &e) {
		logger::get()->error("Klaida gaunant egzemplioriu (e: {}; u: {}): {}", egzId, userId, e.what());
		return IsdavimoStatusas::KLAIDA;
	}
}

bool valdymas::grazintiEgzemplioriu(const std::string &nuomosId, const std::string &userId) {
	try {
		if (nuomosId.empty() || userId.empty()) {
			throw std::invalid_argument("skolosId arba userId negali būti tušti");
		};

		auto skolaOpt = dbGlobalus->gautiNuomaPagalNuomosId(nuomosId);

		if (!skolaOpt.has_value()) {
			logger::get()->warn("Aktyvi nuoma nerasta grąžinant egzemplioriu (n: {}; u: {})", nuomosId, userId);
			return false;
		}

		const auto &skola = skolaOpt.value();
		if (userId != skola.vartotojo_id) {
			logger::get()->warn("Vartotojo ID nesutampa grąžinant egzemplioriu (n: {}; u: {})", nuomosId, userId);
			return false;
		}

		// Atnaujinti skolas!
		valdymas::atnaujintiSkolas();

		dbGlobalus->executeSql(
			"UPDATE nuoma SET grazinimo_laikas = CURRENT_TIMESTAMP WHERE id = $1::UUID;",
			pqxx::params{nuomosId});

		dbGlobalus->updateEgzemplioriusStatusas(skola.egzemplioriaus_id, "laisva");

		return true;

	} catch (const std::exception &e) {
		logger::get()->error("Klaida grąžinant egzemplioriu (n: {}; u: {}): {}", nuomosId, userId, e.what());
		return false;
	}
}