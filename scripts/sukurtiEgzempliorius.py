# Scriptas sukurti knygu egzemplioriams (paleisti tik 1 karta visai db-ei)
# fmt: off

# DB-es Schema:

# CREATE TABLE IF NOT EXISTS knyga (
#     id UUID PRIMARY KEY DEFAULT gen_random_uuid(),

#     isbn VARCHAR(20) UNIQUE NOT NULL,
#     pavadinimas TEXT NOT NULL,
#     autoriai jsonb NOT NULL,

#     zanras VARCHAR(50),
#     leidykla VARCHAR(100),
#     leidimo_metai DATE,

#     kaina NUMERIC(10, 2),

#     paieskosVektorius tsvector
# );

# CREATE TABLE IF NOT EXISTS egzempliorius (
#     id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
#     knygos_id UUID REFERENCES knyga(id) ON DELETE CASCADE,
    
#     statusas egz_statusas NOT NULL DEFAULT 'laisva',

#     bukle egz_bukle NOT NULL DEFAULT 'nauja',
#     isigyta DATE NOT NULL DEFAULT CURRENT_DATE
# );

import psycopg, os, random

# Konfiguracija
kiek_egz = [0, 3] # Nuo n1 iki n2 egzempliorių kiekvienai knygai
statusai = ['laisva'] # Kiti statusai gali buti: skolinama, rezervuota
bukle = ['nauja', 'gera', 'prasta'] # Visos galimos bukles, paimama bukle atsitiktinai
# isigyta laukelis yra random data nuo 2015-01-01 iki šiandienos

database_url = os.getenv("DATABASE_URL")

if database_url is None:
    raise Exception("Truksta DATABASE_URL.")

db = psycopg.connect(database_url)
cursor = db.cursor()

_ = cursor.execute("SELECT id FROM knyga;")
visiIds = cursor.fetchall()

for (knygos_id,) in visiIds:
    egzKiekis = random.randint(kiek_egz[0], kiek_egz[1])

    for _ in range(egzKiekis):
        egzStatusas = random.choice(statusai)
        egzBukle = random.choice(bukle)

        metai = random.randint(2015, 2024)
        menuo = random.randint(1, 12)
        diena = random.randint(1, 28) # kad butu saugu visiems menesiams

        isigytaData = f"{metai}-{menuo:02d}-{diena:02d}"

        _ = cursor.execute(
            """
            INSERT INTO egzempliorius (knygos_id, statusas, bukle, isigyta)
            VALUES (%s, %s, %s, %s);
            """,
            (knygos_id, egzStatusas, egzBukle, isigytaData)
        )

    print(f"Sukurti {egzKiekis} egzemplioriai knygai: {knygos_id}")

_ = db.commit()
_ = cursor.close()
_ = db.close()
