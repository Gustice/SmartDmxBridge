#include <array>
#include <cstdint>
#include <queue>
#include <string>
#include <string_view>
#include <vector>

namespace nxt {
enum class LogSeverity { Debug,
                         Warning,
                         Error };

using LogCallback = void (*)(LogSeverity, std::string);
    using Buffer = std::vector<uint8_t>;

class Stream {
  public:
    virtual std::vector<uint8_t> read(size_t minSize = 0, unsigned msTimeout = 0) = 0;
    virtual void write(char c) = 0;
    virtual void write(std::string str) = 0;
};

/**
     * @brief Response codes in return Messages
     * see [API-documentation](https://nextion.tech/instruction-set) for details
     */
enum class Return : uint8_t {
    // ACK-Tokens

    // Returned when instruction sent by user has failed
    // Example: 0x00.FF.FF.FF
    InvalidInstruction = 0x00,

    //Returned when instruction sent by user was successful
    // Example: 0x01.FF.FF.FF
    InstructionSuccessful = 0x01,

    // Returned when invalid Component ID or name was used
    // Example: 0x02.FF.FF.FF
    InvalidComponentID = 0x02,

    // Returned when invalid Page ID or name was used
    // Example: 0x03.FF.FF.FF
    InvalidPageID = 0x03,

    // Returned when invalid Picture ID was used
    // Example: 0x04.FF.FF.FF
    InvalidPictureID = 0x04,

    // Returned when invalid Font ID was used
    // Example: 0x05.FF.FF.FF
    InvalidFontID = 0x05,

    // Returned when File operation fails
    // Example: 0x06.FF.FF.FF
    InvalidFileOperation = 0x06,

    // Returned when Instructions with CRC validation fails their CRC check
    // Example: 0x09.FF.FF.FF
    InvalidCRC = 0x09,

    // Returned when invalid Baud rate was used
    // Example: 0x11.FF.FF.FF
    InvalidBaudrateSetting = 0x11,
    // (or Channel #) Returned when invalid Waveform ID or Channel # was used
    // Example: 0x12.FF.FF.FF
    InvalidWaveformID = 0x12,

    // Returned when invalid Variable name or invalid attribute was used
    // Example: 0x1A.FF.FF.FF
    InvalidVariableName = 0x1A,

    // Returned when Operation of Variable is invalid. ie:
    //   Text assignment t0.txt=abc or t0.txt=23,
    //   Numeric assignment j0.val=”50″ or j0.val=abc
    // Example: 0x1B.FF.FF.FF
    InvalidVariableOperation = 0x1B,

    // Returned when attribute assignment failed to assign
    // Example: 0x1C.FF.FF.FF
    Assignmentfailed = 0x1C,

    // Returned when an EEPROM Operation has failed
    // Example: 0x1D.FF.FF.FF
    EEPROMOperationFailed = 0x1D,

    // Returned when the number of instruction parameters is invalid
    // Example: 0x1E.FF.FF.FF
    InvalidQuantityOfParameters = 0x1E,

    // Returned when an IO operation has failed
    // Example: 0x1F.FF.FF.FF
    IOOperationFailed = 0x1F,

    // Returned when an unsupported escape character is used
    // Example: 0x20.FF.FF.FF
    EscapeCharacterInvalid = 0x20,

    // Returned when variable name is too long.
    //   Max length is 29 characters: 14 for page + “.” + 14 for component.
    // Example: 0x23.FF.FF.FF
    VariablenNmeTooLong = 0x23,

    // Returned when a Serial Buffer overflow occurs Buffer will continue
    //   to receive the current instruction, all previous instructions are lost.
    // Example: 0x24.FF.FF.FF
    SerialBufferOverflow = 0x24,

    // Returned when Nextion has started or reset.
    // Len=6, // Example: 0x00.00.00.FF.FF.FF
    NextionStartup = 0x00,

    // Returned when Touch occurs and component’s corresponding Send Component ID
    //   is checked in the users HMI design.
    //   0x00 is page number,
    //   first 0x01 is component ID,
    //   second 0x01 is event (0x01 Press and 0x00 Release)
    //   => data: Page 0, Component 2, Pressed
    // Len=7, // Example: 0x65.00.01.01.FF.FF.FF
    TouchEvent = 0x65,

