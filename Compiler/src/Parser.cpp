#include "Parser.hpp"

Parser::Parser(const std::vector<Token>& tokens)
    : m_Tokens{&tokens}
{
}

std::shared_ptr<Ast::Expression> Parser::ParseExpression()
{
    std::shared_ptr<Ast::Expression> lhs = ParsePrimary();
    if (!lhs)
        return nullptr;

    if (GetCurrentToken().type == TokenType::QuestionMark)
        return ParseTernary(lhs);

    if (TokenIsOperator(GetCurrentToken().type))
        return ParseOperationRhs(-1, std::move(lhs));

    return lhs;
}

std::shared_ptr<Ast::Statement> Parser::ParseTopLevelStatement()
{
    const Token& token = GetCurrentToken();
    switch (token.type)
    {
        case TokenType::Keyword:
        {
            const auto keyword = std::get<Keyword>(token.value.value());
            if (IsKeywordType(keyword))
                return ParseTopLevelDeclaration();

            SkipTokens(1);
            return RaiseError("Unexpected keyword {}", keyword);
        }

        case TokenType::Identifier:
            return ParseTopLevelDeclaration();

        default:
            SkipTokens(1);
            return RaiseError("Unexpected token {}", token.type);
    }
}

std::shared_ptr<Ast::Statement> Parser::ParseTopLevelDeclaration()
{
    std::shared_ptr<Ast::TypeIdentifierPair> typeNamePair = ParseTypeIdentifierPair();

    if (GetCurrentToken().type == TokenType::ParenthesisOpen)
        return ParseFunction(std::move(typeNamePair));

    return ParseVariableDeclaration(std::move(typeNamePair));
}

std::shared_ptr<Ast::Statement> Parser::ParseFunctionLevelStatement()
{
    std::shared_ptr<Ast::Statement> result;

    const Token& token = GetCurrentToken();
    switch (token.type)
    {
        case TokenType::Keyword:
        {
            const auto keyword = std::get<Keyword>(token.value.value());
            if (IsKeywordType(keyword))
            {
                result = ParseExpression();
                break;
            }

            switch (keyword)
            {
                case Keyword::Return:
                    SkipTokens(1);
                    result = std::make_shared<Ast::FunctionReturn>(GetCurrentToken().type == TokenType::Semicolon ? nullptr : ParseExpression());
                    break;

                case Keyword::If:
                    return ParseIfElse();

                default:
                    result = RaiseError("Invalid keyword in statement: {}", keyword);
                    SkipTokens(1);
                    break;
            }
            break;
        }

        case TokenType::Identifier:
            result = ParseExpression();
            break;

        default:
            result = RaiseError("Invalid token in statement: {}", token.type);
            SkipTokens(1);
            break;
    }

    if (GetCurrentToken().type != TokenType::Semicolon) // TODO - Might need to check for curly braces
        return RaiseError("Expected ';' in statement, got {}", GetCurrentToken().type);

    SkipTokens(1);

    return result;
}

std::shared_ptr<Ast::Scope> Parser::ParseAll()
{
    Ast::currentScope = nullptr;
    auto topLevelScope = std::make_shared<Ast::Scope>(false);
    // ReSharper disable once CppDFALocalValueEscapesFunction
    Ast::currentScope = topLevelScope;

    do
        topLevelScope->GetBody().push_back(ParseTopLevelStatement());
    while (GetCurrentToken().type != TokenType::EndOfFile);

    Ast::currentScope = nullptr;

    return topLevelScope;
}

const Token& Parser::GetCurrentToken() const
{
    return m_Tokens->at(m_Index);
}

const Token& Parser::GetNextToken()
{
    return m_Tokens->at(++m_Index);
}

void Parser::SkipTokens(const size_t count)
{
    m_Index += count;
}

const Token& Parser::PeekToken(const ptrdiff_t offset) const
{
    return m_Tokens->at(m_Index + offset);
}

