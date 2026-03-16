#pragma once


namespace pe {

template<typename T>
T round_up(T value, T multiplier) {
    return ((value + multiplier - 1) & ~(multiplier));
}

}
