#pragma once

#include "InputFile.hpp"
#include "Position.hpp"
#include "Utils.hpp"

enum class TokenType : uint8_t
{
    Null,

    EndOfFile,
    Semicolon,

    Identifier,

    EqualsSign,

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

class Lexer
{
public:
    explicit Lexer(const InputFile& inputFile);

    void ParseNext();
    void ParseAll(std::vector<Token>& output);

private:
    const InputFile* m_InputFile;
    ptrdiff_t m_Index = -1;
    Position m_Position{};

    Token m_LastToken{};

    char m_Character = '\0';

    void ConsumeNextCharacter();
    void Skip(ptrdiff_t count);
    ATTRIBUTE_NODISCARD
    char PeekCharacter(ptrdiff_t offset) const;

    void SkipSpaces();
    void SkipUntil(const Utils::Predicate<char>& predicate);
    void SkipUntilAfter(std::string_view str);
    void SkipUntilAfter(char c);
    void SkipToNextLine();
    void BuildString(std::string& output, const Utils::Predicate<char>& predicate);

    ATTRIBUTE_NODISCARD
    bool FinishedReading() const;

    template <typename... Args>
    ATTRIBUTE_NORETURN
    void RaiseError(const std::string& message, Args&&... args) const;
};

template <typename ... Args>
void Lexer::RaiseError(const std::string& message, Args&&... args) const
{
    std::println(
        std::cerr,
        "{}({}, {}): Syntax error: {}",
        m_InputFile->GetFilepath(),
        m_Position.line,
        m_Position.column,
        std::vformat(message, std::make_format_args(std::forward<Args>(args)...))
    );

    Utils::Exit(ErrorCodes::SyntaxError);  // NOLINT(concurrency-mt-unsafe)
}
