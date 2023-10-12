/**
 * @file webWrapper.hpp
 * @author Gustice
 * @brief Wrapping Http-client 
 * @date 2023-10-11
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "RapidJson.hpp"
#include "configModel.hpp"
#include "esp_log.h"
#include "esp_vfs.h"
#include "fileAccess.hpp"
#include <algorithm>
#include <array>
#include <esp_http_server.h>
#include <exception>
#include <fcntl.h>
#include <functional>
#include <memory>
#include <sstream>
#include <stdint.h>
#include <string>
#include <string_view>
#include <vector>

constexpr std::string_view welcomePageRoute{"mainPage.html"};
constexpr int ScratchBufferSize = 10240;

/**
 * @brief Exceptions while handling data on spiffs
 */
class WebException : public std::exception {
  public:
    /// @brief Construct a new Web-Exception object
    /// @param msg message
    /// @param path api path
    WebException(const std::string msg, const std::string path)
        : _message(msg), _path(path){};

    /// @brief Getter for exception message
    virtual const char *what() const throw() {
        std::string msg(_message);
        msg += " Request: " + _path;
        return msg.c_str();
    }

  private:
    const std::string _message;
    const std::string _path;
};

/**
 * @brief Key / Value pair for file-extension and http-tag
 */
struct FileExtensionTag {
    /// @brief File-extension
    const std::string extension;

    /// @brief Http-tag
    const std::string httpTag;
};

/**
 * @brief Table to assign File endings to HTTP-tags
 */
static const std::vector<FileExtensionTag> HttpFileTags = {
    {".html", "text/html"},
    {".js", "application/javascript"},
    {".css", "text/css"},
    {".png", "image/png"},
    {".ico", "image/x-icon"},
    {".svg", "text/xml"},
};

/// @brief Tag for log messages
const char *WebTag = "API";

/**
 * @brief Web-API controller for DMX-values
 */
class WebApi {
  public:
    /// @brief Callback for color change
    using ColorCallback = void (*)(AmbientType type, Color color);
    /// @brief Callback for intensity change
    using IntensityCallback = void (*)(StageIntensity intensities);

    /// @brief Configuration object for class
    struct Stage {
        FileAccess &filesSystem;
        AmbientColorSet &ambientSet;
        StageIntensity &intensities;
        ColorCallback colorCb = nullptr;
        IntensityCallback intensityCb = nullptr;
    };

    /// @brief Constructor
    /// @details This instances connects itself to ip-driver no further actions needed
    /// @param stage Web configuration
    WebApi(Stage stage)
        : _stage(stage), _webFs(stage.filesSystem) {
        ESP_LOGI(WebTag, "Initialization of web interface ...");

        assert(_webFs.MountingPoint.size() <= ESP_VFS_PATH_MAX && "Limit for base path length");
        server = startWebserver();
    }

    ~WebApi() = default;

  private:
    struct httpd_postUri_t {
        std::string_view uri;
        std::function<std::string(std::string)> pProcessPost;
    };

    struct httpd_getUri_t {
        std::string_view uri;
        std::function<std::string(std::string)> pProcessGet;
    };

    std::unique_ptr<std::array<char, ScratchBufferSize>> scratch{std::make_unique<std::array<char, ScratchBufferSize>>()};
    httpd_handle_t server = nullptr;
    Stage _stage;
    FileAccess &_webFs;

    std::vector<httpd_getUri_t> getValueHandlers{
        {"/api/getIntensity/", [this](std::string in) { return this->processGetIntensity(in); }},
        {"/api/getColor/", [this](std::string in) { return this->processGetColor(in); }},
        {"/api/getConfig/", [this](std::string in) { return this->processGetDeviceConfig(in); }},
        {"/api/getType/", [this](std::string in) { return this->processGetDeviceType(in); }},
    };

    std::vector<httpd_postUri_t> postUriHandlers{
        {"/api/setIntensity", [this](std::string in) { return this->processSetIntensity(in); }},
        {"/api/setColor", [this](std::string in) { return this->processSetColor(in); }},
        {"/api/setConfig", [this](std::string in) { return this->processSetDeviceConfig(in); }},
    };

    const httpd_uri_t setPortReq{
        .uri = "/api/*", .method = HTTP_POST, .handler = data_post_handler, .user_ctx = this};

    const httpd_uri_t getPortReq = {
        .uri = "/api/*", .method = HTTP_GET, .handler = data_get_handler, .user_ctx = this};

    httpd_uri_t pagePart = {
        .uri = "/*", .method = HTTP_GET, .handler = pagePart_GetHandler, .user_ctx = this};

