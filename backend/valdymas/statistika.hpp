#ifndef UB_BACKEND_VALDYMAS_STATISTIKA_HPP
#define UB_BACKEND_VALDYMAS_STATISTIKA_HPP

#include <cstdint>
#include <string>
#include <vector>

struct IsduotuKnyguStatistika {
    uint32_t DabarIsduota;
    uint32_t VeluojamaGrazinti;
};

struct PopuliarumoKnygosData {
    std::string Pavadinimas;
    std::string Id;
    uint32_t IsdavimoKiekis;
};

struct PopuliarumoStatistika {
    std::vector<PopuliarumoKnygosData> D30;
    std::vector<PopuliarumoKnygosData> D180;
};

struct AktyviausioVartotojoData {
    std::string Username;
    std::string Id;
    uint32_t IsdavimuKiekis;
};

struct AktyviausiuStatistika {
    std::vector<AktyviausioVartotojoData> D30;
    std::vector<AktyviausioVartotojoData> D180;
};

struct Statistika {
    IsduotuKnyguStatistika IsduotuKnygu;
    PopuliarumoStatistika PopuliariausiosKnygos;
    AktyviausiuStatistika Aktyviausiu;
};

namespace valdymas {
IsduotuKnyguStatistika gautiIsduotuKnyguStatistika();
PopuliarumoStatistika gautiPopuliarumoStatistika();
AktyviausiuStatistika gautiAktyviausiuStatistika();
Statistika gautiBendraStatistika();
} // namespace valdymas

#endif // UB_BACKEND_VALDYMAS_STATISTIKA_HPP