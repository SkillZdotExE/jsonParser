#pragma once

#include "JsonEntity.hpp"
#include "string.hpp"

namespace json
{

    /// @brief Represents a JSON-value
    class JsonValue : public JsonEntity
    {
    private:
        std::string _data;

    public:
        /// @brief Constructor from string
        JsonValue(std::string raw);

        /// @brief Parameterized Constructor
        template <typename T>
        inline JsonValue(T t)
        {
            std::ostringstream outStream;
            outStream << t;
            _data = outStream.str();
        }

        /// @brief Assignment operator
        template <typename T>
        inline JsonValue &operator=(T t)
        {
            set(t);
            return *this;
        }

        /// @brief Sets the value
        void setString(std::string raw) override;
        /// @brief Gets the value as raw string
        std::string getString() const override;
        /// @brief Same as getString in this class
        std::string getStringF(const JsonFormattingOptions &options = defaultJsonFormattingOptions, size_t tabs = 0) const override;

        /// @brief Gets the value as T.
        /// @tparam T Type of the entity. (Converted from string via operator<<(std::ostream&, std::string))
        template <typename T>
        inline T get() const
        {
            return strn::string_to<T>(_data);
        }

        /// @brief Sets the entity
        template <typename T>
        inline void set(T val)
        {
            std::ostringstream outStream;
            outStream << val;
            _data = outStream.str();
        }

        /// @brief Returns the size of the object in JSON-entities
        size_t size() const override;

        /// @brief [library internal] Returns true when the array does not contain any arrays or objects.
        bool _isBottomLayer() const override;
    };
}