#pragma once

#include "esp_log.h"
#include <embedded_cli.h>
#include <functional>
#include <uart.hpp>
#include "configModel.hpp"
#include "otaUpdate.hpp"

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
    Ui(Cli &shell, StageConfig & stage, DmxChannels & channels, OtaHandler & ota)  {
        _stage = &stage;
        _channels = &channels;
        _ota = &ota;

        shell.addBinding({"help", "Print Welcome Stream", false, (void *)WelcomeString.begin(), onWelcome});
        shell.addBinding({"update", "Start OTA-Update-Process", false, nullptr, startUpdate});
        shell.addBinding({"get-layout", "Get channel info", false, nullptr, getDmxLayout});
        shell.addBinding(
            {"set-ch", "'c v' Set DMX-Channel c to value v", true, nullptr, setDmxChannel});
        shell.addBinding(
            {"get-ch", "'c' Get DMX-Channel c (omit c for all)", true, nullptr, getDmxChannel});
    }

    static void getDmxLayout(EmbeddedCli *cli, char *args, void *context) {
        auto &port = static_cast<Cli *>(cli->appContext)->_port;
        port.write(_stage->getStageConfigStr().c_str());
    }

    static void getDmxChannel(EmbeddedCli *cli, char *args, void *context) {
        auto &port = static_cast<Cli *>(cli->appContext)->_port;

        if (embeddedCliGetTokenCount(args) == 0) {
            port.write(_channels->getValuesStr().c_str());
            return;
        }
        
        auto arg = embeddedCliGetToken(args, 1);
        port.write(_channels->getValueStr(arg).c_str());
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

        _channels->values[ch-1] = value;
        auto output = std::string {" Ch "} + std::to_string(ch) + " = " + std::to_string(value);
        port.write(output + "\n");
    }

    static void onCommand(EmbeddedCli *embeddedCli, CliCommand *command) {
        ESP_LOGI("UI", "Received command: %s", command->name);
        embeddedCliTokenizeArgs(command->args);
        for (int i = 1; i <= embeddedCliGetTokenCount(command->args); ++i) {
            ESP_LOGI("UI", "    arg %d : %s", i, embeddedCliGetToken(command->args, i));
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
        _ota->enableUpdateTask();
    }

    private:
    static StageConfig * _stage;
    static DmxChannels * _channels;
    static OtaHandler * _ota;
};