#ifndef PMPL_UTILS_H
#define PMPL_UTILS_H

#include <chrono>

template <typename Function>
double timeIt(Function function) {
    auto start = std::chrono::high_resolution_clock::now();
    function();
    auto end = std::chrono::high_resolution_clock::now();
    auto nanosecondDuration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
    return nanosecondDuration * 1e-9;
}

#endif //PMPL_UTILS_H
