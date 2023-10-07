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

/**
 * @brief Exception to notify problems in serialized data
 */
class DeserializeException : public std::exception {
  public:
    /// @brief Overwritten getter for problem
    /// @return problem as string
    const char *what() const noexcept override {
        return _message.c_str();
    }

  /// @brief Constructor
  /// @param message Full message
  DeserializeException(std::string message);

  /// @brief Constructor
  /// @param propertyChain path / breadcrumbs to problematic property
  /// @param message Description of problem with named property
  DeserializeException(std::vector<std::string> propertyChain, std::string message);

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
