#include "api/api.hpp"
#include "database/db.hpp"
#include "utils/config.hpp"
#include "valdymas/skolos.hpp"

#include <atomic>
#include <csignal>
#include <cstdint>

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
	WebApi api(config::get()["apiPort"], dbGlobalus.get());
	api.run();

	uint64_t runtime = 0;

	while (running) {
		std::this_thread::sleep_for(std::chrono::seconds(1));
		runtime++;

		if (!api.isRunning()) {
			logger::get()->warn("API nustojo veikti. Išjungiama.");
			running = false;
		}

		// Kas valanda:
		if (runtime % 3600 == 0) {
			valdymas::atnaujintiSkolas();

			runtime = 0;
		}
	}

	api.stop();

	return 0;
}