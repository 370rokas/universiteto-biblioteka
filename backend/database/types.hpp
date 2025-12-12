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

struct Egzempliorius {
	std::string id;
	std::string knygos_id;
	std::string statusas;
	std::string bukle;
	std::string isigyta;
};

struct SkolinimoIstorijosIrasas {
	std::string id;
	std::string pavadinimas;
	std::string autoriai;
	std::string nuoma_nuo;
	std::string nuoma_iki;
	std::string grazinimo_laikas;
	std::string suma;
	bool sumoketa;
};

struct SkolosDuomenysAtnaujinimui {
	std::string skolos_id;
	double skolos_daugiklis;
	std::string nuoma_nuo;
	std::string nuoma_iki;
	double kaina;
};

struct AktyviosNuomosData {
	std::string nuoma_id;
	std::string egzemplioriaus_id;
	std::string vartotojo_id;
	std::string nuoma_nuo;
	std::string nuoma_iki;
	double skolos_suma;
	bool skola_sumoketa;
};

#endif // UB_BACKEND_DATABASE_TYPES_HPP