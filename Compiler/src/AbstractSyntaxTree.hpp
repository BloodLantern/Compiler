#pragma once

#include "Common.hpp"
#include "Globals.hpp"
#include "Operator.hpp"
#include "Token.hpp"
#include "Utility/Utils.hpp"

namespace Ast
{
    class Scope;

    inline std::shared_ptr<Scope> currentScope;

    llvm::AllocaInst* CreateEntryBlockAlloca(llvm::Function* function, llvm::StringRef variableName);

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
        ATTRIBUTE_NODISCARD
        virtual llvm::Value* GenerateCode() const = 0;

        ATTRIBUTE_NODISCARD
        llvm::AllocaInst* LookupIdentifier(const std::string& name) const;
        void DeclareIdentifier(const std::string& name, llvm::AllocaInst* variable) const;

        GETTER_M(const std::shared_ptr<Scope>&, ParentScope)

    protected:
        template <typename T = llvm::Value, typename... Args>
        T* RaiseError(const std::string& message, Args&&... args) const;
        template <typename... Args>
        void RaiseWarning(const std::string& message, Args&&... args) const;

    private:
        std::shared_ptr<Scope> m_ParentScope;
        // Position m_Start, m_End; // TODO - Store file position for errors and warnings
    };

    class Expression : public Statement
    {
    };

    class Scope : public Statement
    {
    public:
        using IdentifierMap = std::unordered_map<std::string, llvm::AllocaInst*>;

        Scope(std::vector<std::shared_ptr<Statement>> body, bool isFunctionLevel);
        Scope(std::shared_ptr<Statement> body, bool isFunctionLevel);
        explicit Scope(bool isFunctionLevel);

        /// @returns Always returns @c nullptr
        ATTRIBUTE_NODISCARD
        llvm::Value* GenerateCode() const override;

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

        ATTRIBUTE_NODISCARD
        llvm::Value* GenerateCode() const override;

        GETTER_M(NumberType, Value)

    private:
        NumberType m_Value;
    };

    class LiteralCharacter : public Expression
    {
    public:
        explicit LiteralCharacter(c8 value);

        ATTRIBUTE_NODISCARD
        llvm::Value* GenerateCode() const override;

        GETTER_M(c8, Value)

    private:
        c8 m_Value;
    };

    class LiteralString : public Expression
    {
    public:
        explicit LiteralString(std::string value);

        ATTRIBUTE_NODISCARD
        llvm::Value* GenerateCode() const override;

        GETTER_M(const std::string&, Value)

    private:
        std::string m_Value;
    };

    class VariableDeclaration : public Expression
    {
    public:
        explicit VariableDeclaration(std::shared_ptr<TypeIdentifierPair> typeName);
        VariableDeclaration(std::shared_ptr<TypeIdentifierPair> typeName, std::shared_ptr<Expression> initialValue);

        ATTRIBUTE_NODISCARD
        llvm::Value* GenerateCode() const override;

        GETTER_M(const std::shared_ptr<TypeIdentifierPair>&, TypeName)
        GETTER_M(const std::shared_ptr<Expression>&, InitialValue)

    private:
        std::shared_ptr<TypeIdentifierPair> m_TypeName;
        std::shared_ptr<Expression> m_InitialValue;
    };

    class VariableUsage : public Expression
    {
    public:
        explicit VariableUsage(std::string name);

        ATTRIBUTE_NODISCARD
        llvm::Value* GenerateCode() const override;

        GETTER_M(const std::string&, Name)

    private:
        std::string m_Name;
    };

    class Operation : public Expression
    {
    public:
        Operation(Operator operatorType, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs);

        ATTRIBUTE_NODISCARD
        llvm::Value* GenerateCode() const override;

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

        ATTRIBUTE_NODISCARD
        llvm::Value* GenerateCode() const override;

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

        ATTRIBUTE_NODISCARD
        llvm::Function* GenerateCode() const override;

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

        ATTRIBUTE_NODISCARD
        llvm::Function* GenerateCode() const override;

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

        ATTRIBUTE_NODISCARD
        llvm::Value* GenerateCode() const override;

        GETTER_M(const std::shared_ptr<Expression>&, ReturnValue)

    private:
        std::shared_ptr<Expression> m_ReturnValue;
    };

    class IfElse : public Statement
    {
    public:
        explicit IfElse(std::shared_ptr<Expression> condition, std::shared_ptr<Scope> ifBody, std::shared_ptr<Scope> elseBody);

        ATTRIBUTE_NODISCARD
        llvm::Value* GenerateCode() const override;

        GETTER_M(const std::shared_ptr<Expression>&, Condition)
        GETTER_M(const std::shared_ptr<Scope>&, IfBody)
        GETTER_M(const std::shared_ptr<Scope>&, ElseBody)

    private:
        std::shared_ptr<Expression> m_Condition;
        std::shared_ptr<Scope> m_IfBody, m_ElseBody;
    };

    class Ternary : public Expression
    {
    public:
        explicit Ternary(std::shared_ptr<Expression> condition, std::shared_ptr<Expression> trueResult, std::shared_ptr<Expression> falseResult);

        ATTRIBUTE_NODISCARD
        llvm::Value* GenerateCode() const override;

        GETTER_M(const std::shared_ptr<Expression>&, Condition)
        GETTER_M(const std::shared_ptr<Expression>&, TrueResult)
        GETTER_M(const std::shared_ptr<Expression>&, FalseResult)

    private:
        std::shared_ptr<Expression> m_Condition;
        std::shared_ptr<Expression> m_TrueResult, m_FalseResult;
    };
}

// Start of AST.inl

namespace Ast
{
    template <typename T, typename... Args>
    T* Statement::RaiseError(const std::string& message, Args&&... args) const
    {
        std::println(
            std::cerr,
            "Code generation error: {}",
            std::vformat(message, std::make_format_args(args...))
        );

        return nullptr;
    }

    template <typename ... Args>
    void Statement::RaiseWarning(const std::string& message, Args&&... args) const
    {
        std::println(
            std::cout,
            "Code generation warning: {}",
            std::vformat(message, std::make_format_args(args...))
        );
    }

    template <std::integral NumberType>
    LiteralNumber<NumberType>::LiteralNumber(NumberType value)
        : m_Value{value}
    {
    }

    template <std::integral NumberType>
    llvm::Value* LiteralNumber<NumberType>::GenerateCode() const
    {
        if constexpr (std::is_same_v<NumberType, s32>)
            return llvm::ConstantInt::get(*llvmContext, llvm::APInt(32, m_Value, true));
        else
            static_assert(false, "Unhandled numeric type");

        std::unreachable();
    }
}
