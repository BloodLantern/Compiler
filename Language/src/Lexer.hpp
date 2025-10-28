#pragma once

#include <istream>
#include <variant>

enum class Token : uint8_t
{
    Null,

    EndOfFile,

    Identifier,
    Number
};

class Lexer
{
public:
    explicit Lexer(std::istream& text);

    Token ParseNext();

private:
    std::istream* m_Text;

    Token m_LastToken = Token::Null;
    std::variant<std::string> m_LastTokenValue;
};
