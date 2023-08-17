#pragma once

#include <assert.h>
#include <type_traits>

template<typename T>

class ScaledValue {
  public:
    struct Range {
        T min;
        T max;
    };

    ScaledValue(Range input, Range output)
        : _input(input), _intputDelta(input.max - input.min), _output(output),
          _outputDelta(output.max - output.min) {
            static_assert(std::is_integral_v<T>);
            // todo warning if loss of precision
          }

    T scale(T input) {
        return (input - _input.min) *_outputDelta / _intputDelta + _output.min;
    }

  private:
    const Range _input;
    const int _intputDelta;
    const Range _output;
    const int _outputDelta;
};
