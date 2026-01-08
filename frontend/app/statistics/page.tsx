"use client";

import { useEffect, useState } from "react";
import { useRouter } from "next/navigation";
import { api } from "@/lib/api";
import {
    PieChart,
    Pie,
    Cell,
    Tooltip,
    ResponsiveContainer,
    PieLabelRenderProps,
} from "recharts";

interface StatistikosResponse {
    ok: boolean;
    message?: string;
    isduotuKnyguStatistika: {
        dabarIsduota: number;
        veluojamaGrazinti: number;
    };
    populiariausiosKnygos: {
        d30: PopuliariausiaKnyga[];
        d180: PopuliariausiaKnyga[];
    };
    aktyviausiVartotojai: {
        d30: AktyvusVartotojas[];
        d180: AktyvusVartotojas[];
    };
}

interface PopuliariausiaKnyga {
    id: string;
    pavadinimas: string;
    isdavimoKiekis: number;
}

interface AktyvusVartotojas {
    id: string;
    username: string;
    isdavimuKiekis: number;
}

const renderPieLabel = ({
    name,
    value,
    percent,
}: PieLabelRenderProps) => {
    if (name == null || value == null || percent == null) return null;

    return `${name}: ${value} (${(percent * 100).toFixed(0)}%)`;
};

export default function Statistics() {
    const router = useRouter();

    const [data, setData] = useState<StatistikosResponse | null>(null);
    const [loading, setLoading] = useState(true);
    const [period, setPeriod] = useState<"d30" | "d180">("d30");

    useEffect(() => {
        const token = localStorage.getItem("authToken");
        if (!token) {
            router.push("/login");
            return;
        }

        api
            .gautiStatistika()
            .then((res) => {
                if (res.ok) setData(res as StatistikosResponse);
            })
            .catch(console.error)
            .finally(() => setLoading(false));
    }, [router]);

    if (loading) {
        return (
            <div className="min-h-screen flex items-center justify-center">
                Kraunama...
            </div>
        );
    }

    if (!data) return null;

    const pieData = [
        {
            name: "Išduota",
            value: data.isduotuKnyguStatistika.dabarIsduota - data.isduotuKnyguStatistika.veluojamaGrazinti,
        },
        {
            name: "Vėluojama grąžinti",
            value: data.isduotuKnyguStatistika.veluojamaGrazinti,
        },
    ];

    const COLORS = ["#22c55e", "#ef4444"];

    return (
        <div className="min-h-screen bg-[var(--background)] p-6 space-y-10">
            <section>
                <h2 className="text-xl font-semibold mb-4">
                    Išduotų knygų statistika
                </h2>

                <div className="w-full h-72 bg-[var(--card)] rounded shadow p-4 flex flex-col items-center justify-center">
                    <ResponsiveContainer width="100%" height="100%">
                        <PieChart>
                            <Pie
                                data={pieData}
                                dataKey="value"
                                nameKey="name"
                                label={renderPieLabel}
                                labelLine={false}
                            >
                                {pieData.map((_, i) => (
                                    <Cell key={i} fill={COLORS[i]} />
                                ))}
                            </Pie>
                            <Tooltip />
                        </PieChart>
                    </ResponsiveContainer>

                    <div className="mt-2 text-center text-sm text-[var(--muted-foreground)]">
                        Iš viso išduota knygų:{" "}
                        <span className="font-semibold text-[var(--foreground)]">
                            {data.isduotuKnyguStatistika.dabarIsduota}
                        </span>
                    </div>
                </div>
            </section>

            <div className="flex gap-2">
                {(["d30", "d180"] as const).map((p) => (
                    <button
                        key={p}
                        onClick={() => setPeriod(p)}
                        className={`px-4 py-2 rounded ${period === p
                            ? "bg-blue-600 text-white"
                            : "bg-[var(--card)]"
                            }`}
                    >
                        {p === "d30" ? "30 dienų" : "180 dienų"}
                    </button>
                ))}
            </div>

            <section>
                <h2 className="text-xl font-semibold mb-2">
                    Populiariausios knygos
                </h2>

                <table className="w-full bg-[var(--card)] rounded shadow">
                    <thead>
                        <tr className="text-left border-b">
                            <th className="p-3">Pavadinimas</th>
                            <th className="p-3">Išdavimų kiekis</th>
                        </tr>
                    </thead>
                    <tbody>
                        {data.populiariausiosKnygos[period].map((k) => (
                            <tr key={k.id} className="border-b last:border-0">
                                <td className="p-3">{k.pavadinimas}</td>
                                <td className="p-3">{k.isdavimoKiekis}</td>
                            </tr>
                        ))}
                    </tbody>
                </table>
            </section>

            <section>
                <h2 className="text-xl font-semibold mb-2">
                    Aktyviausi vartotojai
                </h2>

                <table className="w-full bg-[var(--card)] rounded shadow">
                    <thead>
                        <tr className="text-left border-b">
                            <th className="p-3">Vartotojas</th>
                            <th className="p-3">Išdavimų kiekis</th>
                        </tr>
                    </thead>
                    <tbody>
                        {data.aktyviausiVartotojai[period].map((u) => (
                            <tr key={u.id} className="border-b last:border-0">
                                <td className="p-3">{u.username}</td>
                                <td className="p-3">{u.isdavimuKiekis}</td>
                            </tr>
                        ))}
                    </tbody>
                </table>
            </section>
        </div>
    );
}
