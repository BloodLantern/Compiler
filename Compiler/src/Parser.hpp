#pragma once

#include "AbstractSyntaxTree.hpp"
#include "Lexer.hpp"

class Parser
{
public:
    explicit Parser(std::vector<Token>& tokens);

    std::unique_ptr<Ast::Expression> ParseCurrent();
    void ParseAll(std::vector<std::unique_ptr<Ast::Expression>>& output);

    ATTRIBUTE_NODISCARD
    const Token& GetCurrentToken() const;
    ATTRIBUTE_NODISCARD
    const Token& GetNextToken();
    void Skip(size_t count);

    GETTER_M(bool, Failure)

private:
    const std::vector<Token>* m_Tokens;
    size_t m_Index = 0;

    bool m_Failure = false;

    void PreProcessOperators(std::vector<Token>& tokens);
    std::optional<Operator> GetOperator(Token* previous, Token& current, Token* next);

    std::unique_ptr<Ast::Expression> ParsePrimary();
    std::unique_ptr<Ast::Expression> ParseIdentifier();
    std::unique_ptr<Ast::Expression> ParseParentheses();
    std::unique_ptr<Ast::Expression> ParseLiteral();
    std::unique_ptr<Ast::Expression> ParseOperationRhs(size_t expressionPrecedence, std::unique_ptr<Ast::Expression> lhs);

    template <typename... Args>
    std::unique_ptr<Ast::Expression> RaiseError(const std::string& message, Args&&... args);
    template <typename... Args>
    void RaiseError(const Token& token, const std::string& message, Args&&... args);
};

template <typename... Args>
std::unique_ptr<Ast::Expression> Parser::RaiseError(const std::string& message, Args&&... args)
{
    RaiseError(GetCurrentToken(), message, std::forward<Args>(args)...);
    return nullptr;
}

template <typename ... Args>
void Parser::RaiseError(const Token& token, const std::string& message, Args&&... args)  // NOLINT(cppcoreguidelines-missing-std-forward)
{
    std::println(
        std::cerr,
        "{}({}, {}): Syntax error: {}",
        token.inputFile->GetFilepath(),
        token.position.line,
        token.position.column,
        std::vformat(message, std::make_format_args(args...))
    );

    m_Failure = true;
}
