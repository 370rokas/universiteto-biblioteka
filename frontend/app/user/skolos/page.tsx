"use client";

import { api, SkoluIstorijosIrasas } from "@/lib/api";
import { useEffect, useState } from "react";

export default function SkolosPage() {
    const [data, setData] = useState<SkoluIstorijosIrasas[] | null>(null);
    const [loading, setLoading] = useState(false);
    const a = "";

    useEffect(() => {
        setLoading(true);
        api.skoluIstorija().then(response => {
            setData(response.skolos || null);
            setLoading(false);
        }).catch(error => {
            alert("Klaida gaunant skolas:" + error.message);
            setLoading(false);
        });
    }, [a]);

    return (
        <div className="flex items-center justify-center min-h-screen text-[var(--muted-foreground)]">
            {loading ? (
                <div>Kraunama...</div>
            ) : (
                <div className="w-full max-w-2xl p-4 bg-[var(--card)] text-[var(--card-foreground)] rounded-lg shadow-lg">
                    <h1 className="text-2xl font-bold mb-4">Mano skolos</h1>
                    {data && data.length > 0 ? (
                        <table className="w-full table-auto border-collapse">
                            <thead>
                                <tr>
                                    <th className="border-b p-2 text-left">Grąžinimo laikas</th>
                                    <th className="border-b p-2 text-left">Suma</th>
                                    <th className="border-b p-2 text-left">Sumoketa</th>
                                </tr>
                            </thead>
                            <tbody>
                                {data.map((skola) => (
                                    <tr key={skola.skola_id}>
                                        <td className="border-b p-2">{skola.grazinimo_laikas.length > 0 ? skola.grazinimo_laikas : "Knyga dar negražinta"}</td>
                                        <td className="border-b p-2">{skola.suma} €</td>
                                        <td className="border-b p-2">{skola.sumoketa ? "Taip" : "Ne"}</td>
                                    </tr>
                                ))}
                            </tbody>
                        </table>
                    ) : (
                        <div>Nėra skolų.</div>
                    )}
                </div>
            )}
        </div>
    );
}