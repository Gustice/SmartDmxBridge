/**
 * @file ScaledValue.hpp
 * @author Gustice
 * @brief Scaled value
 * @date 2023-10-03
 * 
 * @copyright Copyright (c) 2023
 */
#pragma once

#include <assert.h>
#include <type_traits>

/**
 * @brief Scaled value class. Scales input to configured scale
 * 
 * @tparam T Type of value
 */
template <typename T>
class ScaledValue {
  public:
    /**
     * @brief Range class to define the scale
     */
    struct Range {
        /// @brief Minimum
        T min;
        /// @brief Maximum
        T max;
    };

    /// @brief Constructor
    /// @param input Defines scale of input values
    /// @param output Defines scale of output values
    ScaledValue(Range input, Range output)
        : _input(input), _intputDelta(input.max - input.min), _output(output),
          _outputDelta(output.max - output.min) {
        static_assert(std::is_integral_v<T>);
        // todo warning if loss of precision
    }

    /// @brief Scale a value to given output scale
    /// @param input input value to scale
    /// @return 
    T scale(T input) {
        return (input - _input.min) * _outputDelta / _intputDelta + _output.min;
    }

  private:
    const Range _input;
    const int _intputDelta;
    const Range _output;
    const int _outputDelta;
};
