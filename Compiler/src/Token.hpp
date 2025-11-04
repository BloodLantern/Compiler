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

    LiteralInteger,
    LiteralCharacter,
    LiteralString,

    /// @brief If a token has the Operator type, it has a value that corresponds to an Operator enum entry.
    Operator,

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
};

constexpr bool IsTokenOperator(TokenType tokenType);

constexpr std::array SimpleTokenList
{
    std::make_pair("<<=", TokenType::DoubleLessThanEquals),
    std::make_pair(">>=", TokenType::DoubleGreaterThanEquals),
    std::make_pair("??=", TokenType::DoubleQuestionMarkEquals),  // NOLINT(clang-diagnostic-trigraphs)

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

constexpr std::array KeywordStrings
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

enum class Operator : uint8_t
{
    ScopeResolution,

    PostIncrement,
    PostDecrement,
    FunctionCall,
    Subscript,
    MemberAccessDot,
    MemberAccessArrow,

    PreIncrement,
    PreDecrement,
    UnaryMinus,
    LogicalNot,
    BinaryNot,
    Cast,
    Dereference,
    AddressOf,

    PointerToMemberDot,
    PointerToMemberArrow,

    Multiplication,
    Division,
    Modulo,

    Addition,
    Subtraction,

    BitshiftLeft,
    BitshiftRight,

    Less,
    LessEquals,
    Greater,
    GreaterEquals,

    Equals,
    NotEquals,

    BinaryAnd,

    BinaryXor,

    BinaryOr,

    LogicalAnd,

    LogicalOr,

    NullCoalescing,

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
};

inline std::unordered_map OperatorConversionLookup
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

using TokenValue = std::variant<std::string, int, char, Keyword, Operator>;

struct Token
{
    TokenType type;
    std::optional<TokenValue> value;

    const InputFile* inputFile;
    Position position;
};

// Start of Token.inl

constexpr bool IsTokenOperator(const TokenType tokenType) { return tokenType >= TokenType::Equals; }
