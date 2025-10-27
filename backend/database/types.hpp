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

#endif // UB_BACKEND_DATABASE_TYPES_HPP