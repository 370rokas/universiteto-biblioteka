#ifndef UB_BACKEND_DATABASE_DB_HPP
#define UB_BACKEND_DATABASE_DB_HPP

#define DATABASE_POOL_SIZE 5
#define N_SQL_STATEMENTS 16

#include "database/pool.hpp"
#include "database/types.hpp"

#include <array>

// clang-format off
constexpr std::array<std::pair<const char *, const char *>, N_SQL_STATEMENTS> sqlStatements = {{
    {"createUser", 
     "INSERT INTO vartotojai (username, password) VALUES ($1::text, $2::text)"},

     {"getUserByUsername", 
     "SELECT id, username, password, role, pareigos FROM vartotojai WHERE username = $1::text;"},

     {"getUserById", 
     "SELECT id, username, password, role, pareigos FROM vartotojai WHERE id = $1::uuid;"},

     {"searchBook",
    "SELECT id, isbn, pavadinimas, autoriai, zanras, leidykla, leidimo_metai, kaina FROM knyga WHERE "
    "paieskosVektorius @@ to_tsquery('simple', regexp_replace($1::text, '\\s+', ' & ', 'g') || ':*') "
    "OR isbn ILIKE '%' || $1::text || '%' "
    "ORDER BY ts_rank_cd(paieskosVektorius, to_tsquery('simple', regexp_replace($1::text, '\\s+', ' & ', 'g') || ':*')) DESC "
    "LIMIT 50;"},

    {"getBookById", 
    "SELECT id, isbn, pavadinimas, autoriai, zanras, leidykla, leidimo_metai, kaina FROM knyga WHERE id = $1::uuid;"},

    {"getEgzemplioriaiByBookId",
    "SELECT id, knygos_id, statusas, bukle, isigyta FROM egzempliorius WHERE knygos_id = $1::uuid;"},

    {"getEgzemplioriusById",
    "SELECT id, knygos_id, statusas, bukle, isigyta FROM egzempliorius WHERE id = $1::uuid;"},

    {"updateEgzemplioriausStatusa",
    "UPDATE egzempliorius SET statusas = CAST($2::text as egz_statusas) WHERE id = $1::uuid;"},

    {"sukurtiNuomosIrasa",
    "INSERT INTO nuoma (egzemplioriaus_id, vartotojo_id, nuoma_nuo) VALUES ($1::uuid, $2::uuid, NOW());"},

    {"gautiNuomosIstorija",
    "SELECT n.id, n.nuoma_nuo, n.nuoma_iki, n.grazinimo_laikas, k.pavadinimas, k.autoriai, s.suma, s.sumoketa "
    "FROM nuoma n "
    "JOIN egzempliorius e ON n.egzemplioriaus_id = e.id "
    "JOIN knyga k ON e.knygos_id = k.id "
    "LEFT JOIN skola s ON n.skolos_id = s.id "
    "WHERE n.vartotojo_id = $1::uuid "
    "ORDER BY n.nuoma_nuo DESC;"},

    {"gautiSkolasAtnaujinimui",
      "SELECT s.id as skolos_id, p.skolosdaugiklis as skolos_daugiklis, n.nuoma_nuo, n.nuoma_iki, k.kaina "
        "FROM skola s "
        "JOIN nuoma n ON n.skolos_id = s.id "
        "JOIN egzempliorius e ON e.id = n.egzemplioriaus_id "
        "JOIN knyga k ON k.id = e.knygos_id "
        "JOIN vartotojai v ON s.vartotojo_id = v.id "
        "JOIN pareigos p ON p.pavadinimas = v.pareigos "
      "WHERE s.sumoketa IS FALSE;"},

    {"sukurtiSkolas",
      "DO $$ "
        "DECLARE "
            "r RECORD; "
            "new_skola_id UUID; "
        "BEGIN "
            "FOR r IN " 
                "SELECT vartotojo_id, id " 
                "FROM nuoma "
                "WHERE skolos_id IS NULL "
                  "AND CURRENT_DATE > nuoma_iki "
            "LOOP "
                "INSERT INTO skola (vartotojo_id, suma) "
                "VALUES (r.vartotojo_id, 0) "
                "RETURNING id INTO new_skola_id; "
                "UPDATE nuoma "
                "SET skolos_id = new_skola_id "
                "WHERE id = r.id; "
            "END LOOP; "
        "END; "
      "$$;"},

      {"atnaujintiSkolosSuma",
      "UPDATE skola SET suma = $1::NUMERIC(10,2) WHERE id = $2::UUID;"},

      {"gautiAktyviaSkolaPagalEgzemplioriausId",
          "SELECT n.id, n.vartotojo_id, n.egzemplioriaus_id, n.nuoma_nuo, n.nuoma_iki, s.suma, s.sumoketa "
          "FROM nuoma n LEFT JOIN skola s ON n.skolos_id = s.id "
          "WHERE n.egzemplioriaus_id = $1::UUID AND n.grazinimo_laikas IS NULL;"},

      {"gautiAktyviaSkolaPagalSkolosId",
          "SELECT n.id, n.vartotojo_id, n.egzemplioriaus_id, n.nuoma_nuo, n.nuoma_iki, s.suma, s.sumoketa "
          "FROM nuoma n LEFT JOIN skola s ON n.skolos_id = s.id "
          "WHERE n.id = $1::UUID AND n.grazinimo_laikas IS NULL;"},
        
          {"gautiVisasSkolasPagalVartotojoId",
          "SELECT s.id, s.suma, s.sumoketa, n.id as nuomos_id, n.grazinimo_laikas "
          "FROM skola s "
          "JOIN nuoma n ON s.id = n.skolos_id "
          "WHERE s.vartotojo_id = $1::UUID;"},
}};
// clang-format on

class Database {
  private:
	ConnectionPool<N_SQL_STATEMENTS> _pool;

  public:
	Database(const std::string &databaseUrl);
	~Database();

	pqxx::result executeStatement(const std::string &statementName,
								  const pqxx::params &params);
	pqxx::result executeSql(const std::string &sql, const pqxx::params &params);

	std::vector<Knyga> searchBooks(const std::string &query);
	std::optional<Knyga> getBookById(const std::string &id);

	std::pair<bool, std::string> createUser(const std::string &username,
											const std::string &password);
	std::optional<Vartotojas> getUserByUsername(const std::string &username);
	std::optional<Vartotojas> getUserById(const std::string &id);

	std::optional<std::vector<Egzempliorius>> getEgzemplioriaiByBookId(const std::string &id);
	std::optional<Egzempliorius> getEgzemplioriusById(const std::string &id);
	bool updateEgzemplioriusStatusas(const std::string &id, const std::string &statusas);
	bool sukurtiNuomosIrasa(const std::string &egzId, const std::string &userId);
	std::vector<SkolinimoIstorijosIrasas> gautiNuomosIstorija(const std::string &userId);

	std::vector<SkolosDuomenysAtnaujinimui> gautiSkolasAtnaujinimui();

	std::optional<AktyviosNuomosData> gautiAktyviaNuomaPagalEgzId(const std::string &egzId);
	std::optional<AktyviosNuomosData> gautiNuomaPagalNuomosId(const std::string &nuomosId);

	std::optional<std::vector<VartotojoSkoluData>> gautiVisasSkolasPagalVartotojoId(const std::string &userId);
};

extern std::shared_ptr<Database> dbGlobalus;

#endif // UB_BACKEND_DATABASE_DB_HPP