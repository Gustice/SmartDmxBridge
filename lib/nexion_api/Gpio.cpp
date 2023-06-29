

#include "Gpio.hpp"

namespace Nxt {

bool Gpio::pin_mode(uint32_t port,uint32_t mode,uint32_t control_id)
{
    std::string cmd;
    cmd += "cfgpio ";
    cmd += port + '0';
    cmd += ',';
    cmd += mode + '0';
    cmd += ',';
    cmd += control_id + '0';

    sendCommand(cmd);
    return recvRetCommandFinished();
}

bool Gpio::digital_write(uint32_t port,uint32_t value)
{
    std::string cmd;
    cmd += "pio";
    cmd += port + '0';
    cmd += '=';
    cmd += value + '0';
    
    sendCommand(cmd);
    return recvRetCommandFinished();
}

uint32_t Gpio::digital_read(uint32_t port)
{
    std::string cmd("get ");
    cmd += "pio";
    cmd += port + '0';
    
    sendCommand(cmd);
    return recvRetNumber();
}

bool Gpio::analog_write(uint32_t port,uint32_t value)
{
    std::string cmd{"pwm"};
    cmd += port + '0';
    cmd += '=';
    cmd += std::to_string(value);
    
    sendCommand(cmd);
    return recvRetCommandFinished();   
}

bool Gpio::set_pwmfreq(uint32_t value)
{
    sendCommand(std::string{"pwmf="} + std::to_string(value));
    return recvRetCommandFinished();
}

uint32_t Gpio::get_pwmfreq()
{
    sendCommand(std::string{"get pwmf"});
    return recvRetNumber();
}

}