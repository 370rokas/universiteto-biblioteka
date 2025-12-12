"use client";

import { api, SkolinimuIstorijosIrasas } from "@/lib/api";
import { ColumnDef } from "@tanstack/react-table";
import { useEffect, useState } from "react";
import { IstorijaTable } from "./istorija-table";
import { Button } from "@/components/ui/button";

export const columns: ColumnDef<SkolinimuIstorijosIrasas>[] = [
    {
        accessorKey: "pavadinimas",
        header: "Pavadinimas",
    },
    {
        accessorKey: "autoriai",
        header: "Autoriai",
        cell: ({ row }) => {
            const autoriai: string = row.getValue("autoriai");
            const a = JSON.parse(autoriai);
            return <span>{a.join(", ")}</span>;
        }
    },
    {
        accessorKey: "nuoma_nuo",
        header: "Nuoma nuo",
    },
    {
        accessorKey: "nuoma_iki",
        header: "Nuoma iki",
    },
    {
        accessorKey: "grazinimo_laikas",
        header: "Grazininimo laikas",
        cell: ({ row }) => {
            const grazinimo_laikas: string = row.getValue("grazinimo_laikas");

            if (!grazinimo_laikas) {
                return <Button variant="outline" size="sm" onClick={() => {
                    const id: string = row.original.id;
                    api.grazintiEgzemplioriu(id).then(() => {
                        alert("Knyga sėkmingai grąžinta!");
                        window.location.reload();
                    }).catch((error) => {
                        alert("Klaida grąžinant knygą: " + error.message);
                    });
                }}> Gražinti </Button>;

            } else {
                const date = new Date(grazinimo_laikas);
                return date.toLocaleDateString();
            }

        }
    },
    {
        accessorKey: "suma",
        header: "Skola",
        cell: ({ row }) => {
            const s = row.getValue("suma") || 0;
            return s ? `${s} €` : "-";
        },
    }
];

export default function IstorijaPage() {
    const [loading, setLoading] = useState(false);
    const [istorija, setIstorija] = useState<SkolinimuIstorijosIrasas[] | null>(null);

    useEffect(() => {
        setLoading(true);
        api.skolinimuIstorija().then(response => {
            setIstorija(response.istorija || null);
            setLoading(false);
        }).catch(error => {
            alert("Klaida gaunant skolinimų istoriją:" + error.message);
            setLoading(false);
        });
    }, []);

    if (loading) {
        return (
            <div className="w-full flex justify-center items-center h-40 text-xl">
                Kraunama...
            </div>
        );
    }

    return (
        <div className="max-w-8xl mx-auto p-6 rounded-lg shadow">
            <h1 className="mb-6 text-3xl font-bold text-gray-200">
                Skolinimų istorija
            </h1>

            {istorija ? (
                <IstorijaTable columns={columns} data={istorija} />
            ) : (
                <div className="text-gray-600">Nėra skolinimų istorijos įrašų.</div>
            )}
        </div>
    );
}