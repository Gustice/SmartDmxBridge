#pragma once

#include "esp_log.h"
#include <embedded_cli.h>
#include <functional>
#include <uart.hpp>
#include "dmx_uart.hpp"
#include "configModel.hpp"
#include "otaUpdate.hpp"
#include <functional>
#include <memory>

struct CliOptions {
    struct PrintOut {
        bool DmxValues;
        bool PotiValues;
    };
};

struct MonitorOptions {
    bool infos = false;
    bool standAlone = false;
};

enum MonitorType {
    RingCheck,
    Sensing
};

struct TaskControl {
    bool cancelled;

    void cancel() {cancelled = true;}
};

constexpr static int CLI_RX_BUFFER_SIZE = 16;
constexpr static int CLI_CMD_BUFFER_SIZE = 32;
constexpr static int CLI_HISTORY_SIZE = 16;
constexpr static int CLI_BINDING_COUNT = 6;

extern const std::string_view WelcomeString;

class Cli {
  public:
    using commandCb = void (*)(EmbeddedCli *embeddedCli, CliCommand *command);
    using writeCb = void (*)(EmbeddedCli *embeddedCli, char c);

    Cli(CharStream &port, commandCb cmd) : _port(port) {
        EmbeddedCliConfig *config = embeddedCliDefaultConfig();
        // config->cliBuffer = cliBuffer; // Allocate as much as you need
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
        cli->onCommand = cmd;
        cli->writeChar = writeToken;
        cli->appContext = this;
    }
    ~Cli() {}

    bool addBinding(CliCommandBinding binding) {
        return embeddedCliAddBinding(cli, binding);
    }

    void process() {
        auto r = _port.read();
        for (auto &&c : r) {
            embeddedCliReceiveChar(cli, c);
        }
        embeddedCliProcess(cli);
    }

    CharStream &_port;

  private:
    EmbeddedCli *cli = nullptr;

    static void writeToken(EmbeddedCli *embeddedCli, char c) {
        static_cast<Cli *>(embeddedCli->appContext)->_port.write(c);
    }
};


class Ui {
    public:
    using abortCallback = void(*)();
    using systemCallback = void(*)();
    using startMonitorCallback = void(*)(MonitorType type, std::shared_ptr<TaskControl> token);
    struct Config {
        StageConfig & stage;
        Dmx512 & dmx;
        OtaHandler & ota;
        startMonitorCallback startMonitor;
        systemCallback showHealth;
    };
        
    Ui(CharStream &port, Config &config) : _shell(port, onCommand) {
        _config = &config;
        // _shell.addBinding({"help", "Print Welcome Stream", false, (void *)WelcomeString.begin(), onWelcome});
        _shell.addBinding({"update", "Start OTA-Update-Process", false, nullptr, startUpdate});
        _shell.addBinding({"get-layout", "Get channel info", false, nullptr, getDmxLayout});
        _shell.addBinding(
            {"set-ch", "'c v' Set DMX-Channel c to value v", true, nullptr, setDmxChannel});
        _shell.addBinding(
            {"get-ch", "'c' Get DMX-Channel c (omit c for all)", true, nullptr, getDmxChannel});
        _shell.addBinding(
            {"start-monitor", "program to start ring-monitor or input-monitor", false, nullptr, startMonitor});
        _shell.addBinding(
            {"dump-status", "show system status", false, nullptr, showHealth});
    }

    static void getDmxLayout(EmbeddedCli *cli, char *args, void *context) {
        auto &port = static_cast<Cli *>(cli->appContext)->_port;
        port.write(_config->stage.getStageConfigStr().c_str());
    }

    static void getDmxChannel(EmbeddedCli *cli, char *args, void *context) {
        auto &port = static_cast<Cli *>(cli->appContext)->_port;

        if (embeddedCliGetTokenCount(args) == 0) {
            port.write(_config->dmx.getValues().getValuesStr().c_str());
            return;
        }
        
        auto arg = embeddedCliGetToken(args, 1);
        port.write(_config->dmx.getValues().getValueStr(arg).c_str());
    }

    static void setDmxChannel(EmbeddedCli *cli, char *args, void *context) {
        auto &port = static_cast<Cli *>(cli->appContext)->_port;

        if (embeddedCliGetTokenCount(args) != 2) {
            port.write("Need 2 arguments\n");
            return;
        }
        
        auto ch = std::stoi(embeddedCliGetToken(args, 1));
        auto value = std::stoi(embeddedCliGetToken(args, 2));
        
        value = std::min(value, 0xFF); // todo limit-check-check

        _config->dmx.set(ch-1, value);
        auto output = std::string {" Ch "} + std::to_string(ch) + " = " + std::to_string(value);
        port.write(output + "\n");
    }

    static void startMonitor(EmbeddedCli *cli, char *args, void *context) {
        auto &port = static_cast<Cli *>(cli->appContext)->_port;

        if (embeddedCliGetTokenCount(args) != 2) {
            port.write("Need argument, one of: 'ring' 'input'\n");
            return;
        }
        
        auto type = std::string(embeddedCliGetToken(args, 1));

        if (type == "ring")
        {
            _runningTaskToken = std::make_shared<TaskControl>();
            _config->startMonitor(MonitorType::RingCheck, _runningTaskToken);
        } else if (type == "input") {
            _runningTaskToken = std::make_shared<TaskControl>();
            _config->startMonitor(MonitorType::Sensing, _runningTaskToken);
        } else {
            port.write("Error, unknown type. Must be one of: 'ring' 'input'\n");
        }
        
        port.write("Starting monitor program. Abort with 'c'\n");
    }
    
    static void showHealth(EmbeddedCli *cli, char *args, void *context) {
        auto &port = static_cast<Cli *>(cli->appContext)->_port;
        port.write("Calling system stats output (will be dumped in Log interface)\n");
        _config->showHealth();
    }

    static void onCommand(EmbeddedCli *embeddedCli, CliCommand *command) {
        ESP_LOGI("UI", "Received command: %s", command->name);
        embeddedCliTokenizeArgs(command->args);
        for (int i = 1; i <= embeddedCliGetTokenCount(command->args); ++i) {
            ESP_LOGI("UI", "    arg %d : %s", i, embeddedCliGetToken(command->args, i));
        }

        if(command->name == std::string{"c"}) {
            if (_runningTaskToken.get() != nullptr)
                _runningTaskToken->cancel();
        }
    }

    static void onWelcome(EmbeddedCli *cli, char *args, void *context) {
        ESP_LOGI("UI", "Hello ");
        auto &port = static_cast<Cli *>(cli->appContext)->_port;
        port.write((const char *)context);
        if (embeddedCliGetTokenCount(args) == 0)
            ESP_LOGI("UI", "%s", (const char *)context);
        else
            ESP_LOGI("UI", "%s", embeddedCliGetToken(args, 1));
    }

    static void startUpdate(EmbeddedCli *cli, char *args, void *context) {
        _config->ota.enableUpdateTask();
    }

    void process() {
        _shell.process();
    }

    private:
    static Config * _config;
    Cli _shell;
    static Cli * _rShell;
    static abortCallback _abortCbk ;
    static std::shared_ptr<TaskControl> _runningTaskToken;
};