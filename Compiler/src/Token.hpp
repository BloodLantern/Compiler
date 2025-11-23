#pragma once

#include "Common.hpp"
#include "InputFile.hpp"
#include "Position.hpp"
#include "Keyword.hpp"

enum class TokenType : u8
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

    Colon,

    FirstOperator,

    Equals,

    DoubleColon,

    Dot,
    MinusGreaterThan,

    EqualsGreaterThan,

    DoubleDot,

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
constexpr bool TokenTypeIsOperator(TokenType tokenType);

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

    std::make_pair("..", TokenType::DoubleDot),

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

using TokenValue = std::variant<std::string, i32, c8, Keyword>;

struct Token
{
    TokenType type{};
    std::optional<TokenValue> value;

    const InputFile* inputFile = nullptr;
    Position position;

    /// @brief Checks whether the token is a type token, i.e., a token that is either an identifier or a builtin type keyword.
    /// @return @c true if the token is a type token, @c false otherwise
    ATTRIBUTE_NODISCARD
    constexpr bool IsTypeTokenOrIdentifier() const;
};

// Start of Token.inl

constexpr bool TokenTypeIsOperator(const TokenType tokenType) { return tokenType > TokenType::FirstOperator && tokenType < TokenType::LastOperator; }

constexpr bool Token::IsTypeTokenOrIdentifier() const
{
    return type == TokenType::Identifier
        || (type == TokenType::Keyword && KeywordIsType(std::get<Keyword>(value.value())));
}
