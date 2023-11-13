#include "../inc/Value.hpp"
#include "../inc/string.hpp"
#include "../inc/Raw.hpp"

namespace json
{

    Value::Value()
        : JsonEntity(JsonEntity::value)
    {
    }

    Value::Value(const Value &other)
        : JsonEntity(JsonEntity::value), _data(other._data)
    {
    }

    void Value::fromString(const std::string &raw)
    {
        _data = raw;
    }

    std::string Value::toString() const
    {
        return _data;
    }

    bool Value::_isBottomLayer() const
    {
        return true;
    }

    std::string Value::toStringF(const FormattingOptions &, size_t) const
    {
        return _data;
    }

    void Value::set(const std::string &value)
    {
        _data = "\"" + value + "\"";
    }

    void Value::set(const char *value)
    {
        set(std::string(value));
    }
}