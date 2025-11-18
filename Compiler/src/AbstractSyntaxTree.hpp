#pragma once

#include "Common.hpp"
#include "Globals.hpp"
#include "Operator.hpp"
#include "Token.hpp"
#include "Utility/Utils.hpp"

namespace Ast
{
    class Scope;

    inline Scope* currentScope = nullptr;

    /// @brief Helper class that represents a type name, i.e., either a builtin type such as @c int or a user-defined type.
    class TypeName
    {
    public:
        explicit TypeName(std::variant<std::string, BuiltinType> type);

        ATTRIBUTE_NODISCARD
        llvm::Type* GetLlvmType() const;

        ATTRIBUTE_NODISCARD
        std::string_view GetType() const;

    private:
        std::variant<std::string, BuiltinType> m_Type;
    };

    /// @brief Helper class that represents a combination of a type and a name.
    class TypeIdentifierPair
    {
    public:
        explicit TypeIdentifierPair(std::shared_ptr<TypeName> type, std::string name);

        GETTER_M(const std::shared_ptr<TypeName>&, Type)
        GETTER_M(const std::string&, Name)

    private:
        std::shared_ptr<TypeName> m_Type;
        std::string m_Name;
    };

    class Statement
    {
    public:
        Statement();
        virtual ~Statement() = default;
        DEFAULT_COPY_MOVE_OPERATIONS(Statement)

        /// @returns For @c Expression, returns the expression result value. Otherwise, this returns @c nullptr.
        virtual llvm::Value* GenerateCode() = 0;

        ATTRIBUTE_NODISCARD
        llvm::Value* LookupIdentifier(std::string_view name) const;

        GETTER_M(Scope*, ParentScope)

    protected:
        template <typename T = llvm::Value, typename... Args>
        T* RaiseError(const std::string& message, Args&&... args);

    private:
        Scope* m_ParentScope = nullptr;
    };

    class Expression : public Statement
    {
    };

    class Scope : public Statement
    {
    public:
        using IdentifierMap = std::unordered_map<std::string_view, llvm::Value*>;

        Scope(std::vector<std::shared_ptr<Statement>> body, bool isFunctionLevel);
        Scope(std::shared_ptr<Statement> body, bool isFunctionLevel);
        explicit Scope(bool isFunctionLevel);

        /// @returns Always returns @c nullptr
        llvm::Value* GenerateCode() override;

        ATTRIBUTE_NODISCARD
        bool IsSingleStatement() const;

        GETTER_NON_CONST_M(std::vector<std::shared_ptr<Statement>>&, Body)
        GETTER_M(bool, IsFunctionLevel)
        GETTER_NON_CONST_M(IdentifierMap&, Identifiers)

    private:
        std::vector<std::shared_ptr<Statement>> m_Body;
        bool m_IsFunctionLevel;

        IdentifierMap m_Identifiers;
    };

    template <std::integral NumberType>
    class LiteralNumber : public Expression
    {
    public:
        explicit LiteralNumber(NumberType value);

        llvm::Value* GenerateCode() override;

        GETTER_M(NumberType, Value)

    private:
        NumberType m_Value;
    };

    class LiteralCharacter : public Expression
    {
    public:
        explicit LiteralCharacter(c8 value);

        llvm::Value* GenerateCode() override;

        GETTER_M(c8, Value)

    private:
        c8 m_Value;
    };

    class LiteralString : public Expression
    {
    public:
        explicit LiteralString(std::string value);

        llvm::Value* GenerateCode() override;

        GETTER_M(const std::string&, Value)

    private:
        std::string m_Value;
    };

    class VariableDeclaration : public Expression
    {
    public:
        explicit VariableDeclaration(std::shared_ptr<TypeIdentifierPair> typeName);

        llvm::Value* GenerateCode() override;

        GETTER_M(const std::shared_ptr<TypeIdentifierPair>&, TypeName)

    private:
        std::shared_ptr<TypeIdentifierPair> m_TypeName;
    };

    class VariableUsage : public Expression
    {
    public:
        explicit VariableUsage(std::string name);

