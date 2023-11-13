#pragma once

#include <string>
#include <cstdint>

namespace json
{
    /// @brief Options to control the formatting of generated JSON-strings
    struct FormattingOptions
    {
        /// @brief Sets the first Bracket of Objects and Arrays to be in the next line, at the same indentation as the key
        bool firstBracketInNewline = false;
        /// @brief Sets the last Bracket of Objects and Arrays to be in the next line, at the same indentation as the key
        bool lastBracketInNewline = true;
        /// @brief Inlines short arrays if the contain no objects or arrays
        bool inlineShortBottomLevelArrays = true;
        /// @brief Inlines short objects if the contain no objects or arrays
        bool inlineShortBottomLevelObjects = true;
        /// @brief Only inlines objects that are below this character limit (to avoid long lines)
        size_t maxLengthToInline = 64;
        /// @brief Whether to put a space before colons in objects
        bool spaceBeforeColon = false;
        /// @brief Whether to put a space after colons in objects
        bool spaceAfterColon = true;
        /// @brief Whether to put a space after commas in inline arrays
        bool spaceAfterComma = true;
        /// @brief Whether to put a space after opening and before closing brackets
        bool spaceAfterOpeningBeforeClosingBrackets = true;
        /// @brief Forces the entire output to be inline
        bool forceInline = false;
        /// @brief Keeps the output as compact as possible. Overwrites all other options.
        bool forceCompact = false;
        /// @brief How many spaces to use as a tab. Set to 0 to instead use a tab character.
        uint8_t tabSpaces = 4;

        // /// @brief Returns a example JSON-string to showcase the format
        // [[nodiscard]] std::string getFormattingExample() const;

    private:
        /// @brief [library internal] Returns a string representing the indentation
        [[nodiscard]] inline std::string _getTab(uint8_t tabs) const
        {
            std::string ret;
            while (tabs--)
                ret += tabSpaces == 0 ? std::string(1, '\t') : std::string(tabSpaces, ' ');
            return ret;
        }

        friend class JsonEntity;
        friend class Array;
        friend class Object;
        friend class Value;
    };

    extern const FormattingOptions defaultJsonFormattingOptions;

    /// @brief Base class for all JSON-entities
    class JsonEntity
    {
    public:
        const enum JsonEntityType : uint8_t {
            value,
            array,
            object
        } type;

        /// @brief Parameterized Constructor
        JsonEntity(JsonEntityType type_);

        /// @brief Set the JSON-string the object represents
        virtual void fromString(const std::string &raw) = 0;

        /// @brief Get the JSON-string the object represents
        [[nodiscard]] virtual std::string toString() const = 0;

        /// @brief Returns a formatted string.
        /// @param options Formatting options.
        /// @param tabs Indendation of all lines. Usually Zero.
        [[nodiscard]] virtual std::string toStringF(const FormattingOptions &options = defaultJsonFormattingOptions, size_t tabs = 0) const = 0;

        /// @brief Deconstructor
        virtual ~JsonEntity();

    protected:
        /// @brief [library internal] Returns true if the entity does not contain any arrays or objects.
        [[nodiscard]] virtual bool _isBottomLayer() const = 0;

        friend class Array;
        friend class Object;
        friend class Value;
    };
}

std::ostream &operator<<(std::ostream &os, const json::JsonEntity &entity);