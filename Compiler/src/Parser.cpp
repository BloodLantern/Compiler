#include "Parser.hpp"

Parser::Parser(const std::vector<Token>& tokens)
    : m_Tokens{&tokens}
{
}

std::unique_ptr<Ast::Expression> Parser::ParseExpression()
{
    std::unique_ptr<Ast::Expression> lhs = ParsePrimary();
    if (!lhs)
        return nullptr;

    return ParseOperationRhs(-1, std::move(lhs));
}

std::unique_ptr<Ast::Statement> Parser::ParseTopLevelStatement()
{
    // const size_t startIndex = m_Index;
    const Token& token = GetCurrentToken();
    switch (token.type)
    {
        case TokenType::Keyword:
        {
            Keyword keyword = std::get<Keyword>(token.value.value());
            if (IsKeywordType(keyword))
                return ParseTopLevelDeclaration();

            return RaiseError<Ast::Statement>("Unexpected keyword {}", keyword);
        }

        case TokenType::Identifier:
            return ParseTopLevelDeclaration();

        default:
            return RaiseError<Ast::Statement>("Unexpected token {}", token.type);
    }
}

std::unique_ptr<Ast::Statement> Parser::ParseTopLevelDeclaration()
{
    if (GetNextToken().type != TokenType::Identifier)
        return RaiseError<Ast::Statement>("Expected identifier for top level declaration, got {}", GetCurrentToken().type);

    if (GetNextToken().type == TokenType::ParenthesisOpen)
        return ParseFunction();

    return ParseVariableDeclaration();
}

std::unique_ptr<Ast::Statement> Parser::ParseFunctionLevelStatement()
{
    return ParseExpression(); // TODO
}

void Parser::ParseAll(std::vector<std::unique_ptr<Ast::Statement>>& output)
{
    /*do
        output.push_back(ParseTopLevelStatement());
    while (GetCurrentToken().type != TokenType::EndOfFile);*/
    output.push_back(ParseFunctionDefinition()); // Assume that we always receive the main function
}

const Token& Parser::GetCurrentToken() const
{
    return (*m_Tokens)[m_Index];
}

const Token& Parser::GetNextToken()
{
    return (*m_Tokens)[++m_Index];
}

void Parser::SkipTokens(const size_t count)
{
    m_Index += count;
}

std::optional<Operator> Parser::ParseCurrentOperator()
{
    const Token& current = GetCurrentToken();

    if (!IsTokenOperator(current.type))
        return {};

    const Token* previous = m_Index == 0 ? nullptr : &(*m_Tokens)[m_Index - 1];
    const Token* next = m_Index == m_Tokens->size() - 1 ? nullptr : &(*m_Tokens)[m_Index + 1];

    const auto InvalidToken = [current, this] -> std::optional<Operator>
    {
        RaiseError(current, "Invalid token");
        return {};
    };

    if (!previous || !next)
        return InvalidToken();

    switch (current.type)
    {
        case TokenType::Dot:
            if (previous->type == TokenType::Identifier)
            {
                if (next->type == TokenType::Identifier)
                    return Operator::MemberAccessDot;
                if (next->type == TokenType::Star)
                    return Operator::PointerToMemberDot;
            }
            return InvalidToken();

        case TokenType::MinusGreaterThan:
            if (previous->type == TokenType::Identifier)
            {
                if (next->type == TokenType::Identifier)
                    return Operator::MemberAccessArrow;
                if (next->type == TokenType::Star)
                    return Operator::PointerToMemberArrow;
            }
            return InvalidToken();

        case TokenType::Star:
            if (previous->type != TokenType::Identifier && previous->type != TokenType::LiteralInteger)
                return Operator::Dereference;
            return Operator::Multiplication;

        case TokenType::DoublePlus:
            if (previous->type == TokenType::Identifier)
                return Operator::PostIncrement;
            if (next->type == TokenType::Identifier)
                return Operator::PreIncrement;
            return InvalidToken();

        case TokenType::DoubleMinus:
            if (previous->type == TokenType::Identifier)
                return Operator::PostDecrement;
            if (next->type == TokenType::Identifier)
                return Operator::PreDecrement;
            return InvalidToken();

        case TokenType::Ampersand:
            if (previous->type != TokenType::Identifier && previous->type != TokenType::LiteralInteger)
                return Operator::AddressOf;
            return Operator::BinaryAnd;

        default:
            return BaseOperatorConversionLookup.at(current.type);
    }
}