        llvm::Value* GenerateCode() override;

        GETTER_M(const std::string&, Name)

    private:
        std::string m_Name;
    };

    class Operation : public Expression
    {
    public:
        Operation(Operator operatorType, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs);

        llvm::Value* GenerateCode() override;

        GETTER_M(Operator, OperatorType)
        GETTER_M(const std::shared_ptr<Expression>&, Lhs)
        GETTER_M(const std::shared_ptr<Expression>&, Rhs)

    private:
        Operator m_OperatorType;
        std::shared_ptr<Expression> m_Lhs, m_Rhs;
    };

    class FunctionCall : public Expression
    {
    public:
        FunctionCall(std::string callee, std::vector<std::shared_ptr<Expression>> arguments);

        llvm::Value* GenerateCode() override;

        GETTER_M(const std::string&, Callee)
        GETTER_M(const std::vector<std::shared_ptr<Expression>>&, Arguments)

    private:
        std::string m_Callee;
        std::vector<std::shared_ptr<Expression>> m_Arguments;
    };

    /// @brief Represents a function prototype, which captures its return type, name and its argument types and names.
    class FunctionPrototype : public Statement
    {
    public:
        using Parameter = std::shared_ptr<TypeIdentifierPair>;

        FunctionPrototype(std::shared_ptr<TypeIdentifierPair> typeNamePair, std::vector<Parameter> parameters);

        llvm::Function* GenerateCode() override;

        GETTER_M(const std::shared_ptr<TypeIdentifierPair>&, TypeNamePair)
        GETTER_M(const std::vector<Parameter>&, Parameters)

    private:
        std::shared_ptr<TypeIdentifierPair> m_TypeNamePair;
        std::vector<Parameter> m_Parameters;
    };

    /// @brief Represents a function definition, which includes its prototype and body.
    class FunctionDefinition : public Statement
    {
    public:
        FunctionDefinition(std::shared_ptr<FunctionPrototype> prototype, std::shared_ptr<Scope> body);

        llvm::Function* GenerateCode() override;

        GETTER_M(const std::shared_ptr<FunctionPrototype>&, Prototype)
        GETTER_M(const std::shared_ptr<Scope>&, Body)

    private:
        std::shared_ptr<FunctionPrototype> m_Prototype;
        std::shared_ptr<Scope> m_Body;
    };

    class FunctionReturn : public Statement
    {
    public:
        explicit FunctionReturn(std::shared_ptr<Expression> returnValue);

        llvm::Value* GenerateCode() override;

        GETTER_M(const std::shared_ptr<Expression>&, ReturnValue)

    private:
        std::shared_ptr<Expression> m_ReturnValue;
    };

    class IfElse : public Statement
    {
    public:
        explicit IfElse(std::shared_ptr<Expression> condition, std::shared_ptr<Scope> ifBody, std::shared_ptr<Scope> elseBody);

        llvm::Value* GenerateCode() override;

        GETTER_M(const std::shared_ptr<Expression>&, Condition)
        GETTER_M(const std::shared_ptr<Scope>&, IfBody)
        GETTER_M(const std::shared_ptr<Scope>&, ElseBody)

    private:
        std::shared_ptr<Expression> m_Condition;
        std::shared_ptr<Scope> m_IfBody, m_ElseBody;
    };
}

// Start of AST.inl

namespace Ast
{
    template <typename T, typename... Args>
    T* Statement::RaiseError(const std::string& message, Args&&... args)
    {
        std::println(
            std::cerr,
            "Code generation error: {}",
            std::vformat(message, std::make_format_args(args...))
        );

        return nullptr;
    }

    template <std::integral NumberType>
    LiteralNumber<NumberType>::LiteralNumber(NumberType value)
        : m_Value{value}
    {
    }

    template <std::integral NumberType>
    llvm::Value* LiteralNumber<NumberType>::GenerateCode()
    {
        if constexpr (std::is_same_v<NumberType, s32>)
            return llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, m_Value, true));
        else
            static_assert(false, "Unhandled numeric type");

        std::unreachable();
    }
}
