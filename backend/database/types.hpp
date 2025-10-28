#ifndef UB_BACKEND_DATABASE_TYPES_HPP
#define UB_BACKEND_DATABASE_TYPES_HPP

#include <string>

struct Knyga {
	std::string id;

	std::string isbn;
	std::string pavadinimas;
	std::string autoriai;

	std::string zanras;
	std::string leidykla;
	std::string leidimo_metai;

	double kaina;
};

struct Vartotojas {
	std::string id;
	std::string username;
	std::string password_hash;

	std::string role;
	std::string pareigos;
};

#endif // UB_BACKEND_DATABASE_TYPES_HPP