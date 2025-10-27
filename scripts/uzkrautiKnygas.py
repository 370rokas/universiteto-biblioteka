# Scriptas sukrauti knygoms i sistema is goodreads dump'o
# fmt: off

import requests, os, zipfile, shutil, psycopg
import xml.etree.ElementTree as ElementTree
from psycopg.types.json import Jsonb
from dotenv import load_dotenv

_ = load_dotenv()

knygu_url = "https://github.com/zygmuntz/goodbooks-10k/raw/refs/heads/master/books_xml/books_xml.zip"
knygu_folderis = "books_xml"
database_url = os.getenv("DATABASE_URL")

if database_url is None:
    raise Exception("Truksta DATABASE_URL.")

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

if not os.path.exists("books_xml.zip"):
    print("Siunčiami knygų duomenys...")
    response = requests.get(knygu_url)
    with open("books_xml.zip", "wb") as f:
        _ = f.write(response.content)
    print("Knygų duomenys atsisiųsti.")

if os.path.exists(knygu_folderis):
    shutil.rmtree(knygu_folderis)

print("Išpakuojami knygų duomenys...")
with zipfile.ZipFile("books_xml.zip", "r") as zip_ref:
    zip_ref.extractall()
print("Knygų duomenys išpakuoti į 'books_xml' folderi.")

db = psycopg.connect(database_url)
cursor = db.cursor()

for file in os.listdir(knygu_folderis):
    if not file.endswith(".xml"):
        continue

    file_path = os.path.join(knygu_folderis, file)
    tree = ElementTree.parse(file_path)
    root = tree.getroot()

    bookData = root.find("book")
    if bookData is None:
        continue

    title = bookData.find("title").text  # pyright: ignore[reportOptionalMemberAccess]
    publisher = bookData.find("publisher").text  # pyright: ignore[reportOptionalMemberAccess]
    pages = bookData.find("num_pages").text  # pyright: ignore[reportOptionalMemberAccess]

    isbn = bookData.find("isbn13").text  # pyright: ignore[reportOptionalMemberAccess]
    if not isbn:
        isbn = bookData.find("isbn").text  # pyright: ignore[reportOptionalMemberAccess]
    if not isbn:
        continue


    authors: list[str] = []
    authorsData = bookData.find("authors")
    if authorsData is not None:
        for author in authorsData.findall("author"):
            author_name = author.find("name")
            if author_name is not None and author_name.text is not None:
                authors.append(author_name.text)

    genre = ""
    genreBlacklist = ["to-read", "owned", "favorites", "currently-reading", "books-i-own", "ya", "re-read", "default", "my-books", "all-time-favourites", "favourite-books", "my-library"]
    genresData = bookData.find("popular_shelves")
    if genresData is not None:
        for genreData in genresData.findall("shelf"):
            genre_name = genreData.get("name")
            if genre_name is not None and genre_name not in genreBlacklist:
                genre = genre_name
                break
    
    if genre:
        genre = genre.replace("-", " ").lower()

    releaseDate = ""
    year = bookData.find("publication_year").text  # pyright: ignore[reportOptionalMemberAccess]
    month = bookData.find("publication_month").text  # pyright: ignore[reportOptionalMemberAccess]
    day = bookData.find("publication_day").text  # pyright: ignore[reportOptionalMemberAccess]
    if year and month and day:
        releaseDate = f"{year}-{int(month):02d}-{int(day):02d}"
    elif year and month:
        releaseDate = f"{year}-{int(month):02d}-01"
    elif year:
        releaseDate = f"{year}-01-01"
    
    price = int(pages) * 0.2 if pages and pages.isdigit() else 10.0

    print(f"Kraunama knyga: {title} (ISBN: {isbn})")
    print(f"  Autoriai: {', '.join(authors)}")
    print(f"  Žanras: {genre}")
    print(f"  Leidykla: {publisher}")
    print(f"  Leidimo data: {releaseDate}")
    print(f"  Kaina: {price:.2f} EUR")

    _ = cursor.execute(
        """
        INSERT INTO knyga (isbn, pavadinimas, autoriai, zanras, leidykla, leidimo_metai, kaina)
        VALUES (%s, %s, %s, %s, %s, %s, %s)
        ON CONFLICT (isbn) DO NOTHING;
        """,
        (isbn, title, Jsonb(authors), genre, publisher, releaseDate if releaseDate else None, price)
    )

db.commit()
cursor.close()
db.close()
