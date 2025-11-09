#pragma once

#include "Token.hpp"
#include "Utils.hpp"

namespace Ast
{
    class Statement
    {
    public:
        Statement() = default;
        virtual ~Statement() = default;
        DEFAULT_COPY_MOVE_OPERATIONS(Statement)

        virtual llvm::Value* CodeGeneration() = 0;
    };

    class Expression : public Statement
    {
    };

    template <std::integral NumberType>
    class Number : public Expression
    {
    public:
        explicit Number(NumberType value);

        GETTER_M(NumberType, Value)

    private:
        NumberType m_Value;
    };

    class Character : public Expression
    {
    public:
        explicit Character(char value);

        GETTER_M(char, Value)

    private:
        char m_Value;
    };

    class String : public Expression
    {
    public:
        explicit String(std::string value);

        GETTER_M(const std::string&, Value)

    private:
        std::string m_Value;
    };

    class Variable : public Expression
    {
    public:
        explicit Variable(std::string name);

        GETTER_M(const std::string&, Name)

    private:
        std::string m_Name;
    };

    class Operation : public Expression
    {
    public:
        Operation(Operator operatorType, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs);

        GETTER_M(Operator, OperatorType)
        GETTER_M(const std::unique_ptr<Expression>&, Lhs)
        GETTER_M(const std::unique_ptr<Expression>&, Rhs)

    private:
        Operator m_OperatorType;
        std::unique_ptr<Expression> m_Lhs, m_Rhs;
    };

    class FunctionCall : public Expression
    {
    public:
        FunctionCall(std::string callee, std::vector<std::unique_ptr<Expression>> arguments);

        GETTER_M(const std::string&, Callee)
        GETTER_M(const std::vector<std::unique_ptr<Expression>>&, Arguments)

    private:
        std::string m_Callee;
        std::vector<std::unique_ptr<Expression>> m_Arguments;
    };

    /// @brief Represents a type name, i.e., either a builtin type such as @c int or a user-defined type.
    class TypeName
    {
    public:
        explicit TypeName(std::string name);

        GETTER_M(const std::string&, Name)

    private:
        std::string m_Name;
    };

    /// @brief Represents a combination of a type and a name.
    class TypeNamePair : public Expression
    {
    public:
        explicit TypeNamePair(std::unique_ptr<TypeName> type, std::string name);

        GETTER_M(const std::unique_ptr<TypeName>&, Type)
        GETTER_M(const std::string&, Name)

    private:
        std::unique_ptr<TypeName> m_Type;
        std::string m_Name;
    };

    /// @brief Represents a function prototype, which captures its return type, name and its argument types and names.
    class FunctionPrototype : public Statement
    {
    public:
        using Parameter = std::unique_ptr<TypeNamePair>;

        FunctionPrototype(std::unique_ptr<TypeNamePair> typeNamePair, std::vector<Parameter> parameters);

        GETTER_M(const std::unique_ptr<TypeNamePair>&, TypeNamePair)
        GETTER_M(const std::vector<Parameter>&, Parameters)

    private:
        std::unique_ptr<TypeNamePair> m_TypeNamePair;
        std::vector<Parameter> m_Parameters;
    };

    /// @brief Represents a function definition, which includes its prototype and body.
    class FunctionDefinition : public Statement
    {
    public:
        FunctionDefinition(std::unique_ptr<FunctionPrototype> prototype, std::vector<std::unique_ptr<Statement>> body);

        GETTER_M(const std::unique_ptr<FunctionPrototype>&, Prototype)
        GETTER_M(const std::vector<std::unique_ptr<Statement>>&, Body)

    private:
        std::unique_ptr<FunctionPrototype> m_Prototype;
        std::vector<std::unique_ptr<Statement>> m_Body;
    };
}

// Start of AST.inl

namespace Ast
{
    template <std::integral NumberType>
    Number<NumberType>::Number(NumberType value)
        : m_Value{value}
    {
    }
}
