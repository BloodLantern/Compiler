#pragma once

#include "../Common.hpp"
#include "../ErrorCodes.hpp"
#include "../Position.hpp"

// ReSharper disable CppClangTidyBugproneMacroParentheses
/// @brief Creates default copy and move operations for a given @p type.
///
/// This macro should be used for any type that defines at least one of:
/// - A non-default destructor (this includes @code = default @endcode implementations),
/// - A non-default copy constructor
/// - A non-default move constructor
/// - A non-default copy operator
/// - A non-default move operator
///
/// It should be used right after the last constructor declaration:
/// @code
/// class A
/// {
/// public:
///     A();
///     A(int);
///
///     DEFAULT_COPY_MOVE_OPERATIONS(A)
/// };
/// @endcode
/// Or after the destructor declaration if there is one:
/// @code
/// class A
/// {
/// public:
///     A();
///     A(int);
///     ~A();
///
///     DEFAULT_COPY_MOVE_OPERATIONS(A)
/// };
/// @endcode
///
/// @param type The type to default the copy and move operations of.
#define DEFAULT_COPY_MOVE_OPERATIONS(type) \
    type(const type& other) noexcept = default; \
    type(type&& other) noexcept = default; \
    type& operator=(const type& other) noexcept = default; \
    type& operator=(type&& other) noexcept = default;

/// @brief Deletes the copy and move operations of a given @p type.
/// @param type The type to delete the copy and move operations of.
/// @see @ref DEFAULT_COPY_MOVE_OPERATIONS for usage.
#define DELETE_COPY_MOVE_OPERATIONS(type) \
    type(const type& other) = delete; \
    type(type&& other) = delete; \
    type& operator=(const type& other) = delete; \
    type& operator=(type&& other) = delete;

/// @brief Defines binary flag operators for an enum type.
/// @param enumName The enum to define the flag operations of.
/// @note This macro **must** be used in the global namespace, as it declares a template specialization of a @c magic_enum struct.
#define ENUM_FLAGS(enumName) \
    static_assert(std::is_enum_v<enumName>, "enumName must be a valid enum type"); \
    \
    inline std::underlying_type_t<enumName> operator&(const enumName lhs, const enumName rhs) { return static_cast<std::underlying_type_t<enumName>>(lhs) & static_cast<std::underlying_type_t<enumName>>(rhs); } \
    inline enumName operator|(const enumName lhs, const enumName rhs) { return static_cast<enumName>(static_cast<std::underlying_type_t<enumName>>(lhs) | static_cast<std::underlying_type_t<enumName>>(rhs)); } \
    inline std::underlying_type_t<enumName> operator&(const std::underlying_type_t<enumName> lhs, const enumName rhs) { return lhs & static_cast<std::underlying_type_t<enumName>>(rhs); } \
    inline std::underlying_type_t<enumName> operator|(const std::underlying_type_t<enumName> lhs, const enumName rhs) { return lhs | static_cast<std::underlying_type_t<enumName>>(rhs); } \
    inline std::underlying_type_t<enumName> operator&(const enumName lhs, const std::underlying_type_t<enumName> rhs) { return static_cast<std::underlying_type_t<enumName>>(lhs) & rhs; } \
    inline std::underlying_type_t<enumName> operator|(const enumName lhs, const std::underlying_type_t<enumName> rhs) { return static_cast<std::underlying_type_t<enumName>>(lhs) | rhs; } \
    inline std::underlying_type_t<enumName>& operator&=(std::underlying_type_t<enumName>& lhs, const enumName rhs) { return lhs = lhs & rhs; } \
    inline std::underlying_type_t<enumName>& operator|=(std::underlying_type_t<enumName>& lhs, const enumName rhs) { return lhs = lhs | rhs; } \
    inline enumName& operator&=(enumName& lhs, const enumName rhs) { return lhs = static_cast<enumName>(lhs & rhs); } \
    inline enumName& operator|=(enumName& lhs, const enumName rhs) { return lhs = static_cast<enumName>(lhs | rhs); } \
    \
    template <> \
    struct magic_enum::customize::enum_range<enumName> \
    { \
        static constexpr bool is_flags = true; \
    };

/// @brief Defines a getter for the field @p internalName, of type @p type, with the name @c Get##name
#define GETTER(type, name, internalName) ATTRIBUTE_NODISCARD type Get##name() const noexcept { return internalName; }
/// @brief Defines a getter for the field @c m_##name, of type @p type, with the name @c Get##name
#define GETTER_M(type, name) GETTER(type, name, m_##name)

/// @brief Defines a static getter for the field @p internalName, of type @p type, with the name @c Get##name
#define STATIC_GETTER(type, name, internalName) ATTRIBUTE_NODISCARD static type Get##name() noexcept { return internalName; }
/// @brief Defines a static getter for the field @c m_##name, of type @p type, with the name @c Get##name
#define STATIC_GETTER_M(type, name, internalName) STATIC_GETTER(type, name, m_##name)

/// @brief Defines getters for the field @p internalName, of type @p type, with the name @c Get##name.
/// One getter is @c const and returns a @c const value.
#define GETTER_NON_CONST(type, name, internalName) \
    ATTRIBUTE_NODISCARD type Get##name() noexcept { return internalName; } \
    GETTER(const type, name, internalName)
