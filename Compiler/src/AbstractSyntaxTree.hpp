#pragma once

#include "Globals.hpp"
#include "Operator.hpp"
#include "Token.hpp"
#include "Utils.hpp"

namespace Ast
{
    class Scope;

    inline Scope* currentScope = nullptr;

    class Statement
    {
    public:
        Statement();
        virtual ~Statement() = default;
        DEFAULT_COPY_MOVE_OPERATIONS(Statement)

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

        Scope(std::vector<std::unique_ptr<Statement>> body, bool isFunctionLevel);
        Scope(std::unique_ptr<Statement> body, bool isFunctionLevel);
        explicit Scope(bool isFunctionLevel);

        llvm::Value* GenerateCode() override;

        GETTER_NON_CONST_M(std::vector<std::unique_ptr<Statement>>&, Body)
        GETTER_M(bool, IsFunctionLevel)
        GETTER_NON_CONST_M(IdentifierMap&, Identifiers)

    private:
        std::vector<std::unique_ptr<Statement>> m_Body;
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
        explicit LiteralCharacter(char value);

        llvm::Value* GenerateCode() override;

        GETTER_M(char, Value)

    private:
        char m_Value;
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

    class LocalVariable : public Expression
    {
    public:
        explicit LocalVariable(std::string name);

        llvm::Value* GenerateCode() override;

        GETTER_M(const std::string&, Name)

    private:
        std::string m_Name;
    };

    class Operation : public Expression
    {
    public:
        Operation(Operator operatorType, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs);

        llvm::Value* GenerateCode() override;

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

        llvm::Value* GenerateCode() override;

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
        explicit TypeName(std::variant<std::string, BuiltinType> type);

        ATTRIBUTE_NODISCARD
        llvm::Type* GetLlvmType() const;

        ATTRIBUTE_NODISCARD
        std::string_view GetType() const;

    private:
        std::variant<std::string, BuiltinType> m_Type;
    };

    /// @brief Represents a combination of a type and a name.
    class TypeIdentifierPair
    {
    public:
        explicit TypeIdentifierPair(std::unique_ptr<TypeName> type, std::string name);

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
        using Parameter = std::unique_ptr<TypeIdentifierPair>;

        FunctionPrototype(std::unique_ptr<TypeIdentifierPair> typeNamePair, std::vector<Parameter> parameters);

        llvm::Function* GenerateCode() override;

        GETTER_M(const std::unique_ptr<TypeIdentifierPair>&, TypeNamePair)
        GETTER_M(const std::vector<Parameter>&, Parameters)

    private:
        std::unique_ptr<TypeIdentifierPair> m_TypeNamePair;
        std::vector<Parameter> m_Parameters;
    };

    /// @brief Represents a function definition, which includes its prototype and body.
    class FunctionDefinition : public Statement
    {
    public:
        using BodyType = std::variant<std::unique_ptr<Scope>, std::unique_ptr<Expression>>;

        FunctionDefinition(std::unique_ptr<FunctionPrototype> prototype, BodyType body);

        llvm::Function* GenerateCode() override;

        GETTER_M(const std::unique_ptr<FunctionPrototype>&, Prototype)
        GETTER_M(const BodyType&, Body)

    private:
        std::unique_ptr<FunctionPrototype> m_Prototype;
        BodyType m_Body;
    };

    class FunctionReturn : public Statement
    {
    public:
        explicit FunctionReturn(std::unique_ptr<Expression> returnValue);

        llvm::Value* GenerateCode() override;

        GETTER_M(const std::unique_ptr<Expression>&, ReturnValue)

    private:
        std::unique_ptr<Expression> m_ReturnValue;
    };
}

// Start of AST.inl

namespace Ast
{
    template <typename T, typename ... Args>
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
        if constexpr (std::is_same_v<NumberType, int>)
            return llvm::ConstantInt::get(*theContext, llvm::APInt(32, m_Value, true));
        else
            static_assert(false, "Unhandled numeric type");

        std::unreachable();
    }
}