    httpd_handle_t startWebserver() {
        httpd_handle_t server = nullptr;
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.uri_match_fn = httpd_uri_match_wildcard;
        config.lru_purge_enable = true;
        config.max_open_sockets = 4;

        // Start the httpd server
        ESP_LOGI(WebTag, "Starting server on port: %d", config.server_port);
        if (httpd_start(&server, &config) == ESP_OK) {
            // Set URI handlers
            ESP_LOGI(WebTag, "Registering URI handlers");
            httpd_register_uri_handler(server, &setPortReq);
            httpd_register_uri_handler(server, &getPortReq);
            httpd_register_uri_handler(server, &pagePart);
            return server;
        }

        ESP_LOGE(WebTag, "Error starting server!");
        return NULL;
    }

    /// @brief Handler for HTTP-Get-operations to API-Endpoints
    static esp_err_t data_get_handler(httpd_req_t *req) {
        try {
            WebApi &api = *static_cast<WebApi *>(req->user_ctx);
            std::string uri(req->uri);
            ESP_LOGI(WebTag, "Requested Uri: %s", uri.c_str());

            auto matchUri = [uri](const httpd_getUri_t &h) -> bool {
                return uri.find(h.uri, 0) != std::string::npos;
            };
            auto ref = std::find_if(api.getValueHandlers.begin(), api.getValueHandlers.end(), matchUri);
            if (ref == api.getValueHandlers.end()) {
                httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Source does not exist");
                return ESP_FAIL;
            }
            std::string request = uri.substr(ref->uri.length());
            std::string content = ReadContentIntoBufferThrowIfSizeToLarge(req, 256);

            ESP_LOGI(WebTag, "=== RECEIVED DATA ===\n\r Raw: \n\r%s\n\r====================================", request.c_str());
            std::string result = ref->pProcessGet(request);

            httpd_resp_set_type(req, "application/json");
            httpd_resp_sendstr(req, result.c_str());

            ESP_LOGI(WebTag, "=== SENDING DATA ===\n\r Raw: \n\r%s\n\r====================================", result.c_str());
            return ESP_OK;
        } catch (const WebException &e) {
            ESP_LOGE(WebTag, "Problem occurred: %s", e.what());
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, e.what());
        } catch (const char *msg) {
            ESP_LOGE(WebTag, "Error ocurred: %s", msg);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
                                "Something went wrong during processing");
        }
        return ESP_FAIL;
    }

    /// @brief Handler for HTTP-Post-operations to API-Endpoints
    static esp_err_t data_post_handler(httpd_req_t *req) {
        try {
            WebApi &api = *static_cast<WebApi *>(req->user_ctx);
            std::string uri(req->uri);
            ESP_LOGI(WebTag, "Posted Uri: %s", uri.c_str());

            auto matchUri = [uri](const httpd_postUri_t &h) -> bool {
                return uri.find(h.uri, 0) != std::string::npos;
            };
            auto ref = std::find_if(api.postUriHandlers.begin(), api.postUriHandlers.end(), matchUri);
            if (ref == api.postUriHandlers.end()) {
                httpd_resp_send_err(req, HTTPD_404_NOT_FOUND, "Source does not exist");
                return ESP_FAIL;
            }
            std::string request = uri.substr(ref->uri.length());
            std::string content = ReadContentIntoBufferThrowIfSizeToLarge(req, 256);

            ESP_LOGI(WebTag, "=== RECEIVED DATA ===\n\r Req: %s\n\r Buf: %s\n\r====================================", request.c_str(), content.c_str());

            /* Executing registered Handler-Function */
            (void)ref->pProcessPost(content);

            httpd_resp_sendstr(req, "Post control value successfully");
            httpd_resp_send_chunk(req, NULL, 0);
            return ESP_OK;

        } catch (const WebException &e) {
            ESP_LOGE(WebTag, "Problem occurred: %s", e.what());
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, e.what());
        } catch (const char *msg) {
            ESP_LOGE(WebTag, "Error ocurred: %s", msg);
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Something went wrong during processing");
        }
        return ESP_FAIL;
    }

    /// @brief Handler for HTTP-Get-operations to files (HTML, JS, CSS)
    static esp_err_t pagePart_GetHandler(httpd_req_t *req) {
        try {
            WebApi &api = *static_cast<WebApi *>(req->user_ctx);
            std::string uri(req->uri);
            std::string filePath;
            filePath.reserve(ESP_VFS_PATH_MAX + 128);

            if (uri[0] != '/') {
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
                                    "Wrong URI format. Missing leading '/'");
                return ESP_FAIL;
            }
            std::string reqUri = uri.substr(1); // Ignore first character

            if (reqUri.length() == 0 || reqUri.back() == '/') // no URI or Ends with '/'
                filePath.append(welcomePageRoute);
            else
                filePath.append(reqUri);

            // Check if Route misses .html-Ending -> Append .html
            ESP_LOGI(WebTag, "Requesting %s", filePath.c_str());
            if (filePath.find('.') ==
                std::string::npos) {
                filePath.append(".html");
            }

            // Check if icon is requested -> Change to png
            std::string::size_type iconPath = filePath.find("favicon.ico");
            if (iconPath != std::string::npos)
                filePath.replace(iconPath, strlen("favicon.ico"), "favicon.png");

            if (filePath.length() >= filePath.capacity()) {
                ESP_LOGE(WebTag, "Requested file '%s' from uri '%s' too long. (max=%d)",
                         filePath.c_str(), req->uri, filePath.capacity());
                httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,
                                    "Requested Filepath too long");
                return ESP_FAIL;
            }

            ESP_LOGI(WebTag, "Opening file '%s'", filePath.c_str());

            auto readerRef = api._webFs.getChunkReader(filePath);
            ChunkedReader &reader = *(readerRef.get());

            auto type = GetTypeAccordingToExtension(filePath);
            httpd_resp_set_type(req, type.c_str());

            ESP_LOGI(WebTag, "Reding file '%s' in chunks", filePath.c_str());

            std::array<char, ScratchBufferSize> &scratch = *(api.scratch);
            ChunkedReader::ReadStatus_t res{0, false};
            do {
                res = reader.readChunk(scratch.begin(), ScratchBufferSize);
                if (res.ReadBytes > 0) {
                    if (httpd_resp_send_chunk(req, scratch.begin(), res.ReadBytes) != ESP_OK) {
                        httpd_resp_sendstr_chunk(req, NULL); // Abort sending
                        throw("File sending failed!");
                    }
                }
            } while (!res.EndOfFile);

            ESP_LOGI(WebTag, "File sending complete");
            /* Respond with an empty chunk to signal HTTP response completion */
            httpd_resp_send_chunk(req, NULL, 0);
            return ESP_OK;
        } catch (const FileSystemException &e) {
            ESP_LOGE(WebTag, "Problem occurred: %s", e.what());
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, e.what());
        } catch (const char *msg) {
            ESP_LOGE(WebTag, "Error ocurred: %s", msg);
        }
        return ESP_FAIL;
    }

    // Demo Data
    // 127.0.0.1 - - [12/Oct/2023 23:22:56] "POST /api/setIntensity HTTP/1.1" 200 -
    //    Payload {"I":100,"A":200}
    std::string processSetIntensity(const std::string input) {
        rapidjson::Document object;
        if (object.Parse(input.c_str()).HasParseError()) {
            ESP_LOGW(WebTag, "Parsing error '%s' with code %d", input.c_str(), object.GetParseError());
            throw WebException("Parsing error of input", input);
        }

        uint8_t i = object["I"].GetInt();
        uint8_t a = object["A"].GetInt();

        StageIntensity value{i, a};

        if (_stage.intensityCb == nullptr) {
            throw WebException("Internal error", input);
        }
        _stage.intensityCb(value);

        return "{}";
    }

    // Demo Data
    // 127.0.0.1 - - [12/Oct/2023 23:22:57] "POST /api/setColor HTTP/1.1" 200 -
    //         Payload {"T":"foreground","R":0,"G":0,"B":0}
    // 127.0.0.1 - - [12/Oct/2023 23:22:57] "POST /api/setColor HTTP/1.1" 200 -
    //         Payload {"T":"background","R":0,"G":0,"B":0}
    std::string processSetColor(const std::string input) {
        rapidjson::Document object;
        if (object.Parse(input.c_str()).HasParseError()) {
            ESP_LOGW(WebTag, "Parsing error '%s' with code %d", input.c_str(), object.GetParseError());
            throw WebException("Parsing error of input", "processSetIntensity");
        }

        std::string type = object["T"].GetString();
        uint8_t r = object["R"].GetInt();
        uint8_t g = object["G"].GetInt();
        uint8_t b = object["B"].GetInt();

        Color value{r, g, b};

        if (_stage.colorCb == nullptr) {
            throw WebException("Internal error", input);
        }

        if (type == "FG") {
            _stage.colorCb(AmbientType::Foreground, value);
        } else if (type == "BG") {
            _stage.colorCb(AmbientType::Background, value);
        } else {
            throw WebException("Unknown Ambient set ", type);
        }

        return "{}";
    }

    std::string processSetDeviceConfig(const std::string input) {
        rapidjson::Document config;
        if (config.Parse(input.c_str()).HasParseError()) {
            ESP_LOGW(WebTag, "Parsing error with code %d", static_cast<int>(config.GetParseError()));
            throw WebException("Parsing error of input", "processSetIntensity");
        }

        ESP_LOGI(WebTag, "Writing Content to File %s: \n%s", DeviceConfigFilename.begin(), input.c_str());
        _webFs.writeFile(DeviceConfigFilename.begin(), input);
        return "{}";
    }

    // Demo Data
    // Evaluation request path '/api/getIntensity'
    // => Responding: '{"I": 100, "A": 200}'
    // 127.0.0.1 - - [12/Oct/2023 23:56:30] "GET /api/getIntensity HTTP/1.1" 200 -
    std::string processGetIntensity(const std::string input) {
        StageIntensity value = _stage.intensities;

        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);

        writer.StartObject();
        writer.Key("I");
        writer.Uint(value.illumination);
        writer.Key("A");
        writer.Uint(value.ambiente);
        writer.EndObject();

        return s.GetString();
    }

    // Payload "foreground" or "background"
    // Demo Data
    // Evaluation request path '/api/getColor/foreground'
    // => Responding: '{"R": 210, "G": 220, "B": 230}'
    // 127.0.0.1 - - [12/Oct/2023 23:22:33] "GET /api/getColor/foreground HTTP/1.1" 200 -
    // Evaluation request path '/api/getColor/background'
    // => Responding: '{"R": 110, "G": 120, "B": 130}'
    // 127.0.0.1 - - [12/Oct/2023 23:22:33] "GET /api/getColor/background HTTP/1.1" 200 -
    std::string processGetColor(const std::string input) {
        Color value;
        if (input == "foreground") {
            value = _stage.ambientSet.foregroundColor;
        } else if (input == "background") {
            value = _stage.ambientSet.backgroundColor;
        } else {
            throw WebException("Unknown Ambient set ", input);
        }

        rapidjson::StringBuffer s;
        rapidjson::Writer<rapidjson::StringBuffer> writer(s);

        writer.StartObject();
        writer.Key("R");
        writer.Uint(value.red);
        writer.Key("G");
        writer.Uint(value.green);
        writer.Key("B");
        writer.Uint(value.blue);
        writer.EndObject();

        return s.GetString();
    }

    // Demo Data
    // Evaluation request path '/api/getConfig'
    //     => Responding: '{"dmxOutputs": 24, "illuminationWeights": [0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 255, 127, 0, 0, 0, 255, 0, 255, 0, 255, 255, 255, 255], "ambientChannels": {"foreground": [1, 2, 3], "background": [6, 7, 8]}, "colorPresets": [{"foreground": [255, 0, 0], "background": [0, 255, 0]}, {"foreground": [0, 255, 0], "background": [0, 0, 255]}, {"foreground": [0, 0, 255], "background": [255, 0, 0]}]}'
    // 127.0.0.1 - - [12/Oct/2023 23:56:30] "GET /api/getConfig HTTP/1.1" 200 -
    std::string processGetDeviceConfig(const std::string input) {
        auto output = _webFs.readFile(DeviceConfigFilename.begin());
        return output;
    }

    std::string processGetDeviceType(const std::string input) {
        return "";
    }

    static std::string ReadContentIntoBufferThrowIfSizeToLarge(httpd_req_t *req, size_t maxSize) {
        std::string content;
        content.reserve(maxSize);
        if (req->content_len >= content.capacity())
            throw("content too long for buffer");

        while (content.size() < req->content_len) {
            char buf[content.capacity() + 1];
            auto received = httpd_req_recv(req, buf, req->content_len);
            if (received <= 0)
                throw("failed to read input buffer");

            buf[received] = '\0';
            content.append(buf);
        }

        return content;
    }

    static const std::string &GetTypeAccordingToExtension(const std::string &filepath) {
        static const std::string DefaultTag("text/plain");
        for (const auto &t : HttpFileTags) {
            if (filepath.size() <= t.extension.size())
                continue;

            auto fE = filepath.substr(filepath.size() - t.extension.size());
            if (fE == t.extension)
                return t.httpTag;
        }
        return DefaultTag;
    }
};
