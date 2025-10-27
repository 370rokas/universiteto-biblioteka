#ifndef UB_BACKEND_DATABASE_DB_HPP
#define UB_BACKEND_DATABASE_DB_HPP

#define DATABASE_POOL_SIZE 5
#define N_SQL_STATEMENTS 2

#include "database/pool.hpp"
#include "database/types.hpp"

#include <array>

// clang-format off
constexpr std::array<std::pair<const char *, const char *>, N_SQL_STATEMENTS> sqlStatements = {{
    {"createUser", 
     "INSERT INTO vartotojai (username, password) VALUES ($1::text, $2::text)"},

     {"searchBook",
    "SELECT id, isbn, pavadinimas, autoriai, zanras, leidykla, leidimo_metai, kaina FROM knyga WHERE "
    "paieskosVektorius @@ to_tsquery('simple', regexp_replace($1::text, '\\s+', ' & ', 'g') || ':*') "
    "ORDER BY ts_rank_cd(paieskosVektorius, to_tsquery('simple', regexp_replace($1::text, '\\s+', ' & ', 'g') || ':*')) DESC;"}
}};
// clang-format on

class Database {
      private:
	ConnectionPool<N_SQL_STATEMENTS> _pool;

      public:
	Database(const std::string &databaseUrl);
	~Database();

	std::vector<Knyga> searchBooks(const std::string &query);
};

#endif // UB_BACKEND_DATABASE_DB_HPP