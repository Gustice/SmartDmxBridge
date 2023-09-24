#pragma once

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

class BinDiff {
  public:
    struct Result {
        bool areSame;
        std::string diff;
    };

    BinDiff(int blk) : _blockSize(blk) {}

    Result compareBytes(const std::vector<uint8_t> &left, const std::vector<uint8_t> &right,
                        const std::array<std::string, 2> labels) {
        auto minLen = std::min(left.size(), right.size());
        auto maxLen = std::max(left.size(), right.size());

        if (minLen == maxLen && std::equal(left.cbegin(), left.end(), right.begin())) {
            return {true, ""};
        }

        unsigned lD = std::max(labels[0].length(), labels[1].length());

        std::stringstream o;
        o << "diff found ...\n";
        o << labels[0] << ":" << std::setfill(' ') << std::setw(lD - labels[0].length() + 3) 
          << " 0x";
        for (auto &&v : left) {
            o << std::setfill('0') << std::right << std::setw(2) << std::hex
              << static_cast<unsigned>(v);
        }
        o << "\n";
        o << labels[1] << ":" << std::setfill(' ') << std::setw(lD - labels[1].length() + 3) 
          << " 0x";
        for (auto &&v : right) {
            o << std::setfill('0') << std::right << std::setw(2) << std::hex
              << static_cast<unsigned>(v);
        }
        o << "\n";
        o << "diff:    ";
        for (size_t i = 0; i < minLen; i++) {
            if (left[i] == right[i]) {
                o << "  ";
            } else {
                o << "##";
            }
        }
        for (size_t i = minLen; i < maxLen; i++) {
            o << "##";
        }
        return {false, o.str()};
    }

    template <typename T> std::string stringify_list(T idx, const T end) {
        std::stringstream o;
        unsigned i = 0;
        o << std::setfill('0') << std::setw(4) << std::hex;
        while (idx != end) {
            o << "Blk " << i++ << ": ";
            auto b = _blockSize;
            while (--b >= 0 && idx < end) {
                o << "0x" << static_cast<unsigned>(*idx++) << ", ";
            }
            o << "\n";
        }
        return o.str();
    }

    template <typename T> std::string stringify_list(const std::vector<T> &list) {
        std::stringstream o;
        unsigned i = 0;
        o << std::setfill('0') << std::setw(4) << std::hex;
        auto idx = list.cbegin();
        while (idx != list.cend()) {
            o << "Blk " << i++ << ": ";
            auto b = _blockSize;
            while (--b >= 0 && idx < list.cend()) {
                o << "0x" << static_cast<unsigned>(*idx++) << ", ";
            }
            o << "\n";
        }
        return o.str();
    }

  private:
    int _blockSize;
};
