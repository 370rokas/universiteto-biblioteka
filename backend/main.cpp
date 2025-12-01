#include "api/api.hpp"
#include "database/db.hpp"
#include "utils/config.hpp"

#include <atomic>
#include <csignal>

std::atomic<bool> running(true);

void signaluHandleris(int signal) {
	logger::get()->info("Gautas signalas {}. Išjungiama...", signal);
	running = false;
}

int main() {
	config::load("config.json");

	// Setupinam signalu handlerius
	std::signal(SIGINT, signaluHandleris);
	std::signal(SIGTERM, signaluHandleris);

	// Paleidziam DB ir API
	dbGlobalus = std::make_shared<Database>(config::get()["databaseUrl"]);
	// Database db(config::get()["databaseUrl"]);
	WebApi api(config::get()["apiPort"], dbGlobalus.get());
	api.run();

	while (running) {
		std::this_thread::sleep_for(std::chrono::seconds(1));

		if (!api.isRunning()) {
			logger::get()->warn("API nustojo veikti. Išjungiama.");
			running = false;
		}
	}

	api.stop();

	return 0;
}