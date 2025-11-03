#pragma once
#include "AbstractSyntaxTree.hpp"
#include "Lexer.hpp"

class Parser
{
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::unique_ptr<AbstractSyntaxTree::Expression> ParseCurrent();
    void ParseAll(std::vector<std::unique_ptr<AbstractSyntaxTree::Expression>>& output);

    ATTRIBUTE_NODISCARD
    const Token& GetCurrentToken() const;
    ATTRIBUTE_NODISCARD
    const Token& GetNextToken();
    void Skip(size_t count);

private:
    const std::vector<Token>* m_Tokens;
    size_t m_Index = 0;

    std::unique_ptr<AbstractSyntaxTree::Expression> ParseIdentifier();
    std::unique_ptr<AbstractSyntaxTree::Expression> ParseParentheses();
    std::unique_ptr<AbstractSyntaxTree::Expression> ParseNumber();

    template <typename... Args>
    std::unique_ptr<AbstractSyntaxTree::Expression> RaiseError(const std::string& message, Args&&... args);
};

template <typename... Args>
std::unique_ptr<AbstractSyntaxTree::Expression> Parser::RaiseError(const std::string& message, Args&&... args)  // NOLINT(cppcoreguidelines-missing-std-forward)
{
    const Token& currentToken = GetCurrentToken();

    std::println(
        std::cerr,
        "{}({}, {}): Syntax error: {}",
        currentToken.inputFile->GetFilepath(),
        currentToken.position.line,
        currentToken.position.column,
        std::vformat(message, std::make_format_args(args...))
    );

    return nullptr;
}
