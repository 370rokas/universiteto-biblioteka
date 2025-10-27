#include "config.hpp"

#include <fstream>

nlohmann::json config::configData;

void config::load(const std::string &filename) {
  std::ifstream file(filename, std::ios::in);
  if (file.is_open()) {
    file >> configData;

  } else {
    throw std::runtime_error("Nepavyko atidaryti konfigūracijos failo: " +
                             filename);
  }
}

nlohmann::json &config::get() { return configData; }