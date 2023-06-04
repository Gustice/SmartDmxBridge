#include "socket.hpp"
#include "esp_log.h"
#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include <lwip/netdb.h>

static const char *TAG = "ETH";

Socket::Socket(Config &config, int listener) {
    struct sockaddr_storage source_addr; // Large enough for both IPv4 or IPv6
    socklen_t addr_len = sizeof(source_addr);
    _socket = accept(listener, (struct sockaddr *)&source_addr, &addr_len);
    if (_socket < 0) {
        ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
        return;
    }

    // Set tcp keepalive option
    setsockopt(_socket, SOL_SOCKET, SO_KEEPALIVE, &config.keepAlive, sizeof(int));
    setsockopt(_socket, IPPROTO_TCP, TCP_KEEPIDLE, &config.keepIdle, sizeof(int));
    setsockopt(_socket, IPPROTO_TCP, TCP_KEEPINTVL, &config.keepInterval, sizeof(int));
    setsockopt(_socket, IPPROTO_TCP, TCP_KEEPCNT, &config.keepCount, sizeof(int));

    inet_ntoa_r(((struct sockaddr_in *)&source_addr)->sin_addr, _addr_str, sizeof(_addr_str) - 1);
    ESP_LOGI(TAG, "Socket accepted ip address: %s", _addr_str);

    active = true;
}
Socket::~Socket() {
    shutdown(_socket, 0);
    close(_socket);
}

std::string Socket::read() {
    ESP_LOGI(TAG, " ... start reading port");

    int len = recv(_socket, _data, sizeof(_data) - 1, 0);

    if (len < 0) {
        ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
        active = false;
        return "";
    }

    if (len == 0) {
        ESP_LOGW(TAG, "Connection closed");
        active = false;
        return "";
    }

    _data[len] = 0;
    int i = 0;
    while (_data[i] == 0xFF) { // while telnet command
        i += 3;                // skip this command
    }

    if (i == len) {                   // only configuration
        send(_socket, _data, len, 0); // just mirrow it ... don't have a clue of what i am doing
        return "";
    }

    ESP_LOGI(TAG, "Received %d bytes: %s", len, _data);
    std::string ret((const char *)&_data[i]);
    return ret;
}

void Socket::write(char c) {
    int written = send(_socket, &c, 1, 0);
    if (written < 0) {
        ESP_LOGE(TAG, "Error occurred during sending: errno %d", errno);
    }
}
