#pragma once

#include "AbstractSyntaxTree.hpp"
#include "Lexer.hpp"
#include "Operator.hpp"

class Parser
{
public:
    explicit Parser(const std::vector<Token>& tokens);

    std::shared_ptr<Ast::Expression> ParseExpression();
    std::shared_ptr<Ast::Statement> ParseTopLevelStatement();
    /// @brief Parse a top-level variable or function declaration.
    std::shared_ptr<Ast::Statement> ParseTopLevelDeclaration();
    std::shared_ptr<Ast::Statement> ParseFunctionLevelStatement();
    std::shared_ptr<Ast::Scope> ParseAll();

    ATTRIBUTE_NODISCARD
    const Token& GetCurrentToken() const;
    ATTRIBUTE_NODISCARD
    const Token& GetNextToken();
    void SkipTokens(size_t count);
    ATTRIBUTE_NODISCARD
    const Token& PeekToken(ptrdiff_t offset) const;

    GETTER_M(bool, Failure)

private:
    const std::vector<Token>* m_Tokens;
    size_t m_Index = 0;

    bool m_Failure = false;

    /// @brief Parses the current token as an operator.
    /// @return If the current token isn't an operator (as of @c IsTokenOperator()), returns an empty optional.
    /// Otherwise, returns the corresponding operator based on the previous and next tokens.
    std::optional<Operator> ParseCurrentOperator(const std::shared_ptr<Ast::Expression>& lhs);

    std::shared_ptr<Ast::Expression> ParsePrimary();
    std::shared_ptr<Ast::Expression> ParseIdentifier();
    std::shared_ptr<Ast::Expression> ParseParentheses();
    std::shared_ptr<Ast::Expression> ParseLiteral();
    std::shared_ptr<Ast::Expression> ParseOperationRhs(s32 expressionPrecedence, std::shared_ptr<Ast::Expression> lhs);
    std::shared_ptr<Ast::Type> ParseType();
    std::shared_ptr<Ast::TypeIdentifierPair> ParseTypeIdentifierPair();
    std::shared_ptr<Ast::IfElse> ParseIfElse();
    std::shared_ptr<Ast::Ternary> ParseTernary(std::shared_ptr<Ast::Expression> condition);
    std::shared_ptr<Ast::WhileLoop> ParseWhileLoop();
    std::shared_ptr<Ast::DoWhileLoop> ParseDoWhileLoop();
    std::shared_ptr<Ast::ForLoop> ParseForLoop();
    std::shared_ptr<Ast::Expression> ParseParenthesizedCondition();

    std::shared_ptr<Ast::Scope> ParseScope(bool isFunctionLevel);
    /// @brief Parses a top-level scope such as namespaces or type definitions
    std::shared_ptr<Ast::Scope> ParseTopLevelScope();
    /// @brief Parses a function-level scope defined by either a wide arrow for a single-statement scope
    /// or curly braces for multi-statement scopes.
    std::shared_ptr<Ast::Scope> ParseFunctionLevelScope();

    std::shared_ptr<Ast::VariableDeclaration> ParseVariableDeclaration(std::shared_ptr<Ast::TypeIdentifierPair> typeNamePair);
    std::shared_ptr<Ast::Statement> ParseFunction(std::shared_ptr<Ast::TypeIdentifierPair> typeNamePair);
    void ParseFunctionParameters(std::vector<Ast::FunctionPrototype::Parameter>& output);
    std::shared_ptr<Ast::FunctionPrototype> ParseFunctionPrototype();
    std::shared_ptr<Ast::FunctionDefinition> ParseFunctionDefinition();

    template <typename T = Ast::Statement, typename... Args>
    std::shared_ptr<T> RaiseError(const std::string& message, Args&&... args);
    template <typename... Args>
    void RaiseError(const Token& token, const std::string& message, Args&&... args);
};

template <typename T, typename ... Args>
std::shared_ptr<T> Parser::RaiseError(const std::string& message, Args&&... args)
{
    RaiseError(GetCurrentToken(), message, std::forward<Args>(args)...);
    return nullptr;
}

template <typename ... Args>
void Parser::RaiseError(const Token& token, const std::string& message, Args&&... args)  // NOLINT(cppcoreguidelines-missing-std-forward)
{
    std::println(
        std::cerr,
        "{}: Syntax error: {}",
        Utils::FormatFileLink(token.inputFile->GetFilepath(), token.position),
        std::vformat(message, std::make_format_args(args...))
    );

    m_Failure = true;
}
