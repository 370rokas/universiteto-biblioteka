#include <iostream>
#include <string>
#include <ub/api.hpp>

int main() {
	std::string url;
	std::string user;
	std::string pass;

	std::cout << "Enter URL: ";
	std::getline(std::cin, url);

	ub::Connection api(url);
	auto r = api.getApiInfo();

	std::cout << "API Name: " << r.serverName << std::endl;
	return 0;
}