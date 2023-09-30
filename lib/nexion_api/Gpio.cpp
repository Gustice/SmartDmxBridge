

#include "Gpio.hpp"

namespace nxt {

bool Gpio::pin_mode(uint32_t port,uint32_t mode,uint32_t control_id)
{
    std::string cmd{"cfgpio "};
    cmd += port + '0';
    cmd += ',';
    cmd += mode + '0';
    cmd += ',';
    cmd += control_id + '0';

    _port.sendCommand(cmd);
    return _port.recvRetCommandFinished();
}

bool Gpio::digital_write(uint32_t port,uint32_t value)
{
    std::string cmd{"pio"};
    cmd += port + '0';
    cmd += '=';
    cmd += value + '0';
    
    _port.sendCommand(cmd);
    return _port.recvRetCommandFinished();
}

uint32_t Gpio::digital_read(uint32_t port)
{
    std::string cmd{"get "};
    cmd += "pio";
    cmd += port + '0';
    
    _port.sendCommand(cmd);
    return _port.recvRetNumber();
}

bool Gpio::analog_write(uint32_t port,uint32_t value)
{
    std::string cmd{"pwm"};
    cmd += port + '0';
    cmd += '=';
    cmd += std::to_string(value);
    
    _port.sendCommand(cmd);
    return _port.recvRetCommandFinished();   
}

bool Gpio::set_pwmfreq(uint32_t value)
{
    _port.sendCommand(std::string{"pwmf="} + std::to_string(value));
    return _port.recvRetCommandFinished();
}

uint32_t Gpio::get_pwmfreq()
{
    _port.sendCommand(std::string{"get pwmf"});
    return _port.recvRetNumber();
}

}