std::optional<Operator> Parser::ParseCurrentOperator(const std::shared_ptr<Ast::Expression>& lhs)
{
    const Token& current = GetCurrentToken();

    const auto InvalidToken = [current, this] -> std::optional<Operator>
    {
        RaiseError(current, "Invalid token: {}", current.type);
        return {};
    };

    if (m_Index == 0 || m_Index == m_Tokens->size() - 1)
        return InvalidToken();

    const bool hasPrevious = lhs != nullptr;
    const Token& next = (*m_Tokens)[m_Index + 1];

    switch (current.type)
    {
        case TokenType::Dot:
            if (hasPrevious)
            {
                if (next.type == TokenType::Identifier)
                    return Operator::MemberAccessDot;
                if (next.type == TokenType::Star)
                    return Operator::PointerToMemberDot;
            }
            return InvalidToken();

        case TokenType::MinusGreaterThan:
            if (hasPrevious)
            {
                if (next.type == TokenType::Identifier)
                    return Operator::MemberAccessArrow;
                if (next.type == TokenType::Star)
                    return Operator::PointerToMemberArrow;
            }
            return InvalidToken();

        case TokenType::Star:
            if (!hasPrevious)
                return Operator::Dereference;
            return Operator::Multiplication;

        case TokenType::DoublePlus:
            if (hasPrevious)
                return Operator::PostIncrement;
            if (next.type == TokenType::Identifier)
                return Operator::PreIncrement;
            return InvalidToken();

        case TokenType::DoubleMinus:
            if (hasPrevious)
                return Operator::PostDecrement;
            if (next.type == TokenType::Identifier)
                return Operator::PreDecrement;
            return InvalidToken();

        case TokenType::Ampersand:
            if (!hasPrevious)
                return Operator::AddressOf;
            return Operator::BinaryAnd;

        case TokenType::Tilde:
            if (hasPrevious)
                return InvalidToken();
            return Operator::BinaryNot;

        case TokenType::Minus:
            if (hasPrevious)
                return Operator::Subtraction;
            return Operator::Negation;

        default:
            return BaseOperatorConversionLookup.contains(current.type) ? BaseOperatorConversionLookup.at(current.type) : std::optional<Operator>{};
    }
}

std::shared_ptr<Ast::Expression> Parser::ParsePrimary()
{
    const Token& token = GetCurrentToken();

    if (TokenIsOperator(token.type))
        return ParseOperationRhs(-1, nullptr); // Single-operand operation (e.g., dereference)

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
            auto result = RaiseError<Ast::Expression>("Invalid token in primary expression: {}", token.type);
            SkipTokens(1);
            return result;
    }
}

