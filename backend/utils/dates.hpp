#ifndef UB_BACKEND_UTILS_DATES_HPP
#define UB_BACKEND_UTILS_DATES_HPP

#include <chrono>
#include <string>

namespace utils {

inline std::chrono::system_clock::time_point parseDate(const std::string &dateStr) {
	std::tm tm = {};
	std::istringstream ss(dateStr);
	ss >> std::get_time(&tm, "%Y-%m-%d");
	return std::chrono::system_clock::from_time_t(std::mktime(&tm));
};

inline std::string formatDate(const std::chrono::system_clock::time_point &date) {
	std::time_t time = std::chrono::system_clock::to_time_t(date);
	std::tm *tm = std::localtime(&time);
	char buffer[11];
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d", tm);
	return std::string(buffer);
};

inline int dienosTarpDatu(const std::chrono::system_clock::time_point &nuomaNuo,
						  const std::chrono::system_clock::time_point &nuomaIki) {
	auto duration = std::chrono::duration_cast<std::chrono::hours>(nuomaIki - nuomaNuo);
	return duration.count() / 24;
};

}; // namespace utils

#endif