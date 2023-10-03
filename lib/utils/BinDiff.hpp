/**
 * @file BinDiff.hpp
 * @author Gustice
 * @brief Diff-utility for serial data
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */

#pragma once

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

/**
 * @brief Diff class to annotate changes in serial data
 */
class BinDiff {
  public:
    /**
     * @brief Definition of return type
     */
    struct Result {
        /// @brief true if serial data matches

        bool areSame;
        /// @brief annotated diff
        /// @details annotation format <br>
        ///   diff found ... <br>
        ///   left:  0x11121314 <br>
        ///   right: 0x11121310 <br>
        ///   diff:          # <br>
        std::string diff;
    };

    /// @brief Constructor
    /// @param blk block-size for splitting annotated diff in blocks
    BinDiff(int blk)
        : _blockSize(blk) {}

    /// @brief compare two series of values
    /// @param left left block, i.e. expected series
    /// @param right right block, i.e. actual series
    /// @param labels labels for left and right block in case of output
    /// @return result of comparison
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

    /// @brief Create string from input in form "Blk 0: 0x11, 0x12, 0x13, 0x14"
    /// @tparam T Value type
    /// @param idx index to start from
    /// @param end index for end
    /// @return output string
    template <typename T>
    std::string stringify_list(T idx, const T end) {
        std::stringstream o;
        o << std::setfill('0') << std::setw(4) << std::hex;
        serializeList(o, idx, end);

        return o.str();
    }

    /// @brief Create string from input in form "Blk 0: 0x11, 0x12, 0x13, 0x14"
    /// @tparam T Value type
    /// @param list list to print
    /// @return output string
    template <typename T>
    std::string stringify_list(const std::vector<T> &list) {
        std::stringstream o;
        o << std::setfill('0') << std::setw(4) << std::hex;
        serializeList(o, list.cbegin(), list.cend());
        return o.str();
    }

  private:
    int _blockSize;

    template <typename T>
    void serializeList(std::stringstream &o, T idx, const T end) {
        unsigned i = 0;
        while (idx != end) {
            o << "Blk " << i++ << ": ";
            auto b = _blockSize;
            while (--b >= 0 && idx < end) {
                o << "0x" << static_cast<unsigned>(*idx++) << ", ";
            }
            o << "\n";
        }
    }
};