std::shared_ptr<Ast::Expression> Parser::ParseIdentifier()
{
    const auto& identifier = std::get<std::string>(GetCurrentToken().value.value());

    if (GetNextToken().type != TokenType::ParenthesisOpen)
        return std::make_shared<Ast::VariableUsage>(identifier);

    std::vector<std::shared_ptr<Ast::Expression>> arguments;
    if (GetNextToken().type != TokenType::ParenthesisClose)
    {
        while (true)
        {
            if (std::shared_ptr<Ast::Expression> argument = ParseExpression())
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

    return std::make_shared<Ast::FunctionCall>(identifier, std::move(arguments));
}

std::shared_ptr<Ast::Expression> Parser::ParseParentheses()
{
    SkipTokens(1);
    std::shared_ptr<Ast::Expression> expression = ParseExpression();
    if (!expression)
        return nullptr;

    const Token& next = GetCurrentToken();
    if (next.type != TokenType::ParenthesisClose)
        return RaiseError<Ast::Expression>("Expected ')', got: {}", next.type);

    SkipTokens(1);
    return expression;
}

std::shared_ptr<Ast::Expression> Parser::ParseLiteral()
{
    const Token& token = GetCurrentToken();
    std::shared_ptr<Ast::Expression> result;

    switch (token.type)
    {
        case TokenType::LiteralInteger:
            result = std::make_shared<Ast::LiteralNumber<s32>>(std::get<s32>(token.value.value()));
            break;

        case TokenType::LiteralCharacter:
            result = std::make_shared<Ast::LiteralCharacter>(std::get<c8>(token.value.value()));
            break;

        case TokenType::LiteralString:
            result = std::make_shared<Ast::LiteralString>(std::get<std::string>(token.value.value()));
            break;

        default:
            std::unreachable();
    }

    SkipTokens(1);
    return result;
}

std::shared_ptr<Ast::Expression> Parser::ParseOperationRhs(const s32 expressionPrecedence, std::shared_ptr<Ast::Expression> lhs)
{
    assert(TokenIsOperator(GetCurrentToken().type) && "ParseOperationRhs called on non-operator token");

    while (true)
    {
        const std::optional<Operator> optionalOperator = ParseCurrentOperator(lhs);
        if (!optionalOperator.has_value())
            return lhs;

        const Operator op = optionalOperator.value();

        if (!lhs && OperatorHasLeftOperand(op))
            return RaiseError<Ast::Expression>("Operator {} expected a left operand", op);

        if (op == Operator::Ternary)
            return ParseTernary(std::move(lhs));

        const s32 opPrecedence = OperatorGetPrecedence(op);

        if (opPrecedence < expressionPrecedence)
            return lhs;

        SkipTokens(1); // Skip the operator token

        auto rhs = TokenIsOperator(GetCurrentToken().type) ? ParseOperationRhs(opPrecedence + 1, nullptr) : ParsePrimary();
        if (!rhs && OperatorHasRightOperand(op))
            return nullptr;

        const std::optional<Operator> nextOptionalOperator = ParseCurrentOperator(nullptr);
        const s32 nextOperatorPrecedence = nextOptionalOperator.has_value() ? OperatorGetPrecedence(nextOptionalOperator.value()) : -1;

        if (opPrecedence < nextOperatorPrecedence)
        {
            rhs = ParseOperationRhs(opPrecedence + 1, std::move(rhs));
            if (!rhs)
                return nullptr;
        }

        lhs = std::make_shared<Ast::Operation>(op, std::move(lhs), std::move(rhs));
    }
}

std::shared_ptr<Ast::TypeName> Parser::ParseType()
{
    const Token& token = GetCurrentToken();

    std::variant<std::string, BuiltinType> type;
    switch (token.type)
    {
        case TokenType::Identifier:
            type = std::get<std::string>(token.value.value());
            break;

        case TokenType::Keyword:
        {
            auto keyword = std::get<Keyword>(token.value.value());
            if (!IsKeywordType(keyword))
                return RaiseError<Ast::TypeName>("Expected a type keyword, got {}", keyword);
            type = KeywordToBuiltinType(keyword);
            break;
        }

        default:
            return RaiseError<Ast::TypeName>("Expected either an identifier or a keyword for the type name, got {}", token.type);
    }

    SkipTokens(1);

    return std::make_shared<Ast::TypeName>(type);
}

std::shared_ptr<Ast::TypeIdentifierPair> Parser::ParseTypeIdentifierPair()
{
    std::shared_ptr<Ast::TypeName> type = ParseType();
    if (!type)
        return nullptr;

    const Token& name = GetCurrentToken();

    if (name.type != TokenType::Identifier)
        return RaiseError<Ast::TypeIdentifierPair>("Expected name in declaration, got {}", name.type);

    SkipTokens(1);

    return std::make_shared<Ast::TypeIdentifierPair>(std::move(type), std::get<std::string>(name.value.value()));
}

std::shared_ptr<Ast::IfElse> Parser::ParseIfElse()
{
    assert(GetCurrentToken().type == TokenType::Keyword && std::get<Keyword>(GetCurrentToken().value.value()) == Keyword::If);

    SkipTokens(1);

    if (GetCurrentToken().type != TokenType::ParenthesisOpen)
        return RaiseError<Ast::IfElse>("Expected '(' in if statement, got {}", GetCurrentToken().type);

    SkipTokens(1);

    std::shared_ptr<Ast::Expression> condition = ParseExpression();
    if (!condition)
        return nullptr;

    if (GetCurrentToken().type != TokenType::ParenthesisClose)
        return RaiseError<Ast::IfElse>("Expected ')' in if statement, got {}", GetCurrentToken().type);

    SkipTokens(1);

    std::shared_ptr<Ast::Scope> ifBody = ParseFunctionLevelScope();
    if (!ifBody)
        return nullptr;

    std::shared_ptr<Ast::Scope> elseBody;
    if (GetCurrentToken().type == TokenType::Keyword && std::get<Keyword>(GetCurrentToken().value.value()) == Keyword::Else)
    {
        SkipTokens(1);
        elseBody = ParseFunctionLevelScope();
    }

    return std::make_shared<Ast::IfElse>(std::move(condition), std::move(ifBody), std::move(elseBody));
}

std::shared_ptr<Ast::Ternary> Parser::ParseTernary(std::shared_ptr<Ast::Expression> condition)
{
    assert(GetCurrentToken().type == TokenType::QuestionMark);

    SkipTokens(1);

    std::shared_ptr<Ast::Expression> trueResult = ParseExpression();
    if (!trueResult)
        return nullptr;

    if (GetCurrentToken().type != TokenType::Colon)
        return RaiseError<Ast::Ternary>("Expected ':' in ternary expression, got {}", GetCurrentToken().type);

    SkipTokens(1);

    std::shared_ptr<Ast::Expression> falseResult = ParseExpression();

    return std::make_shared<Ast::Ternary>(std::move(condition), std::move(trueResult), std::move(falseResult));
}

std::shared_ptr<Ast::Scope> Parser::ParseScope(const bool isFunctionLevel)
{
    switch (GetCurrentToken().type)
    {
        case TokenType::EqualsGreaterThan:
        {
            if (!isFunctionLevel)
                return RaiseError<Ast::Scope>("Expected '{' for top-level scope, got '=>'");

            SkipTokens(1); // Skip '=>'

            auto result = std::make_shared<Ast::Scope>(true);
            // ReSharper disable once CppDFALocalValueEscapesFunction
            Ast::currentScope = result;

            result->GetBody().push_back(std::make_shared<Ast::FunctionReturn>(ParseExpression()));

            if (GetCurrentToken().type != TokenType::Semicolon)
                return RaiseError<Ast::Scope>("Expected ';' after Expression in single-statement function, got {}", GetCurrentToken().type);

            SkipTokens(1);

            Ast::currentScope = result->GetParentScope();
            return result;
        }

        case TokenType::CurlyBraceOpen:
        {
            SkipTokens(1); // Skip '{'

            auto result = std::make_shared<Ast::Scope>(isFunctionLevel);
            // ReSharper disable once CppDFALocalValueEscapesFunction
            Ast::currentScope = result;

            const auto statementParseFunction = isFunctionLevel ? &Parser::ParseFunctionLevelStatement : &Parser::ParseTopLevelStatement;

            std::vector<std::shared_ptr<Ast::Statement>> body;
            while (GetCurrentToken().type != TokenType::CurlyBraceClose)
                body.push_back((this->*statementParseFunction)());

            SkipTokens(1); // Skip '}'

            result->GetBody() = std::move(body);

            Ast::currentScope = result->GetParentScope();
            return result;
        }

        default:
            if (!isFunctionLevel)
                return RaiseError<Ast::Scope>("Expected '{' or '=>' for scope, got {}", GetCurrentToken().type);

            auto result = std::make_shared<Ast::Scope>(true);
            // ReSharper disable once CppDFALocalValueEscapesFunction
            Ast::currentScope = result;

            result->GetBody().push_back(ParseFunctionLevelStatement());

            Ast::currentScope = result->GetParentScope();
            return result;
    }
}

std::shared_ptr<Ast::Scope> Parser::ParseTopLevelScope()
{
    return ParseScope(false);
}

std::shared_ptr<Ast::Scope> Parser::ParseFunctionLevelScope()
{
    return ParseScope(true);
}

std::shared_ptr<Ast::Statement> Parser::ParseVariableDeclaration(std::shared_ptr<Ast::TypeIdentifierPair> typeNamePair)
{
    const Token& currentToken = GetCurrentToken();
    switch (currentToken.type)
    {
        case TokenType::Equals:
            return nullptr; // TODO

        case TokenType::Semicolon:
            return nullptr; // TODO

        default:
            return RaiseError("Expected either '=' or ';' in variable declaration, got {}", currentToken.type);
    }
}

std::shared_ptr<Ast::Statement> Parser::ParseFunction(std::shared_ptr<Ast::TypeIdentifierPair> typeNamePair)
{
    assert(GetCurrentToken().type == TokenType::ParenthesisOpen);

    std::vector<Ast::FunctionPrototype::Parameter> parameters;
    ParseFunctionParameters(parameters);

    auto prototype = std::make_shared<Ast::FunctionPrototype>(std::move(typeNamePair), std::move(parameters));

    switch (GetCurrentToken().type)
    {
        case TokenType::Semicolon:
            SkipTokens(1);
            return prototype;

        case TokenType::CurlyBraceOpen:
        case TokenType::EqualsGreaterThan:
            return std::make_shared<Ast::FunctionDefinition>(std::move(prototype), ParseFunctionLevelScope());

        default:
            return RaiseError("Expected either ';' or '{' in function declaration, got {}", GetCurrentToken().type);
    }

}

void Parser::ParseFunctionParameters(std::vector<Ast::FunctionPrototype::Parameter>& output)
{
    assert(GetCurrentToken().type == TokenType::ParenthesisOpen);

    if (GetNextToken().type != TokenType::ParenthesisClose)
    {
        while (true)
        {
            output.push_back(ParseTypeIdentifierPair());

            const Token& parameterSeparator = GetCurrentToken();

            if (parameterSeparator.type == TokenType::ParenthesisClose)
                break;

            if (parameterSeparator.type != TokenType::Comma)
                RaiseError("Expected ',' in the argument list, got {}", parameterSeparator.type);

            SkipTokens(1);
        }
    }

    SkipTokens(1); // Skip ')'
}

std::shared_ptr<Ast::FunctionPrototype> Parser::ParseFunctionPrototype()
{
    std::shared_ptr<Ast::TypeIdentifierPair> typeName = ParseTypeIdentifierPair();

    if (GetCurrentToken().type != TokenType::ParenthesisOpen)
        return RaiseError<Ast::FunctionPrototype>("Expected '(' in prototype, got {}", GetCurrentToken().type);

    std::vector<Ast::FunctionPrototype::Parameter> parameters;
    ParseFunctionParameters(parameters);

    return std::make_shared<Ast::FunctionPrototype>(std::move(typeName), std::move(parameters));
}

std::shared_ptr<Ast::FunctionDefinition> Parser::ParseFunctionDefinition()
{
    std::shared_ptr<Ast::FunctionPrototype> prototype = ParseFunctionPrototype();
    if (!prototype)
        return nullptr;

    const Token& openBrace = GetCurrentToken();
    if (openBrace.type != TokenType::CurlyBraceOpen)
        return RaiseError<Ast::FunctionDefinition>("Expected '{' in function definition, got {}", openBrace.type);

    return std::make_shared<Ast::FunctionDefinition>(std::move(prototype), ParseFunctionLevelScope());
}
