#include "PrecompiledHeader.hpp"
#include "AbstractSyntaxTree.hpp"

using namespace Ast;

Variable::Variable(std::string name)
    : m_Name{std::move(name)}
{
}

Operation::Operation(const TokenType operatorType, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs)
    : m_OperatorType{operatorType}
    , m_Lhs{std::move(lhs)}
    , m_Rhs{std::move(rhs)}
{
}

FunctionCall::FunctionCall(std::string callee, std::vector<std::unique_ptr<Expression>> arguments)
    : m_Callee{std::move(callee)}
    , m_Arguments{std::move(arguments)}
{
}

Prototype::Prototype(std::string name, std::vector<Parameter> parameters)
    : m_Name{std::move(name)}
    , m_Parameters{std::move(parameters)}
{
}

Function::Function(std::unique_ptr<Prototype> prototype, std::unique_ptr<Expression> body)
    : m_Prototype{std::move(prototype)}
    , m_Body{std::move(body)}
{
}
