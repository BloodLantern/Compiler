#pragma once

enum class TokenType : uint8_t
{
    Null,

    EndOfFile,
    Semicolon,

    Identifier,

    OperatorEquals,

    OperatorPlus,
    OperatorMinus,
    OperatorStar,
    OperatorSlash,

    LiteralInteger,
    LiteralCharacter,
    LiteralString,
};

using TokenValue = std::variant<std::string, int, char>;

struct Token
{
    TokenType type;
    std::optional<TokenValue> value;
};
