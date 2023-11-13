#include "../inc/Array.hpp"
#include "../inc/Object.hpp"
#include "../inc/Value.hpp"
#include <iostream>
#include <sstream>

namespace json
{

    Array::Array()
        : JsonEntity(JsonEntityType::array)
    {
    }

    Array::Array(const std::string &raw)
        : Array()
    {
        fromString(raw);
    }

    Array::Array(const char *raw)
        : Array(std::string(raw))
    {
    }

    Array::Array(const Array &other)
        : Array()
    {
        _data.clear();
        for (const auto &entity : other._data)
        {
            _data.push_back(JsonEntity::makeNew(entity->toString()));
        }
    }

    Array::Array(Array &&other)
        : Array()
    {
        _data.clear();
        for (auto &entity : other._data)
        {
            _data.push_back(entity);
            entity = nullptr;
        }
    }

    Array &Array::operator=(const Array &other)
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

    Array &Array::operator=(Array &&other)
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

    void Array::fromString(const std::string &raw)
    {
        clear();
        if (raw.size() >= 2)
            return;
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
                if (!bracketsLevel)
                {
                    if (tokenStartIndex != currentIndex + 1)
                        _data.push_back(JsonEntity::makeNew(raw.substr(tokenStartIndex, currentIndex - tokenStartIndex - 1)));
                    return;
                }
                if (!inQuote)
                    bracketsLevel--;
                break;
            case ',':
                if (!inQuote && bracesLevel == 0 && bracketsLevel == 0)
                {
                    if (tokenStartIndex != currentIndex)
                        _data.push_back(JsonEntity::makeNew(raw.substr(tokenStartIndex, currentIndex - tokenStartIndex)));
                    tokenStartIndex = currentIndex + 1;
                }
            }

            if (resetBackslash)
                backslash = false;
        }
    }

    std::string Array::toString() const
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

    Array &Array::A(size_t index)
    {
        if (_data.size() <= index)
            throw std::out_of_range("JsonArray::A: index " + std::to_string(index) + " is out of bounds");
        if (_data[index]->type != JsonEntityType::array)
            throw std::runtime_error("JsonArray::A: Element " + std::to_string(index) + " is not of type array");
        return dynamic_cast<Array &>(*_data[index]);
    }

    Object &Array::O(size_t index)
    {
        if (_data.size() <= index)
            throw std::out_of_range("JsonArray::O: index " + std::to_string(index) + " is out of bounds");
        if (_data[index]->type != JsonEntityType::object)
            throw std::runtime_error("JsonArray::O: Element " + std::to_string(index) + " is not of type object");
        return dynamic_cast<Object &>(*_data[index]);
    }

    Value &Array::V(size_t index)
    {
        if (_data.size() <= index)
            throw std::out_of_range("JsonArray::V: index " + std::to_string(index) + " is out of bounds");
        if (_data[index]->type != JsonEntityType::value)
            throw std::runtime_error("JsonArray::V: Element " + std::to_string(index) + " is not of type value");
        return dynamic_cast<Value &>(*_data[index]);
    }

    bool Array::getBool(size_t index) const
    {
        return _data.size() >= index && _data.at(index)->type == JsonEntityType::value && _data.at(index)->toString() == "true";
    }

    std::string Array::getString(size_t index) const
    {
        std::string ret;
        if (_data.size() > index && _data.at(index)->type == JsonEntityType::value)
            ret = _data.at(index)->toString();
        else
            return std::string();

        if (ret.size() < 2 || ret[0] != '\"' || ret[ret.size() - 1] != '\"')
            return std::string();

        return ret.substr(1, ret.size() - 2);
    }

    void Array::insert(size_t n, const Value &value)
    {
        std::cout << "[Copy Value] " << __PRETTY_FUNCTION__ << "\n";
        _data.insert(_data.begin() + n, new Value(value));
    }

    void Array::insert(size_t n, const Raw &value)
    {
        _data.insert(_data.begin() + n, new Value(value));
    }

    void Array::clear()
    {
        _data.clear();
    }

    void Array::erase(size_t index)
    {
        _data.erase(_data.begin() + index);
    }

    void Array::erase(size_t start, size_t end)
    {
        _data.erase(_data.begin() + start, _data.begin() + end);
    }

    std::string Array::getType(size_t index) const
    {
        if (_data.size() <= index)
            throw std::out_of_range("JsonArray::getType index " + std::to_string(index) + " is out of bounds");

        switch (_data[index]->type)
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

    size_t Array::size() const
    {
        return _data.size();
    }

    bool Array::_isBottomLayer() const
    {
        for (const auto &entity : _data)
            if (entity->type == JsonEntityType::object || entity->type == JsonEntityType::array)
                return false;
        return true;
    }

    std::string Array::toStringF(const FormattingOptions &options, size_t tabs) const
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

            if (!isInline && options.firstBracketInNewline && entity->type != JsonEntityType::value && entity->type != JsonEntityType::array)
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

    Array::~Array()
    {
        for (const auto &entity : _data)
        {
            delete entity;
        }
    }
}
