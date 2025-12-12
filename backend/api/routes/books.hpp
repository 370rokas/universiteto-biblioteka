#ifndef UB_API_ROUTES_BOOKS_HPP
#define UB_API_ROUTES_BOOKS_HPP

#include "api/middlewares.hpp"
#include "database/db.hpp"
#include "valdymas/isdavimas.hpp"

inline void setupBooksRoutes(crow::App<UB_CROW_MIDDLEWARES> &app, Database *db) {

	// Knygų paieška
	CROW_ROUTE(app, "/books/search")
		.methods(crow::HTTPMethod::GET)([&app, db](const crow::request &req) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);
			if (!mw::IsLoggedIn(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Unauthorized"}});
			}

			auto queryParam = req.url_params.get("q");
			if (!queryParam) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Truksta 'q' url parametro"}});
			}

			std::string query = queryParam;

			// Aptvarkom
			if (query.empty()) {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "'q' parametras negali būti tuščias"}});
			} else if (query.length() > 100) {
				query = query.substr(0, 100);
			}

			auto books = db->searchBooks(query);

			auto booksJson = crow::json::wvalue::list();

			for (const auto &book : books) {
				booksJson.push_back(crow::json::wvalue{
					{"id", book.id},
					{"isbn", book.isbn},
					{"pavadinimas", book.pavadinimas},
					{"autoriai", book.autoriai},
					{"zanras", book.zanras},
					{"leidykla", book.leidykla},
					{"leidimo_metai", book.leidimo_metai},
					{"kaina", book.kaina}});
			}

			return crow::response(200, crow::json::wvalue{
										   {"ok", true},
										   {"books", booksJson}});
		});

	// Gauti knyga pagal ID
	CROW_ROUTE(app, "/books/<string>")
		.methods(crow::HTTPMethod::GET)([&app, db](const crow::request &req, const std::string &bookId) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);
			if (!mw::IsLoggedIn(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Unauthorized"}});
			}

			auto bookOpt = db->getBookById(bookId);
			if (!bookOpt.has_value()) {
				return crow::response(404, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Knyga nerasta"}});
			}

			const auto &book = bookOpt.value();
			return crow::response(200, crow::json::wvalue{
										   {"ok", true},
										   {"book", crow::json::wvalue{
														{"id", book.id},
														{"isbn", book.isbn},
														{"pavadinimas", book.pavadinimas},
														{"autoriai", book.autoriai},
														{"zanras", book.zanras},
														{"leidykla", book.leidykla},
														{"leidimo_metai", book.leidimo_metai},
														{"kaina", book.kaina}}}});
		});

	// Gauti egzempliorius pagal knygos ID
	CROW_ROUTE(app, "/books/<string>/egzemplioriai")
		.methods(crow::HTTPMethod::GET)([&app, db](const crow::request &req, const std::string &egzId) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);
			if (!mw::IsLoggedIn(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Unauthorized"}});
			}

			auto egzemplioriai = db->getEgzemplioriaiByBookId(egzId);

			auto egzJson = crow::json::wvalue::list();

			for (const auto &egz : egzemplioriai.value()) {
				egzJson.push_back(crow::json::wvalue{
					{"id", egz.id},
					{"knygos_id", egz.knygos_id},
					{"statusas", egz.statusas},
					{"bukle", egz.bukle},
					{"isigyta", egz.isigyta}});
			}

			return crow::response(200, crow::json::wvalue{
										   {"ok", true},
										   {"egzemplioriai", egzJson}});
		});

	// Egzemplioriaus info pagal id
	CROW_ROUTE(app, "/books/egzempliorius/<string>")
		.methods(crow::HTTPMethod::GET)([&app, db](const crow::request &req, const std::string &egzId) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);
			if (!mw::IsLoggedIn(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Unauthorized"}});
			}

			auto egzOpt = db->getEgzemplioriusById(egzId);

			if (!egzOpt.has_value()) {
				return crow::response(404, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Egzempliorius nerastas"}});
			}

			const auto &egz = egzOpt.value();
			return crow::response(200, crow::json::wvalue{
										   {"ok", true},
										   {"egzempliorius", crow::json::wvalue{
																 {"id", egz.id},
																 {"knygos_id", egz.knygos_id},
																 {"statusas", egz.statusas},
																 {"bukle", egz.bukle},
																 {"isigyta", egz.isigyta}}}});
		});

	// Pasiskolinti egzempliorių
	CROW_ROUTE(app, "/books/egzempliorius/<string>/borrow")
		.methods(crow::HTTPMethod::POST)([&app, db](const crow::request &req, const std::string &egzId) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);
			if (!mw::IsLoggedIn(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Unauthorized"}});
			}

			auto userId = ctx.tokenData.userId;

			auto resp = valdymas::isduotiEgzemplioriu(egzId, userId);

			switch (resp) {
			case IsdavimoStatusas::SEKMINGAI:
				return crow::response(200, crow::json::wvalue{
											   {"ok", true},
											   {"message", "Egzempliorius sėkmingai pasiskolintas"}});
			case IsdavimoStatusas::EGZEMPLIORIUS_NERASTAS:
				return crow::response(404, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Egzempliorius nerastas"}});
			case IsdavimoStatusas::JAU_ISDUOTAS:
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Egzempliorius yra jau išduotas"}});
			default:
				return crow::response(500, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Įvyko serverio klaida"}});
			};
		});

	// Grąžinti egzempliorių
	CROW_ROUTE(app, "/books/egzempliorius/<string>/return")
		.methods(crow::HTTPMethod::POST)([&app, db](const crow::request &req, const std::string &skolosId) {
			auto &ctx = app.get_context<mw::TokenAuth>(req);
			if (!mw::IsLoggedIn(ctx)) {
				return crow::response(401, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Unauthorized"}});
			}

			auto userId = ctx.tokenData.userId;
			auto pavyko = valdymas::grazintiEgzemplioriu(skolosId, userId);

			if (pavyko) {
				return crow::response(200, crow::json::wvalue{
											   {"ok", true},
											   {"message", "Egzempliorius sėkmingai grąžintas"}});
			} else {
				return crow::response(400, crow::json::wvalue{
											   {"ok", false},
											   {"message", "Egzemplioriaus grąžinimas nepavyko"}});
			}
		});
}

#endif // UB_API_ROUTES_BOOKS_HPP