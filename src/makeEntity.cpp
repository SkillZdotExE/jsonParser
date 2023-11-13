#include "../inc/makeEntity.hpp"
#include "../inc/JsonEntity.hpp"
#include "../inc/Value.hpp"
#include "../inc/Array.hpp"
#include "../inc/Object.hpp"

namespace json {

    JsonEntity* make(const std::string& str)
    {
        std::string raw(str);
        strn::trim(raw);
        if (raw.size() >= 2 && *raw.begin() == '{' && *(raw.end() - 1) == '}')
        {
            return new Object(raw);
        }
        else if (raw.size() >= 2 && *raw.begin() == '[' && *(raw.end() - 1) == ']')
        {
            auto ret = new Array();
            ret->fromString(raw);
            return ret;
        }
        else
        {
            return new Value(raw);
        }
    }

    JsonEntity* make(const char *str)
    {
        std::string raw(str);
        strn::trim(raw);
        if (raw.size() >= 2 && *raw.begin() == '{' && *(raw.end() - 1) == '}')
        {
            return new Object(raw);
        }
        else if (raw.size() >= 2 && *raw.begin() == '[' && *(raw.end() - 1) == ']')
        {
            auto ret = new Array();
            ret->fromString(raw);
            return ret;
        }
        else
        {
            return new Value(raw);
        }
    }
}