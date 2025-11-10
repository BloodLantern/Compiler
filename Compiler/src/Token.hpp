#pragma once

#include "InputFile.hpp"
#include "Position.hpp"

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

    LiteralInteger,
    LiteralCharacter,
    LiteralString,

    FirstOperator,

    Equals,

    DoubleColon,

    Dot,
    MinusGreaterThan,

    EqualsGreaterThan,

    Plus,
    Minus,
    Star,
    Slash,
    Percent,

    DoublePlus,
    DoubleMinus,

    DoubleEquals,
    ExclamationMarkEquals,
    LessThan,
    LessThanEquals,
    GreaterThan,
    GreaterThanEquals,

    DoubleAmpersand,
    DoubleVerticalBar,
    ExclamationMark,

    Ampersand,
    VerticalBar,
    Caret,
    Tilde,

    DoubleLessThan,
    DoubleGreaterThan,

    QuestionMark,
    DoubleQuestionMark,
    Colon,

    PlusEquals,
    MinusEquals,
    StarEquals,
    SlashEquals,
    PercentEquals,
    AmpersandEquals,
    VerticalBarEquals,
    CaretEquals,
    DoubleLessThanEquals,
    DoubleGreaterThanEquals,
    DoubleQuestionMarkEquals,

    LastOperator,
};

/// @brief Checks whether the given @c TokenType is an operator token, i.e.,
/// a token that can be converted to an @c Operator using @c BaseOperatorConversionLookup.
/// @param tokenType The token type to check
/// @return @c true if @p tokenType is an operator token, @c false otherwise
constexpr bool IsTokenOperator(TokenType tokenType);

constexpr std::array SimpleTokenList
{
    std::make_pair("<<=", TokenType::DoubleLessThanEquals),
    std::make_pair(">>=", TokenType::DoubleGreaterThanEquals),
    std::make_pair(R"(??=)", TokenType::DoubleQuestionMarkEquals),

    std::make_pair("++", TokenType::DoublePlus),
    std::make_pair("--", TokenType::DoubleMinus),

    std::make_pair("==", TokenType::DoubleEquals),
    std::make_pair("!=", TokenType::ExclamationMarkEquals),
    std::make_pair("<", TokenType::LessThan),
    std::make_pair("<=", TokenType::LessThanEquals),
    std::make_pair(">", TokenType::GreaterThan),
    std::make_pair(">=", TokenType::GreaterThanEquals),

    std::make_pair("&&", TokenType::DoubleAmpersand),
    std::make_pair("||", TokenType::DoubleVerticalBar),

    std::make_pair("<<", TokenType::DoubleLessThan),
    std::make_pair(">>", TokenType::DoubleGreaterThan),

    std::make_pair("??", TokenType::DoubleQuestionMark),

    std::make_pair("+=", TokenType::PlusEquals),
    std::make_pair("-=", TokenType::MinusEquals),
    std::make_pair("*=", TokenType::StarEquals),
    std::make_pair("/=", TokenType::SlashEquals),
    std::make_pair("%=", TokenType::PercentEquals),
    std::make_pair("&=", TokenType::AmpersandEquals),
    std::make_pair("|=", TokenType::VerticalBarEquals),
    std::make_pair("^=", TokenType::CaretEquals),

    std::make_pair("::", TokenType::DoubleColon),
    std::make_pair("->", TokenType::MinusGreaterThan),
    std::make_pair("=>", TokenType::EqualsGreaterThan),

    std::make_pair(".", TokenType::Dot),

    std::make_pair("!", TokenType::ExclamationMark),

    std::make_pair(";", TokenType::Semicolon),
    std::make_pair(",", TokenType::Comma),

    std::make_pair("(", TokenType::ParenthesisOpen),
    std::make_pair(")", TokenType::ParenthesisClose),
    std::make_pair("[", TokenType::SquareBracketOpen),
    std::make_pair("]", TokenType::SquareBracketClose),
    std::make_pair("{", TokenType::CurlyBraceOpen),
    std::make_pair("}", TokenType::CurlyBraceClose),

    std::make_pair("=", TokenType::Equals),

    std::make_pair("+", TokenType::Plus),
    std::make_pair("-", TokenType::Minus),
    std::make_pair("*", TokenType::Star),
    std::make_pair("/", TokenType::Slash),
    std::make_pair("%", TokenType::Percent),

    std::make_pair("&", TokenType::Ampersand),
    std::make_pair("|", TokenType::VerticalBar),
    std::make_pair("^", TokenType::Caret),
    std::make_pair("~", TokenType::Tilde),

    std::make_pair("?", TokenType::QuestionMark),
    std::make_pair(":", TokenType::Colon),
};

/// @brief Reserved language keywords.
enum class Keyword : uint8_t
{
    None,

