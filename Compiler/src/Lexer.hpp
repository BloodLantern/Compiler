#pragma once

#include "InputFile.hpp"
#include "Position.hpp"
#include "Token.hpp"
#include "Utils.hpp"

class Lexer
{
public:
    explicit Lexer(const InputFile& inputFile);

    void ParseNext();
    void ParseAll(std::vector<Token>& output);
    static void PrintDebug(const std::vector<Token>& tokens);

    GETTER_M(const Token&, LastToken)
    GETTER_M(bool, Failure)

private:
    const InputFile* m_InputFile;
    ptrdiff_t m_Index = -1;
    Position m_Position{};

    Token m_LastToken{};

    c8 m_Character = '\0';

    bool m_Failure = false;

    void ConsumeNextCharacter();
    void SkipCharacters(ptrdiff_t count);
    ATTRIBUTE_NODISCARD
    c8 PeekCharacter(ptrdiff_t offset) const;

    void SkipSpaces();
    void SkipUntil(const Utils::Predicate<c8>& predicate);
    void SkipUntilAfter(std::string_view str);
    void SkipUntilAfter(c8 c);
    void SkipToNextLine();
    void BuildString(std::string& output, const Utils::Predicate<c8>& predicate);
    ATTRIBUTE_NODISCARD
    bool CheckString(std::string_view str) const;
    void HandleSimpleToken(TokenType type, size_t strSize = 1);

    ATTRIBUTE_NODISCARD
    bool FinishedReading() const;

    template <typename... Args>
    void RaiseError(const std::string& message, Args&&... args);
};

template <typename... Args>
void Lexer::RaiseError(const std::string& message, Args&&... args)  // NOLINT(cppcoreguidelines-missing-std-forward)
{
    std::println(
        std::cerr,
        "{}: Fatal error: {}",
        Utils::FormatFileLink(m_InputFile->GetFilepath(), m_Position),
        std::vformat(message, std::make_format_args(args...))
    );

    m_LastToken.type = TokenType::None;
    m_LastToken.value.reset();

    m_Failure = true;
}
