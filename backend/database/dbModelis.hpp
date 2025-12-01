#ifndef DB_MODELIS_HPP
#define DB_MODELIS_HPP

#include <string_view>

inline constexpr std::string_view DB_MODELIS = R"sql(
-- Tipai
DO $$
BEGIN
    CREATE TYPE var_role AS ENUM ('administratorius', 'vartotojas');
EXCEPTION
    WHEN duplicate_object THEN NULL;
END
$$;

DO $$
BEGIN
    CREATE TYPE egz_bukle AS ENUM ('nauja', 'gera', 'prasta');
EXCEPTION
    WHEN duplicate_object THEN NULL;
END
$$;

DO $$
BEGIN
    CREATE TYPE egz_statusas AS ENUM ('skolinama', 'laisva', 'rezervuota');
EXCEPTION
    WHEN duplicate_object THEN NULL;
END
$$;

DO $$
BEGIN
    CREATE TYPE rez_statusas AS ENUM ('laukiama', 'pranesta', 'ivykdyta');
EXCEPTION
    WHEN duplicate_object THEN NULL;
END
$$;

-- Lentelės
CREATE TABLE IF NOT EXISTS pareigos (
    pavadinimas VARCHAR(50) PRIMARY KEY NOT NULL,

    skolinimoTerminas INTEGER NOT NULL,
    skolosDaugiklis NUMERIC(10, 2) NOT NULL
);

INSERT INTO pareigos (pavadinimas, skolinimoTerminas, skolosDaugiklis)
VALUES
    ('studentas', 14, 1.0),
    ('destytojas', 90, 0.5),
    ('administracija', 365, 0.0)
ON CONFLICT (pavadinimas) DO NOTHING;

CREATE TABLE IF NOT EXISTS vartotojai (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    username VARCHAR(20) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,

    role var_role NOT NULL DEFAULT 'vartotojas',
    pareigos VARCHAR(50) REFERENCES pareigos(pavadinimas) ON DELETE SET NULL DEFAULT 'studentas',

    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS knyga (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),

    isbn VARCHAR(20) UNIQUE NOT NULL,
    pavadinimas TEXT NOT NULL,
    autoriai jsonb NOT NULL,
    
    zanras TEXT,
    leidykla TEXT,
    leidimo_metai DATE,

    kaina NUMERIC(10, 2),

    paieskosVektorius tsvector
);

CREATE TABLE IF NOT EXISTS egzempliorius (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    knygos_id UUID REFERENCES knyga(id) ON DELETE CASCADE,
    
    statusas egz_statusas NOT NULL DEFAULT 'laisva',

    bukle egz_bukle NOT NULL DEFAULT 'nauja',
    isigyta DATE NOT NULL DEFAULT CURRENT_DATE
);

CREATE TABLE IF NOT EXISTS skola (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),

    vartotojo_id UUID REFERENCES vartotojai(id) ON DELETE CASCADE,

    suma NUMERIC(10, 2) CHECK (suma >= 0),
    sumoketa BOOLEAN NOT NULL DEFAULT FALSE
);

CREATE TABLE IF NOT EXISTS nuoma (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),

    vartotojo_id UUID REFERENCES vartotojai(id) ON DELETE CASCADE,
    egzemplioriaus_id UUID REFERENCES egzempliorius(id) ON DELETE SET NULL,

    nuoma_nuo DATE NOT NULL DEFAULT CURRENT_DATE,
    nuoma_iki DATE NOT NULL, 
    grazinimo_laikas timestamptz NULL,

    skolos_id UUID REFERENCES skola(id) ON DELETE SET NULL
);

CREATE TABLE IF NOT EXISTS rezervacija (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),

    vartotojo_id UUID REFERENCES vartotojai(id) ON DELETE CASCADE,
    knygos_id UUID REFERENCES knyga(id) ON DELETE CASCADE,

    rezervacijos_data TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    statusas rez_statusas NOT NULL DEFAULT 'laukiama'
);

