#pragma once

#include <vector>

#include "../Base.h"

template <typename T>
struct SSBO {
        static_assert(std::is_fundamental_v<T> || std::same_as<T, std::any> || HasPadding<T>, "SSBO type must be builtin type or created via the $SSBO macro.");
        static constexpr size_t alignment = []() constexpr -> size_t {
                if constexpr (HasPadding<T>)
                        return T::padding;
                else
                        return getTypePad<T>();
        }();

        std::vector<T> *const pData;

        bool hasSizeAsUniform;
        bool needsUpdate = true;

        uint32_t binding;
        uint32_t bufferID{};

        SSBO(int binding, std::vector<T> *pData, bool hasSizeAsUniform = true) :
                binding(binding), pData(pData), hasSizeAsUniform(hasSizeAsUniform) {}
};
