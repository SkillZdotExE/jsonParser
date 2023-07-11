#include "../inc/JsonInterface.hpp"
#include "../inc/JsonObject.hpp"
#include "../inc/JsonArray.hpp"
#include "../inc/JsonValue.hpp"
#include "../inc/string.hpp"

namespace json
{
    JsonFormattingOptions defaultJsonFormattingOptions;

    JsonInterface::JsonInterface(JsonInterfaceType t)
        : type(t)
    {
    }

    JsonInterface *JsonInterface::makeNew(std::string str)
    {
        strn::trim(str);
        if (str.size() > 2 && *str.begin() == '{' && *(str.end() - 1) == '}')
        {
            return new JsonObject(str);
        }
        else if (str.size() > 2 && *str.begin() == '[' && *(str.end() - 1) == ']')
        {
            return new JsonArray(str);
        }
        else
        {
            return new JsonValue(str);
        }
    }

    JsonInterface::JsonInterfaceType JsonInterface::_getType()
    {
        return type;
    }

    JsonInterface::~JsonInterface()
    {
    }
}

std::ostream &operator<<(std::ostream &os, const json::JsonInterface &j)
{
    os << j.getString();
    return os;
}