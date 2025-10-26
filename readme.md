# Universiteto Bibliotekos sistema

Reikia sukurti sistemą kuri valdytų universiteto bibliotekos knygų išdavimo ir grąžinimo procesus bet ne paprastą o su daug niuansų.

Viena knyga gali turėti kelis egzempliorius ir kiekvienas egzempliorius turi:
- savo unikalų ID numerį
- būseną kada buvo įsigyta ir kokia jo fizinė būklė.
- studentai ir dėstytojai gali skolintis knygas bet skirtingai - studentai gali laikyti knygą 2 savaites o dėstytojai net 3 mėnesius.
- dar yra administracijos darbuotojai kurie taip pat gali skolintis bet jiems taikomi kitokie terminai.

Sistema turi sekti kas kada ir kokią knygą paėmė, kada turi grąžinti ir ar grąžino laiku.

Jeigu žmogus vėluoja grąžinti knygą tai skaičiuojama bauda:
- už kiekvieną pavėluotą dieną skirtinga suma priklausomai nuo to ar tai studentas dėstytojas ar darbuotojas.
- brangesnėms knygoms baudos dydis didesnis.

Kai kurios knygos yra labai populiarios todėl joms galima daryti rezervacijas:
- jeigu knyga skolinta o kažkas nori ją rezervuoti tai sistema turi tai leisti padaryti.
- kai knyga grąžinama automatiškai pranešti rezervavusiam asmeniui kad dabar jis gali atsiimti.

Turi veikti paieška:
- pagal pavadinimą autorių ISBN numerį leidimo metus žanrą.
- paieška turi būti greita net kai bibliotekoje yra dešimtys tūkstančių knygų įrašų.

Dar reikia statistikos funkcijų
- kiek knygų dabar išduota kiek vėluojama grąžinti.
- kurios knygos populiariausios per pastarąjį mėnesį ar pusmetį
- kokie vartotojai aktyviausi.

Dar svarbu kad būtų įdiegtas autentifikacijos mechanizmas:
- administratorius turi galėti prisijungti su slaptažodžiu ir tik jis gali pridėti naujas knygas ištrinti arba redaguoti informaciją
- paprasti vartotojai gali tik peržiūrėti katalogą ir matyti savo skolinimų istoriją.
- slaptažodžiai negali būti saugomi kaip paprastas tekstas turi būti užhašuoti.

## Stackas:

### Frontend
Palaiko visas kompiuterių OS.
- UI - [FTXUI](https://github.com/ArthurSonzogni/FTXUI)
- HTTP requestams - [CPR](https://github.com/libcpr/cpr)

### Backend
Palaiko Linux sistemas, galima paleisti ant Windows įrenginių su WSL pagalba.
- HTTP serveris - [Crow](https://github.com/CrowCpp/Crow)
- DB klientas - [libpqxx](https://github.com/jtv/libpqxx)
- Duombazė - PostgreSQL
- Konteinerizacija - Docker

### Dokumentacija
- API dokumentavimui - [Bruno](https://www.usebruno.com/)