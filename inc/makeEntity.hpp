#pragma once
#include <string>
#include "../inc/Value.hpp"

namespace json {
    class JsonEntity;
    class Array;
    class Object;
    class Raw;

    JsonEntity* make(const std::string& str);

    JsonEntity* make(const char *str);

    template <typename T>
    inline JsonEntity* make(const T& raw)
    {
        return new Value(raw);
    }

}
