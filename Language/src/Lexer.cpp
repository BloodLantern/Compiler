#include "Lexer.hpp"

Lexer::Lexer(std::istream& text)
    : m_Text{&text}
{
}

// TODO - https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl01.html

Token Lexer::ParseNext()
{
    char current;

    do
        *m_Text >> current;
    while (std::isspace(current));

    // Check for an identifier
    if (std::isalpha(current))
    {
        m_LastToken = Token::Identifier;
    }
}
