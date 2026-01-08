"use client";

import { useEffect, useState } from "react";
import { useRouter } from "next/navigation";
import { Button } from "@/components/ui/button";
import { Card, CardContent, CardHeader, CardTitle } from "@/components/ui/card";
import { api, VartotojoZinute } from "@/lib/api";

export default function Dashboard() {
  const router = useRouter();
  const [userInfo, setUserInfo] = useState<any>(null);
  const [loading, setLoading] = useState(true);
  const [messages, setMessages] = useState<VartotojoZinute[]>([]);

  useEffect(() => {
    const token = localStorage.getItem("authToken");
    if (!token) {
      router.push("/login");
      return;
    }

    api
      .tokenInfo()
      .then(setUserInfo)
      .catch(() => {
        localStorage.removeItem("authToken");
        router.push("/login");
      })
      .finally(() => setLoading(false));

    api.gautiZinutes().then((res) => {
      if (res.ok && res.messages) setMessages(res.messages);
    });
  }, [router]);

  if (loading) {
    return (
      <div className="flex items-center justify-center min-h-screen text-[var(--muted-foreground)]">
        Kraunama...
      </div>
    );
  }

  const clearMessages = () => {
    api.perskaitytiZinutes().then(() => {
      window.location.reload();
    });
  };

  return (
    <div className="flex min-h-screen items-center justify-center bg-[var(--background)] p-4">
      <Card className="w-[500px] bg-[var(--card)] text-[var(--card-foreground)] shadow-lg">
        <CardHeader>
          <CardTitle className="text-center text-2xl">
            Sveiki, {userInfo?.username || "Vartotojau"}!
          </CardTitle>
        </CardHeader>
        <CardContent className="flex flex-col gap-4 mt-4">
          <Button
            variant="default"
            className="bg-[var(--primary)] hover:bg-[var(--secondary)] text-[var(--primary-foreground)]"
            onClick={() => router.push("/books/search")}
          >
            Ieškoti knygų
          </Button>

          <Button
            variant="default"
            className="bg-[var(--secondary)] hover:bg-[var(--accent)] text-[var(--secondary-foreground)]"
            onClick={() => router.push("/user/istorija")}
          >
            Mano skolinimai
          </Button>

          <Button
            variant="default"
            className="bg-[var(--accent)] hover:bg-[var(--primary)] text-[var(--accent-foreground)]"
            onClick={() => router.push("/user/rezervacijos")}
          >
            Mano rezervacijos
          </Button>

          <Button
            variant="default"
            className="bg-[var(--accent)] hover:bg-[var(--primary)] text-[var(--accent-foreground)]"
            onClick={() => router.push("/user/skolos")}
          >
            Mano skolos
          </Button>

          {userInfo?.user.role === "administratorius" && (
            <Button
              variant="default"
              className="bg-[var(--primary)] hover:bg-[var(--secondary)] text-[var(--primary-foreground)]"
              onClick={() => router.push("/statistics")}
            >
              Statistikos
            </Button>
          )}

          {userInfo?.user.role === "administratorius" && (
            <Button
              variant="default"
              className="bg-[var(--primary)] hover:bg-[var(--secondary)] text-[var(--primary-foreground)]"
              onClick={() => router.push("/manage")}
            >
              Knygu valdymas
            </Button>
          )}

          <Button
            variant="default"
            className="bg-[var(--destructive)] hover:bg-[var(--destructive-hover)] text-[var(--destructive-foreground)]"
            onClick={() => {
              localStorage.removeItem("authToken");
              router.push("/login");
            }}
          >
            Atsijungti
          </Button>
        </CardContent>
      </Card>

      {messages.length > 0 && (
        <Card className="w-[500px] bg-[var(--card)] text-[var(--card-foreground)] shadow-lg">
          <CardHeader>
            <CardTitle className="text-center text-xl">Jūsų žinutės</CardTitle>
          </CardHeader>
          <CardContent className="flex flex-col gap-2">
            {messages.map((msg) => (
              <div
                key={msg.id}
                className="border rounded p-2 bg-[var(--background)] text-[var(--foreground)]"
              >
                <div className="text-sm">{msg.pranesimas}</div>
                <div className="text-xs text-[var(--muted-foreground)]">{msg.issiuntimo_data}</div>
              </div>
            ))}

            <Button
              variant="destructive"
              className="mt-2"
              onClick={clearMessages}
            >
              Išvalyti žinutes
            </Button>
          </CardContent>
        </Card>
      )}
    </div>
  );
}
