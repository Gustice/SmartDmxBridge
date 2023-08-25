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

    template <typename T> Result compare(const std::vector<T> &left, const std::vector<T> &right) {
        auto minLen = std::min(left.size(), right.size());

        if (!std::equal(left.cbegin(), left.cbegin() + minLen, right.begin())) {
            std::stringstream o;
            o << "differences found ...\n";

            unsigned i = 0;
            o << std::setfill('0') << std::setw(4) << std::hex;
            auto end = left.cbegin() + minLen;
            auto s1 = left.cbegin();
            auto s2 = right.cbegin();
            while (s1 != end) {
                o << "Blk " << i++ << ": ";
                auto b = _blockSize;
                while (--b >= 0 && s1 < end) {
                    o << "0x" << static_cast<unsigned>(*s1++) << "/"
                      << "0x" << static_cast<unsigned>(*s2++) << ", ";
                }
                o << "\n";
            }
            return {false, o.str()};
        }

        if (left.size() != right.size()) {
            std::string o;
            if (left.size() > right.size()) {
                o += "additional elements on left side ...\n";
                o += stringify_list(left.cbegin() + minLen, left.cend());
            } else {
                o += "additional elements on right side ...\n";
                o += stringify_list(right.cbegin() + minLen, right.cend());
            }

            return {false, o};
        }

        return {true, ""};
    }

    template <typename T> 
    std::string stringify_list(T idx, const T end) {
        std::stringstream o;
        unsigned i = 0;
        o << std::setfill('0') << std::setw(4) << std::hex;
        while (idx != end) {
            o << "Blk " << i++ << ": ";
            auto b = _blockSize;
            while (--b >= 0 && idx < end) {
                o << "0x" << static_cast<unsigned>(*idx++)  << ", ";
            }
            o << "\n";
        }
        return o.str();
    }

    template <typename T> 
        std::string stringify_list(const std::vector<T> & list) {
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
