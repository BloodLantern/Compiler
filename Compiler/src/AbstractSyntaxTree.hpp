#pragma once

#include "Token.hpp"
#include "Utils.hpp"

namespace Ast
{
    class Expression
    {
    public:
        Expression() = default;
        virtual ~Expression() = default;
        DEFAULT_COPY_MOVE_OPERATIONS(Expression)
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
        Operation(TokenType operatorType, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs);

        GETTER_M(TokenType, OperatorType)
        GETTER_M(const std::unique_ptr<Expression>&, Lhs)
        GETTER_M(const std::unique_ptr<Expression>&, Rhs)

    private:
        TokenType m_OperatorType;
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

    /// @brief Represents a function prototype, which captures its name and its argument types and names.
    class Prototype
    {
    public:
        /// @brief A parameter consists of a type and a name.
        using Parameter = std::pair<std::string, std::string>;

        Prototype(std::string name, std::vector<Parameter> parameters);

        GETTER_M(const std::string&, Name)
        GETTER_M(const std::vector<Parameter>&, Parameters)

    private:
        std::string m_Name;
        std::vector<Parameter> m_Parameters;
    };

    /// @brief Represents a function definition, which includes its prototype and body.
    class Function
    {
    public:
        Function(std::unique_ptr<Prototype> prototype, std::unique_ptr<Expression> body);

        GETTER_M(const std::unique_ptr<Prototype>&, Prototype)
        GETTER_M(const std::unique_ptr<Expression>&, Body)

    private:
        std::unique_ptr<Prototype> m_Prototype;
        std::unique_ptr<Expression> m_Body;
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
