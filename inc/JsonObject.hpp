#pragma once
#include "JsonInterface.hpp"
#include "string.hpp"
#include <sstream>
#include <iostream>
#include <map>

namespace json
{
    class JsonArray;
    class JsonValue;

    /// @brief Represents a JSON-object
    class JsonObject : public JsonInterface
    {
    private:
        std::map<std::string, JsonInterface *> _data;

    public:
        /// @brief Default-Constructor
        JsonObject();
        /// @brief Constructor from a JSON-string
        JsonObject(std::string str);
        /// @brief Copy-Constructor
        JsonObject(const JsonObject &other);
        /// @brief Move-Constructor
        JsonObject(JsonObject &&other);
        /// @brief Copy-Assignment operator
        JsonObject &operator=(const JsonObject &other);
        /// @brief Move-Assignment operator
        JsonObject &operator=(JsonObject &&other);

        /// @brief Inserts an item into the object
        template <typename T>
        inline void insert(std::string key, T val)
        {
            remove(key);
            std::ostringstream inStr;
            inStr << val;
            _data.insert({key, JsonInterface::makeNew(inStr.str())});
        }

        /// @brief Constructor from individual key-value pairs
        template <typename T1, typename... Ts>
        inline JsonObject(std::string key, T1 value, Ts... ts)
            : JsonObject(ts...)
        {
            insert(key, value);
        }

        /// @brief Set the JSON-string the object represents
        void setString(std::string str) override;
        /// @brief Get the JSON-string the object represents
        [[nodiscard]] std::string getString() const override;

        /// @brief Reads a .json file
        /// @param path A path accepted by fstream
        bool readFromFile(std::string path);
        /// @brief Writes to a .json file
        /// @param path A path accepted by fstream
        bool writeToFile(std::string path, JsonFormattingOptions options = defaultJsonFormattingOptions) const;

        /// @brief Returns the value of keyas an array. Throws a std::runtime_error if the nth item is not an of another type.
        [[nodiscard]] JsonArray &A(std::string key);
        /// @brief Returns the value of key as an object. Throws a std::runtime_error if the nth item is not an of another type.
        [[nodiscard]] JsonObject &O(std::string key);
        /// @brief Returns the value of key as a value. Throws a std::runtime_error if the nth item is not an of another type.
        [[nodiscard]] JsonValue &V(std::string key);
        /// @brief Returns the value of key as a compact string.
        [[nodiscard]] std::string S(std::string key) const;

        /// @brief Returns the type of the value of key.
        /// @return "Array", "Object" or "Value"
        [[nodiscard]] std::string getType(std::string key) const;

        /// @brief Gets the value of key.
        /// @tparam T Type of the item. (Converted from string via operator<<(std::ostream&, std::string))
        template <typename T>
        [[nodiscard]] inline T get(std::string key) const
        {
            if (!_data.contains(key))
                return T();
            return strn::string_to<T>(_data.at(key)->getString());
        }

        /// @brief Removes an element from the array. Does nothing if the element doesn't exist.
        void remove(std::string key);

        /// @brief Returns the number of items in the array
        [[nodiscard]] size_t size() const override;
        /// @brief Returns true if the value of key is null
        [[nodiscard]] bool isNull(std::string key) const;
        /// @brief Returns true if the object contains key
        [[nodiscard]] bool contains(std::string key) const;
        /// @brief Returns true if the object is empty
        [[nodiscard]] bool isEmpty() const;

        /// @brief [library internal] Returns true when the array does not contain any arrays or objects.
        [[nodiscard]] bool _isBottomLayer() const override;

        /// @brief Returns a formatted string.
        /// @param tabs At how many tabs to start. Usually zero.
        /// @param options Formatting options.
        [[nodiscard]] std::string getStringF(const JsonFormattingOptions &options = defaultJsonFormattingOptions, size_t tabs = 0) const override;

        /// @brief Destructor
        ~JsonObject();
    };

}