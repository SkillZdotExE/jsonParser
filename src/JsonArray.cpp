#include "../inc/JsonArray.hpp"
#include "../inc/JsonObject.hpp"
#include "../inc/JsonValue.hpp"
#include <iostream>
#include <sstream>

namespace json
{

    JsonArray::JsonArray()
        : JsonEntity(JsonEntityType::array)
    {
    }

    JsonArray::JsonArray(const JsonArray &other)
        : JsonArray()
    {
        _data.clear();
        for (const auto &entity : other._data)
        {
            _data.push_back(JsonEntity::makeNew(entity->toString()));
        }
    }

    JsonArray::JsonArray(JsonArray &&other)
        : JsonArray()
    {
        _data.clear();
        for (auto &entity : other._data)
        {
            _data.push_back(entity);
            entity = nullptr;
        }
    }

    JsonArray &JsonArray::operator=(const JsonArray &other)
    {
        if (this == &other)
            return *this;

        for (auto &entity : _data)
        {
            delete entity;
        }

        _data.clear();

        for (const auto &entity : other._data)
        {
            _data.push_back({JsonEntity::makeNew(entity->toString())});
        }
        return *this;
    }

    JsonArray &JsonArray::operator=(JsonArray &&other)
    {
        for (auto &entity : _data)
        {
            delete entity;
        }

        _data.clear();

        for (const auto &entity : other._data)
        {
            _data.push_back(entity);
        }
        return *this;
    }

    void JsonArray::fromString(std::string raw)
    {
        size_t currentIndex = 1,
               tokenStartIndex = 1;
        uint16_t bracesLevel = 0,
                 bracketsLevel = 0;
        bool inQuote = false,
             backslash = false;

        for (; currentIndex < raw.size(); currentIndex++)
        {
            bool resetBackslash = backslash;

            switch (raw[currentIndex])
            {
            case '\\':
                backslash = !backslash;
                break;
            case '\"':
                if (!backslash)
                    inQuote = !inQuote;
                break;
            case '{':
                if (!inQuote)
                    bracesLevel++;
                break;
            case '}':
                if (!inQuote)
                    bracesLevel--;
                break;
            case '[':
                if (!inQuote)
                    bracketsLevel++;
                break;
            case ']':
                if (!inQuote)
                    bracketsLevel--;
                break;
            case ',':
                if (!inQuote && bracesLevel == 0 && bracketsLevel == 0)
                {
                    _data.push_back(JsonEntity::makeNew(raw.substr(tokenStartIndex, currentIndex - tokenStartIndex)));
                    tokenStartIndex = currentIndex + 1;
                }
            }

            if (resetBackslash)
                backslash = false;
        }
        _data.push_back(JsonEntity::makeNew(raw.substr(tokenStartIndex, currentIndex - tokenStartIndex - 1)));
    }

    std::string JsonArray::toString() const
    {
        std::ostringstream outStream;

        outStream << '[';

        size_t i = 0;
        for (auto &e : _data)
        {
            i++;
            outStream << e->toString();
            if (i < _data.size())
                outStream << ',';
        }

        outStream << ']';

        return outStream.str();
    }

    JsonArray &JsonArray::A(size_t index)
    {
        if (_data.size() <= index)
            throw std::out_of_range("JsonArray::A: index " + std::to_string(index) + " is out of bounds");
        if (_data[index]->_getType() != JsonEntityType::array)
            throw std::runtime_error("JsonArray::A: Element " + std::to_string(index) + " is not of type array");
        return dynamic_cast<JsonArray &>(*_data[index]);
    }

    JsonObject &JsonArray::O(size_t index)
    {
        if (_data.size() <= index)
            throw std::out_of_range("JsonArray::O: index " + std::to_string(index) + " is out of bounds");
        if (_data[index]->_getType() != JsonEntityType::object)
            throw std::runtime_error("JsonArray::O: Element " + std::to_string(index) + " is not of type object");
        return dynamic_cast<JsonObject &>(*_data[index]);
    }

    JsonValue &JsonArray::V(size_t index)
    {
        if (_data.size() <= index)
            throw std::out_of_range("JsonArray::V: index " + std::to_string(index) + " is out of bounds");
        if (_data[index]->_getType() != JsonEntityType::value)
            throw std::runtime_error("JsonArray::V: Element " + std::to_string(index) + " is not of type value");
        return dynamic_cast<JsonValue &>(*_data[index]);
    }

    std::string JsonArray::S(size_t index) const
    {
        if (_data.size() <= index)
            throw std::out_of_range("JsonArray::S: index " + std::to_string(index) + " is out of bounds");
        if (_data.at(index)->_getType() != JsonEntityType::value)
            throw std::runtime_error("JsonArray::S: Element " + std::to_string(index) + " is not of type string");
        return _data.at(index)->toString();
    }

    bool JsonArray::getBool(size_t index) const
    {
        return _data.size() >= index && _data.at(index)->_getType() == JsonEntityType::value && _data.at(index)->toString() == "true";
    }

    std::string JsonArray::getString(size_t index) const
    {
        std::string ret;
        if (_data.size() > index && _data.at(index)->_getType() == JsonEntityType::value)
            ret = _data.at(index)->toString();
        else
            return std::string();

        if (ret.size() < 2 || ret[0] != '\"' || ret[ret.size() - 1] != '\"')
            return std::string();

        return ret.substr(1, ret.size() - 2);
    }
    
    template<>
    inline void JsonArray::push_back<std::string>(std::string str)
    {
        _data.push_back(JsonEntity::makeNew("\"" + str + "\""));
    }
    
    template <>
    inline void JsonArray::insert<std::string>(size_t n, std::string str)
    {
        _data.insert(_data.begin() + n, JsonEntity::makeNew("\"" + str + "\""));
    }

    void JsonArray::erase(size_t index)
    {
        _data.erase(_data.begin() + index);
    }

    void JsonArray::erase(size_t start, size_t end)
    {
        _data.erase(_data.begin() + start, _data.begin() + end);
    }

    std::string JsonArray::getType(size_t index) const
    {
        if (_data.size() <= index)
            throw std::out_of_range("JsonArray::getType index " + std::to_string(index) + " is out of bounds");

        switch (_data[index]->_getType())
        {
        case JsonEntityType::array:
            return "array";
        case JsonEntityType::object:
            return "object";
        case JsonEntityType::value:
            return "value";
        }
        return "invalid type";
    }

    size_t JsonArray::size() const
    {
        return _data.size();
    }

    bool JsonArray::_isBottomLayer() const
    {
        for (const auto &entity : _data)
        {
            if (entity->_getType() == JsonEntityType::object || entity->_getType() == JsonEntityType::array)
                return false;
        }
        return true;
    }

    std::string JsonArray::toStringF(const JsonFormattingOptions &options, size_t tabs) const
    {
        if (options.forceCompact)
            return toString();
        std::ostringstream outStream;
        bool isInline = options.forceInline || (options.inlineShortBottomLevelArrays && _isBottomLayer() && toString().size() < options.maxLengthToInline);

        // if (!isInline && (options.firstBracketInNewline) && tabs != 0)
        //     outStr << '\n'
        //            << options._getTab(tabs);

        outStream << '[';

        if (isInline && options.spaceAfterOpeningBeforeClosingBrackets)
            outStream << ' ';

        if (!isInline)
            outStream << '\n';

        tabs++;

        size_t i = 0;
        for (auto &entity : _data)
        {
            i++;

            if (!isInline)
                outStream << options._getTab(tabs);

            if (!isInline && options.firstBracketInNewline && entity->_getType() != JsonEntityType::value && entity->_getType() != JsonEntityType::array)
                outStream << '\n'
                          << options._getTab(tabs);
            outStream << entity->toStringF(options, tabs);

            if (i < _data.size())
            {
                outStream << ',';
                if (options.spaceAfterComma && isInline)
                    outStream << ' ';
            }
            if (!isInline && (options.lastBracketInNewline || i < _data.size()))
                outStream << '\n';
        }

        if (!isInline)
            outStream << (tabs != 0 ? options._getTab(tabs - 1) : "");

        if (isInline && options.spaceAfterOpeningBeforeClosingBrackets)
            outStream << ' ';

        outStream << ']';

        return outStream.str();
    }

    JsonArray::~JsonArray()
    {
        for (const auto &entity : _data)
        {
            delete entity;
        }
    }
}
