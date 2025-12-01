#ifndef UB_API_ROUTES_USER_HPP
#define UB_API_ROUTES_USER_HPP

#include "api/middlewares.hpp"
#include "database/db.hpp"

inline void setupUserRoutes(crow::App<UB_CROW_MIDDLEWARES> &app, Database *db) {

	// Skolinimu istorija
	CROW_ROUTE(app, "/user/istorija")
		.methods(crow::HTTPMethod::GET)([&app, db](const crow::request &req) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);
			if (!mw::IsLoggedIn(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Unauthorized"}});
			}

			auto userId = ctx.tokenData.userId;
			auto istorija = db->gautiNuomosIstorija(userId);

			auto istorijaJson = crow::json::wvalue::list();
			for (const auto &item : istorija) {
				istorijaJson.push_back(crow::json::wvalue{
					{"id", item.id},
					{"pavadinimas", item.pavadinimas},
					{"autoriai", item.autoriai},
					{"nuoma_nuo", item.nuoma_nuo},
					{"nuoma_iki", item.nuoma_iki},
					{"grazinimo_laikas", item.grazinimo_laikas},
					{"suma", item.suma},
					{"sumoketa", item.sumoketa}});
			};

			return crow::response(200, crow::json::wvalue{
										   {"ok", true},
										   {"istorija", istorijaJson}});
		});
};

#endif // UB_API_ROUTES_USER_HPP