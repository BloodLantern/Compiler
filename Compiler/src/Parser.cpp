#include "PrecompiledHeader.hpp"
#include "Parser.hpp"

Parser::Parser(const std::vector<Token>& tokens)
    : m_Tokens{&tokens}
{
}

std::unique_ptr<AbstractSyntaxTree::Expression> Parser::ParseCurrent()
{
    const Token& token = GetCurrentToken();
    switch (token.type)
    {
        case TokenType::Identifier: return ParseIdentifier();
        case TokenType::LiteralInteger: return ParseNumber();
        case TokenType::ParenthesisOpen: return ParseParentheses();

        default:
            RaiseError("Invalid token: {}", magic_enum::enum_name(token.type));
            Skip(1);
            return nullptr;
    }
}

void Parser::ParseAll(std::vector<std::unique_ptr<AbstractSyntaxTree::Expression>>& output)
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

std::unique_ptr<AbstractSyntaxTree::Expression> Parser::ParseIdentifier()
{
    const std::string& identifier = std::get<std::string>(GetCurrentToken().value.value());

    if (GetNextToken().type != TokenType::ParenthesisOpen)
        return std::make_unique<AbstractSyntaxTree::Variable>(identifier);

    std::vector<std::unique_ptr<AbstractSyntaxTree::Expression>> arguments;
    if (GetNextToken().type != TokenType::ParenthesisClose)
    {
        while (true)
        {
            std::unique_ptr<AbstractSyntaxTree::Expression> argument = ParseCurrent();
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

    return std::make_unique<AbstractSyntaxTree::FunctionCall>(identifier, std::move(arguments));
}

std::unique_ptr<AbstractSyntaxTree::Expression> Parser::ParseParentheses()
{
    Skip(1);
    std::unique_ptr<AbstractSyntaxTree::Expression> expression = ParseCurrent();
    if (!expression)
        return nullptr;

    const Token& next = GetCurrentToken();
    if (next.type != TokenType::ParenthesisClose)
        return RaiseError("Expected ')', got: {}", magic_enum::enum_name(next.type));

    Skip(1);
    return expression;
}

std::unique_ptr<AbstractSyntaxTree::Expression> Parser::ParseNumber()
{
    std::unique_ptr<AbstractSyntaxTree::Number<int>> result = std::make_unique<AbstractSyntaxTree::Number<int>>(
        std::get<int>(GetCurrentToken().value.value())
    );
    Skip(1);
    return result;
}