CREATE TABLE IF NOT EXISTS zinutes (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),

    gavejo_id UUID REFERENCES vartotojai(id) ON DELETE CASCADE,

    pranesimas TEXT NOT NULL,
    issiuntimo_data TIMESTAMP NOT NULL DEFAULT CURRENT_TIMESTAMP,
    perskaityta BOOLEAN NOT NULL DEFAULT FALSE
);

-- Indeksai

--- Knygos paieska pagal teksta
CREATE INDEX IF NOT EXISTS idx_knyga_paieska ON knyga USING gin(paieskosVektorius);

--- Knygos paieskai pagal isbn, zanra, metus
CREATE INDEX IF NOT EXISTS idx_knyga_isbn ON knyga(isbn);
CREATE INDEX IF NOT EXISTS idx_knyga_zanras ON knyga(zanras);
CREATE INDEX IF NOT EXISTS idx_knyga_leidimo_metai ON knyga(leidimo_metai);

--- Skolu paieskai
CREATE INDEX IF NOT EXISTS idx_skola_vartotojo ON skola(vartotojo_id);
CREATE INDEX IF NOT EXISTS idx_nuoma_skolos ON nuoma(skolos_id);

-- Funkcijos ir trigeriai

--- Apskaiciuoti grazinimo data pagal vartotojo pareigas
CREATE OR REPLACE FUNCTION apskaiciuoti_grazinimo_data(vartotojo_id UUID, paskolinimo_data DATE)
RETURNS DATE AS $$
DECLARE
    skolinimo_terminas INTEGER;
    grazinimo_data DATE;
BEGIN
    SELECT p.skolinimoTerminas INTO skolinimo_terminas
    FROM vartotojai v
    JOIN pareigos p ON v.pareigos = p.pavadinimas
    WHERE v.id = vartotojo_id;

    IF skolinimo_terminas IS NULL THEN
        RAISE EXCEPTION 'Nepavyko rasti skolinimo termino vartotojui %', vartotojo_id;
    END IF;

    grazinimo_data := paskolinimo_data + skolinimo_terminas;
    RETURN grazinimo_data;
END;
$$ LANGUAGE plpgsql;

CREATE OR REPLACE FUNCTION trg_nustatyti_nuomos_grazinimo_data()
RETURNS trigger AS $$
BEGIN
    -- Jei nuoma_iki nenurodyta – apskaičiuojame pagal vartotojo pareigas
    IF NEW.nuoma_iki IS NULL THEN
        NEW.nuoma_iki := apskaiciuoti_grazinimo_data(NEW.vartotojo_id, NEW.nuoma_nuo);
    END IF;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS nustatyti_nuomos_grazinimo_data ON nuoma;
CREATE TRIGGER nustatyti_nuomos_grazinimo_data
BEFORE INSERT ON nuoma
FOR EACH ROW
EXECUTE FUNCTION trg_nustatyti_nuomos_grazinimo_data();

--- Atnaujinti knygos paieskos vektoriu (svarbumo eiga pavadinimas -> autoriai)
CREATE OR REPLACE FUNCTION knygos_paieskos_vektoriaus_atnaujinimas()
RETURNS trigger AS $$
BEGIN
    NEW.paieskosVektorius :=
        setweight(to_tsvector('simple', coalesce(NEW.pavadinimas, '')), 'A') ||
        setweight(to_tsvector('simple', coalesce((SELECT string_agg(autorius, ' ') FROM jsonb_array_elements_text(NEW.autoriai) AS autorius), '')), 'B');

    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

DROP TRIGGER IF EXISTS trg_knyga_tsv ON knyga;
CREATE TRIGGER trg_knyga_tsv BEFORE INSERT OR UPDATE ON knyga
FOR EACH ROW EXECUTE FUNCTION knygos_paieskos_vektoriaus_atnaujinimas();
)sql";

#endif // DB_MODELIS_HPP