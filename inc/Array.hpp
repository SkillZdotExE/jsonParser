#pragma once

#include "JsonEntity.hpp"
#include "string.hpp"
#include "Raw.hpp"
#include <vector>
#include <iostream>

namespace json
{
    class Object;
    class Value;

    /// @brief Represents a JSON-array
    class Array : public JsonEntity
    {
    private:
        std::vector<JsonEntity *> _data;

        template <typename T, typename... Ts>
        void _initialize(T t, Ts... ts)
        {
            push_back(t);
            _initialize(ts...);
        }

        void _initialize()
        {
        }

    public:
        /// @brief Default-Constructor
        Array();

        /// @brief Copy-Constructor
        Array(const Array &other);

        /// @brief Constructor from raw string
        /// @param raw
        Array(const std::string &raw);
        Array(const char *raw);

        /// @brief Constructor from individual elements
        template <typename T, typename... Ts>
        Array(const T &t, const Ts &...ts)
            : Array()
        {
            _data.reserve(sizeof...(Ts) + 1);
            _initialize(t, ts...);
        }

        /// @brief Move-Constructor
        Array(Array &&other);

        /// @brief Copy-Assignment operator
        Array &operator=(const Array &other);

        /// @brief Move-Assignment operator
        Array &operator=(Array &&other);

        /// @brief Changes the JSON-string the array represents
        void fromString(const std::string &raw) override;

        /// @brief Retrieves the JSON-String the array represents
        /// @note Use toStringF() for a formatted and more readable JSON-String
        [[nodiscard]] std::string toString() const override;

        /// @brief Returns a formatted string.
        /// @param tabs  Indendation of all lines. Usually Zero.
        /// @param options Formatting options.
        [[nodiscard]] std::string toStringF(const FormattingOptions &options = defaultJsonFormattingOptions, size_t tabs = 0) const override;

        /// @brief Returns the nth entity in the array as an array. Throws a std::runtime_error if the nth entity is of another type.
        [[nodiscard]] Array &A(size_t index);

        /// @brief Returns the nth entity in the array as an object. Throws a std::runtime_error if the nth entity is of another type.
        [[nodiscard]] Object &O(size_t index);

        /// @brief Returns the nth entity in the array as a value. Throws a std::runtime_error if the nth entity is of another type.
        [[nodiscard]] Value &V(size_t index);

        /// @brief Returns whether a value exists, is a bool and is true
        [[nodiscard]] bool getBool(size_t index) const;

        /// @brief Returns the value at key or an empty string if key doesnt exist or is not a string
        [[nodiscard]] std::string getString(size_t index) const;

        /// @brief Gets the nth entity in the array.
        /// @tparam T Type of the entity. (Converted from string via operator<<(std::ostream&, std::string))
        template <typename T>
        [[nodiscard]] inline T get(size_t index) const
        {
            if (_data.size() <= index)
                return T();
            return strn::string_to<T>(_data[index]->toString());
        }

        /// @brief Adds an entity to the back of the array
        template <typename T>
        inline void push_back(const T &value)
        {
            insert(_data.size(), value);
        }

        /// @brief Inserts an entity at the index n
        template <typename T>
        inline void insert(size_t n, const T &value)
        {
            std::cout << "[Default construction] " << __PRETTY_FUNCTION__ << "\n";
            std::ostringstream outStream;
            outStream << value;
            _data.insert(_data.begin() + n, JsonEntity::makeNew(outStream.str()));
        }

        void insert(size_t n, const Value &value);

        void insert(size_t n, const Raw &value);

        /// @brief Clears the entire array
        void clear();

        /// @brief Erases an entity at the given index
        void erase(size_t index);

        /// @brief Erases entities int the range [start; end)
        void erase(size_t start, size_t end);

        /// @brief Returns the type of the nth entity in the array.
        /// @return "Array", "Object" or "Value"
        [[nodiscard]] std::string getType(size_t index) const;

        /// @brief Returns the number of entities in the array
        [[nodiscard]] size_t size() const;

        /// @brief Deconstructor
        ~Array();

    private:
        /// @brief [library internal] Returns true if the array does not contain any arrays or objects.
        [[nodiscard]] bool _isBottomLayer() const override;

        friend JsonEntity;
        friend Object;
        friend Value;
    };
}
