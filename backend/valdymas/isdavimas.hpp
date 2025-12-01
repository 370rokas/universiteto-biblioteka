#ifndef UB_BACKEND_VALDYMAS_ISDAVIMAS_HPP
#define UB_BACKEND_VALDYMAS_ISDAVIMAS_HPP

#include <string>

enum class IsdavimoStatusas {
	SEKMINGAI,
	EGZEMPLIORIUS_NERASTAS,
	JAU_ISDUOTAS,
	KLAIDA
};

namespace valdymas {
IsdavimoStatusas isduotiEgzemplioriu(const std::string &egzId, const std::string &userId);
void atnaujintiSkolas();
} // namespace valdymas

#endif // UB_BACKEND_VALDYMAS_ISDAVIMAS_HPP