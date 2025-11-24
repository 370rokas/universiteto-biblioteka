"use client";

import { Button } from "@/components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { useState, useEffect } from "react";
import { api } from "@/lib/api";

export default function Home() {
  const [serverInfo, setServerInfo] = useState<any>(null);
  const [loading, setLoading] = useState(true);

  useEffect(() => {
    api
      .getServerInfo()
      .then(setServerInfo)
      .catch(console.error)
      .finally(() => setLoading(false));
  }, []);

  if (loading) {
    return (
      <div className="flex items-center justify-center min-h-screen text-[var(--muted-foreground)]">
        Kraunama...
      </div>
    );
  }

  if (!serverInfo || serverInfo.ok !== true) {
    return (
      <div className="flex items-center justify-center min-h-screen text-[var(--destructive)]">
        Klaida gaunant serverio informaciją.
      </div>
    );
  }

  return (
    <div className="flex min-h-screen items-center justify-center bg-[var(--background)]">
      <Card className="w-[400px] bg-[var(--card)] text-[var(--card-foreground)] shadow-lg">
        <CardHeader>
          <CardTitle className="text-center text-2xl">
            Sveiki atvykę į: {serverInfo.serverName}
          </CardTitle>
        </CardHeader>
        <CardContent className="flex flex-col gap-4 mt-6">
          <Button
            className="bg-[var(--primary)] hover:bg-[var(--secondary)] text-[var(--primary-foreground)]"
            onClick={() => (window.location.href = "/login")}
          >
            Prisijungti
          </Button>
          <Button
            className="bg-[var(--secondary)] hover:bg-[var(--accent)] text-[var(--secondary-foreground)]"
            onClick={() => (window.location.href = "/register")}
          >
            Registruotis
          </Button>
        </CardContent>
      </Card>
    </div>
  );
}
