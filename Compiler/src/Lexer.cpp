#include "Lexer.hpp"

#include <cassert>

#include "Utils.hpp"

Lexer::Lexer(const InputFile& inputFile)
    : m_InputFile{&inputFile}
{
}

void Lexer::ParseNext()
{
    if (m_Index < 0)
    {
        ConsumeNextCharacter();
        m_Position = {};
    }

    SkipSpaces();

    m_LastToken.position = m_Position;
    m_LastToken.inputFile = m_InputFile;

    if (FinishedReading())
    {
        m_LastToken.value.reset();
        m_LastToken.type = TokenType::EndOfFile;
        return;
    }

    for (const std::pair<const char*, TokenType>& token : SimpleTokenList)
    {
        if (!CheckString(token.first))
            continue;

        if (token.second == TokenType::Slash)
        {
            // Check for a comment
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
                SkipCharacters(1);
                SkipUntilAfter("*/");
                return ParseNext();
            }

            // Single slash, interpret this as an operator
            HandleSimpleToken(TokenType::Slash);
            return;
        }

        HandleSimpleToken(token.second, std::string_view{token.first}.size());
        return;
    }

    switch (m_Character)
    {
        case '\'':
        {
            const char literalEndCharacter = PeekCharacter(2);
            if (literalEndCharacter != '\'')
            {
                SkipCharacters(2);
                RaiseError("Expected end of character literal, got " CHAR_FORMAT_ARGUMENT(0), literalEndCharacter);
                return;
            }

            m_LastToken.value = PeekCharacter(1);
            m_LastToken.type = TokenType::LiteralCharacter;
            SkipCharacters(3);
            return;
        }

        case '"':
        {
            ConsumeNextCharacter();

            std::string identifier;
            BuildString(identifier, [](const char c) { return c != '"' && c != EOF; });

            if (m_Character == EOF)
            {
                RaiseError("Expected end of character literal, got 'EOF'");
                return;
            }

            m_LastToken.value = identifier;
            m_LastToken.type = TokenType::LiteralString;
            ConsumeNextCharacter();
            return;
        }

        default:
            break;
    }

    // Check for a literal integer
    if (std::isdigit(m_Character))
    {
        std::string intString;
        BuildString(intString, [](const char c) -> bool { return std::isdigit(c); });

        m_LastToken.value = static_cast<int>(std::strtol(intString.c_str(), nullptr, 10));
        m_LastToken.type = TokenType::LiteralInteger;
        return;
    }

    // Check for a keyword
    for (const auto& pair : KeywordStrings)
    {
        const std::string_view keyword = pair.second;
        if (!CheckString(keyword))
            continue;

        const ptrdiff_t followingCharacter = static_cast<ptrdiff_t>(keyword.size());
        if (std::isalnum(PeekCharacter(followingCharacter)))
            continue;

        m_LastToken.type = TokenType::Keyword;
        m_LastToken.value = pair.first;
        SkipCharacters(followingCharacter);
        return;
    }

    // Check for an identifier
    if (m_Character == '_' || std::isalpha(m_Character))
    {
        std::string identifier;
        BuildString(identifier, [](const char c) -> bool { return c == '_' || std::isalnum(c); });

        m_LastToken.value = std::move(identifier);
        m_LastToken.type = TokenType::Identifier;
        return;
    }

    ATTRIBUTE_UNLIKELY
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

void Lexer::SkipCharacters(const ptrdiff_t count)
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

bool Lexer::CheckString(const std::string_view str) const
{
    return m_InputFile->GetText().substr(m_Index).starts_with(str);
}

void Lexer::HandleSimpleToken(const TokenType type, const size_t strSize)
{
    m_LastToken.value.reset();
    m_LastToken.type = type;
    SkipCharacters(static_cast<ptrdiff_t>(strSize));
}

bool Lexer::FinishedReading() const
{
    return m_Index >= 0 && (m_Character == EOF || static_cast<size_t>(m_Index) >= m_InputFile->GetText().size());
}
