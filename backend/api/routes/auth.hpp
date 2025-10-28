#ifndef UB_API_ROUTES_AUTH_HPP
#define UB_API_ROUTES_AUTH_HPP

#include <bcrypt.h>
#include <crow.h>

#include "api/middlewares.hpp"
#include "database/db.hpp"
#include "utils/tokens.hpp"

inline void setupAuthRoutes(crow::App<UB_CROW_MIDDLEWARES> &app, Database *db) {

	// Registracija
	CROW_ROUTE(app, "/auth/register")
		.methods(crow::HTTPMethod::POST)([db](const crow::request &req) {
			auto body = crow::json::load(req.body);
			if (!body) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas JSON formatas"}});
			};

			std::string username = body["username"].s();
			std::string password = body["password"].s();

			if (password.empty() || password.length() < 6) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Slaptažodis turi būti bent 6 simbolių ilgio"}});
			}

			if (username.length() < 3 || username.length() > 20) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Vartotojo vardas turi būti nuo 3 iki 20 simbolių ilgio"}});
			}

			if (username.find_first_not_of("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-") != std::string::npos) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Vartotojo vardas gali būti sudarytas tik iš raidžių, skaičių, '_' ir '-'"}});
			}

			auto resp = db->createUser(username, password);

			if (resp.first == true) {
				logger::get()->info("[AUTH] [REGSITER] {}", username);

				return crow::response(200, crow::json::wvalue{
											   {"ok", true},
											   {"message", "Vartotojas sėkmingai užregistruotas"}});
			} else {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", resp.second}});
			}
		});

	// Prisijungimas
	CROW_ROUTE(app, "/auth/login")
		.methods(crow::HTTPMethod::POST)([db](const crow::request &req) {
			auto body = crow::json::load(req.body);
			if (!body) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas JSON formatas"}});
			};

			std::string username = body["username"].s();
			std::string password = body["password"].s();

			auto userOpt = db->getUserByUsername(username);
			if (!userOpt.has_value()) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas vartotojo vardas arba slaptažodis"}});
			}

			Vartotojas user = userOpt.value();
			if (!bcrypt::validatePassword(password, user.password_hash)) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas vartotojo vardas arba slaptažodis"}});
			} else {
				std::string token = tokens::generateToken(user.id, user.role);
				logger::get()->info("[AUTH] [LOGIN] {}", username);

				return crow::response(200, crow::json::wvalue{
											   {"ok", true},
											   {"message", "Sėkmingai prisijungta"},
											   {"user", crow::json::wvalue{
															{"id", user.id},
															{"username", user.username},
															{"role", user.role},
															{"pareigos", user.pareigos}}},
											   {"token", token}});
			}
		});

	// Sesijos info
	CROW_ROUTE(app, "/auth/")
		.methods(crow::HTTPMethod::GET)([&](const crow::request &req) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);

			if (!mw::IsLoggedIn(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas arba pasibaigęs tokenas"}});
			}

			return crow::response(200, crow::json::wvalue{
										   {"ok", true},
										   {"user", crow::json::wvalue{
														{"id", ctx.tokenData.userId},
														{"role", ctx.tokenData.role}}}});
		});
}

#endif // UB_API_ROUTES_AUTH_HPP