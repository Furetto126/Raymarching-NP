#pragma once

#include <any>
#include <iostream>
#include <glm/glm.hpp>

template<typename T, typename ...Args>
using Function = T (*)(Args...);

#define OPENGL_MESSAGE_BUFFER_SIZE 512

#define PARENS ()
#define EXPAND(...) EXPAND4(EXPAND4(EXPAND4(EXPAND4(__VA_ARGS__))))
#define EXPAND4(...) EXPAND3(EXPAND3(EXPAND3(EXPAND3(__VA_ARGS__))))
#define EXPAND3(...) EXPAND2(EXPAND2(EXPAND2(EXPAND2(__VA_ARGS__))))
#define EXPAND2(...) EXPAND1(EXPAND1(EXPAND1(EXPAND1(__VA_ARGS__))))
#define EXPAND1(...) __VA_ARGS__

#define CASE(type, ...) type, { __VA_ARGS__; }

#define CONSTEXPR_SWITCH(var, ...)                                              \
        do {                                                                    \
                __VA_OPT__(EXPAND(__CONSTEXPR_SWITCH(var, __VA_ARGS__)))        \
        } while (false)

// The actual macro definition.
#define __CONSTEXPR_SWITCH(var, case1, codeBlock1, ...)                         \
        if constexpr (std::same_as<var, case1>)                                 \
                codeBlock1                                                      \
        __VA_OPT__(else __CONSTEXPR_SWITCH2 PARENS (var, __VA_ARGS__))

#define __CONSTEXPR_SWITCH2() __CONSTEXPR_SWITCH

template<typename T>
constexpr size_t getTypePad()
{
        CONSTEXPR_SWITCH(T,
                CASE(int,       return  4),
                CASE(float,     return  4),
                CASE(glm::vec2, return  8),
                CASE(glm::vec3, return 16),
                CASE(glm::vec4, return 16),
                CASE(std::any,  return 0),
                // !std::same_as<T, T> is false, but for some reasons in some systems
                // using 'false' causes compilations error.
                CASE(T, static_assert(!std::same_as<T, T>, "Struct member type padding not implemented."))
        );

        return 0;
}

template<typename ...Ts>
constexpr size_t getMaxSize()
{
        return std::max({getTypePad<Ts>()...});
}

template<typename T>
concept HasPadding = requires(T) { T::padding; };

#define $SSBO(name, ...)                                                        \
    struct name {                                                               \
        static constexpr size_t padding = getMaxSize<GET_TYPES(__VA_ARGS__)>(); \
        DECLARE_TYPES(__VA_ARGS__);                                             \
    }

#define GET_TYPES(...) __VA_OPT__(EXPAND(__GET_TYPES(__VA_ARGS__)))

#define __GET_TYPES(type, _, ...)                                               \
    type __VA_OPT__(, __GET_TYPES2 PARENS (__VA_ARGS__))

#define __GET_TYPES2() __GET_TYPES

#define DECLARE_TYPES(...) __VA_OPT__(EXPAND(__DECLARE_TYPES(__VA_ARGS__)))

#define __DECLARE_TYPES(type, name, ...)                                        \
        type name                                                               \
        __VA_OPT__(; __DECLARE_TYPES2 PARENS (__VA_ARGS__))

#define __DECLARE_TYPES2() __DECLARE_TYPES