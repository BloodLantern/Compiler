#pragma once

#include "ErrorCodes.hpp"

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
#define GETTER_NON_CONST_M(type, name, internalName) GETTER_NON_CONST(type, name, m_##name)

#define CHAR_FORMAT_ARGUMENT(formatArgumentIndex) "{" #formatArgumentIndex ":?} (0x{" #formatArgumentIndex ":X})"
// ReSharper enable CppClangTidyBugproneMacroParentheses

namespace Utils
{
    template <typename T>
    using Predicate = std::function<bool(T value)>;

    ATTRIBUTE_NORETURN
    void Exit(ErrorCodes code);
}

// ReSharper disable CppClangTidyCertDcl58Cpp
namespace std
{
    size_t replace_all(std::string& str, std::string_view pattern, std::string_view replacement);
}
// ReSharper enable CppClangTidyCertDcl58Cpp
