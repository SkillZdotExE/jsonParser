#include "../inc/JsonValue.hpp"
#include "../inc/string.hpp"
#include "../inc/Raw.hpp"

namespace json
{
    void JsonValue::fromString(std::string raw)
    {
        _data = raw;
    }

    std::string JsonValue::toString() const
    {
        return _data;
    }

    size_t JsonValue::size() const
    {
        return 1;
    }

    bool JsonValue::_isBottomLayer() const
    {
        return true;
    }

    std::string JsonValue::toStringF(const JsonFormattingOptions &, size_t) const
    {
        return _data;
    }
    
    template <>
    inline void JsonValue::set<std::string>(const std::string& str)
    {
        _data = "\"" + str + "\"";
    }
    
    template <>
    inline void JsonValue::set<Raw>(const Raw& val)
    {
        _data = val;
    }
    
    void JsonValue::setString(const std::string& str)
    {
        set<std::string>("\"" + str + "\"");
    }
}