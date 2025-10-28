#ifndef UB_UTILS_TOKENS_HPP
#define UB_UTILS_TOKENS_HPP

#include "utils/config.hpp"
#include "utils/logger.hpp"

#include <jwt-cpp/jwt.h>
#include <string>

namespace tokens {

struct TokenPayload {
	std::string userId;
	std::string role;
	std::chrono::system_clock::time_point expiresAt;
};

inline std::string _jwtSecret;

inline std::string generateToken(const std::string &userId, const std::string &role) {
	if (_jwtSecret.empty()) {
		_jwtSecret = config::get()["jwtSecret"].get<std::string>();
	}

	auto token = jwt::create()
					 .set_issuer("universiteto-biblioteka")
					 .set_subject(userId)
					 .set_payload_claim("role", jwt::claim(role))
					 .set_issued_at(std::chrono::system_clock::now())
					 .set_expires_at(std::chrono::system_clock::now() + std::chrono::hours{24})
					 .sign(jwt::algorithm::hs256{_jwtSecret});

	return token;
}

inline std::optional<TokenPayload> verifyToken(const std::string &token) {
	if (_jwtSecret.empty()) {
		_jwtSecret = config::get()["jwtSecret"].get<std::string>();
	}

	try {
		auto decoded = jwt::decode(token);

		auto verifier = jwt::verify()
							.allow_algorithm(jwt::algorithm::hs256{_jwtSecret})
							.with_issuer("universiteto-biblioteka");

		verifier.verify(decoded);

		return TokenPayload{
			.userId = decoded.get_subject(),
			.role = decoded.get_payload_claim("role").as_string(),
			.expiresAt = decoded.get_expires_at()};

	} catch (const std::exception &e) {
		logger::get()->warn("Neteisingas JWT tokenas: {}", e.what());
		return std::nullopt;
	}
};

} // namespace tokens

#endif // UB_UTILS_TOKENS_HPP