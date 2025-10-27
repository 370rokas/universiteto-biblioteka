#ifndef BACKEND_API_API_HPP
#define BACKEND_API_API_HPP

#include "database/db.hpp"

#include "crow.h"

class WebApi {
      private:
	Database *_db;
	crow::App<> _app;

	const int _port;

      public:
	WebApi(int port, Database *db) : _port(port), _db(db) {};
	~WebApi();

	void run() {
		_app.port(_port).multithreaded().run();
		logger::get()->info("API running on port {}", _port);
	};
};

#endif // BACKEND_API_API_HPP