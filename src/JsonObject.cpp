#include "../inc/JsonObject.hpp"
#include "../inc/JsonArray.hpp"
#include "../inc/JsonValue.hpp"
#include "../inc/string.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

namespace json
{
    JsonObject::JsonObject()
        : JsonEntity(JsonEntityType::object)
    {
    }

    JsonObject::JsonObject(std::string raw)
        : JsonEntity(JsonEntityType::object)
    {
        fromString(raw);
    }

    JsonObject::JsonObject(const JsonObject &other)
        : JsonObject()
    {
        _data.clear();
        for (const auto &entity : other._data)
        {
            _data.insert({entity.first, JsonEntity::makeNew(entity.second->toString())});
        }
    }

    JsonObject::JsonObject(JsonObject &&other)
        : JsonObject()
    {
        _data.clear();
        for (auto &entity : other._data)
        {
            _data.insert(entity);
            entity.second = nullptr;
        }
    }
    
    template <>
    inline void JsonObject::insert<std::string>(std::string key, std::string str)
    {
        remove(key);
        _data.insert({key, JsonEntity::makeNew("\"" + str + "\"")});
    }

    JsonObject &JsonObject::operator=(const JsonObject &other)
    {
        if (this == &other)
            return *this;

        for (auto &entity : _data)
        {
            delete entity.second;
        }

        _data.clear();

        for (const auto &entity : other._data)
        {
            _data.insert({entity.first, JsonEntity::makeNew(entity.second->toString())});
        }
        return *this;
    }

    JsonObject &JsonObject::operator=(JsonObject &&other)
    {
        for (auto &entity : _data)
        {
            delete entity.second;
        }

        _data.clear();

        for (auto &entity : other._data)
        {
            _data.insert(entity);
            entity.second = nullptr;
        }

        return *this;
    }

    void JsonObject::fromString(std::string raw)
    {
        for (auto &entity : _data)
        {
            delete entity.second;
        }
        _data.clear();

        size_t currentIndex = 1,
               tokenStartIndex = 1;
        uint16_t bracesLevel = 0,
                 bracketsLevel = 0;
        std::pair<std::string, JsonEntity *> keyValPair = {"", nullptr};
        bool inQuote = false,
             backslash = false,
             inColonComma = false;
        for (; currentIndex < raw.size() - 1; currentIndex++)
        {
            bool resetBackslash = backslash;

            switch (raw[currentIndex])
            {
            case '\\':
                backslash = !backslash;
                break;
            case '"':
                if (!backslash && !inColonComma && bracesLevel == 0 && bracketsLevel == 0)
                {
                    if (!inQuote)
                        tokenStartIndex = currentIndex;
                    else
                    {
                        keyValPair.first = raw.substr(tokenStartIndex + 1, currentIndex - tokenStartIndex - 1);
                    }
                }
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
            case ':':
                if (!inQuote && bracesLevel == 0 && bracketsLevel == 0)
                {
                    inColonComma = true;
                    tokenStartIndex = currentIndex + 1;
                }
                break;
            case ',':
                if (inColonComma && !inQuote && bracesLevel == 0 && bracketsLevel == 0)
                {
                    keyValPair.second = JsonEntity::makeNew(raw.substr(tokenStartIndex, currentIndex - tokenStartIndex));
                    _data.insert(keyValPair);
                    inColonComma = false;
                }
            }

            if (resetBackslash)
                backslash = false;
        }

        if (inColonComma)
        {
            keyValPair.second = JsonEntity::makeNew(raw.substr(tokenStartIndex, currentIndex - tokenStartIndex));
            _data.insert(keyValPair);
        }
    }

    std::string JsonObject::toString() const
    {
        std::ostringstream outStream;

        outStream << '{';

        size_t i = 0;
        for (auto &entity : _data)
        {
            i++;
            outStream << '\"' << entity.first << "\":" << entity.second->toString();
            if (i < _data.size())
                outStream << ',';
        }

        outStream << '}';

        return outStream.str();
    }

    bool JsonObject::readFromFile(std::string path)
    {
        std::ifstream inFile(path);
        if (!inFile.is_open())
            return false;
        std::string inFileContents((std::istreambuf_iterator<char>(inFile)),
                                   (std::istreambuf_iterator<char>()));
        fromString(inFileContents);
        return true;
    }

    bool JsonObject::writeToFile(std::string path, JsonFormattingOptions options) const
    {
        std::ofstream outFile(path);
        if (!outFile.is_open())
            return false;
        outFile << toStringF(options);
        return true;
    }

