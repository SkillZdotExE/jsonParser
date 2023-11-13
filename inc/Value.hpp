#pragma once

#include "JsonEntity.hpp"
#include "Raw.hpp"
#include "string.hpp"

#include <iostream>

namespace json
{

    /// @brief Represents a JSON-value
    class Value : public JsonEntity
    {
    private:
        std::string _data;

    public:
        /// @brief Default Constructor
        Value();

        /// @brief Parameterized Constructor
        template <typename T>
        inline Value(const T &t)
            : Value()
        {
            set(t);
        }

        /// @brief Copy Constructor
        Value(const Value &other);

        /// @brief Assignment operator
        template <typename T>
        inline Value &operator=(const T &t)
        {
            set(t);
            return *this;
        }

        /// @brief Sets the value
        void fromString(const std::string &raw) override;

        /// @brief Gets the value as raw string
        std::string toString() const override;

        /// @brief Same as toString in this class
        std::string toStringF(const FormattingOptions &options = defaultJsonFormattingOptions, size_t tabs = 0) const override;

        /// @brief Gets the value as T.
        /// @tparam T Type of the entity. (Converted from string via operator<<(std::ostream&, std::string))
        template <typename T>
        inline T get() const
        {
            return strn::string_to<T>(_data);
        }

        /// @brief Sets the entity
        template <typename T>
        inline void set(const T &value)
        {
            std::ostringstream outStream;
            outStream << value;
            _data = outStream.str();
        }

        void set(const std::string &value);

        void set(const char *value);

        /// @brief [library internal] Always true for Value.
        bool _isBottomLayer() const override;
    };

}
