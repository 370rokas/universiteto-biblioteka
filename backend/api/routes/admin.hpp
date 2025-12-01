#ifndef UB_API_ROUTES_ADMIN_HPP
#define UB_API_ROUTES_ADMIN_HPP

#include "api/middlewares.hpp"
#include "crow/app.h"
#include "database/db.hpp"
#include "valdymas/skolos.hpp"

inline void setupAdminRoutes(crow::App<UB_CROW_MIDDLEWARES> &app, Database *db) {

	// Sesijos info
	CROW_ROUTE(app, "/admin/atnaujintiSkolas")
		.methods(crow::HTTPMethod::GET)([&](const crow::request &req) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);

			if (!mw::IsAdmin(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas arba pasibaigęs tokenas"}});
			}

			valdymas::atnaujintiSkolas();

			return crow::response(200, crow::json::wvalue{
										   {"ok", true}});
		});
}

#endif // UB_API_ROUTES_ADMIN_HPP