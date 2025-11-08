#include "PrecompiledHeader.hpp"
#include "AbstractSyntaxTree.hpp"

using namespace Ast;

Character::Character(const char value)
    : m_Value{value}
{
}

String::String(std::string value)
    : m_Value{std::move(value)}
{
}

Variable::Variable(std::string name)
    : m_Name{std::move(name)}
{
}

Operation::Operation(const Operator operatorType, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs)
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

FunctionDefinition::FunctionDefinition(std::unique_ptr<FunctionPrototype> prototype, std::vector<std::unique_ptr<Statement>> body)
    : m_Prototype{std::move(prototype)}
    , m_Body{std::move(body)}
{
}

TypeName::TypeName(std::string name)
    : m_Name{std::move(name)}
{
}

TypeNamePair::TypeNamePair(std::unique_ptr<TypeName> type, std::string name)
    : m_Type{std::move(type)}
    , m_Name{std::move(name)}
{
}

FunctionPrototype::FunctionPrototype(std::unique_ptr<TypeNamePair> typeNamePair, std::vector<Parameter> parameters)
    : m_TypeNamePair{std::move(typeNamePair)}
    , m_Parameters{std::move(parameters)}
{
}
