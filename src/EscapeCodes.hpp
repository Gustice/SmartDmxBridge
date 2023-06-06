#pragma once
#include <string>
#include <sstream>

enum class AnsiColor : char {
    Black = 0,
    Red = 1,
    Green = 2,
    Yellow = 3,
    Blue = 4,
    Magenta = 5,
    Cyan = 6,
    White = 7,
    Grey = 60,
    Bright_red = 61,
    Bright_green = 62,
    Bright_yellow = 63,
    Bright_blue = 64,
    Bright_magenta = 65,
    Bright_cyan = 66,
    Bright_white = 67,
};

class EscapeCodes {
  public:
    static std::string home() {
        return escape() + "H";
    }
    static std::string cls() {
        return escape() + "1J";
    }
    static std::string clearLine() {
        return escape() + "2K";
    }
    static std::string clearEoLine() {
        return escape() + "K";
    }

    static std::string cursorXY(int x, int y) {
        std::stringstream ret(escape());
        ret << y << ";" << x << "H";
        return ret.str();
    }
    static std::string cursorUp(int x) {
        return escapeSequence(x, 'A');
    }
    static std::string cursorDown(int x) {
        return escapeSequence(x, 'B');
    }
    static std::string cursorLeft(int x) {
        return escapeSequence(x, 'D');
    }
    static std::string cursorRight(int x) {
        return escapeSequence(x, 'C');
    }

    static std::string setBG(int color) {
        return setAttribute(color + 40);
    }
    static std::string setFG(int color) {
        return setAttribute(color + 30);
    }

    static std::string bold(std::string str) {
        return setAttribute(1) + str + setAttribute(22);
    }
    static std::string italic(std::string str) {
        return setAttribute(3) + str + setAttribute(23);
    }
    static std::string underline(std::string str) {
        return setAttribute(4) + str + setAttribute(24);
    }
    static std::string blink(std::string str) {
        return setAttribute(5) + str + setAttribute(25);
    }
    static std::string inverse(std::string str) {
        return setAttribute(7) + str + setAttribute(27);
    }

    static std::string showCursor(bool blink) {
        return escape() + "?25" + (blink ? 'h' : 'l');
    }
    static std::string reset() {
        return escape() + 'm';
    }

  protected:
    static std::string escape() {
        return std::string("\033[");
    }
    static std::string escapeSequence(int x, char v) {
        std::stringstream ret(escape());
        ret << x << v;
        return ret.str();
    }
    static std::string setAttribute(int a) {
        return escapeSequence(a, 'm');
    }
};
