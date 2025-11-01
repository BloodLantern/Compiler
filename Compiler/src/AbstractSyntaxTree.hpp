#pragma once

#include "Utils.hpp"

namespace AbstractSyntaxTree
{
    class Expression
    {
    public:
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

    class Variable : public Expression
    {
    public:
        explicit Variable(const std::string& name);

        GETTER_M(const std::string&, Name)

    private:
        std::string m_Name;
    };

    class Operation : public Expression
    {
    public:
        Operation(char operatorCharacter, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs);

    private:
        char m_Operator;
        std::unique_ptr<Expression> m_Lhs, m_Rhs;
    };

    class FunctionCall : public Expression
    {
    public:
        FunctionCall(const std::string& callee, std::vector<std::unique_ptr<Expression>> arguments);

    private:
        std::string m_Callee;
        std::vector<std::unique_ptr<Expression>> m_Arguments;
    };

    /// @brief Represents a function prototype, which captures its name and its argument types and names.
    class Prototype
    {
    private:
        std::string m_Name;

    };
}
