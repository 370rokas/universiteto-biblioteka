#ifndef UB_BACKEND_VALDYMAS_REZERVAVIMAS_HPP
#define UB_BACKEND_VALDYMAS_REZERVAVIMAS_HPP

#include <optional>
#include <string>
#include <vector>

struct RezervacijaInfo {
    std::string id;
    std::string knygosId;
    std::string vartotojoId;
    std::string rezervacijosData;
    std::string statusas;
};

namespace valdymas {
bool rezervuotiKnyga(const std::string &userId, const std::string &knygosId);
bool atsauktiRezervacija(const std::string &rezervacijosId);
bool susietiRezervacijaIrEgzemplioriu(const std::string &rezervacijosId, const std::string &egzId);

std::optional<RezervacijaInfo> gautiRezervacijaPagalId(const std::string &rezervacijosId);
std::vector<RezervacijaInfo> gautiVartotojoRezervacijas(const std::string &userId);
std::vector<RezervacijaInfo> gautiAktyviasKnygosRezervacijas(const std::string &knygosId);

void valdytiKnygosRezervacijuEile(const std::string &knygosId);
} // namespace valdymas

#endif // UB_BACKEND_VALDYMAS_REZERVAVIMAS_HPP