    JsonArray &JsonObject::A(std::string key)
    {
        if (!_data.contains(key))
            throw std::out_of_range("JsonObject::A: there is no element with key " + key);
        if (_data[key]->_getType() != JsonEntityType::array)
            throw std::runtime_error("JsonObject::A: Value of element with key " + key + " is not of type array");
        return dynamic_cast<JsonArray &>(*_data[key]);
    }

    JsonObject &JsonObject::O(std::string key)
    {
        if (!_data.contains(key))
            throw std::out_of_range("JsonObject::O: there is no element with key " + key);
        if (_data[key]->_getType() != JsonEntityType::object)
            throw std::runtime_error("JsonObject::O: Value of element with key " + key + " is not of type object");
        return dynamic_cast<JsonObject &>(*_data[key]);
    }

    JsonValue &JsonObject::V(std::string key)
    {
        if (!_data.contains(key))
            throw std::out_of_range("JsonObject::V: there is no element with key " + key);
        if (_data[key]->_getType() != JsonEntityType::value)
            throw std::runtime_error("JsonObject::V: Value of element with key " + key + " is not of type value");
        return dynamic_cast<JsonValue &>(*_data[key]);
    }

    std::string JsonObject::S(std::string key) const
    {
        if (!_data.contains(key))
            throw std::out_of_range("JsonObject::S: there is no element with key " + key);
        if (_data.at(key)->_getType() != JsonEntityType::value)
            throw std::runtime_error("JsonObject::S: Value of element with key " + key + " is not of type string");
        return _data.at(key)->toString();
    }

    bool JsonObject::getBool(std::string key) const
    {
        return _data.contains(key) && _data.at(key)->_getType() == JsonEntityType::value && _data.at(key)->toString() == "true";
    }

    std::string JsonObject::getString(std::string key) const
    {
        std::string ret;
        if (_data.contains(key) && _data.at(key)->_getType() == JsonEntityType::value)
            ret = _data.at(key)->toString();
        else
            return std::string();

        if (ret.size() < 2 || ret[0] != '\"' || ret[ret.size() - 1] != '\"')
            return std::string();

        return ret.substr(1, ret.size() - 2);
    }

    std::string JsonObject::getType(std::string key) const
    {
        if (!_data.contains(key))
            throw std::runtime_error("JsonObject::getType: there is no element with key " + key);

        switch (_data.at(key)->_getType())
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

    void JsonObject::remove(std::string key)
    {
        if (!_data.contains(key))
            return;
        delete _data[key];
        _data.erase(key);
    }

    size_t JsonObject::size() const
    {
        return _data.size();
    }

    bool JsonObject::isNull(std::string key) const
    {
        if (!_data.contains(key))
            throw std::runtime_error("JsonObject::isNull: there is no element with key " + key);
        return _data.at(key)->toString() == "null";
    }

    bool JsonObject::contains(std::string key) const
    {
        return _data.contains(key);
    }

    bool JsonObject::isEmpty() const
    {
        return _data.empty();
    }

    bool JsonObject::_isBottomLayer() const
    {
        for (const auto &entity : _data)
        {
            if (entity.second->_getType() == JsonEntityType::object || entity.second->_getType() == JsonEntityType::array)
                return false;
        }
        return true;
    }

    std::string JsonObject::toStringF(const JsonFormattingOptions &options, size_t tabs) const
    {
        if (options.forceCompact)
            return toString();
        std::ostringstream outStream;
        bool isInline = options.forceInline || (options.inlineShortBottomLevelObjects && _isBottomLayer() && toString().size() < options.maxLengthToInline);

        outStream << '{';

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

            outStream << '\"' << entity.first << '\"'
                      << (options.spaceBeforeColon ? " " : "")
                      << ':'
                      << (options.spaceAfterColon ? " " : "");

            const bool isItemInline = options.forceInline || (options.inlineShortBottomLevelObjects && entity.second->_isBottomLayer() && entity.second->toString().size() < options.maxLengthToInline);

            if (!isInline && !isItemInline && options.firstBracketInNewline && entity.second->_getType() != JsonEntityType::value)
                outStream << '\n'
                          << options._getTab(tabs);
            outStream << entity.second->toStringF(options, tabs);

            if (i < _data.size())
            {
                outStream << ',';
                if (options.spaceAfterComma)
                    outStream << ' ';
            }

            if (!isInline && (options.lastBracketInNewline || i < _data.size()))
                outStream << '\n';
        }

        if (!isInline && options.lastBracketInNewline)
            outStream << (tabs != 0 ? options._getTab(tabs - 1) : "");

        if (isInline && options.spaceAfterOpeningBeforeClosingBrackets)
            outStream << ' ';

        outStream << '}';

        return outStream.str();
    }

    JsonObject::~JsonObject()
    {
        for (auto &entity : _data)
        {
            delete entity.second;
        }
    }

}