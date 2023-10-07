/**
 * @file FileAccess.h
 * @author Gustice
 * @brief Module to access stored files on partitions
 * @version 0.1
 * @date 2021-07-16
 *
 * @copyright Copyright (c) 2021
 */
#pragma once

#include "esp_spiffs.h"
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>

/**
 * @brief Exceptions while handling data on spiffs
 */
class FileSystemException : public std::exception {
  private:
    // Message
    const std::string _message;
    // File path as appendix
    const std::string _path;

  public:
    /// @brief Construct a new File-System-Exception object
    FileSystemException(const std::string msg, const std::string path)
        : _message(msg), _path(path){};

    /// @brief Getter for exception message
    virtual const char *what() const throw() {
        std::string msg(_message);
        msg += " Path: " + _path;
        return msg.c_str();
    }
};

/**
 * @brief Class for accessing files on a given partition
 */
class FileAccess {
  public:
    /// @brief Name of mounting point
    const std::string MountingPoint;

    /// @brief Construct a new File-Access object
    /// @param rootPath Root to partition, use "/spiffs" if partition not labeled
    /// @param label Label of partition (Name column in partition.csv) can be used to distinguish between several spiffs formatted partitions
    FileAccess(const std::string &rootPath, const std::string &label);

    /// @brief Destroy the File-Access object and closes connection to partition
    /// @note: This unmounts spiffs access
    ~FileAccess();

    /// @brief Writes byte-stream to file
    /// @param fileName File name (without path)
    /// @param stream string to write
    void writeFile(const std::string fileName, const std::string stream);

    /// @brief Reads file and copies content to byte-stream
    /// @param fileName File name (without path)
    /// @param maxBuffer Maximum size to read
    /// @return string read stream
    std::string readFile(const std::string fileName, size_t maxBuffer = SIZE_MAX);

    /// @brief Reads file and copies content to byte-stream
    /// @param fileName File name (without path)
    /// @return esp_err_t ESP_OK if successful
    esp_err_t checkIfFileExists(const std::string fileName);

    /// @brief Deletes file
    /// @param fileName File name (without path)
    /// @return esp_err_t ESP_OK if successful
    esp_err_t deleteFile(const std::string fileName);

    /// @brief Get file size
    /// @param fileName Filename (without path)
    /// @return size_t File size in bytes
    size_t getFileSize(const std::string fileName);

  private:
    const std::string _rootPath;
    const std::string _label;
    bool _active = false;

    void throwIfNotInitiated(const std::string fileName);

    std::string getRootedFileName(const std::string file);
    size_t evaluateFileSize(std::fstream &file, std::string path);
};
