#pragma once


namespace pe {

template<typename T>
T roundUp(T value, T multiplier) {
    return ((value + multiplier - 1) & ~(multiplier));
}

}
