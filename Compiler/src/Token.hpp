#pragma once

enum class TokenType : uint8_t
{
    None,

    EndOfFile,
    Semicolon,
    Comma,

    Identifier,
    Keyword,

    ParenthesisOpen,
    ParenthesisClose,
    SquareBracketOpen,
    SquareBracketClose,
    CurlyBraceOpen,
    CurlyBraceClose,

    OperatorAssign,

    OperatorScopeResolution,

    OperatorDot,
    OperatorArrow,

    OperatorWideArrow,

    OperatorPlus,
    OperatorMinus,
    OperatorStar,
    OperatorSlash,
    OperatorPercent,

    OperatorIncrement,
    OperatorDecrement,

    OperatorEquals,
    OperatorNotEquals,
    OperatorLess,
    OperatorLessEquals,
    OperatorGreater,
    OperatorGreaterEquals,

    OperatorLogicalAnd,
    OperatorLogicalOr,
    OperatorLogicalNot,

    OperatorBinaryAnd,
    OperatorBinaryOr,
    OperatorBinaryXor,
    OperatorBinaryNot,

    OperatorBitshiftLeft,
    OperatorBitshiftRight,

    OperatorQuestionMark,
    OperatorNullCoalescing,
    OperatorColon,

    OperatorPlusEquals,
    OperatorMinusEquals,
    OperatorStarEquals,
    OperatorSlashEquals,
    OperatorPercentEquals,
    OperatorBinaryAndEquals,
    OperatorBinaryOrEquals,
    OperatorBinaryXorEquals,
    OperatorBinaryNotEquals,
    OperatorBitshiftLeftEquals,
    OperatorBitshiftRightEquals,
    OperatorNullCoalescingEquals,

    LiteralInteger,
    LiteralCharacter,
    LiteralString,
};

constexpr std::array<std::pair<std::string_view, TokenType>, 50> SimpleTokenList
{
    std::make_pair("<<=", TokenType::OperatorBitshiftLeftEquals),
    std::make_pair(">>=", TokenType::OperatorBitshiftRightEquals),
    std::make_pair("??=", TokenType::OperatorNullCoalescingEquals),  // NOLINT(clang-diagnostic-trigraphs)

    std::make_pair("++", TokenType::OperatorIncrement),
    std::make_pair("--", TokenType::OperatorDecrement),

    std::make_pair("==", TokenType::OperatorEquals),
    std::make_pair("!=", TokenType::OperatorNotEquals),
    std::make_pair("<", TokenType::OperatorLess),
    std::make_pair("<=", TokenType::OperatorLessEquals),
    std::make_pair(">", TokenType::OperatorGreater),
    std::make_pair(">=", TokenType::OperatorGreaterEquals),

    std::make_pair("&&", TokenType::OperatorLogicalAnd),
    std::make_pair("||", TokenType::OperatorLogicalOr),

    std::make_pair("<<", TokenType::OperatorBitshiftLeft),
    std::make_pair(">>", TokenType::OperatorBitshiftRight),

    std::make_pair("??", TokenType::OperatorNullCoalescing),

    std::make_pair("+=", TokenType::OperatorPlusEquals),
    std::make_pair("-=", TokenType::OperatorMinusEquals),
    std::make_pair("*=", TokenType::OperatorStarEquals),
    std::make_pair("/=", TokenType::OperatorSlashEquals),
    std::make_pair("%=", TokenType::OperatorPercentEquals),
    std::make_pair("&=", TokenType::OperatorBinaryAndEquals),
    std::make_pair("|=", TokenType::OperatorBinaryOrEquals),
    std::make_pair("^=", TokenType::OperatorBinaryXorEquals),
    std::make_pair("~=", TokenType::OperatorBinaryNotEquals),

    std::make_pair("::", TokenType::OperatorScopeResolution),
    std::make_pair("->", TokenType::OperatorArrow),
    std::make_pair("=>", TokenType::OperatorWideArrow),

    std::make_pair(".", TokenType::OperatorDot),

    std::make_pair("!", TokenType::OperatorLogicalNot),

    std::make_pair(";", TokenType::Semicolon),
    std::make_pair(",", TokenType::Comma),

    std::make_pair("(", TokenType::ParenthesisOpen),
    std::make_pair(")", TokenType::ParenthesisClose),
    std::make_pair("[", TokenType::SquareBracketOpen),
    std::make_pair("]", TokenType::SquareBracketClose),
    std::make_pair("{", TokenType::CurlyBraceOpen),
    std::make_pair("}", TokenType::CurlyBraceClose),

    std::make_pair("=", TokenType::OperatorAssign),

    std::make_pair("+", TokenType::OperatorPlus),
    std::make_pair("-", TokenType::OperatorMinus),
    std::make_pair("*", TokenType::OperatorStar),
    std::make_pair("/", TokenType::OperatorSlash),
    std::make_pair("%", TokenType::OperatorPercent),

    std::make_pair("&", TokenType::OperatorBinaryAnd),
    std::make_pair("|", TokenType::OperatorBinaryOr),
    std::make_pair("^", TokenType::OperatorBinaryXor),
    std::make_pair("~", TokenType::OperatorBinaryNot),

    std::make_pair("?", TokenType::OperatorQuestionMark),
    std::make_pair(":", TokenType::OperatorColon),
};

enum class Keyword : uint8_t
{
    None,

    Void,
    Bool,
    Char,
    SByte,
    Byte,
    Short,
    UShort,
    Int,
    UInt,
    Long,
    ULong,
    String,

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

constexpr std::array<std::string_view, magic_enum::enum_count<Keyword>() - 1> KeywordList
{
    "void",
    "bool",
    "char",
    "sbyte",
    "byte",
    "short",
    "ushort",
    "int",
    "uint",
    "long",
    "ulong",
    "string",

    "const",
    "constexpr",
    "static",
    "extern",

    "class",
    "struct",
    "enum",

    "public",
    "private",
    "protected",
    "internal",
    "restricted",

    "true",
    "false",
    "null",

    "if",
    "else",
    "switch",
    "case",
    "break",
    "return",
    "while",
    "do",
    "for",
    "continue",
    "goto",

    "virtual",
    "override",
    "abstract",
    "final",

    "namespace",
    "sizeof",
    "nameof",
    "offsetof",

    "import",
    "using",

    "out",

    "throw",
    "try",
    "catch",
    "finally",
};

std::unordered_map<TokenType, size_t> OperatorPrecedences
{
    /*std::make_pair(TokenType::OperatorScopeResolution, 1),

    std::make_pair(TokenType::OperatorIncrement, 2),
    std::make_pair(TokenType::OperatorDecrement, 2), // TODO - Operator enum I guess
    std::make_pair(TokenType::OperatorDot, 2),
    std::make_pair(TokenType::OperatorArrow, 2),

    std::make_pair(TokenType::OperatorMinus, 3),
    std::make_pair(TokenType::OperatorLogicalNot, 1),
    std::make_pair(TokenType::OperatorBinaryNot, 1),
    std::make_pair(TokenType::OperatorStar, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),
    std::make_pair(TokenType::OperatorScopeResolution, 1),*/
};

using TokenValue = std::variant<std::string, int, char, Keyword>;

struct Token
{
    TokenType type;
    std::optional<TokenValue> value;

    const InputFile* inputFile;
    Position position;
};
