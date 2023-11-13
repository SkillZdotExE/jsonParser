#include "../inc/Object.hpp"
#include "../inc/Array.hpp"
#include "../inc/Value.hpp"
#include "../inc/string.hpp"
#include "../inc/makeEntity.hpp"
#include <iostream>
#include <sstream>
#include <fstream>

namespace json
{
    Object::Object()
        : JsonEntity(JsonEntityType::object)
    {
    }

    Object::Object(const std::string &raw)
        : Object()
    {
        fromString(raw);
    }

    Object::Object(const Object &other)
        : Object()
    {
        _data.clear();
        for (const auto &entity : other._data)
        {
            _data.insert({entity.first, json::make(*entity.second)});
        }
    }

    Object::Object(Object &&other)
        : Object()
    {
        _data.clear();
        for (auto &entity : other._data)
        {
            _data.insert(entity);
            entity.second = nullptr;
        }
    }

    void Object::insert(const std::string &key, const Raw &value)
    {
        remove(key);
        _data.insert({key, new Value(value)});
    }

    void Object::insert(const std::string &key, const Value &value)
    {
        remove(key);
        _data.insert({key, new Value(value)});
    }

    void Object::insert(const std::string &key, const Array &value)
    {
        remove(key);
        std::cout << "Before copy: " << value << "\n";
        std::cout << "After copy: " << Array(value) << "\n";
        _data.insert({key, new Array(value)});
    }

    void Object::insert(const std::string &key, const Object &value)
    {
        remove(key);
        _data.insert({key, new Object(value)});
    }

    Object &Object::operator=(const Object &other)
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
            _data.insert({entity.first, json::make(*entity.second)});
        }
        return *this;
    }

    Object &Object::operator=(Object &&other)
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

    void Object::fromString(const std::string &raw)
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
                    keyValPair.second = json::make(raw.substr(tokenStartIndex, currentIndex - tokenStartIndex));
                    _data.insert(keyValPair);
                    inColonComma = false;
                }
            }

            if (resetBackslash)
                backslash = false;
        }

        if (inColonComma)
        {
            keyValPair.second = json::make(raw.substr(tokenStartIndex, currentIndex - tokenStartIndex));
            _data.insert(keyValPair);
        }
    }

    std::string Object::toString() const
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

    bool Object::readFromFile(const std::string &path)
    {
        std::ifstream inFile(path);
        if (!inFile.is_open())
            return false;
        std::string inFileContents((std::istreambuf_iterator<char>(inFile)),
                                   (std::istreambuf_iterator<char>()));
        fromString(inFileContents);
        return true;
    }

    bool Object::writeToFile(const std::string &path, const FormattingOptions &options) const
    {
        std::ofstream outFile(path);
        if (!outFile.is_open())
            return false;
        outFile << toStringF(options);
        return true;
    }

    Array &Object::A(const std::string &key)
    {
        if (!_data.contains(key))
            throw std::out_of_range("JsonObject::A: there is no element with key " + key);
        if (_data[key]->type != JsonEntityType::array)
            throw std::runtime_error("JsonObject::A: Value of element with key " + key + " is not of type array");
        return dynamic_cast<Array &>(*_data[key]);
    }

    Object &Object::O(const std::string &key)
    {
        if (!_data.contains(key))
            throw std::out_of_range("JsonObject::O: there is no element with key " + key);
        if (_data[key]->type != JsonEntityType::object)
            throw std::runtime_error("JsonObject::O: Value of element with key " + key + " is not of type object");
        return dynamic_cast<Object &>(*_data[key]);
    }

    Value &Object::V(const std::string &key)
    {
        if (!_data.contains(key))
            throw std::out_of_range("JsonObject::V: there is no element with key " + key);
        if (_data[key]->type != JsonEntityType::value)
            throw std::runtime_error("JsonObject::V: Value of element with key " + key + " is not of type value");
        return dynamic_cast<Value &>(*_data[key]);
    }

    bool Object::getBool(const std::string &key) const
    {
        return _data.contains(key) && _data.at(key)->type == JsonEntityType::value && _data.at(key)->toString() == "true";
    }

    std::string Object::getString(const std::string &key) const
    {
        std::string ret;
        if (_data.contains(key) && _data.at(key)->type == JsonEntityType::value)
            ret = _data.at(key)->toString();
        else
            return std::string();

        if (ret.size() < 2 || ret[0] != '\"' || ret[ret.size() - 1] != '\"')
            return std::string();

        return ret.substr(1, ret.size() - 2);
    }

    std::string Object::getType(const std::string &key) const
    {
        if (!_data.contains(key))
            throw std::runtime_error("JsonObject::getType: there is no element with key " + key);

        switch (_data.at(key)->type)
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

    void Object::remove(const std::string &key)
    {
        if (!_data.contains(key))
            return;
        delete _data[key];
        _data.erase(key);
    }

    void Object::clear()
    {
        _data.clear();
    }

    size_t Object::size() const
    {
        return _data.size();
    }

    bool Object::isNull(const std::string &key) const
    {
        if (!_data.contains(key))
            throw std::runtime_error("JsonObject::isNull: there is no element with key " + key);
        return _data.at(key)->toString() == "null";
    }

    bool Object::contains(const std::string &key) const
    {
        return _data.contains(key);
    }

    bool Object::isEmpty() const
    {
        return _data.empty();
    }

    bool Object::_isBottomLayer() const
    {
        for (const auto &entity : _data)
            if (entity.second->type == JsonEntityType::object || entity.second->type == JsonEntityType::array)
                return false;
        return true;
    }

    std::string Object::toStringF(const FormattingOptions &options, size_t tabs) const
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

            if (!isInline && !isItemInline && options.firstBracketInNewline && entity.second->type != JsonEntityType::value)
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

    Object::~Object()
    {
        for (auto &entity : _data)
        {
            delete entity.second;
        }
    }
}