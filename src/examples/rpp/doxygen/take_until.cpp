#include <rpp/rpp.hpp>

#include <chrono>
#include <iostream>

/**
 * @example take_until.cpp
 **/
int main()
{
    //! [take_until]
    rpp::source::interval(std::chrono::seconds{1}, rpp::schedulers::current_thread{})
        | rpp::ops::take_until(rpp::source::interval(std::chrono::seconds{5}, rpp::schedulers::current_thread{}))
        | rpp::ops::subscribe([](int v) { std::cout << "-" << v; },
                              [](const std::exception_ptr&) { std::cout << "-x" << std::endl; },
                              []() { std::cout << "-|" << std::endl; });
    // source 1: -0-1-2-3-4-5-6-7-     --
    // source 2: ---------0---------1- --
    // Output  : -0-1-2-3-|
    //! [take_until]

    //! [terminate]
    rpp::source::never<int>()
        | rpp::ops::take_until(rpp::source::error<bool>(std::make_exception_ptr(std::runtime_error{""})))
        | rpp::ops::subscribe([](int v) { std::cout << "-" << v; },
                              [](const std::exception_ptr&) { std::cout << "-x" << std::endl; },
                              []() { std::cout << "-|" << std::endl; });
    // source 1: -
    // source 2: -x
    // Output  : -x
    //! [terminate]
    return 0;
}
