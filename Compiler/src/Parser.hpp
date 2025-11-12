#pragma once

#include "AbstractSyntaxTree.hpp"
#include "Lexer.hpp"
#include "Operator.hpp"

class Parser
{
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::unique_ptr<Ast::Expression> ParseExpression();
    std::unique_ptr<Ast::Statement> ParseTopLevelStatement();
    /// @brief Parse a top-level variable or function declaration.
    std::unique_ptr<Ast::Statement> ParseTopLevelDeclaration();
    std::unique_ptr<Ast::Statement> ParseFunctionLevelStatement();
    void ParseAll(std::vector<std::unique_ptr<Ast::Statement>>& output);

    ATTRIBUTE_NODISCARD
    const Token& GetCurrentToken() const;
    ATTRIBUTE_NODISCARD
    const Token& GetNextToken();
    void SkipTokens(size_t count);

    GETTER_M(bool, Failure)

private:
    const std::vector<Token>* m_Tokens;
    size_t m_Index = 0;

    bool m_Failure = false;

    /// @brief Parses the current token as an operator.
    /// @return If the current token isn't an operator (as of @c IsTokenOperator()), returns an empty optional.
    /// Otherwise, returns the corresponding operator based on the previous and next tokens.
    std::optional<Operator> ParseCurrentOperator(const std::unique_ptr<Ast::Expression>& lhs);

    std::unique_ptr<Ast::Expression> ParsePrimary();
    std::unique_ptr<Ast::Expression> ParseIdentifier();
    std::unique_ptr<Ast::Expression> ParseParentheses();
    std::unique_ptr<Ast::Expression> ParseLiteral();
    std::unique_ptr<Ast::Expression> ParseOperationRhs(int expressionPrecedence, std::unique_ptr<Ast::Expression> lhs);
    std::unique_ptr<Ast::TypeName> ParseType();
    std::unique_ptr<Ast::TypeIdentifierPair> ParseTypeIdentifierPair();

    using StatementParseFunction = std::unique_ptr<Ast::Statement>(Parser::*)();
    std::unique_ptr<Ast::Scope> ParseScope(StatementParseFunction statementParseFunction);
    std::unique_ptr<Ast::Scope> ParseTopLevelScope();
    std::unique_ptr<Ast::Scope> ParseFunctionLevelScope();

    std::unique_ptr<Ast::Statement> ParseVariableDeclaration(std::unique_ptr<Ast::TypeIdentifierPair> typeNamePair);
    std::unique_ptr<Ast::Statement> ParseFunction(std::unique_ptr<Ast::TypeIdentifierPair> typeNamePair);
    void ParseFunctionParameters(std::vector<Ast::FunctionPrototype::Parameter>& output);
    std::unique_ptr<Ast::FunctionPrototype> ParseFunctionPrototype();
    std::unique_ptr<Ast::FunctionDefinition> ParseFunctionDefinition();

    template <typename T = Ast::Statement, typename... Args>
    std::unique_ptr<T> RaiseError(const std::string& message, Args&&... args);
    template <typename... Args>
    void RaiseError(const Token& token, const std::string& message, Args&&... args);
};

template <typename T, typename ... Args>
std::unique_ptr<T> Parser::RaiseError(const std::string& message, Args&&... args)
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
