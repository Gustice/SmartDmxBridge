#pragma once

template <typename T> 
class ValueCache {
    public:
    bool isNew() {
        return _new;
    }

    [[nodiscard]] T &getValue() {
        _new = false;
        return _value;
    }

    void setValue(T value) {
        _new = true;
        _value = value;
    }

  private:
    bool _new = true;
    T _value{};
};
