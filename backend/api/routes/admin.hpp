#ifndef UB_API_ROUTES_ADMIN_HPP
#define UB_API_ROUTES_ADMIN_HPP

#include "api/middlewares.hpp"
#include "crow/app.h"
#include "database/db.hpp"
#include "valdymas/skolos.hpp"
#include "valdymas/statistika.hpp"

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

	// Statistikos
	CROW_ROUTE(app, "/admin/statistics")
		.methods(crow::HTTPMethod::GET)([&](const crow::request &req) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);

			if (!mw::IsAdmin(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas arba pasibaigęs tokenas"}});
			}

			Statistika stats = valdymas::gautiBendraStatistika();

			crow::json::wvalue res;
			res["ok"] = true;

			// Isduotu knygu statistika
			res["isduotuKnyguStatistika"]["dabarIsduota"] = stats.IsduotuKnygu.DabarIsduota;
			res["isduotuKnyguStatistika"]["veluojamaGrazinti"] = stats.IsduotuKnygu.VeluojamaGrazinti;

			res["populiariausiosKnygos"]["d30"] = crow::json::wvalue::list();
			res["populiariausiosKnygos"]["d180"] = crow::json::wvalue::list();
			res["aktyviausiVartotojai"]["d30"] = crow::json::wvalue::list();
			res["aktyviausiVartotojai"]["d180"] = crow::json::wvalue::list();

			// Populiariausios knygos
			for (const auto &knyga : stats.PopuliariausiosKnygos.D30) {
				crow::json::wvalue knygaJson;
				knygaJson["pavadinimas"] = knyga.Pavadinimas;
				knygaJson["id"] = knyga.Id;
				knygaJson["isdavimoKiekis"] = knyga.IsdavimoKiekis;

				auto &arr = res["populiariausiosKnygos"]["d30"];
				arr[arr.size()] = std::move(knygaJson);
			}

			// Populiariausios knygos 180 dienu
			for (const auto &knyga : stats.PopuliariausiosKnygos.D180) {
				crow::json::wvalue knygaJson;
				knygaJson["pavadinimas"] = knyga.Pavadinimas;
				knygaJson["id"] = knyga.Id;
				knygaJson["isdavimoKiekis"] = knyga.IsdavimoKiekis;

				auto &arr = res["populiariausiosKnygos"]["d180"];
				arr[arr.size()] = std::move(knygaJson);
			}

			for (const auto &v : stats.Aktyviausiu.D30) {
				crow::json::wvalue j;
				j["username"] = v.Username;
				j["id"] = v.Id;
				j["isdavimuKiekis"] = v.IsdavimuKiekis;

				auto &arr = res["aktyviausiVartotojai"]["d30"];
				arr[arr.size()] = std::move(j);
			}

			for (const auto &v : stats.Aktyviausiu.D180) {
				crow::json::wvalue j;
				j["username"] = v.Username;
				j["id"] = v.Id;
				j["isdavimuKiekis"] = v.IsdavimuKiekis;

				auto &arr = res["aktyviausiVartotojai"]["d180"];
				arr[arr.size()] = std::move(j);
			}

			return crow::response(200, res);
		});

	// Sukurti knyga
	CROW_ROUTE(app, "/admin/kurtiKnyga")
		.methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);

			if (!mw::IsAdmin(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas arba pasibaigęs tokenas"}});
			}

			crow::json::rvalue body;
			try {
				body = crow::json::load(req.body);
			} catch (...) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas JSON formatas"}});
			}

			if (!body) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas JSON formatas"}});
			}

			// reikia isbn string, pavadinimas string, autoriai jsonb
			if (!body.has("isbn") || !body.has("pavadinimas") || !body.has("autoriai")) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Truksta butinu lauku: isbn, pavadinimas, autoriai"}});
			}

			std::string isbn = body["isbn"].s();
			std::string pavadinimas = body["pavadinimas"].s();
			std::string autoriai = body["autoriai"].s();

			try {
				db->executeSql(
					"INSERT INTO knyga (isbn, pavadinimas, autoriai) VALUES ($1::TEXT, $2::TEXT, $3::JSONB);",
					pqxx::params{isbn, pavadinimas, autoriai});
			} catch (const std::exception &e) {
				return crow::response(500, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Klaida kuriant knyga"}});
			}

			return crow::response(200, crow::json::wvalue{
										   {"ok", true}});
		});

	// Sukurti egzemplioriu
	CROW_ROUTE(app, "/admin/kurtiEgzemplioriu")
		.methods(crow::HTTPMethod::POST)([&](const crow::request &req) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);
			if (!mw::IsAdmin(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas arba pasibaigęs tokenas"}});
			}

			crow::json::rvalue body;
			try {
				body = crow::json::load(req.body);
			} catch (...) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas JSON formatas"}});
			}

			if (!body) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Neteisingas JSON formatas"}});
			}

			// reikia knygos_id
			if (!body.has("knygos_id")) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Truksta butino lauko: knygos_id"}});
			}

			std::string knygosId = body["knygos_id"].s();

			try {
				db->executeSql(
					"INSERT INTO egzempliorius (knygos_id) VALUES ($1::UUID);",
					pqxx::params{knygosId});

			} catch (const std::exception &e) {
				return crow::response(500, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Klaida kuriant egzemplioriu"}});
			}

			return crow::response(200, crow::json::wvalue{
										   {"ok", true}});
		});
}

#endif // UB_API_ROUTES_ADMIN_HPP