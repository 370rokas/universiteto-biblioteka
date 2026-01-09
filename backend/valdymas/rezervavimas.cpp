#include "rezervavimas.hpp"

#include "../utils/logger.hpp"
#include "../database/db.hpp"

std::optional<RezervacijaInfo> valdymas::gautiRezervacijaPagalId(const std::string &rezervacijosId) {
    RezervacijaInfo r{};

    try {
        auto rez = dbGlobalus->executeSql(
            R"sql(
                SELECT
                    id,
                    knygos_id,
                    vartotojo_id,
                    rezervacijos_data,
                    statusas
                FROM rezervacija
                WHERE id = $1;
            )sql",
            { rezervacijosId }
        );

        if (rez.size() == 1) {
            r.id = rez[0]["id"].as<std::string>();
            r.knygosId = rez[0]["knygos_id"].as<std::string>();
            r.vartotojoId = rez[0]["vartotojo_id"].as<std::string>();
            r.rezervacijosData = rez[0]["rezervacijos_data"].as<std::string>();
            r.statusas = rez[0]["statusas"].as<std::string>();

            return r;
        }

    } catch (const std::exception &e) {
        logger::get()->error("Klaida gaunant rezervaciją pagal ID: {}", e.what());
    }

    return std::nullopt;
}

std::vector<RezervacijaInfo> valdymas::gautiVartotojoRezervacijas(const std::string &userId) {
    std::vector<RezervacijaInfo> rezervacijos;

    try {
        auto rez = dbGlobalus->executeSql(
            R"sql(
                SELECT
                    id,
                    knygos_id,
                    vartotojo_id,
                    rezervacijos_data,
                    statusas
                FROM rezervacija
                WHERE vartotojo_id = $1;
            )sql",
            { userId }
        );

        for (const auto &row : rez) {
            RezervacijaInfo r{
                row["id"].as<std::string>(),
                row["knygos_id"].as<std::string>(),
                row["vartotojo_id"].as<std::string>(),
                row["rezervacijos_data"].as<std::string>(),
                row["statusas"].as<std::string>()
            };
            rezervacijos.push_back(r);
        }

    } catch (const std::exception &e) {
        logger::get()->error("Klaida gaunant vartotojo rezervacijas: {}", e.what());
    }

    return rezervacijos;
}

std::vector<RezervacijaInfo> valdymas::gautiAktyviasKnygosRezervacijas(const std::string &knygosId) {
    std::vector<RezervacijaInfo> rezervacijos;

    try {
        auto rez = dbGlobalus->executeSql(
            R"sql(
                SELECT
                    id,
                    knygos_id,
                    vartotojo_id,
                    rezervacijos_data,
                    statusas
                FROM rezervacija
                WHERE knygos_id = $1 AND statusas = 'laukiama' 
                ORDER BY rezervacijos_data ASC;
            )sql",
            { knygosId }
        );

        for (const auto &row : rez) {
            RezervacijaInfo r{
                row["id"].as<std::string>(),
                row["knygos_id"].as<std::string>(),
                row["vartotojo_id"].as<std::string>(),
                row["rezervacijos_data"].as<std::string>(),
                row["statusas"].as<std::string>()
            };
            rezervacijos.push_back(r);
        }

    } catch (const std::exception &e) {
        logger::get()->error("Klaida gaunant aktyvias knygos rezervacijas: {}", e.what());
    }

    return rezervacijos;
}

