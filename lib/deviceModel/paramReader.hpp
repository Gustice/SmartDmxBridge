/**
 * @file ParamReader.h
 * @author Gustice
 * @brief Module to read device Parameter
 * @version 0.1
 * @date 2021-01-01
 *
 * @copyright Copyright (c) 2021
 */

#pragma once

#include "configModel.hpp"

#include <stdexcept>
#include <sstream>

class DeserializeException : public std::exception {
  public:
    const char *what() const noexcept override {
        return _message.c_str();
    }

  DeserializeException(std::string message) {
    _message = "Deserialize Error: " + message;
  }

  DeserializeException(std::vector<std::string> propertyChain, std::string message) {
    std::stringstream str;
    str << "Deserialize Error in: ";
    str << "'";
    for (auto &&p : propertyChain)
    {
      str << p << ".";
    }
    str << "'";
    str << ":  ";
    str << message;
    
    _message = str.str();
  }

  private:
  std::string _message;
};

/**
 * @brief Reader class for reading and interpreting Device configuration files
 */
class ParamReader {
  public:
    /**
     * @brief Deserializes description
     * @param serialized Serialized description
     * @return StageConfig
     */
    static StageConfig readDeviceConfig(const std::string serialized);

};
