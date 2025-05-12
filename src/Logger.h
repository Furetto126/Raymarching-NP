#pragma once

#include <format>
#include <iostream>

namespace __logger::log {

        template <class ...Ts>
        void error(const std::format_string<Ts...> &fmt, Ts &&...args)
        {
                const auto str = std::format(fmt, args...);

                std::cout << "[ERROR] " << str << std::endl;
                throw std::runtime_error(str);
        }

}

using namespace __logger;