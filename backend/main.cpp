#include "database/db.hpp"
#include "utils/config.hpp"

#include <iostream>

int main() {
	config::load("config.json");

	logger::get()->info("Starting backend...");

	Database db(config::get()["databaseUrl"]);

	while (true) {
		std::string q;
		std::cin >> q;
		auto books = db.searchBooks(q);
		for (const auto &book : books) {
			std::cout << book.pavadinimas << " by " << book.autoriai
				  << " (ISBN: " << book.isbn << ")\n";
		}
		std::cout << "-----------------------------------\n";
	}

	return 0;
}