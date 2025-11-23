#pragma once

#include "BuiltinType.hpp"

/// @brief Reserved language keywords.
enum class Keyword : uint8_t
{
    None,

    FirstType,
    BUILTIN_TYPE_ENUM_CONTENTS
    LastType,

    Const,
    Constexpr,
    Static,
    Extern,

    Class,
    Struct,
    Enum,

    Public,
    Private,
    Protected,
    Internal,
    Restricted,

    True,
    False,
    Null,

    If,
    Else,
    Switch,
    Case,
    Break,
    Return,
    While,
    Do,
    For,
    Continue,
    Goto,

    Virtual,
    Override,
    Abstract,
    Final,

    Namespace,
    Sizeof,
    Nameof,
    Offsetof,

    Import,
    Using,

    Out,

    Throw,
    Try,
    Catch,
    Finally,
};

/// @brief Checks whether the given @c Keyword is a type keyword, e.g., @c Keyword::Void, @c Keyword::Int, etc...
/// @param keyword The keyword to check
/// @return @c true if @p keyword is a type keyword, @c false otherwise
constexpr bool KeywordIsType(Keyword keyword);

constexpr BuiltinType KeywordToBuiltinType(Keyword keyword);
constexpr Keyword BuiltinTypeToKeyword(BuiltinType builtinType);

inline std::unordered_map KeywordStrings
{
    std::make_pair(Keyword::Void, "void"),
    std::make_pair(Keyword::Bool, "bool"),
    std::make_pair(Keyword::I8, "i8"),
    std::make_pair(Keyword::I16, "i16"),
    std::make_pair(Keyword::I32, "i32"),
    std::make_pair(Keyword::I64, "i64"),
    std::make_pair(Keyword::I128, "i128"),
    std::make_pair(Keyword::U8, "u8"),
    std::make_pair(Keyword::U16, "u16"),
    std::make_pair(Keyword::U32, "u32"),
    std::make_pair(Keyword::U64, "u64"),
    std::make_pair(Keyword::U128, "u128"),
    std::make_pair(Keyword::F16, "f16"),
    std::make_pair(Keyword::F32, "f32"),
    std::make_pair(Keyword::F64, "f64"),
    std::make_pair(Keyword::F128, "f128"),
    std::make_pair(Keyword::ISize, "iSize"),
    std::make_pair(Keyword::USize, "uSize"),
    std::make_pair(Keyword::C8, "c8"),
    std::make_pair(Keyword::C16, "c16"),
    std::make_pair(Keyword::C32, "c32"),
    std::make_pair(Keyword::S8, "s8"),
    std::make_pair(Keyword::S16, "s16"),
    std::make_pair(Keyword::S32, "s32"),

    std::make_pair(Keyword::Const, "const"),
    std::make_pair(Keyword::Constexpr, "constexpr"),
    std::make_pair(Keyword::Static, "static"),
    std::make_pair(Keyword::Extern, "extern"),

    std::make_pair(Keyword::Class, "class"),
    std::make_pair(Keyword::Struct, "struct"),
    std::make_pair(Keyword::Enum, "enum"),

    std::make_pair(Keyword::Public, "public"),
    std::make_pair(Keyword::Private, "private"),
    std::make_pair(Keyword::Protected, "protected"),
    std::make_pair(Keyword::Internal, "internal"),
    std::make_pair(Keyword::Restricted, "restricted"),

    std::make_pair(Keyword::True, "true"),
    std::make_pair(Keyword::False, "false"),
    std::make_pair(Keyword::Null, "null"),

    std::make_pair(Keyword::If, "if"),
    std::make_pair(Keyword::Else, "else"),
    std::make_pair(Keyword::Switch, "switch"),
    std::make_pair(Keyword::Case, "case"),
    std::make_pair(Keyword::Break, "break"),
    std::make_pair(Keyword::Return, "return"),
    std::make_pair(Keyword::While, "while"),
    std::make_pair(Keyword::Do, "do"),
    std::make_pair(Keyword::For, "for"),
    std::make_pair(Keyword::Continue, "continue"),
    std::make_pair(Keyword::Goto, "goto"),

    std::make_pair(Keyword::Virtual, "virtual"),
    std::make_pair(Keyword::Override, "override"),
    std::make_pair(Keyword::Abstract, "abstract"),
    std::make_pair(Keyword::Final, "final"),

    std::make_pair(Keyword::Namespace, "namespace"),
    std::make_pair(Keyword::Sizeof, "sizeof"),
    std::make_pair(Keyword::Nameof, "nameof"),
    std::make_pair(Keyword::Offsetof, "offsetof"),

    std::make_pair(Keyword::Import, "import"),
    std::make_pair(Keyword::Using, "using"),

    std::make_pair(Keyword::Out, "out"),

    std::make_pair(Keyword::Throw, "throw"),
    std::make_pair(Keyword::Try, "try"),
    std::make_pair(Keyword::Catch, "catch"),
    std::make_pair(Keyword::Finally, "finally"),
};

// Start of Keyword.inl

constexpr bool KeywordIsType(const Keyword keyword) { return keyword > Keyword::FirstType && keyword < Keyword::LastType; }

constexpr BuiltinType KeywordToBuiltinType(Keyword keyword)
{
    return static_cast<BuiltinType>(static_cast<std::underlying_type_t<Keyword>>(keyword)
        - static_cast<std::underlying_type_t<Keyword>>(Keyword::FirstType) - 1);
}

constexpr Keyword BuiltinTypeToKeyword(BuiltinType builtinType)
{
    return static_cast<Keyword>(static_cast<std::underlying_type_t<BuiltinType>>(builtinType)
        + static_cast<std::underlying_type_t<Keyword>>(Keyword::FirstType) + 1);
}