std::unique_ptr<Ast::Expression> Parser::ParsePrimary()
{
    const Token& token = GetCurrentToken();
    switch (token.type)
    {
        case TokenType::Identifier:
            return ParseIdentifier();

        case TokenType::LiteralInteger:
        case TokenType::LiteralCharacter:
        case TokenType::LiteralString:
            return ParseLiteral();

        case TokenType::ParenthesisOpen:
            return ParseParentheses();

        default:
            RaiseError<Ast::Expression>("Invalid token: {}", magic_enum::enum_name(token.type));
            SkipTokens(1);
            return nullptr;
    }
}

std::unique_ptr<Ast::Expression> Parser::ParseIdentifier()
{
    const std::string& identifier = std::get<std::string>(GetCurrentToken().value.value());

    if (GetNextToken().type != TokenType::ParenthesisOpen)
        return std::make_unique<Ast::Variable>(identifier);

    std::vector<std::unique_ptr<Ast::Expression>> arguments;
    if (GetNextToken().type != TokenType::ParenthesisClose)
    {
        while (true)
        {
            std::unique_ptr<Ast::Expression> argument = ParseExpression();
            if (argument)
                arguments.push_back(std::move(argument));
            else
                return nullptr;

            const Token& next = GetCurrentToken();
            if (next.type == TokenType::ParenthesisClose)
                break;

            if (next.type != TokenType::Comma)
                return RaiseError<Ast::Expression>("Expected ')' or ',' in the argument list, got: {}", next.type);

            SkipTokens(1);
        }
    }

    SkipTokens(1);

    return std::make_unique<Ast::FunctionCall>(identifier, std::move(arguments));
}

std::unique_ptr<Ast::Expression> Parser::ParseParentheses()
{
    SkipTokens(1);
    std::unique_ptr<Ast::Expression> expression = ParseExpression();
    if (!expression)
        return nullptr;

    const Token& next = GetCurrentToken();
    if (next.type != TokenType::ParenthesisClose)
        return RaiseError<Ast::Expression>("Expected ')', got: {}", next.type);

    SkipTokens(1);
    return expression;
}

std::unique_ptr<Ast::Expression> Parser::ParseLiteral()
{
    const Token& token = GetCurrentToken();
    std::unique_ptr<Ast::Expression> result;

    switch (token.type)
    {
        case TokenType::LiteralInteger:
            result = std::make_unique<Ast::Number<int>>(std::get<int>(token.value.value()));
            break;

        case TokenType::LiteralCharacter:
            result = std::make_unique<Ast::Character>(std::get<char>(token.value.value()));
            break;

        case TokenType::LiteralString:
            result = std::make_unique<Ast::String>(std::get<std::string>(token.value.value()));
            break;

        default:
            std::unreachable();
    }

    SkipTokens(1);
    return result;
}

std::unique_ptr<Ast::Expression> Parser::ParseOperationRhs(const int expressionPrecedence, std::unique_ptr<Ast::Expression> lhs)
{
    while (true)
    {
        const std::optional<Operator> optionalOperator = ParseCurrentOperator();
        if (!optionalOperator.has_value())
            return lhs;

        const Operator op = optionalOperator.value();
        const int opPrecedence = GetOperatorPrecedence(op);

        if (opPrecedence < expressionPrecedence)
            return lhs;

        SkipTokens(1); // Skip the operator token

        auto rhs = IsTokenOperator(GetCurrentToken().type) ? ParseOperationRhs(opPrecedence + 1, nullptr) : ParsePrimary();
        if (!rhs)
            return nullptr;

        const std::optional<Operator> nextOptionalOperator = ParseCurrentOperator();
        const int nextOperatorPrecedence = nextOptionalOperator.has_value() ? GetOperatorPrecedence(nextOptionalOperator.value()) : -1;

        if (opPrecedence < nextOperatorPrecedence)
        {
            rhs = ParseOperationRhs(opPrecedence + 1, std::move(rhs));
            if (!rhs)
                return nullptr;
        }

        lhs = std::make_unique<Ast::Operation>(op, std::move(lhs), std::move(rhs));
    }
}