/// @brief Defines getters for the field @p internalName, of type @p type, with the name @c Get##name.
/// One getter is @c const and returns a @c const value.
#define GETTER_NON_CONST_M(type, name) GETTER_NON_CONST(type, name, m_##name)

#define CHAR_FORMAT_ARGUMENT(formatArgumentIndex) "{" #formatArgumentIndex ":?} (0x{" #formatArgumentIndex ":X})"

#define RETURN_IF_NULL(value) if (!value) return nullptr

#define ANSI_ESC "\x1B"
#define ANSI_OSC ANSI_ESC "]"
#define ANSI_OSC8 ANSI_OSC "8"
#define ANSI_TERM ANSI_ESC "\\"
#define ANSI_CSI ANSI_ESC "["
#define ANSI_STYLE(style) ANSI_CSI style "m"
#define ANSI_COLOR_BLUE ANSI_STYLE("34")
#define ANSI_STYLE_RESET ANSI_STYLE("0")
#define ANSI_LINK(link, text) ANSI_OSC8 ";;" link ANSI_TERM text ANSI_OSC8 ";;" ANSI_TERM
#define ANSI_FILE_LINK(filepath, text) ANSI_OSC8 ";;file:///" filepath ANSI_TERM text ANSI_OSC8 ";;" ANSI_TERM
// ReSharper enable CppClangTidyBugproneMacroParentheses

namespace Utils
{
    template <typename T>
    using Predicate = std::function<bool(T value)>;

    template <typename T>
    concept Enum = std::is_enum_v<T>;

    ATTRIBUTE_NORETURN
    void Exit(ErrorCodes code);

    template <typename... Args>
    int SystemCommand(const std::string& format, Args&&... args);

    std::string FormatFileLink(const std::filesystem::path& filepath, const Position& position);
}

// ReSharper disable CppClangTidyCertDcl58Cpp
namespace std
{
    size_t replace_all(std::string& str, std::string_view pattern, std::string_view replacement); // NOLINT(*-dcl58-cpp)
}
// ReSharper enable CppClangTidyCertDcl58Cpp

namespace llvm
{
    unsigned getTypeBitSize(const Value* value);
}

// Start of Utils.inl

namespace Utils
{
    template <typename ... Args>
    int SystemCommand(const std::string& format, Args&&... args)
    {
        return std::system(std::vformat(format, std::make_format_args(args...)).c_str());
    }
}

/// @brief @c std::formatter template specialization for any @c enum type.
/// @details This prints the @c enum value as a string using @c magic_enum::enum_name(), or as an integer if the @c d format is used.
template <Utils::Enum EnumT>
struct std::formatter<EnumT> // NOLINT(*-dcl58-cpp)
{
    /// @brief Parses the input formatting options.
    constexpr std::format_parse_context::iterator parse(std::format_parse_context& ctx)
    {
        auto it = ctx.begin();
        if (it == ctx.end())
            return it;

        if (*it == 'd')
        {
            m_PrintAsInteger = true;
            it++;
        }

        if (*it != '}')
            throw std::format_error{"Invalid format arguments for enum"};

        return it;
    }

    /// @brief Formats a string using the given @c enum value, according to the given options in the parse function.
    template <typename FormatContext>
    FormatContext::iterator format(const EnumT& e, FormatContext& ctx) const
    {
        if (m_PrintAsInteger)
            return std::format_to(ctx.out(), "{}", static_cast<std::underlying_type_t<EnumT>>(e));
        return std::format_to(ctx.out(), "{}", magic_enum::enum_name(e));
    }

private:
    bool m_PrintAsInteger = false;
};

/// @file Formatter.hpp
/// @brief Defines template specializations of @c std::formatter for external types.
///
/// This file also defines the Formattable concept, as it seems it isn't included in the C++23 MSVC STL.
///
/// In this file, we don't follow our conventions on some things because it is mostly an extension of the STL.
///
/// @see <a href="https://en.cppreference.com/w/cpp/utility/format/formatter">std::formatter</a>
/// @see <a href="https://en.cppreference.com/w/cpp/utility/format/formattable">std::formattable</a>

/// @brief @c std::formatter template specialization for the @c std::filesystem::path type.
/// @see <a href="https://en.cppreference.com/w/cpp/filesystem/path">std::filesystem::path</a>
template <>
struct std::formatter<std::filesystem::path>
{
    /// @brief Parses the input formatting options.
    constexpr std::format_parse_context::iterator parse(std::format_parse_context& ctx)
    {
        const auto it = ctx.begin();
        if (it == ctx.end())
            return it;

        if (*it != '}')
            throw std::format_error{"Invalid format arguments for std::filesystem::path"};

        return it;
    }

    /// @brief Formats a string using the given instance of @c std::filesystem::path, according to the given options in the parse function.
    template <class FormatContext>
    FormatContext::iterator format(const std::filesystem::path& path, FormatContext& ctx) const
    {
        return std::format_to(ctx.out(), "{}", path.string());
    }
};
