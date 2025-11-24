"use client";

import { useState } from "react";
import { useRouter } from "next/navigation";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { Input } from "@/components/ui/input";
import { api } from "@/lib/api";

export default function Login() {
  const router = useRouter();
  const [username, setUsername] = useState("");
  const [password, setPassword] = useState("");
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);

  const handleLogin = async () => {
    setLoading(true);
    setError(null);
    try {
      const data = await api.login(username, password);
      if (data.token) {
        router.push("/home");
      } else {
        throw new Error("Neteisingi prisijungimo duomenys");
      }
    } catch (err: any) {
      setError(err.message || "Prisijungti nepavyko");
    } finally {
      setLoading(false);
    }
  };

  return (
    <div className="flex min-h-screen items-center justify-center bg-[var(--background)]">
      <Card className="w-[400px] bg-[var(--card)] text-[var(--card-foreground)] shadow-lg">
        <CardHeader>
          <CardTitle className="text-center text-2xl">Prisijungimas</CardTitle>
        </CardHeader>
        <CardContent className="flex flex-col gap-4 mt-4">
          {error && (
            <div className="text-[var(--destructive)] text-center">{error}</div>
          )}
          <Input
            placeholder="Vartotojo vardas"
            value={username}
            onChange={(e) => setUsername(e.target.value)}
            className="bg-[var(--input)] text-[var(--foreground)]"
          />
          <Input
            type="password"
            placeholder="Slaptažodis"
            value={password}
            onChange={(e) => setPassword(e.target.value)}
            className="bg-[var(--input)] text-[var(--foreground)]"
          />
          <Button
            className="bg-[var(--primary)] hover:bg-[var(--secondary)] text-[var(--primary-foreground)]"
            onClick={handleLogin}
            disabled={loading}
          >
            {loading ? "Prisijungiama..." : "Prisijungti"}
          </Button>
          <Button
            variant="link"
            className="text-[var(--accent-foreground)] hover:text-[var(--accent)]"
            onClick={() => router.push("/register")}
          >
            Neturite paskyros? Registruotis
          </Button>
        </CardContent>
      </Card>
    </div>
  );
}