std::unique_ptr<Ast::TypeName> Parser::ParseType()
{
    const Token& token = GetCurrentToken();

    std::string name;
    switch (token.type)
    {
        case TokenType::Identifier:
            name = std::get<std::string>(token.value.value());
            break;

        case TokenType::Keyword:
        {
            Keyword keyword = std::get<Keyword>(token.value.value());
            if (!IsKeywordType(keyword))
                return RaiseError<Ast::TypeName>("Expected a type keyword, got {}", keyword);
            name = KeywordStrings.at(keyword);
            break;
        }

        default:
            return RaiseError<Ast::TypeName>("Expected either an identifier or a keyword for the type name, got {}", token.type);
    }

    SkipTokens(1);

    return std::make_unique<Ast::TypeName>(name);
}

std::unique_ptr<Ast::TypeNamePair> Parser::ParseTypeNamePair()
{
    std::unique_ptr<Ast::TypeName> type = ParseType();
    if (!type)
        return nullptr;

    const Token& name = GetCurrentToken();

    if (name.type != TokenType::Identifier)
        return RaiseError<Ast::TypeNamePair>("Expected name in declaration, got {}", name.type);

    SkipTokens(1);

    return std::make_unique<Ast::TypeNamePair>(std::move(type), std::get<std::string>(name.value.value()));
}

std::unique_ptr<Ast::Statement> Parser::ParseVariableDeclaration()
{
    std::unique_ptr<Ast::TypeNamePair> typeName = ParseTypeNamePair();

    const Token& currentToken = GetCurrentToken();
    switch (currentToken.type)
    {
        case TokenType::Equals:
            return nullptr; // TODO

        case TokenType::Semicolon:
            return nullptr; // TODO

        default:
            return RaiseError<Ast::Statement>("Expected either '=' or ';' in variable declaration, got {}", currentToken.type);
    }
}

std::unique_ptr<Ast::Statement> Parser::ParseFunction()
{
    return nullptr; // TODO
}

std::unique_ptr<Ast::FunctionPrototype> Parser::ParseFunctionPrototype()
{
    std::unique_ptr<Ast::TypeNamePair> typeName = ParseTypeNamePair();

    if (GetCurrentToken().type != TokenType::ParenthesisOpen)
        return RaiseError<Ast::FunctionPrototype>("Expected '(' in prototype, got {}", GetCurrentToken().type);

    std::vector<Ast::FunctionPrototype::Parameter> arguments;
    while (GetNextToken().type != TokenType::ParenthesisClose)
    {
        arguments.push_back(ParseTypeNamePair());

        const Token& parameterSeparator = GetCurrentToken();
        if (parameterSeparator.type != TokenType::Comma)
            return RaiseError<Ast::FunctionPrototype>("Expected ',' in the argument list, got {}", parameterSeparator.type);
    }

    SkipTokens(1); // Skip ')'

    return std::make_unique<Ast::FunctionPrototype>(std::move(typeName), std::move(arguments));
}

std::unique_ptr<Ast::FunctionDefinition> Parser::ParseFunctionDefinition()
{
    std::unique_ptr<Ast::FunctionPrototype> prototype = ParseFunctionPrototype();
    if (!prototype)
        return nullptr;

    const Token& openBrace = GetCurrentToken();
    if (openBrace.type != TokenType::CurlyBraceOpen)
        return RaiseError<Ast::FunctionDefinition>("Expected '{' in function definition, got {}", openBrace.type);

    std::vector<std::unique_ptr<Ast::Statement>> body;

    while (GetNextToken().type != TokenType::CurlyBraceClose)
        body.push_back(ParseFunctionLevelStatement());

    SkipTokens(1); // Skip '}'

    return std::make_unique<Ast::FunctionDefinition>(std::move(prototype), std::move(body));
}
