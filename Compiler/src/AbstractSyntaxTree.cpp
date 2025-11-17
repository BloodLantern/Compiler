#include "AbstractSyntaxTree.hpp"

#include <llvm/IR/Verifier.h>

using namespace Ast;

Statement::Statement()
    : m_ParentScope{currentScope}
{
}

llvm::Value* Statement::LookupIdentifier(const std::string_view name) const
{
    if (!m_ParentScope)
        return nullptr;

    if (!m_ParentScope->GetIdentifiers().contains(name))
        return m_ParentScope->LookupIdentifier(name);
    return m_ParentScope->GetIdentifiers().at(name);
}

Scope::Scope(std::vector<std::unique_ptr<Statement>> body, const bool isFunctionLevel)
    : m_Body{std::move(body)}
    , m_IsFunctionLevel{isFunctionLevel}
{
}

Scope::Scope(std::unique_ptr<Statement> body, const bool isFunctionLevel)
    : m_IsFunctionLevel{isFunctionLevel}
{
    m_Body.push_back(std::move(body));
}

Scope::Scope(const bool isFunctionLevel)
    : m_IsFunctionLevel{isFunctionLevel}
{
}

llvm::Value* Scope::GenerateCode()
{
    for (size_t i = 0; i < m_Body.size() - 1; i++)
    {
        if (m_Body[i]->GenerateCode() == nullptr)
            return nullptr;
    }
    return m_Body[m_Body.size() - 1]->GenerateCode();
}

bool Scope::IsSingleStatement() const
{
    return m_Body.size() == 1;
}

LiteralCharacter::LiteralCharacter(const c8 value)
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

VariableDeclaration::VariableDeclaration(std::unique_ptr<TypeIdentifierPair> typeName)
    : m_TypeName{std::move(typeName)}
{
}

llvm::Value* VariableDeclaration::GenerateCode()
{
    return RaiseError("Variable declarations are currently not supported"); // TODO - Add variable declarations support
}

VariableUsage::VariableUsage(std::string name)
    : m_Name{std::move(name)}
{
}

llvm::Value* VariableUsage::GenerateCode()
{
    llvm::Value* v = LookupIdentifier(m_Name);
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
    assert(m_Lhs && "Unhandled operation");

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
    // Look up the name in the global module table
    llvm::Function* calleeFunction = theModule->getFunction(m_Callee);
    if (!calleeFunction)
        return RaiseError("Unknown function referenced");

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
    // First, check for an existing function from a previous declaration.
    llvm::Function* function = theModule->getFunction(m_Prototype->GetTypeNamePair()->GetName());

    if (!function)
        function = m_Prototype->GenerateCode();

    if (!function)
        return nullptr;

    if (!function->empty())
        return RaiseError<llvm::Function>("Function {} has already been defined", function->getName());

    // Create a new basic block to start insertion into
    llvm::BasicBlock* block = llvm::BasicBlock::Create(*theContext, "entry", function);
    builder->SetInsertPoint(block);

    auto& identifiers = m_Body->GetIdentifiers();
    // Record the function arguments in the identifier map
    identifiers.clear();
    for (llvm::Argument& argument : function->args())
        identifiers[argument.getName().data()] = &argument;

    if (!m_Body->GenerateCode()) // If body code generation failed
    {
        // Error reading body, remove function
        function->eraseFromParent();
        return nullptr;
    }

    // Validate the generated code, checking for consistency
    llvm::verifyFunction(*function);

    return function;
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

IfElse::IfElse(std::unique_ptr<Expression> condition, std::unique_ptr<Scope> ifBody, std::unique_ptr<Scope> elseBody)
    : m_Condition{std::move(condition)}
    , m_IfBody{std::move(ifBody)}
    , m_ElseBody{std::move(elseBody)}
{
}

llvm::Value* IfElse::GenerateCode()
{
    llvm::Value* condition = m_Condition->GenerateCode();
    if (!condition)
        return nullptr;

    condition = builder->CreateICmpNE(condition, llvm::ConstantInt::get(*theContext, llvm::APInt{1, 0}), "ifcond");

    llvm::Function* function = builder->GetInsertBlock()->getParent();

    // Create blocks for the if and else cases
    // Insert the 'if' block at the end of the function
    llvm::BasicBlock* ifBlock = llvm::BasicBlock::Create(*theContext, "if", function);
    llvm::BasicBlock* elseBlock = llvm::BasicBlock::Create(*theContext, "else");
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*theContext, "ifcont");

    builder->CreateCondBr(condition, ifBlock, elseBlock);

    // Emit if value
    builder->SetInsertPoint(ifBlock);

    llvm::Value* ifReturnValue = m_IfBody->GenerateCode();
    if (!ifReturnValue)
        return nullptr;

    builder->CreateBr(mergeBlock);
    // Codegen of 'if' can change the current block, update ifBlock for the Phi
    ifBlock = builder->GetInsertBlock();

    // Emit else block
    function->insert(function->end(), elseBlock);
    builder->SetInsertPoint(elseBlock);

    llvm::Value* elseReturnValue = m_ElseBody->GenerateCode();
    if (!elseReturnValue)
        return nullptr;

    builder->CreateBr(mergeBlock);
    // Codegen of 'else' can change the current block, update elseBlock for the Phi
    elseBlock = builder->GetInsertBlock();

    // Emit merge block
    function->insert(function->end(), mergeBlock);
    builder->SetInsertPoint(mergeBlock);
    llvm::PHINode* phiNode = builder->CreatePHI(llvm::Type::getInt32Ty(*theContext), 2, "iftmp");

    phiNode->addIncoming(ifReturnValue, ifBlock);
    phiNode->addIncoming(elseReturnValue, elseBlock);
    return phiNode;
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

    // Set names for all arguments
    for (size_t i = 0; i < function->arg_size(); i++)
        (function->arg_begin() + i)->setName(m_Parameters[i]->GetName());

    return function;
}
