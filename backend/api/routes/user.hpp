#ifndef UB_API_ROUTES_USER_HPP
#define UB_API_ROUTES_USER_HPP

#include "valdymas/rezervavimas.hpp"
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

	CROW_ROUTE(app, "/user/skolos")
		.methods(crow::HTTPMethod::GET)([&app, db](const crow::request &req) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);
			if (!mw::IsLoggedIn(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Unauthorized"}});
			}

			auto userId = ctx.tokenData.userId;

			auto skolos = db->gautiVisasSkolasPagalVartotojoId(userId);

			if (!skolos.has_value()) {
				return crow::response(200, crow::json::wvalue{
											   {"ok", true},
											   {"skolos", crow::json::wvalue::list()}});
			}

			auto skolosJson = crow::json::wvalue::list();
			for (const auto &item : skolos.value()) {
				skolosJson.push_back(crow::json::wvalue{
					{"skola_id", item.skola_id},
					{"suma", item.suma},
					{"sumoketa", item.sumoketa},
					{"nuomos_id", item.nuomos_id},
					{"grazinimo_laikas", item.grazinimo_laikas}});
			};

			return crow::response(200, crow::json::wvalue{
										   {"ok", true},
										   {"skolos", skolosJson}});
		});

	CROW_ROUTE(app, "/user/getMessages")
		.methods(crow::HTTPMethod::GET)([&app, db](const crow::request &req) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);
			if (!mw::IsLoggedIn(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Unauthorized"}});
			}

			auto userId = ctx.tokenData.userId;

			auto messages = db->gautiVartotojoZinutes(userId);

			if (!messages.has_value()) {
				return crow::response(200, crow::json::wvalue{
											   {"ok", true},
											   {"messages", crow::json::wvalue::list()}});
			}

			auto messagesJson = crow::json::wvalue::list();
			for (const auto &item: messages.value()) {
				messagesJson.push_back(crow::json::wvalue{
					{"id", item.id},
					{"pranesimas", item.pranesimas},
					{"issiuntimo_data", item.issiuntimo_data}});
			};

			return crow::response(200, crow::json::wvalue{
										   {"ok", true},
										   {"messages", messagesJson}});
		});

	CROW_ROUTE(app, "/user/markMessagesRead")
		.methods(crow::HTTPMethod::GET)([&app, db](const crow::request &req) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);
			if (!mw::IsLoggedIn(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Unauthorized"}});
			}

			auto userId = ctx.tokenData.userId;

			db->perskaitytiZinutes(userId);

			return crow::response(200, crow::json::wvalue{
										   {"ok", true},
										   {"message", "All messages marked as read"}});
		});

	CROW_ROUTE(app, "/user/reservations")
		.methods(crow::HTTPMethod::GET)([&app, db](const crow::request &req) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);
			if (!mw::IsLoggedIn(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Unauthorized"}});
			}

			auto userId = ctx.tokenData.userId;

			auto reservations = valdymas::gautiVartotojoRezervacijas(userId);
			logger::get()->info("Gautos vartotojo ('{}') rezervacijos: {}", userId, reservations.size());

			if (reservations.empty()) {
				return crow::response(200, crow::json::wvalue{
											   {"ok", true},
											   {"reservations", crow::json::wvalue::list()}});
			}

			auto reservationsJson = crow::json::wvalue::list();
			for (const auto &item : reservations) {
				reservationsJson.push_back(crow::json::wvalue{
					{"id", item.id},
					{"knygosId", item.knygosId},
					{"vartotojoId", item.vartotojoId},
					{"rezervacijosData", item.rezervacijosData},
					{"statusas", item.statusas}});
			};

			return crow::response(200, crow::json::wvalue{
										   {"ok", true},
										   {"reservations", reservationsJson}});
		});
};

#endif // UB_API_ROUTES_USER_HPP