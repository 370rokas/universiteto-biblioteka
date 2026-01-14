#ifndef UB_API_MIDDLEWARES_HPP
#define UB_API_MIDDLEWARES_HPP

#include "utils/tokens.hpp"

#include <crow.h>
#include <crow/middlewares/cors.h>

#define UB_CROW_MIDDLEWARES mw::LoggerMW, mw::TokenAuth, crow::CORSHandler

namespace mw {

struct LoggerMW {
	struct context {
		std::chrono::steady_clock::time_point startTime;
	};

	void before_handle(crow::request &req, crow::response &res, context &ctx) {
		ctx.startTime = std::chrono::steady_clock::now();
	}

	void after_handle(crow::request &req, crow::response &res, context &ctx) {
		logger::get()->info("[{}] {} -> {} ({} | {}ms)",
							crow::method_name(req.method),
							req.url,
							res.code,
							req.remote_ip_address,
							std::chrono::duration_cast<std::chrono::milliseconds>(
								std::chrono::steady_clock::now() - ctx.startTime)
								.count());
	}
};

struct TokenAuth {
	struct context {
		bool tokenValid = false;
		tokens::TokenPayload tokenData;
	};

	void before_handle(crow::request &req, crow::response &res, context &ctx) {
		auto authHeader = req.get_header_value("Authorization");

		if (authHeader.empty() || authHeader.find("Bearer ") != 0) {
			ctx.tokenValid = false;
			return;
		}

		std::string token = authHeader.substr(7); // Pašalinam "Bearer "
		auto tokenDataOpt = tokens::verifyToken(token);

		if (!tokenDataOpt.has_value()) {
			ctx.tokenValid = false;
			return;
		}

		ctx.tokenValid = true;
		ctx.tokenData = tokenDataOpt.value();
	}

	void
	after_handle(crow::request &, crow::response &, context &) {
	}
};

inline bool IsLoggedIn(TokenAuth::context &ctx) {
	return ctx.tokenValid;
}

inline bool IsAdmin(TokenAuth::context &ctx) {
	return ctx.tokenValid && ctx.tokenData.role == "administratorius";
}

}; // namespace mw

#endif // UB_API_MIDDLEWARES_HPP