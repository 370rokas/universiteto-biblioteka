#include "statistika.hpp"
#include "../utils/logger.hpp"
#include "../database/db.hpp"

IsduotuKnyguStatistika valdymas::gautiIsduotuKnyguStatistika() {
    IsduotuKnyguStatistika s{};

    try {
        auto rez = dbGlobalus->executeSql(
            R"sql(
                SELECT
                    COUNT(*) FILTER (
                        WHERE grazinimo_laikas IS NULL
                    ) AS isduota,

                    COUNT(*) FILTER (
                        WHERE grazinimo_laikas IS NULL
                        AND nuoma_iki < CURRENT_DATE
                    ) AS veluojama
                FROM nuoma;            
            )sql",
            {}
        );

        if (rez.size() == 1) {
            s.DabarIsduota = rez[0]["isduota"].as<uint32_t>();
            s.VeluojamaGrazinti = rez[0]["veluojama"].as<uint32_t>();
        }

    } catch (const std::exception &e) {
        logger::get()->error("Klaida gaunant išduotų knygų statistiką: {}", e.what());
    }

    return s;
}

PopuliarumoStatistika valdymas::gautiPopuliarumoStatistika() {
    PopuliarumoStatistika ps{};

    try {
        auto rez30 = dbGlobalus->executeSql(
            R"sql(
                SELECT
                    k.pavadinimas,
                    k.id,
                    COUNT(*) AS isdavimo_kiekis
                FROM nuoma n
                JOIN egzempliorius e ON n.egzemplioriaus_id = e.id
                JOIN knyga k ON e.knygos_id = k.id
                WHERE n.nuoma_nuo >= CURRENT_DATE - INTERVAL '30 days'
                GROUP BY k.id
                ORDER BY isdavimo_kiekis DESC
                LIMIT 10;
            )sql",
            {}
        );

        for (const auto &row : rez30) {
            PopuliarumoKnygosData data{
                row["pavadinimas"].as<std::string>(),
                row["id"].as<std::string>(),
                row["isdavimo_kiekis"].as<uint32_t>()
            };
            ps.D30.push_back(data);
        }

        auto rez180 = dbGlobalus->executeSql(
            R"sql(
                SELECT
                    k.pavadinimas,
                    k.id,
                    COUNT(*) AS isdavimo_kiekis
                FROM nuoma n
                JOIN egzempliorius e ON n.egzemplioriaus_id = e.id
                JOIN knyga k ON e.knygos_id = k.id
                WHERE n.nuoma_nuo >= CURRENT_DATE - INTERVAL '180 days'
                GROUP BY k.id
                ORDER BY isdavimo_kiekis DESC
                LIMIT 10;
            )sql",
            {}
        );

        for (const auto &row : rez180) {
            PopuliarumoKnygosData data{
                row["pavadinimas"].as<std::string>(),
                row["id"].as<std::string>(),
                row["isdavimo_kiekis"].as<uint32_t>()
            };
            ps.D180.push_back(data);
        }

    } catch (const std::exception &e) {
        logger::get()->error("Klaida gaunant populiarumo statistiką: {}", e.what());
    }

    return ps;
}

AktyviausiuStatistika valdymas::gautiAktyviausiuStatistika() {
    AktyviausiuStatistika as{};

    try {
        auto rez30 = dbGlobalus->executeSql(
            R"sql(
                SELECT
                    v.username,
                    v.id,
                    COUNT(*) AS isdavimu_kiekis
                FROM nuoma n
                JOIN vartotojai v ON n.vartotojo_id = v.id
                WHERE n.nuoma_nuo >= CURRENT_DATE - INTERVAL '30 days'
                GROUP BY v.id
                ORDER BY isdavimu_kiekis DESC
                LIMIT 10;
            )sql",
            {}
        );

        for (const auto &row : rez30) {
            AktyviausioVartotojoData data{
                row["username"].as<std::string>(),
                row["id"].as<std::string>(),
                row["isdavimu_kiekis"].as<uint32_t>()
            };
            as.D30.push_back(data);
        }

        auto rez180 = dbGlobalus->executeSql(
            R"sql(
                SELECT
                    v.username,
                    v.id,
                    COUNT(*) AS isdavimu_kiekis
                FROM nuoma n
                JOIN vartotojai v ON n.vartotojo_id = v.id
                WHERE n.nuoma_nuo >= CURRENT_DATE - INTERVAL '180 days'
                GROUP BY v.id
                ORDER BY isdavimu_kiekis DESC
                LIMIT 10;
            )sql",
            {}
        );

        for (const auto &row : rez180) {
            AktyviausioVartotojoData data{
                row["username"].as<std::string>(),
                row["id"].as<std::string>(),
                row["isdavimu_kiekis"].as<uint32_t>()
            };
            as.D180.push_back(data);
        }

    } catch (const std::exception &e) {
        logger::get()->error("Klaida gaunant aktyviausių vartotojų statistiką: {}", e.what());
    }

    return as;
}

Statistika valdymas::gautiBendraStatistika() {
    Statistika stat{};

    stat.IsduotuKnygu = gautiIsduotuKnyguStatistika();
    stat.PopuliariausiosKnygos = gautiPopuliarumoStatistika();
    stat.Aktyviausiu = gautiAktyviausiuStatistika();

    return stat;
}