"use client";

import { useState, useEffect, useCallback } from "react";
import { useRouter } from "next/navigation";
import { Input } from "@/components/ui/input";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { api, Book } from "@/lib/api";
import debounce from "lodash.debounce";

function parseAuthors(authorsStr: string): string {
  try {
    const authors = JSON.parse(authorsStr) as string[];
    return authors.join(", ");
  } catch {
    return authorsStr;
  }
}

export default function BookSearch() {
  const router = useRouter();
  const [query, setQuery] = useState("");
  const [books, setBooks] = useState<Book[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const fetchBooks = useCallback(
    debounce(async (q: string) => {
      if (!q.trim()) {
        setBooks([]);
        setLoading(false);
        return;
      }

      setLoading(true);
      setError(null);
      try {
        const data = await api.searchBooks(q);
        if (data.ok && data.books) {
          setBooks(data.books);
        } else {
          setBooks([]);
          setError(data.message || "Knygų nerasta");
        }
      } catch (err: any) {
        setError(err.message || "Klaida atliekant paiešką");
      } finally {
        setLoading(false);
      }
    }, 500),
    []
  );

  useEffect(() => {
    fetchBooks(query);
  }, [query, fetchBooks]);

  return (
    <div className="flex flex-col items-center bg-[var(--background)] min-h-screen p-6">
      <h1 className="text-3xl font-bold text-[var(--foreground)] mb-6">
        Ieškoti knygų
      </h1>

      <Input
        placeholder="Įveskite knygos pavadinimą arba autorių..."
        className="w-full max-w-lg bg-[var(--input)] text-[var(--foreground)]"
        value={query}
        onChange={(e) => setQuery(e.target.value)}
      />

      {loading && (
        <div className="text-[var(--muted-foreground)] mt-4">Kraunama...</div>
      )}
      {error && <div className="text-[var(--destructive)] mt-4">{error}</div>}

      <div className="grid grid-cols-1 sm:grid-cols-2 md:grid-cols-3 gap-4 mt-6 w-full max-w-4xl">
        {books.map((book) => (
          <Card
            key={book.id}
            className="bg-[var(--card)] text-[var(--card-foreground)] shadow-md cursor-pointer hover:shadow-lg transition"
            onClick={() => router.push(`/books/${book.id}`)}
          >
            <CardHeader>
              <CardTitle className="text-lg font-semibold">
                {book.pavadinimas}
              </CardTitle>
            </CardHeader>
            <CardContent className="text-sm">
              <p>
                <strong>Autoriai:</strong> {parseAuthors(book.autoriai)}
              </p>
              <p>
                <strong>Žanras:</strong> {book.zanras}
              </p>
              <p>
                <strong>Kaina:</strong> {book.kaina} €
              </p>
            </CardContent>
          </Card>
        ))}
      </div>
    </div>
  );
}
