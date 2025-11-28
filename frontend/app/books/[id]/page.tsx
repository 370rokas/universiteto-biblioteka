"use client";

import { useEffect, useState } from "react";
import { useParams, useRouter } from "next/navigation";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { api, Book, Egzempliorius } from "@/lib/api";

export default function BookDetail() {
  const router = useRouter();
  const { id } = useParams(); // expects /books/[id]
  const [book, setBook] = useState<Book | null>(null);
  const [authors, setAuthors] = useState<string[] | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [egzemplioriai, setEgzemplioriai] = useState<Egzempliorius[] | null>(null);

  useEffect(() => {
    if (!id) return;

    setLoading(true);
    setError(null);

    api
      .getBookById(id as string)
      .then((data) => {
        if (data.ok && data.book) {
          // Parse authors JSON
          try {
            const parsedAuthors = JSON.parse(data.book.autoriai);
            if (Array.isArray(parsedAuthors)) {
              setAuthors(parsedAuthors);
            } else {
              setAuthors([data.book.autoriai]);
            }
          } catch {
            setAuthors([data.book.autoriai]);
          }
          setBook(data.book);
        } else {
          setError(data.message || "Knyga nerasta");
        }
      })
      .catch((err) => setError(err.message || "Klaida gaunant knygos duomenis"))
      .finally(() => setLoading(false));

    api
      .getBookEgzemplioriai(id as string)
      .then((data) => {
        if (data.ok && data.egzemplioriai) {
          setEgzemplioriai(data.egzemplioriai);
        }
      })
      .catch(() => {
        setEgzemplioriai([]);
      });
  }, [id]);

  if (loading) {
    return (
      <div className="flex items-center justify-center min-h-screen text-[var(--muted-foreground)]">
        Kraunama...
      </div>
    );
  }

  if (error || !book) {
    return (
      <div className="flex items-center justify-center min-h-screen text-[var(--destructive)]">
        {error || "Knyga nerasta"}
      </div>
    );
  }

  return (
    <div className="flex flex-col items-center bg-[var(--background)] min-h-screen p-6">
      <button
        className="mb-4 px-4 py-2 bg-[var(--secondary)] text-[var(--secondary-foreground)] rounded hover:bg-[var(--accent)]"
        onClick={() => router.back()}
      >
        &larr; Atgal
      </button>

      <Card className="w-full max-w-2xl bg-[var(--card)] text-[var(--card-foreground)] shadow-lg">
        <CardHeader>
          <CardTitle className="text-2xl font-bold">
            {book.pavadinimas}
          </CardTitle>
        </CardHeader>

        <CardContent className="flex flex-col gap-2 text-[var(--foreground)]">
          <p>
            <strong>Autoriai:</strong> {authors}
          </p>
          <p>
            <strong>Leidykla:</strong> {book.leidykla}
          </p>
          <p>
            <strong>ISBN:</strong> {book.isbn}
          </p>
          <p>
            <strong>Žanras:</strong> {book.zanras}
          </p>
          <p>
            <strong>Leidimo metai:</strong> {book.leidimo_metai}
          </p>
          <p>
            <strong>Kaina:</strong> {book.kaina} €
          </p>
        </CardContent>
      </Card>

      { /* Egzemplioriu sekcija */}
      <Card className="w-full max-w-2xl bg-[var(--card)] text-[var(--card-foreground)] shadow-lg mt-6">
        <CardHeader>
          <CardTitle className="text-xl font-bold">
            Egzemplioriai
          </CardTitle>
        </CardHeader>

        <CardContent className="flex flex-col gap-2 text-[var(--foreground)]">
          {egzemplioriai && egzemplioriai.length > 0 ? (
            egzemplioriai.map((egzempliorius) => (
              <div key={egzempliorius.id} className="border-b border-[var(--border)] pb-2 mb-2">
                <p>
                  <strong>Būsena:</strong> {egzempliorius.statusas}
                </p>
                <p>
                  <strong>Būkle:</strong> {egzempliorius.bukle}
                </p>
                <p>
                  <strong>Isigijimo data:</strong> {egzempliorius.isigyta}
                </p>
              </div>
            ))
          ) : (
            <p>Nėra egzempliorių šiai knygai.</p>
          )}
        </CardContent>
      </Card>
    </div>
  );
}
