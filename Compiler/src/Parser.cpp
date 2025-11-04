#include "PrecompiledHeader.hpp"
#include "Parser.hpp"

Parser::Parser(std::vector<Token>& tokens)
    : m_Tokens{&tokens}
{
    // PreProcessOperators(tokens);
}

std::unique_ptr<Ast::Expression> Parser::ParseCurrent()
{
    std::unique_ptr<Ast::Expression> lhs = ParsePrimary();
    if (!lhs)
        return nullptr;

    return ParseOperationRhs(magic_enum::enum_count<Operator>(), std::move(lhs));
}

void Parser::ParseAll(std::vector<std::unique_ptr<Ast::Expression>>& output)
{
    do
        output.push_back(ParseCurrent());
    while (GetCurrentToken().type != TokenType::EndOfFile);
}

const Token& Parser::GetCurrentToken() const
{
    return (*m_Tokens)[m_Index];
}

const Token& Parser::GetNextToken()
{
    return (*m_Tokens)[++m_Index];
}

void Parser::Skip(const size_t count)
{
    m_Index += count;
}

void Parser::PreProcessOperators(std::vector<Token>& tokens)
{
    for (auto it = tokens.begin(); it != tokens.end(); ++it)
    {
        Token& current = *it;

        if (current.type == TokenType::None)
        {
            it = tokens.erase(it);
            continue;
        }

        if (!OperatorConversionLookup.contains(current.type))
            continue;

        Token* previous = it == tokens.begin() ? nullptr : &*(it - 1);
        Token* next = it == tokens.end() - 1 ? nullptr : &*(it + 1);

        const std::optional<Operator> op = GetOperator(previous, current, next);

        if (op.has_value())
            current.value = op.value();

        current.type = TokenType::Operator;
    }
}

std::optional<Operator> Parser::GetOperator(Token* previous, Token& current, Token* next)
{
    static const auto InvalidToken = [current, this] -> std::optional<Operator>
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
                {
                    next->type = TokenType::None;
                    return Operator::PointerToMemberDot;
                }
            }
            return InvalidToken();

        case TokenType::MinusGreaterThan:
            if (previous->type == TokenType::Identifier)
            {
                if (next->type == TokenType::Identifier)
                    return Operator::MemberAccessArrow;
                if (next->type == TokenType::Star)
                {
                    next->type = TokenType::None;
                    return Operator::PointerToMemberArrow;
                }
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
            return OperatorConversionLookup.at(current.type);
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
            RaiseError("Invalid token: {}", magic_enum::enum_name(token.type));
            Skip(1);
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
            std::unique_ptr<Ast::Expression> argument = ParseCurrent();
            if (argument)
                arguments.push_back(std::move(argument));
            else
                return nullptr;

            const Token& next = GetCurrentToken();
            if (next.type == TokenType::ParenthesisClose)
                break;

            if (next.type != TokenType::Comma)
                return RaiseError("Expected ')' or ',' in argument list, got: {}", magic_enum::enum_name(next.type));

            Skip(1);
        }
    }

    Skip(1);

    return std::make_unique<Ast::FunctionCall>(identifier, std::move(arguments));
}

std::unique_ptr<Ast::Expression> Parser::ParseParentheses()
{
    Skip(1);
    std::unique_ptr<Ast::Expression> expression = ParseCurrent();
    if (!expression)
        return nullptr;

    const Token& next = GetCurrentToken();
    if (next.type != TokenType::ParenthesisClose)
        return RaiseError("Expected ')', got: {}", magic_enum::enum_name(next.type));

    Skip(1);
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

    Skip(1);
    return result;
}

std::unique_ptr<Ast::Expression> Parser::ParseOperationRhs(size_t expressionPrecedence, std::unique_ptr<Ast::Expression> lhs)
{
    const Token& current = GetCurrentToken();
    if (!IsTokenOperator(current.type))
        return lhs;

    Skip(1);

    return lhs;
}
