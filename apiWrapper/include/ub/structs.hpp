#ifndef UB_API_STRUCTS_HPP
#define UB_API_STRUCTS_HPP

#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include <vector>

namespace ub {

struct ServerInfoResp {
	bool ok;
	std::string serverName;
};

inline void from_json(const nlohmann::json &j, ServerInfoResp &s) {
	j.at("ok").get_to(s.ok);
	j.at("serverName").get_to(s.serverName);
};

struct AuthCredsBody {
	std::string username;
	std::string password;
};

inline void to_json(nlohmann::json &j, const AuthCredsBody &a) {
	j = nlohmann::json{
		{"username", a.username},
		{"password", a.password}};
};

struct UserInfo {
	std::string id;
	std::string username;
	std::string role;
	std::string pareigos;
};

inline void from_json(const nlohmann::json &j, UserInfo &u) {
	j.at("id").get_to(u.id);
	j.at("username").get_to(u.username);
	j.at("role").get_to(u.role);
	j.at("pareigos").get_to(u.pareigos);
};

struct UserInfoFromToken {
	std::string id;
	std::string role;
};

inline void from_json(const nlohmann::json &j, UserInfoFromToken &u) {
	j.at("id").get_to(u.id);
	j.at("role").get_to(u.role);
};

struct RegisterResp {
	bool ok;
	std::string message;
};

inline void from_json(const nlohmann::json &j, RegisterResp &r) {
	j.at("ok").get_to(r.ok);
	j.at("message").get_to(r.message);
};

struct LoginResp {
	bool ok;
	std::string message;

	std::optional<UserInfo> user;
	std::optional<std::string> token;
};

inline void from_json(const nlohmann::json &j, LoginResp &r) {
	j.at("ok").get_to(r.ok);
	j.at("message").get_to(r.message);

	if (j.contains("user") && !j.at("user").is_null()) {
		r.user = j.at("user").get<UserInfo>();
	}

	if (j.contains("token") && !j.at("token").is_null()) {
		r.token = j.at("token").get<std::string>();
	}
};

struct SessionInfoResp {
	bool ok;
	std::optional<std::string> message;
	std::optional<UserInfoFromToken> user;
};

inline void from_json(const nlohmann::json &j, SessionInfoResp &r) {
	j.at("ok").get_to(r.ok);

	if (j.contains("message") && !j.at("message").is_null()) {
		r.message = j.at("message").get<std::string>();
	}

	if (j.contains("user") && !j.at("user").is_null()) {
		r.user = j.at("user").get<UserInfoFromToken>();
	}
};

struct BookInfo {
	std::string id;
	std::string isbn;
	std::string pavadinimas;
	std::string autoriai;
	std::string zanras;
	std::string leidykla;
	std::string leidimo_metai;
	double kaina;
};

inline void from_json(const nlohmann::json &j, BookInfo &b) {
	j.at("id").get_to(b.id);
	j.at("isbn").get_to(b.isbn);
	j.at("pavadinimas").get_to(b.pavadinimas);
	j.at("autoriai").get_to(b.autoriai);
	j.at("zanras").get_to(b.zanras);
	j.at("leidykla").get_to(b.leidykla);
	j.at("leidimo_metai").get_to(b.leidimo_metai);
	j.at("kaina").get_to(b.kaina);
};

struct SearchResp {
	bool ok;
	std::optional<std::string> message;
	std::optional<std::vector<ub::BookInfo>> books;
};

inline void from_json(const nlohmann::json &j, SearchResp &s) {
	j.at("ok").get_to(s.ok);

	if (j.contains("message") && !j.at("message").is_null()) {
		s.message = j.at("message").get<std::string>();
	}

	if (j.contains("books") && !j.at("books").is_null()) {
		s.books = j.at("books").get<std::vector<ub::BookInfo>>();
	}
};

struct GetBookByIdResp {
	bool ok;
	std::optional<std::string> message;
	std::optional<ub::BookInfo> book;
};

inline void from_json(const nlohmann::json &j, GetBookByIdResp &g) {
	j.at("ok").get_to(g.ok);

	if (j.contains("message") && !j.at("message").is_null()) {
		g.message = j.at("message").get<std::string>();
	}

	if (j.contains("book") && !j.at("book").is_null()) {
		g.book = j.at("book").get<ub::BookInfo>();
	}
};

} // namespace ub

#endif // UB_API_STRUCTS_HPP