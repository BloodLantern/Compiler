#include "PrecompiledHeader.hpp"
#include "Lexer.hpp"

#include <cassert>

#include "Utils.hpp"

Lexer::Lexer(const InputFile& inputFile)
    : m_InputFile{&inputFile}
{
}

// TODO - https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/LangImpl01.html

void Lexer::ParseNext()
{
    if (m_Index < 0)
    {
        ConsumeNextCharacter();
        m_Position = {};
    }

    SkipSpaces();

    if (FinishedReading())
    {
        m_LastToken.value.reset();
        m_LastToken.type = TokenType::EndOfFile;
        return;
    }

    // Check for a comment
    if (m_Character == '/')
    {
        const char nextCharacter = PeekCharacter(1);

        // Single-line comment
        if (nextCharacter == '/')
        {
            SkipToNextLine();
            return ParseNext();
        }

        // Multi-line comment
        if (nextCharacter == '*')
        {
            Skip(1);
            SkipUntilAfter("*/");
            return ParseNext();
        }

        RaiseError("Invalid character: '/'");
    }

    if (m_Character == '=')
    {
        m_LastToken.value.reset();
        m_LastToken.type = TokenType::EqualsSign;
        ConsumeNextCharacter();
        return;
    }

    if (m_Character == ';')
    {
        m_LastToken.value.reset();
        m_LastToken.type = TokenType::Semicolon;
        ConsumeNextCharacter();
        return;
    }

    // Check for a literal integer
    if (m_Character == '-' || std::isdigit(m_Character))
    {
        bool negative = false;
        if (m_Character == '-')
        {
            ConsumeNextCharacter();
            SkipSpaces();
            negative = true;
        }

        std::string intString;
        BuildString(intString, [](const char c) -> bool { return std::isdigit(c); });

        int value = static_cast<int>(std::strtol(intString.c_str(), nullptr, 10));
        if (negative)
            value = -value;

        m_LastToken.value = value;
        m_LastToken.type = TokenType::LiteralInteger;
        return;
    }

    // Check for a literal character
    if (m_Character == '\'')
    {
        const char literalEndCharacter = PeekCharacter(2);
        if (literalEndCharacter != '\'')
        {
            Skip(2);
            RaiseError("Expected end of character literal, got " CHAR_FORMAT_ARGUMENT(0), literalEndCharacter);
        }

        m_LastToken.value = PeekCharacter(1);
        m_LastToken.type = TokenType::LiteralCharacter;
        Skip(3);
        return;
    }

    // Check for a literal string
    if (m_Character == '"')
    {
        ConsumeNextCharacter();

        std::string identifier;
        BuildString(identifier, [](const char c) { return c != '"' && c != EOF; });

        if (m_Character == EOF)
            RaiseError("Expected end of character literal, got 'EOF'");

        m_LastToken.value = identifier;
        m_LastToken.type = TokenType::LiteralString;
        ConsumeNextCharacter();
        return;
    }

    // Check for an identifier
    if (std::isalpha(m_Character))
    {
        std::string identifier;
        BuildString(identifier, [](const char c) -> bool { return std::isalnum(c); });

        m_LastToken.value = std::move(identifier);
        m_LastToken.type = TokenType::Identifier;
        return;
    }

    RaiseError("Invalid character: " CHAR_FORMAT_ARGUMENT(0), m_Character);
}

void Lexer::ParseAll(std::vector<Token>& output)
{
    do
    {
        ParseNext();
        output.push_back(m_LastToken);
    }
    while (m_LastToken.type != TokenType::EndOfFile);
}

void Lexer::ConsumeNextCharacter()
{
    if (m_Character == '\n')
    {
        ++m_Position.line;
        m_Position.column = 1;
    }
    else
    {
        ++m_Position.column;
    }

    if (FinishedReading())
    {
        m_Character = EOF;
        return;
    }

    m_Character = (*m_InputFile)[++m_Index];
}

void Lexer::Skip(const ptrdiff_t count)
{
    if (count == 0)
        return;

    assert(count > 0);

    const ptrdiff_t offset = count - 1;
    m_Index += offset;
    m_Position.column += offset;
    ConsumeNextCharacter();
}

char Lexer::PeekCharacter(const ptrdiff_t offset) const
{
    const ptrdiff_t position = m_Index + offset;
    assert(position >= 0 && std::cmp_less(position, m_InputFile->GetText().size()));
    return (*m_InputFile)[position];
}

void Lexer::SkipSpaces()
{
    SkipUntil([](const char c) { return !std::isspace(c) && c != '\0'; });
}

void Lexer::SkipUntil(const Utils::Predicate<char>& predicate)
{
    while (!predicate(m_Character))
        ConsumeNextCharacter();
}

void Lexer::SkipUntilAfter(const std::string_view str)
{
    while (std::string_view{m_InputFile->GetText().c_str() + m_Index, str.size()} != str)
        ConsumeNextCharacter();

    for (size_t i = 0; i < str.size(); i++)
        ConsumeNextCharacter();
}

void Lexer::SkipUntilAfter(const char c)
{
    while (m_Character != c)
        ConsumeNextCharacter();
    ConsumeNextCharacter();
}

void Lexer::SkipToNextLine()
{
    SkipUntilAfter("\n");
}

void Lexer::BuildString(std::string& output, const Utils::Predicate<char>& predicate)
{
    while (predicate(m_Character))
    {
        output += m_Character;
        ConsumeNextCharacter();
    }
}

bool Lexer::FinishedReading() const
{
    return m_Index >= 0 && (m_Character == EOF || static_cast<size_t>(m_Index) >= m_InputFile->GetText().size());
}
