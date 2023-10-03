/**
 * @file ValueCache.hpp
 * @author Gustice
 * @brief Cached value
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once

/**
 * @brief Value cache class. Keeps track of wether a value mas manipulated
 * 
 * @tparam T Type of value
 */
template <typename T> 
class ValueCache {
    public:
    /// @brief Checks if values is new
    /// @return true if new
    bool isNew() {
        return _new;
    }

    /// @brief Return current value
    /// @return current value
    [[nodiscard]] T &getValue() {
        _new = false;
        return _value;
    }

    /// @brief Sets value
    /// @param value new value
    void setValue(T value) {
        _new = true;
        _value = value;
    }

  private:
    bool _new = true;
    T _value{};
};