    FirstType,

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
constexpr bool IsKeywordType(Keyword keyword);

inline std::unordered_map KeywordStrings
{
    std::make_pair(Keyword::Void, "void"),
    std::make_pair(Keyword::Bool, "bool"),
    std::make_pair(Keyword::Char, "char"),
    std::make_pair(Keyword::SByte, "sbyte"),
    std::make_pair(Keyword::Byte, "byte"),
    std::make_pair(Keyword::Short, "short"),
    std::make_pair(Keyword::UShort, "ushort"),
    std::make_pair(Keyword::Int, "int"),
    std::make_pair(Keyword::UInt, "uint"),
    std::make_pair(Keyword::Long, "long"),
    std::make_pair(Keyword::ULong, "ulong"),
    std::make_pair(Keyword::String, "string"),

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

/// @brief Operators sorted by precedence, with a higher precedence being executed first.
enum class Operator : uint8_t
{
    None,

    Precedence0,
    Ternary,
    Assignment,
    AddEquals,
    SubtractEquals,
    MultiplyEquals,
    DivideEquals,
    ModuloEquals,
    BitshiftLeftEquals,
    BitshiftRightEquals,
    BinaryAndEquals,
    BinaryXorEquals,
    BinaryOrEquals,
    NullCoalescingEquals,
    WideArrow,

    Precedence1,
    NullCoalescing,

    Precedence2,
    LogicalOr,

    Precedence3,
    LogicalAnd,

    Precedence4,
    BinaryOr,

    Precedence5,
    BinaryXor,

    Precedence6,
    BinaryAnd,

    Precedence7,
    Equals,
    NotEquals,

    Precedence8,
    Less,
    LessEquals,
    Greater,
    GreaterEquals,

    Precedence9,
    BitshiftLeft,
    BitshiftRight,

    Precedence10,
    Addition,
    Subtraction,

    Precedence11,
    Multiplication,
    Division,
    Modulo,

    Precedence12,
    PointerToMemberDot,
    PointerToMemberArrow,

    Precedence13,
    PreIncrement,
    PreDecrement,
    UnaryMinus,
    LogicalNot,
    BinaryNot,
    Cast,
    Dereference,
    AddressOf,

    Precedence14,
    PostIncrement,
    PostDecrement,
    FunctionCall,
    Subscript,
    MemberAccessDot,
    MemberAccessArrow,

    Precedence15,
    ScopeResolution,
};

constexpr int GetOperatorPrecedence(Operator op);

inline std::unordered_map BaseOperatorConversionLookup
{
    std::make_pair(TokenType::Equals, Operator::Assignment),

    std::make_pair(TokenType::DoubleColon, Operator::ScopeResolution),

    std::make_pair(TokenType::Dot, Operator::MemberAccessDot),
    std::make_pair(TokenType::MinusGreaterThan, Operator::MemberAccessArrow),

    std::make_pair(TokenType::EqualsGreaterThan, Operator::WideArrow),

    std::make_pair(TokenType::Plus, Operator::Addition),
    std::make_pair(TokenType::Minus, Operator::Subtraction),
    std::make_pair(TokenType::Star, Operator::Multiplication),
    std::make_pair(TokenType::Slash, Operator::Division),
    std::make_pair(TokenType::Percent, Operator::Modulo),

    std::make_pair(TokenType::DoublePlus, Operator::PreIncrement),
    std::make_pair(TokenType::DoubleMinus, Operator::PreDecrement),

    std::make_pair(TokenType::DoubleEquals, Operator::Equals),
    std::make_pair(TokenType::ExclamationMarkEquals, Operator::NotEquals),
    std::make_pair(TokenType::LessThan, Operator::Less),
    std::make_pair(TokenType::LessThanEquals, Operator::LessEquals),
    std::make_pair(TokenType::GreaterThan, Operator::Greater),
    std::make_pair(TokenType::GreaterThanEquals, Operator::GreaterEquals),

    std::make_pair(TokenType::DoubleAmpersand, Operator::LogicalAnd),
    std::make_pair(TokenType::DoubleVerticalBar, Operator::LogicalOr),
    std::make_pair(TokenType::ExclamationMark, Operator::LogicalNot),

    std::make_pair(TokenType::Ampersand, Operator::BinaryAnd),
    std::make_pair(TokenType::VerticalBar, Operator::BinaryOr),
    std::make_pair(TokenType::Caret, Operator::BinaryXor),
    std::make_pair(TokenType::Tilde, Operator::BinaryNot),

    std::make_pair(TokenType::DoubleLessThan, Operator::BitshiftLeft),
    std::make_pair(TokenType::DoubleGreaterThan, Operator::BitshiftRight),

    std::make_pair(TokenType::QuestionMark, Operator::Ternary),
    std::make_pair(TokenType::DoubleQuestionMark, Operator::NullCoalescing),
    std::make_pair(TokenType::Colon, Operator::Ternary),

    std::make_pair(TokenType::PlusEquals, Operator::AddEquals),
    std::make_pair(TokenType::MinusEquals, Operator::SubtractEquals),
    std::make_pair(TokenType::StarEquals, Operator::MultiplyEquals),
    std::make_pair(TokenType::SlashEquals, Operator::DivideEquals),
    std::make_pair(TokenType::PercentEquals, Operator::ModuloEquals),
    std::make_pair(TokenType::AmpersandEquals, Operator::BinaryAndEquals),
    std::make_pair(TokenType::VerticalBarEquals, Operator::BinaryOrEquals),
    std::make_pair(TokenType::CaretEquals, Operator::BinaryXorEquals),
    std::make_pair(TokenType::DoubleLessThanEquals, Operator::BitshiftLeftEquals),
    std::make_pair(TokenType::DoubleGreaterThanEquals, Operator::BitshiftRightEquals),
    std::make_pair(TokenType::DoubleQuestionMarkEquals, Operator::NullCoalescingEquals),
};

using TokenValue = std::variant<std::string, int, char, Keyword>;

struct Token
{
    TokenType type{};
    std::optional<TokenValue> value;

    const InputFile* inputFile = nullptr;
    Position position;
};

// Start of Token.inl

constexpr bool IsTokenOperator(const TokenType tokenType) { return tokenType > TokenType::FirstOperator && tokenType < TokenType::LastOperator; }

constexpr bool IsKeywordType(const Keyword keyword) { return keyword > Keyword::FirstType && keyword < Keyword::LastType; }

#define GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, precedence) if ((op) > Operator::Precedence##precedence) return (precedence) * 10;

constexpr int GetOperatorPrecedence(const Operator op)
{
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 15)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 14)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 13)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 12)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 11)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 10)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 9)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 8)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 7)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 6)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 5)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 4)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 3)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 2)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 1)

    return 0;
}

#undef GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE
