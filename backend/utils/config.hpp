#ifndef UB_UTILS_CONFIG_HPP
#define UB_UTILS_CONFIG_HPP

#include <nlohmann/json.hpp>
#include <string>

class config {
public:
  static void load(const std::string &filename);
  static nlohmann::json &get();

private:
  static nlohmann::json configData;
};

#endif // UB_UTILS_CONFIG_HPP
