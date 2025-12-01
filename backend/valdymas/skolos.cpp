#include "skolos.hpp"

#include "../utils/config.hpp"
#include "database/db.hpp"
#include "utils/dates.hpp"

void valdymas::atnaujintiSkolas() {
	const double bazinisKainosDaugiklis = config::get()["skolosProcentasNuoKainosKasdien"].get<double>();

	try {
		// SQL funkcija, kuri sukuria skolu irasus toms funkcijoms
		dbGlobalus->executeStatement("sukurtiSkolas", pqxx::params{});

		// Gauti visas skolas, kurios dar nėra sumokėtos
		auto skolos = dbGlobalus->gautiSkolasAtnaujinimui();

		for (const auto &skola : skolos) {
			auto vienosDienosKaina = skola.kaina * bazinisKainosDaugiklis * skola.skolos_daugiklis;

			auto nuomaIkiD = utils::parseDate(skola.nuoma_iki);
			auto dienuVeluoja = utils::dienosTarpDatu(nuomaIkiD, std::chrono::system_clock::now());

			auto skolosSuma = vienosDienosKaina * dienuVeluoja;

			// Atnaujinti skolos suma duomenu bazeje
			dbGlobalus->executeStatement("atnaujintiSkolosSuma", pqxx::params{skolosSuma, skola.skolos_id});
		}

		logger::get()->info("Atnaujinta {} skolu.", skolos.size());

	} catch (const std::exception &e) {
		logger::get()->error("Klaida atnaujinant skolas: {}", e.what());
	}
}