/**
 * @file cliWrapper.hpp
 * @author Gustice
 * @brief Wrapping Command-Line-Interfaces with custom endpoints
 * @date 2023-09-24
 *
 * @copyright Copyright (c) 2023
 */
#pragma once

#include "configModel.hpp"
#include "dmx_uart.hpp"
#include "esp_log.h"
#include "otaUpdate.hpp"
#include "uart.hpp"
#include <embedded_cli.h>
#include <functional>
#include <list>
#include <memory>

/* Module configuration */
static constexpr int CLI_RX_BUFFER_SIZE = 16;
static constexpr int CLI_CMD_BUFFER_SIZE = 32;
static constexpr int CLI_HISTORY_SIZE = 16;
static constexpr int CLI_BINDING_COUNT = 8;
static constexpr std::string_view CliWelcomeString{
    "** Smart-DMX-Bridge **\n"
    "  send 'help' for help-page\n"
    "  NOTE: Syslogs are sent to client of this session\n"};

/**
 * @brief Descriptive flags for Debugging options. (Supposed to be enabled by CLI-commands)
 */
struct DebuggingOptions {
    bool infos = false;
    bool standAlone = false;
    bool standAloneDiff = false;
};

/**
 * @brief Monitor options for hardware diagnosis
 * @details This station can be used to either diagnose a ring
 *      or another sending device
 */
enum MonitorType {
    /// @brief Send data through ring and check if received data matches sent packages
    RingCheck,
    /// @brief Read incoming data and send diff-packages if data changes
    Sensing
};

/**
 * @brief Controlling child tasks
 */
struct TaskControl {
    bool isCanceled() {
        return _canceled;
    }
    void cancel() {
        _canceled = true;
    }

  private:
    bool _canceled;
};

/**
 * @brief Wrapper for c-implementation of CLI
 */
class Cli {
  public:
    using CArgs = std::vector<std::string> &;
    using BindCb = std::function<void(CArgs)>;
    using CommandCb = std::function<void(std::string, CArgs)>;

    /// @brief Parameter structure for command-binding
    struct Binding {
        std::string_view name;
        std::string_view help_output;
        BindCb callback;
    };

    Cli(CharStream &port, CommandCb cmd) : _port(port), _commandCb(cmd) {
        EmbeddedCliConfig *config = embeddedCliDefaultConfig();
        // NOTE: Note setting cliBuffer allows Cli to allocate needed buffer in correct size
        // config->cliBuffer = cliBuffer;
        // config->cliBufferSize = CLI_BUFFER_SIZE;
        config->rxBufferSize = CLI_RX_BUFFER_SIZE;
        config->cmdBufferSize = CLI_CMD_BUFFER_SIZE;
        config->historyBufferSize = CLI_HISTORY_SIZE;
        config->maxBindingCount = CLI_BINDING_COUNT;
        cli = embeddedCliNew(config);

        if (cli == NULL) {
            ESP_LOGE("CLI", "Cli was not created, requires %d bytes. Check sizes!",
                     embeddedCliRequiredSize(config));
            return;
        }
        cli->onCommand = genericCallback;
        cli->writeChar = writeToken;
        // appContext is a user-end variable and is bound to current instance
        cli->appContext = this;
    }

    ~Cli() {
        embeddedCliFree(cli);
    }

    /// @brief Create command binding
    bool addBinding(Binding bind) {
        BindCb *cb = new BindCb{bind.callback};

        CliCommandBinding cBinding{.name = bind.name.begin(),
                                   .help = bind.help_output.begin(),
                                   .tokenizeArgs = true,
                                   // NOTE: this hard reference only works because we only provide
                                   // an add-function without remove
                                   .context = (void *)(cb),
                                   .binding = boundCallback};
        return embeddedCliAddBinding(cli, cBinding);
    }

    /// @brief Static method to generate c-compatible function-pointer for bound-command-callback
    /// @details - receives and wraps arguments in string,
    ///          - determines object according to appContext
    ///          - calls appropriate method of appropriate object
    static void boundCallback(EmbeddedCli *cli, char *args, void *context) {
        if (context == nullptr) {
            return;
            ESP_LOGE("UI", "No context in callback available");
        }
        ESP_LOGD("UI", "Received command:");

        BindCb &cb = *((BindCb *)context);
        uint len = embeddedCliGetTokenCount(args);
        std::vector<std::string> sArgs;
        while (len-- > 0) {
            const char *const arg = embeddedCliGetToken(args, 1);
            ESP_LOGD("UI", "    arg: %s", arg);
            sArgs.emplace_back(arg);
            // shift position to end of this element to save time
            args += sArgs.back().size() + 1; // needs also to be shifted by null-delimiter
        }
        cb(sArgs);
    }

    /// @brief Static method to generate c-compatible function-pointer for command-callback
    /// @details - receives and wraps arguments in string,
    ///          - determines object according to appContext
    ///          - calls command-callback of appropriate object
    static void genericCallback(EmbeddedCli *cli, CliCommand *command) {
        std::string name(command->name);
        ESP_LOGI("UI", "Received generic command: %s", name.c_str());

        embeddedCliTokenizeArgs(command->args);
        std::vector<std::string> sArgs;
        for (int i = 1; i <= embeddedCliGetTokenCount(command->args); ++i) {
            sArgs.emplace_back(embeddedCliGetToken(command->args, i));
            ESP_LOGI("UI", "    arg %d : %s", i, sArgs.back().c_str());
        }
        auto &commandCb = static_cast<Cli *>(cli->appContext)->_commandCb;
        commandCb(name, sArgs);
    }

