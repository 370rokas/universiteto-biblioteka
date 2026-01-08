const API_BASE_URL =
  process.env.NEXT_PUBLIC_API_BASE_URL || "http://localhost:2233/";

const getToken = () =>
  typeof window !== "undefined" ? localStorage.getItem("authToken") : null;

async function request<T>(
  endpoint: string,
  options: RequestInit = {}
): Promise<T> {
  const token = getToken();

  const headers: HeadersInit = {
    "Content-Type": "application/json",
    ...(token ? { Authorization: `Bearer ${token}` } : {}),
    ...options.headers,
  };

  const res = await fetch(`${API_BASE_URL}${endpoint}`, {
    ...options,
    headers,
  });

  if (!res.ok) {
    const errorData = await res.json().catch(() => ({}));
    throw new Error((errorData as any).message || "API request failed");
  }

  return res.json() as Promise<T>;
}

export interface Book {
  id: string;
  leidykla: string;
  isbn: string;
  autoriai: string;
  pavadinimas: string;
  zanras: string;
  leidimo_metai: string;
  kaina: number;
}

export interface Egzempliorius {
  id: string;
  knygos_id: string;
  statusas: string;
  bukle: string;
  isigyta: string;
}

export interface GetServerInfoResponse {
  ok: boolean;
  serverName: string;
}

export interface LoginResponse {
  ok: boolean;
  message: string;
  token?: string;
  user?: {
    id: string;
    username: string;
    role: string;
    pareigos: string;
  };
}

export interface RegisterResponse {
  ok: boolean;
  message: string;
}

export interface TokenInfoResponse {
  ok: boolean;
  message?: string;
  user?: {
    id: string;
    role: string;
  };
}

export interface SearchBooksResponse {
  ok: boolean;
  message?: string;
  books?: Book[];
}

export interface GetBookByIdResponse {
  ok: boolean;
  message?: string;
  book?: Book;
}

export interface GetBookEgzemplioriaiResponse {
  ok: boolean;
  message?: string;
  egzemplioriai?: Egzempliorius[];
}

export interface SkolinimuIstorijosIrasas {
  id: string;
  pavadinimas: string;
  autoriai: string;
  nuoma_nuo: string;
  nuoma_iki: string;
  grazinimo_laikas: string;
  suma: string;
  sumoketa: boolean;
}

export interface SkoluIstorijosIrasas {
  skola_id: string;
  suma: string;
  sumoketa: boolean;
  nuomos_id: string;
  grazinimo_laikas: string;
}

export interface IsduotuKnyguStatistika {
  dabarIsduota: number;
  veluojamaGrazinti: number;
}

export interface PopuliariausiaKnyga {
  id: string;
  pavadinimas: string;
  isdavimoKiekis: number;
}

export interface PopuliariausiosKnygos {
  d30: PopuliariausiaKnyga[];
  d180: PopuliariausiaKnyga[];
}

export interface AktyvusVartotojas {
  id: string;
  username: string;
  isdavimuKiekis: number;
}

export interface AktyviausiVartotojai {
  d30: AktyvusVartotojas[];
  d180: AktyvusVartotojas[];
}

export interface StatistikosResponse {
  ok: boolean;
  message?: string;
  isduotuKnyguStatistika?: IsduotuKnyguStatistika;
  populiariausiosKnygos?: PopuliariausiosKnygos;
  aktyviausiVartotojai?: AktyviausiVartotojai;
}

export interface VartotojoZinute {
  id: string;
  pranesimas: string;
  issiuntimo_data: string;
}

export const session = {
  getToken: () => localStorage.getItem("authToken"),
  getRole: () => localStorage.getItem("userRole"),
  clear: () => {
    localStorage.removeItem("authToken");
    localStorage.removeItem("userRole");
  },
};

export const api = {
  login: async (username: string, password: string): Promise<LoginResponse> => {
    const data = await request<LoginResponse>("auth/login", {
      method: "POST",
      body: JSON.stringify({ username, password }),
    });
    if (data.token) localStorage.setItem("authToken", data.token);
    if (data.user) localStorage.setItem("userRole", data.user.role);
    return data;
  },

  logout: () => session.clear(),

  getUserRole: (): string | null => session.getRole(),

  register: (username: string, password: string): Promise<RegisterResponse> =>
    request<RegisterResponse>("auth/register", {
      method: "POST",
      body: JSON.stringify({ username, password }),
    }),

  getServerInfo: (): Promise<GetServerInfoResponse> =>
    request<GetServerInfoResponse>("", {
      method: "GET",
    }),

  tokenInfo: (): Promise<TokenInfoResponse> =>
    request<TokenInfoResponse>("auth/"),

  searchBooks: (query: string): Promise<SearchBooksResponse> =>
    request<SearchBooksResponse>(`books/search?q=${encodeURIComponent(query)}`),

  getBookById: (bookId: string): Promise<GetBookByIdResponse> =>
    request<GetBookByIdResponse>(`books/${bookId}`),

  getBookEgzemplioriai: (bookId: string): Promise<GetBookEgzemplioriaiResponse> =>
    request<GetBookEgzemplioriaiResponse>(`books/${bookId}/egzemplioriai`),

  nuomotiEgzemplioriu: (egzemplioriusId: string): Promise<{ ok: boolean; message?: string }> =>
    request<{ ok: boolean; message?: string }>(`books/egzempliorius/${egzemplioriusId}/borrow`, {
      method: "POST",
    }),

  rezervuotiEgzemplioriu: (egzemplioriusId: string): Promise<{ ok: boolean; message?: string }> =>
    request<{ ok: boolean; message?: string }>(`books/egzempliorius/${egzemplioriusId}/reserve`, {
      method: "POST",
    }),

  grazintiEgzemplioriu: (egzemplioriusId: string): Promise<{ ok: boolean; message?: string }> =>
    request<{ ok: boolean; message?: string }>(`books/egzempliorius/${egzemplioriusId}/return`, {
      method: "POST",
    }),

  skolinimuIstorija: (): Promise<{ ok: boolean, message?: string, istorija?: SkolinimuIstorijosIrasas[] }> =>
    request<{ ok: boolean, message?: string, istorija?: SkolinimuIstorijosIrasas[] }>("user/istorija"),

  skoluIstorija: (): Promise<{ ok: boolean, message?: string, skolos?: SkoluIstorijosIrasas[] }> =>
    request<{ ok: boolean, message?: string, skolos?: SkoluIstorijosIrasas[] }>("user/skolos"),

  gautiStatistika: (): Promise<{ ok: boolean; message?: string; statistika?: any }> =>
    request<StatistikosResponse>("admin/statistics"),

  gautiZinutes: (): Promise<{ ok: boolean; message?: string; messages?: VartotojoZinute[] }> =>
    request<{ ok: boolean; message?: string; messages?: VartotojoZinute[] }>("user/getMessages"),

  perskaitytiZinutes: (): Promise<{ ok: boolean; message?: string }> =>
    request<{ ok: boolean; message?: string }>("user/markMessagesRead")
};