    // Returned when the sendme command is used.
    //   0x01 is current page number
    //   => data: page 1
    // Len=5, // Example: 0x66.01.FF.FF.FF
    CurrentPageNumber = 0x66,

    // Returned when sendxy=1 and not in sleep mode
    //   0x00 0x7A is x coordinate in big endian order,
    //   0x00 0x1E is y coordinate in big endian order,
    //   0x01 is event (0x01 Press and 0x00 Release)
    //   (0x00*256+0x71,0x00*256+0x1E) data: (122,30) Pressed
    // Len=9, // Example: 0x67.00.7A.00.1E.01.FF.FF.FF
    TouchCoordinateAwake = 0x67,

    // Returned when sendxy=1 and exiting sleep
    //   0x00 0x7A is x coordinate in big endian order,
    //   0x00 0x1E is y coordinate in big endian order,
    //   0x01 is event (0x01 Press and 0x00 Release)
    //   (0x00*256+0x71,0x00*256+0x1E) data: (122,30) Pressed
    // Len=9, // Example: 0x68.00.7A.00.1E.01.FF.FF.FF
    TouchCoordinateSleep = 0x68,

    // Returned when using get command for a string.
    //   Each byte is converted to char. data: ab123
    // Len=##, // Example: 0x70.61.62.31.32.33.FF.FF.FF
    StringDataEnclosed = 0x70,

    // Returned when get command to return a number 4 byte 32-bit value in little endian order.
    //   (0x01+0x02*256+0x03*65536+0x04*16777216) data: 67305985
    // Len=8, // Example: 0x71.01.02.03.04.FF.FF.FF
    NumericDataEnclosed = 0x71,

    // Returned when Nextion enters sleep automatically Using sleep=1 will not return an 0x86
    // Len=4, // Example: 0x86.FF.FF.FF
    AutoEnteredSleepMode = 0x86,

    // Returned when Nextion leaves sleep automatically Using sleep=0 will not return an 0x87
    // Len=4, // Example: 0x87.FF.FF.FF
    AutoWakeFromSleep = 0x87,

    // Returned when Nextion has powered up and is now initialized successfully.
    // Len=4, // Example: 0x88.FF.FF.FF
    NextionReady = 0x88,

    // Returned when power on detects inserted microSD and begins Upgrade by microSD process
    // Len=4, // Example: 0x89.FF.FF.FF
    StartmicroSDUpgrade = 0x89,

    // Returned when all requested bytes of Transparent Data mode have been received,
    //   and is now leaving transparent data mode (see 1.16)
    // Len=4, // Example: 0xFD.FF.FF.FF
    TransparentDataFinished = 0xFD,

    // Returned when requesting Transparent Data mode, and device is now ready to begin
    //   receiving the specified quantity of data (see 1.16)
    // Len=4, // Example: 0xFE.FF.FF.FF
    TransparentDataReady = 0xFE,
};

enum class CodeBin : uint8_t {
    Return = 0,
    Touch,
    Number,
    String,
    Events,
    Trash,
    BinsCount,
};
class Port {
  public:

    Port(Stream &port, nxt::LogCallback logCb = nullptr) 
    : _serialPort(port), _logCallback(logCb) {}

    /**
     * @brief Get Numeric Data
     * 
     * @param timeout timeout in ms
     * @return uint32_t returned integer or 0 (fallback)
     */
    uint32_t recvRetNumber(uint32_t timeout = 100);

    /**
     * @brief Get String Data
     * 
     * @param timeout timeout in ms
     * @return std::string returned string or ""
     */
    std::string recvRetString(uint32_t timeout = 100);

    /**
     * @brief Read Command Response (Acknowledge or error code)
     * 
     * @param timeout timeout in ms
     * @return true success
     * @return false failed
     */
    bool recvRetCommandFinished(uint32_t timeout = 100);

    /**
     * @brief Send command to Nextion.
     * 
     * @param cmd command as string
     */
    void sendCommand(std::string payload);


    Buffer readCode(CodeBin code, unsigned msTimeout = 0);
    Buffer getFromBin(CodeBin code);
    void putInBin(Buffer payload);
    void sendLog(LogSeverity level, std::string log);

  private:
    Stream &_serialPort;
    LogCallback _logCallback;
    std::array<std::queue<Buffer>, static_cast<unsigned>(CodeBin::BinsCount)> bins;
    Buffer _remainder;
};
} // namespace nxt
