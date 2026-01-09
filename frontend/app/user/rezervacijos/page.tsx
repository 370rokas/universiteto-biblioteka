"use client";

import { Button } from "@/components/ui/button";
import { api } from "@/lib/api";
import { useEffect, useState } from "react";

export default function Rezervacijos() {
    const [data, setData] = useState<any[] | null>(null);
    const [loading, setLoading] = useState(false);
    const a = "";

    useEffect(() => {
        setLoading(true);
        api.gautiRezervacijas().then(response => {
            console.log(response);
            setData(response.reservations || null);
            setLoading(false);
        }).catch(error => {
            alert("Klaida gaunant rezervacijas:" + error.message);
            setLoading(false);
        });
    }, [a]);

    console.log(data);

    return (
        <div className="flex items-center justify-center min-h-screen text-[var(--muted-foreground)]">
            {loading ? (
                <div>Kraunama...</div>
            ) : (
                <div className="w-full max-w-2xl p-4 bg-[var(--card)] text-[var(--card-foreground)] rounded-lg shadow-lg">
                    <Button onClick={() => {
                        window.location.href = "/home";
                    }} className="mb-4">Atgal</Button>

                    <h1 className="text-2xl font-bold mb-4">Mano rezervacijos</h1>
                    {data && data.length > 0 ? (
                        <table className="w-full table-auto border-collapse">
                            <thead>
                                <tr>
                                    <th className="border-b p-2 text-left">Data</th>
                                    <th className="border-b p-2 text-left">Knyga</th>
                                    <th className="border-b p-2 text-left">Statusas</th>
                                </tr>
                            </thead>
                            <tbody>
                                {data.map((rez) => (
                                    <tr key={rez.id}>
                                        <td className="border-b p-2">{rez.rezervacijosData}</td>
                                        <td className="border-b p-2">{rez.knygosId}</td>
                                        <td className="border-b p-2">{rez.statusas}</td>
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