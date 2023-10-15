#include "FileAccess.hpp"
#include "esp_log.h"
#include <exception>
#include <iterator>
#include <sstream>
#include <stdint.h>
#include <vector>

using std::string;

/// Tag for logging
static const char *LogTag = "FS";

static esp_vfs_spiffs_conf_t MakeConfig(const char *base, const char *label) {
    esp_vfs_spiffs_conf_t config = {.base_path = base,
                                    .partition_label = label,
                                    .max_files = 4,
                                    .format_if_mount_failed = false};
    return config;
}

FileAccess::FileAccess(const string &rootPath, const string &label)
    : MountingPoint(rootPath), _rootPath(rootPath), _label(label) {
    esp_vfs_spiffs_conf_t _config = MakeConfig(rootPath.c_str(), label.c_str());
    ESP_LOGI(LogTag, "Initializing SPIFFS partition '%s' on root '%s'", label.c_str(),
             rootPath.c_str());
    esp_err_t ret = esp_vfs_spiffs_register(&_config);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(LogTag, "Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            ESP_LOGE(LogTag, "Failed to find SPIFFS partition");
        } else {
            ESP_LOGE(LogTag, "Failed to initialize SPIFFS (%s)", esp_err_to_name(ret));
        }
        return;
    }
    _active = true;

    size_t total = 0, used = 0;
    ret = esp_spiffs_info(label.c_str(), &total, &used);
    if (ret != ESP_OK) {
        ESP_LOGE(LogTag, "Failed to get SPIFFS partition information (%s)", esp_err_to_name(ret));
    } else {
        ESP_LOGI(LogTag, "Partition size: total: %d, used: %d", total, used);
    }
}

FileAccess::~FileAccess() {
    esp_vfs_spiffs_unregister(_label.c_str());
    ESP_LOGI(LogTag, "SPIFFS unmounted");
}

string FileAccess::getRootedFileName(const string file) {
    string path(_rootPath);
    path += "/" + file;
    return path;
}

void FileAccess::throwIfNotInitiated(const string fileName) {
    if (!_active) {
        ESP_LOGE(LogTag, "Setup not finished. File '%s' File cannot opened", fileName.c_str());
        throw FileSystemException("Setup not finished. File cannot opened", fileName);
    }
}

void FileAccess::writeFile(const string fileName, const string stream) {
    throwIfNotInitiated(fileName);
    string path = getRootedFileName(fileName);

    std::fstream file(path, std::ios::out);
    if (!file.is_open())
        throw FileSystemException("Failed to create/open file", fileName);

    ESP_LOGI(LogTag, "Write stream (with %d bytes) to %s", stream.size(), path.c_str());
    file << stream;
}

size_t FileAccess::getFileSize(const string fileName) {
    throwIfNotInitiated(fileName);

    string path = getRootedFileName(fileName);
    std::fstream file(path, std::ios::in);
    if (!file.is_open())
        throw FileSystemException("Failed to create/open file", fileName);

    return evaluateFileSize(file, path);
}

size_t FileAccess::evaluateFileSize(std::fstream &file, string path) {
    auto fPos = file.tellg();
    file.seekg(0, std::ios::end);
    auto size = file.tellg() - fPos;
    if (size < 0)
        throw FileSystemException("Invalid stream size", path);
    file.seekg(fPos);

    return size;
}

string FileAccess::readFile(const string fileName, size_t maxBuffer) {
    throwIfNotInitiated(fileName);

    string path = getRootedFileName(fileName);
    std::fstream file(path, std::ios::in);
    auto size = evaluateFileSize(file, path);

    ESP_LOGI(LogTag, "Read stream (with %d bytes) from %s", (size_t)size, path.c_str());

    if (maxBuffer != SIZE_MAX && size > maxBuffer) {
        std::vector<char> buffer(maxBuffer, 0);
        file.read(buffer.data(), buffer.size());
        string out(buffer.begin(), buffer.end());
        return out;
    } else {
        std::stringstream content;
        content << file.rdbuf();
        return content.str();
    }
}

esp_err_t FileAccess::checkIfFileExists(const string fileName) {
    if (!_active)
        return ESP_FAIL;

    string path = getRootedFileName(fileName);
    std::fstream file(path, std::ios::in);
    if (file.is_open())
        return ESP_OK;

    return ESP_ERR_NOT_FOUND;
}

esp_err_t FileAccess::deleteFile(const string fileName) {
    throwIfNotInitiated(fileName);
    string path = getRootedFileName(fileName);

    if (checkIfFileExists(fileName) == ESP_OK) {
        unlink(path.c_str());
    } else {
        ESP_LOGE(LogTag, "File does not exist");
        return ESP_OK;
    }

    ESP_LOGI(LogTag, "Removing file %s", fileName.c_str());
    if (remove(path.c_str()) != 0) {
        ESP_LOGE(LogTag, "Removing failed");
        return ESP_FAIL;
    }

    return ESP_OK;
}

std::unique_ptr<ChunkedReader> FileAccess::getChunkReader(const string fileName) {
    throwIfNotInitiated(fileName);
    auto path = getRootedFileName(fileName);
    std::unique_ptr<ChunkedReader> reader(new ChunkedReader(path));
    return reader;
}


ChunkedReader::ChunkedReader(const string &path)
    : _file(path, std::ios::in | std::ios::binary) {
    std::fstream &file = _file;
    if (!file.is_open())
        throw FileSystemException("Failed to open file", path);

    auto fsize = file.tellg();
    file.seekg(0, std::ios::end);
    _fileSize = file.tellg() - fsize;
    if (_fileSize < 0)
        throw FileSystemException("Invalid stream size", path);
    file.seekg(0, std::ios::beg);
}

ChunkedReader::ReadStatus_t ChunkedReader::readChunk(char * buffer, int length) {
    _file.read(buffer, length);
    size_t count = _file.gcount();
    return {
        count, 
        _file.eof()
    };
}

size_t ChunkedReader::getFileSize(void) { return _fileSize; }
