#pragma once

#include "esp_log.h"
#include <embedded_cli.h>
#include <functional>
#include <uart.hpp>

constexpr static int CLI_BUFFER_SIZE = 512;
constexpr static int CLI_RX_BUFFER_SIZE = 16;
constexpr static int CLI_CMD_BUFFER_SIZE = 32;
constexpr static int CLI_HISTORY_SIZE = 16;
constexpr static int CLI_BINDING_COUNT = 3;

class Cli {
  public:
    using commandCb = void (*)(EmbeddedCli *embeddedCli, CliCommand *command);
    using writeCb = void (*)(EmbeddedCli *embeddedCli, char c);

    Cli(Uart &port, commandCb cmd) : _port(port) {
        EmbeddedCliConfig *config = embeddedCliDefaultConfig();
        config->cliBuffer = cliBuffer;
        config->cliBufferSize = CLI_BUFFER_SIZE;
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

  private:
    Uart &_port;
    EmbeddedCli *cli = nullptr;
    CLI_UINT cliBuffer[BYTES_TO_CLI_UINTS(CLI_BUFFER_SIZE)];

    static void writeToken(EmbeddedCli *embeddedCli, char c) {
        static_cast<Cli *>(embeddedCli->appContext)->_port.write(c);
    }
};
