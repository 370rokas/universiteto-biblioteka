#ifndef BACKEND_API_API_HPP
#define BACKEND_API_API_HPP

#include "api/middlewares.hpp"
#include "database/db.hpp"
#include "utils/config.hpp"

class CustomLoggeris : public crow::ILogHandler {
  public:
	CustomLoggeris() {}
	void log(const std::string &message, crow::LogLevel level);
};

class WebApi {
  private:
	Database *_db;
	crow::App<UB_CROW_MIDDLEWARES> _app;
	std::future<void> _appThread;

	CustomLoggeris _logger;

	std::string _serverName;

	const int _port;

  public:
	WebApi(int port, Database *db) : _port(port), _db(db) {
		_serverName = config::get()["serverName"].get<std::string>();
	};
	~WebApi() {};

	bool isRunning() {
		return _appThread.valid();
	};

	void run();
	void stop();
};

#endif // BACKEND_API_API_HPP