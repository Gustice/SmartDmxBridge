#pragma once

#include <assert.h>

class ScaledValue {
  public:
    struct Range {
        int min;
        int max;
    };

    ScaledValue(Range input, Range output)
        : _input(input), _intputDelta(input.max - input.min), _output(output),
          _outputDelta(output.max - output.min) {}

    int scale(int input) {
        float r = static_cast<float>(input - _input.min)/_intputDelta;
        return r *_outputDelta + _output.min;
    }

  private:
    const Range _input;
    const int _intputDelta;
    const Range _output;
    const int _outputDelta;
};