    /// @brief Process buffer and trigger commands
    void process() {
        auto r = _port.read();
        for (auto &&c : r) {
            embeddedCliReceiveChar(cli, c);
        }
        embeddedCliProcess(cli);
    }

    /// @brief publicly exposed write method
    void write(std::string output) {
        _port.write(output);
    }

  private:
    /// @brief Reference to hardware port
    CharStream &_port;
    /// @brief Callback for generic command
    CommandCb _commandCb;
    EmbeddedCli *cli = nullptr;

    /// @brief Write callback
    static void writeToken(EmbeddedCli *embeddedCli, char c) {
        static_cast<Cli *>(embeddedCli->appContext)->_port.write(c);
    }
};

/**
 * @brief Class for Command-Line-Interface
 * @details Provides all logical bindings processing and triggers callbacks
 */
class Ui {
  public:
    using abortCallback = void (*)();
    using systemCallback = void (*)();
    using startMonitorCallback = void (*)(MonitorType type, std::shared_ptr<TaskControl> token);
    struct Config {
        StageConfig &stage;
        Dmx512 &dmx;
        OtaHandler &ota;
        startMonitorCallback startMonitor;
        systemCallback showHealth;
        DebuggingOptions &debugOptions;
    };

    Ui(CharStream &port, Config &config)
        : _shell(port, [this](std::string n, Cli::CArgs a) { this->onCommand(n, a); }),
          _config(config) {
        // clang-format off
        _shell.addBinding(
            {"update", "Start OTA-Update-Process", 
            [this](Cli::CArgs a) { this->startUpdate(a);} }
            );
        _shell.addBinding(
            {"get-layout", "Get channel info", 
            [this](Cli::CArgs a) { this->getDmxLayout(a);} }
            );
        _shell.addBinding(
            {"set-ch", "'c v' Set DMX-Channel c to value v",
            [this](Cli::CArgs a) { this->setDmxChannel(a);} }
            );
        _shell.addBinding(
            {"get-ch", "'c' Get DMX-Channel c (omit c for all)",
            [this](Cli::CArgs a) { this->getDmxChannel(a);} }
            );
        _shell.addBinding(
            {"start-monitor", "program to start ring-monitor or input-monitor",
            [this](Cli::CArgs a) { this->startMonitor(a);} }
            );
        _shell.addBinding(
            {"dump-status", "show system status", 
            [this](Cli::CArgs a) { this->showHealth(a);} }
            );
        _shell.addBinding(
            {"set-dbg", "Set debugging options by optionName (empty resets all and displays available options)", 
            [this](Cli::CArgs a) { this->setDebugging(a);} }
            );
        // clang-format on

        _shell.write(CliWelcomeString.begin());
    }

    void getDmxLayout(Cli::CArgs args) {
        _shell.write(_config.stage.getStageConfigStr().c_str());
    }

    void getDmxChannel(Cli::CArgs args) {
        if (args.size() == 0) {
            _shell.write(_config.dmx.getValues().getValuesStr().c_str());
            return;
        }

        _shell.write(_config.dmx.getValues().getValueStr(args[0]).c_str());
    }

    void setDmxChannel(Cli::CArgs args) {
        if (args.size() != 2) {
            _shell.write("Need 2 arguments\n");
            return;
        }

        auto ch = std::stoi(args[0]);
        auto value = std::stoi(args[1]);

        value = std::min(value, 0xFF); // todo limit-check-check

        _config.dmx.set(ch - 1, value);
        auto output = std::string{" Ch "} + std::to_string(ch) + " = " + std::to_string(value);
        _shell.write(output + "\n");
    }

    void startMonitor(Cli::CArgs args) {
        if (args.size() != 1) {
            _shell.write("Need argument, one of: 'ring' 'input'\n");
            return;
        }

        auto type = std::string(args[0]);

        if (type == "ring") {
            _runningTaskToken = std::make_shared<TaskControl>();
            _config.startMonitor(MonitorType::RingCheck, _runningTaskToken);
        } else if (type == "input") {
            _runningTaskToken = std::make_shared<TaskControl>();
            _config.startMonitor(MonitorType::Sensing, _runningTaskToken);
        } else {
            _shell.write("Error, unknown type. Must be one of: 'ring' 'input'\n");
            return;
        }

        _shell.write("Starting monitor program. Abort with 'c'\n");
    }

    void showHealth(Cli::CArgs args) {
        _shell.write("Calling system stats output (will be dumped in Log interface)\n");
        _config.showHealth();
    }

    void setDebugging(Cli::CArgs args) {
        if (args.size() == 0) {
            _shell.write(
                "Need options, one or multiple of: 'infos' 'standAlone' 'standAloneDiff'\n");
            return;
        }
        DebuggingOptions options;
        // @todo: finalize options parsing
        _config.debugOptions = options;
    }

    void onCommand(std::string name, Cli::CArgs args) {
        ESP_LOGI("UI", "Invoking: %s", name.c_str());

        if (name == "c") {
            if (_runningTaskToken.get() != nullptr)
                _runningTaskToken->cancel();
        }
    }

    void startUpdate(Cli::CArgs args) {
        _config.ota.enableUpdateTask();
    }

    /// @brief publicly exposed process method
    void process() {
        _shell.process();
    }

  private:
    Cli _shell;
    Config &_config;
    std::shared_ptr<TaskControl> _runningTaskToken;
};