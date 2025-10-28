#ifndef UB_BACKEND_DATABASE_DB_HPP
#define UB_BACKEND_DATABASE_DB_HPP

#define DATABASE_POOL_SIZE 5
#define N_SQL_STATEMENTS 5

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

	std::vector<Knyga> searchBooks(const std::string &query);
	std::optional<Knyga> getBookById(const std::string &id);

	std::pair<bool, std::string> createUser(const std::string &username,
											const std::string &password);
	std::optional<Vartotojas> getUserByUsername(const std::string &username);
	std::optional<Vartotojas> getUserById(const std::string &id);
};

#endif // UB_BACKEND_DATABASE_DB_HPP