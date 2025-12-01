#include "isdavimas.hpp"
#include "../database/db.hpp"
#include "utils/logger.hpp"
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