bool valdymas::susietiRezervacijaIrEgzemplioriu(const std::string &rezervacijosId, const std::string &egzId) {
    try {
        auto rezInfoOpt = gautiRezervacijaPagalId(rezervacijosId);
        if (!rezInfoOpt.has_value()) {
            logger::get()->error("Rezervacija su ID '{}' nerasta.", rezervacijosId);
            return false;
        }

        auto rezInfo = rezInfoOpt.value();

        dbGlobalus->executeSql(
            R"sql(
                UPDATE rezervacija
                SET statusas = 'pranesta'
                WHERE id = $1;
            )sql",
            { rezervacijosId }
        );

        dbGlobalus->executeSql(
            R"sql(
                UPDATE egzempliorius
                SET statusas = 'rezervuota'
                WHERE id = $1;
            )sql",
            { egzId }
        );

        dbGlobalus->sukurtiZinute(
            rezInfo.vartotojoId,
            "Jūsų rezervacija knygai (ID: " + rezInfo.knygosId + ") buvo patvirtinta. Egzempliorius (ID: " + egzId + ") yra paruoštas atsiėmimui."
        );

        return true;
    } catch (const std::exception &e) {
        logger::get()->error("Klaida susiejant rezervaciją ir egzempliorių: {}", e.what());
        return false;
    }
}

bool valdymas::atsauktiRezervacija(const std::string &rezervacijosId) {
    try {
        auto rezInfoOpt = gautiRezervacijaPagalId(rezervacijosId);
        if (!rezInfoOpt.has_value()) {
            logger::get()->error("Rezervacija su ID '{}' nerasta.", rezervacijosId);
            return false;
        }

        auto rezInfo = rezInfoOpt.value();

        dbGlobalus->executeSql(
            R"sql(
                DELETE FROM rezervacija
                WHERE id = $1;
            )sql",
            { rezervacijosId }
        );

        dbGlobalus->sukurtiZinute(
            rezInfo.vartotojoId,
            "Jūsų rezervacija knygai (ID: " + rezInfo.knygosId + ") buvo atšaukta."
        );

        return true;
    } catch (const std::exception &e) {
        logger::get()->error("Klaida atšaukiant rezervaciją: {}", e.what());
        return false;
    }
}

bool valdymas::rezervuotiKnyga(const std::string &userId, const std::string &knygosId) {
    try {
        dbGlobalus->executeSql(
            R"sql(
                INSERT INTO rezervacija (knygos_id, vartotojo_id, rezervacijos_data, statusas)
                VALUES ($1, $2, NOW(), 'laukiama');
            )sql",
            { knygosId, userId }
        );

        dbGlobalus->sukurtiZinute(
            userId,
            "Jūs sėkmingai rezervavote knygą (ID: " + knygosId + "). Jums bus pranešta, kai egzempliorius bus paruoštas atsiėmimui."
        );

        valdytiKnygosRezervacijuEile(knygosId);

        return true;
    } catch (const std::exception &e) {
        logger::get()->error("Klaida rezervuojant knygą: {}", e.what());
        return false;
    }
}

void valdymas::valdytiKnygosRezervacijuEile(const std::string &knygosId) {
    try {
        auto egzOpt = dbGlobalus->getEgzemplioriaiByBookId(knygosId);
        if (!egzOpt.has_value()) {
            logger::get()->info("Knygai (ID: {}) nėra egzempliorių.", knygosId);
            return;
        }

        auto egz = egzOpt.value();
        std::vector<std::string> laisvuEgzIds;
        for (const auto &e : egz) {
            if (e.statusas == "laisva") {
                laisvuEgzIds.push_back(e.id);
            }
        }

        if (laisvuEgzIds.empty()) {
            logger::get()->info("Knygai (ID: {}) nėra laisvų egzempliorių.", knygosId);
            return;
        }

        auto rezervacijos = gautiAktyviasKnygosRezervacijas(knygosId);
        if (rezervacijos.empty()) {
            logger::get()->info("Knygai (ID: {}) nėra aktyvių rezervacijų.", knygosId);
            return;
        }

        size_t kiekSusieti = std::min(laisvuEgzIds.size(), rezervacijos.size());
        for (size_t i = 0; i < kiekSusieti; ++i) {
            susietiRezervacijaIrEgzemplioriu(rezervacijos[i].id, laisvuEgzIds[i]);
            logger::get()->info(
                "Susieta rezervacija (ID: {}) su egzemplioriumi (ID: {})",
                rezervacijos[i].id,
                laisvuEgzIds[i]
            );
        }
        
    } catch (const std::exception &e) {
        logger::get()->error("Klaida valdant knygos rezervacijų eilę: {}", e.what());
    }
}