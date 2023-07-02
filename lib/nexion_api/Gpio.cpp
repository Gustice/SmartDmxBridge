

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

    NxtIo::sendCommand(cmd);
    return NxtIo::recvRetCommandFinished();
}

bool Gpio::digital_write(uint32_t port,uint32_t value)
{
    std::string cmd;
    cmd += "pio";
    cmd += port + '0';
    cmd += '=';
    cmd += value + '0';
    
    NxtIo::sendCommand(cmd);
    return NxtIo::recvRetCommandFinished();
}

uint32_t Gpio::digital_read(uint32_t port)
{
    std::string cmd("get ");
    cmd += "pio";
    cmd += port + '0';
    
    NxtIo::sendCommand(cmd);
    return NxtIo::recvRetNumber();
}

bool Gpio::analog_write(uint32_t port,uint32_t value)
{
    std::string cmd{"pwm"};
    cmd += port + '0';
    cmd += '=';
    cmd += std::to_string(value);
    
    NxtIo::sendCommand(cmd);
    return NxtIo::recvRetCommandFinished();   
}

bool Gpio::set_pwmfreq(uint32_t value)
{
    NxtIo::sendCommand(std::string{"pwmf="} + std::to_string(value));
    return NxtIo::recvRetCommandFinished();
}

uint32_t Gpio::get_pwmfreq()
{
    NxtIo::sendCommand(std::string{"get pwmf"});
    return NxtIo::recvRetNumber();
}

}