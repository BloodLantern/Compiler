#include "AbstractSyntaxTree.hpp"

using namespace Ast;

Scope::Scope(std::vector<std::unique_ptr<Statement>> body, const bool isFunctionLevel)
    : m_Body{std::move(body)}
    , m_IsFunctionLevel{isFunctionLevel}
{
}

llvm::Value* Scope::GenerateCode()
{
    return m_Body[0]->GenerateCode(); // TODO - Handle multiple statements in scope body
}

LiteralCharacter::LiteralCharacter(const char value)
    : m_Value{value}
{
}

llvm::Value* LiteralCharacter::GenerateCode()
{
    return llvm::ConstantInt::get(*theContext, llvm::APInt{8, static_cast<uint32_t>(m_Value)});
}

LiteralString::LiteralString(std::string value)
    : m_Value{std::move(value)}
{
}

llvm::Value* LiteralString::GenerateCode()
{
    return nullptr;
}

LocalVariable::LocalVariable(std::string name)
    : m_Name{std::move(name)}
{
}

llvm::Value* LocalVariable::GenerateCode()
{
    // Look this variable up in the function.
    llvm::Value* v = namedValues[m_Name];
    if (!v)
        return RaiseError("Unknown variable name: {}", m_Name);
    return v;
}

Operation::Operation(const Operator operatorType, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs)
    : m_OperatorType{operatorType}
    , m_Lhs{std::move(lhs)}
    , m_Rhs{std::move(rhs)}
{
}

llvm::Value* Operation::GenerateCode()
{
    if (!m_Lhs)
    {
        assert(false && "Unhandled operation");
        return nullptr;
    }

    llvm::Value* lhs = m_Lhs->GenerateCode();
    llvm::Value* rhs = m_Rhs->GenerateCode();

    if (!lhs || !rhs)
        return nullptr;

    switch (m_OperatorType)
    {
        case Operator::Addition:
            return builder->CreateAdd(lhs, rhs, "addtmp");
        case Operator::Subtraction:
            return builder->CreateSub(lhs, rhs, "subtmp");
        case Operator::Multiplication:
            return builder->CreateMul(lhs, rhs, "multmp");
        case Operator::Division:
            return builder->CreateSDiv(lhs, rhs, "divtmp");
        default:
            return RaiseError("Invalid binary operator: {}", m_OperatorType);
    }
}

FunctionCall::FunctionCall(std::string callee, std::vector<std::unique_ptr<Expression>> arguments)
    : m_Callee{std::move(callee)}
    , m_Arguments{std::move(arguments)}
{
}

llvm::Value* FunctionCall::GenerateCode()
{
    // Look up the name in the global module table.
    llvm::Function* calleeFunction = theModule->getFunction(m_Callee);
    if (!calleeFunction)
        return RaiseError("Unknown function referenced");

    // If argument mismatch error.
    if (calleeFunction->arg_size() != m_Arguments.size())
        return RaiseError("Incorrect number of arguments passed");

    std::vector<llvm::Value*> arguments;
    for (const std::unique_ptr<Expression>& m_Argument : m_Arguments)
    {
        arguments.push_back(m_Argument->GenerateCode());
        if (!arguments.back())
            return nullptr;
    }

    return builder->CreateCall(calleeFunction, arguments, "calltmp");
}

FunctionDefinition::FunctionDefinition(std::unique_ptr<FunctionPrototype> prototype, std::unique_ptr<Scope> body)
    : m_Prototype{std::move(prototype)}
    , m_Body{std::move(body)}
{
}

llvm::Function* FunctionDefinition::GenerateCode()
{
    // First, check for an existing function from a previous 'extern' declaration.
    llvm::Function* function = theModule->getFunction(m_Prototype->GetTypeNamePair()->GetName());

    if (!function)
        function = m_Prototype->GenerateCode();

    if (!function)
        return nullptr;

    if (!function->empty())
        return RaiseError<llvm::Function>("Function cannot be redefined.");

    // Create a new basic block to start insertion into.
    llvm::BasicBlock* block = llvm::BasicBlock::Create(*theContext, "entry", function);
    builder->SetInsertPoint(block);

    // Record the function arguments in the namedValues map.
    namedValues.clear();
    for (llvm::Argument& argument : function->args())
        namedValues[std::string{argument.getName()}] = &argument;

    if (m_Body->GenerateCode())
    {
        // Validate the generated code, checking for consistency.
        // verifyFunction(*function);

        return function;
    }

    // Error reading body, remove function.
    function->eraseFromParent();
    return nullptr;
}

FunctionReturn::FunctionReturn(std::unique_ptr<Expression> returnValue)
    : m_ReturnValue{std::move(returnValue)}
{
}

llvm::Value* FunctionReturn::GenerateCode()
{
    if (!m_ReturnValue)
        return builder->CreateRet(nullptr); // Return void

    llvm::Value* returnValue = m_ReturnValue->GenerateCode();
    if (!returnValue)
        return nullptr;
    return builder->CreateRet(returnValue);
}

TypeName::TypeName(std::variant<std::string, BuiltinType> type)
    : m_Type{std::move(type)}
{
}

llvm::Type* TypeName::GetLlvmType() const
{
    if (const std::string* value = std::get_if<std::string>(&m_Type))
        return nullptr; // TODO - Struct types

    switch (std::get<BuiltinType>(m_Type))
    {
        case BuiltinType::Void:
            return llvm::Type::getVoidTy(*theContext);

        case BuiltinType::Bool:
            return llvm::Type::getInt1Ty(*theContext);

        case BuiltinType::Char:
        case BuiltinType::SByte:
        case BuiltinType::Byte:
            return llvm::Type::getInt8Ty(*theContext);

        case BuiltinType::Short:
        case BuiltinType::UShort:
            return llvm::Type::getInt16Ty(*theContext);

        case BuiltinType::Int:
        case BuiltinType::UInt:
            return llvm::Type::getInt32Ty(*theContext);

        case BuiltinType::Long:
        case BuiltinType::ULong:
            return llvm::Type::getInt64Ty(*theContext);

        case BuiltinType::String:
            return llvm::PointerType::getUnqual(*theContext);
    }

    std::println(std::cerr,"Invalid BuiltinType value");
    Utils::Exit(ErrorCodes::FatalError);
}

std::string_view TypeName::GetType() const
{
    if (const std::string* value = std::get_if<std::string>(&m_Type))
        return *value;

    return KeywordStrings.at(BuiltinTypeToKeyword(std::get<BuiltinType>(m_Type)));
}

TypeIdentifierPair::TypeIdentifierPair(std::unique_ptr<TypeName> type, std::string name)
    : m_Type{std::move(type)}
    , m_Name{std::move(name)}
{
}

FunctionPrototype::FunctionPrototype(std::unique_ptr<TypeIdentifierPair> typeNamePair, std::vector<Parameter> parameters)
    : m_TypeNamePair{std::move(typeNamePair)}
    , m_Parameters{std::move(parameters)}
{
}

llvm::Function* FunctionPrototype::GenerateCode()
{
    // Make the function type
    const std::vector<llvm::Type*> parameters{m_Parameters.size(), llvm::Type::getInt32Ty(*theContext)};
    llvm::FunctionType* functionType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*theContext), parameters, false);

    llvm::Function* function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, m_TypeNamePair->GetName(), theModule.get());

    // Set names for all arguments.
    for (size_t i = 0; i < function->arg_size(); i++)
        (function->arg_begin() + i)->setName(m_Parameters[i]->GetName());

    return function;
}
