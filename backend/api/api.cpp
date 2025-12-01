#include "api/api.hpp"

#include "api/routes/auth.hpp"
#include "api/routes/books.hpp"
#include "api/routes/user.hpp"

void WebApi::run() {
	crow::logger::setHandler(&_logger);

	CROW_ROUTE(_app, "/")
	([&](const crow::request &req) {
		return crow::json::wvalue{
			{"ok", true},
			{"serverName", _serverName}};
	});

	setupAuthRoutes(_app, _db);
	setupUserRoutes(_app, _db);
	setupBooksRoutes(_app, _db);

	_app.loglevel(crow::LogLevel::WARNING);
	_app.signal_clear();
	_appThread = _app.port(_port)
					 .server_name(_serverName)
					 .multithreaded()
					 .run_async();

	logger::get()->info("API veikia ant port'o {}", _port);
};

void WebApi::stop() {
	_app.stop();

	if (_appThread.valid()) {
		_appThread.wait();
	}

	logger::get()->info("API išjungtas");
};

void CustomLoggeris::log(const std::string &message, crow::LogLevel level) {
	switch (level) {
	case crow::LogLevel::Debug:
		logger::get()->debug("[API] " + message);
		break;
	case crow::LogLevel::Info:
		logger::get()->info("[API] " + message);
		break;
	case crow::LogLevel::Warning:
		logger::get()->warn("[API] " + message);
		break;
	case crow::LogLevel::Error:
		logger::get()->error("[API] " + message);
		break;
	default:
		logger::get()->info("[API] " + message);
		break;
	}
}