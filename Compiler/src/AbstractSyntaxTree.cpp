#include "AbstractSyntaxTree.hpp"

#include <llvm/IR/Verifier.h>

using namespace Ast;

llvm::AllocaInst* Ast::CreateEntryBlockAlloca(llvm::Function* function, llvm::Type* type, const llvm::StringRef variableName)
{
    auto& entryBlock = function->getEntryBlock();
    llvm::IRBuilder temporaryBuilder{&entryBlock, entryBlock.begin()};
    return temporaryBuilder.CreateAlloca(type, nullptr, variableName);
}

Type::Type(std::variant<std::string, BuiltinType> type)
    : m_Type{std::move(type)}
{
}

llvm::Type* Type::GetLlvmType() const
{
    if (const std::string* value = std::get_if<std::string>(&m_Type))
        return nullptr; // TODO - Struct types

    switch (std::get<BuiltinType>(m_Type))
    {
        case BuiltinType::Void:
            return llvm::Type::getVoidTy(*llvmContext);

        case BuiltinType::Bool:
            return llvm::Type::getInt1Ty(*llvmContext);

        case BuiltinType::C8:
        case BuiltinType::S8:
        case BuiltinType::U8:
            return llvm::Type::getInt8Ty(*llvmContext);

        case BuiltinType::C16:
        case BuiltinType::S16:
        case BuiltinType::U16:
            return llvm::Type::getInt16Ty(*llvmContext);

        case BuiltinType::C32:
        case BuiltinType::S32:
        case BuiltinType::U32:
            return llvm::Type::getInt32Ty(*llvmContext);

        case BuiltinType::S64:
        case BuiltinType::U64:
            return llvm::Type::getInt64Ty(*llvmContext);

        case BuiltinType::S128:
        case BuiltinType::U128:
            return llvm::Type::getInt128Ty(*llvmContext);

        case BuiltinType::F16:
            return llvm::Type::getHalfTy(*llvmContext);

        case BuiltinType::F32:
            return llvm::Type::getFloatTy(*llvmContext);

        case BuiltinType::F64:
            return llvm::Type::getDoubleTy(*llvmContext);

        case BuiltinType::F128:
            return llvm::Type::getFP128Ty(*llvmContext);

        case BuiltinType::Str8:
        case BuiltinType::Str16:
        case BuiltinType::Str32:
            return llvm::PointerType::getUnqual(*llvmContext);

        case BuiltinType::USize:
            return llvm::Type::getIntNTy(*llvmContext, llvm::PointerType::getUnqual(*llvmContext)->getIntegerBitWidth()); // FIXME - This creates a pointer only to get its bit width
    }

    std::println(std::cerr,"Invalid BuiltinType value");
    Utils::Exit(ErrorCodes::FatalError);
}

std::string_view Type::GetTypeName() const
{
    if (const std::string* value = std::get_if<std::string>(&m_Type))
        return *value;

    return KeywordStrings.at(BuiltinTypeToKeyword(std::get<BuiltinType>(m_Type)));
}

BuiltinType Type::GetBuiltinType() const
{
    assert(std::holds_alternative<BuiltinType>(m_Type));
    return std::get<BuiltinType>(m_Type);
}

bool Type::IsBuiltinType() const
{
    return std::holds_alternative<BuiltinType>(m_Type);
}

TypeIdentifierPair::TypeIdentifierPair(std::shared_ptr<Type> type, std::string name)
    : m_Type{std::move(type)}
    , m_Name{std::move(name)}
{
}

Statement::Statement()
    : m_ParentScope{currentScope}
{
}

llvm::AllocaInst* Statement::LookupIdentifier(const std::string& name) const
{
    RETURN_IF_NULL(m_ParentScope);

    if (!m_ParentScope->GetIdentifiers().contains(name))
        return m_ParentScope->LookupIdentifier(name);
    return m_ParentScope->GetIdentifiers().at(name);
}

void Statement::DeclareIdentifier(const std::string& name, llvm::AllocaInst* variable) const
{
    m_ParentScope->GetIdentifiers()[name] = variable;
}

Scope::Scope(std::vector<std::shared_ptr<Statement>> body, const bool isFunctionLevel)
    : m_Body{std::move(body)}
    , m_IsFunctionLevel{isFunctionLevel}
{
}

Scope::Scope(std::shared_ptr<Statement> body, const bool isFunctionLevel)
    : m_IsFunctionLevel{isFunctionLevel}
{
    m_Body.push_back(std::move(body));
}

Scope::Scope(const bool isFunctionLevel)
    : m_IsFunctionLevel{isFunctionLevel}
{
}

llvm::Value* Scope::GenerateCode() const
{
    for (size_t i = 0; i < m_Body.size() - 1; i++)
    {
        RETURN_IF_NULL(m_Body[i]->GenerateCode());
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

llvm::Value* LiteralCharacter::GenerateCode() const
{
    return llvm::ConstantInt::get(*llvmContext, llvm::APInt{8, static_cast<uint32_t>(m_Value)});
}

LiteralString::LiteralString(std::string value)
    : m_Value{std::move(value)}
{
}

llvm::Value* LiteralString::GenerateCode() const
{
    return nullptr;
}

VariableDeclaration::VariableDeclaration(std::shared_ptr<TypeIdentifierPair> typeName)
    : m_TypeName{std::move(typeName)}
{
}

VariableDeclaration::VariableDeclaration(std::shared_ptr<TypeIdentifierPair> typeName, std::shared_ptr<Expression> initialValue)
    : m_TypeName{std::move(typeName)}
    , m_InitialValue{std::move(initialValue)}
{
}

llvm::Value* VariableDeclaration::GenerateCode() const
{
    const auto variableName = m_TypeName->GetName();
    const auto allocatedVariable = CreateEntryBlockAlloca(builder->GetInsertBlock()->getParent(), m_TypeName->GetType()->GetLlvmType(), variableName);

    if (m_InitialValue)
    {
        llvm::Value* initialValue = m_InitialValue->GenerateCode();
        RETURN_IF_NULL(initialValue);

        builder->CreateStore(initialValue, allocatedVariable);
    }

    DeclareIdentifier(variableName, allocatedVariable);
    return allocatedVariable;
}

VariableUsage::VariableUsage(std::string name)
    : m_Name{std::move(name)}
{
}

llvm::Value* VariableUsage::GenerateCode() const
{
    llvm::AllocaInst* v = LookupIdentifier(m_Name);
    if (!v)
        return RaiseError("Unknown variable name: {}", m_Name);
    return builder->CreateLoad(v->getAllocatedType(), v, m_Name);
}

Operation::Operation(const Operator operatorType, std::shared_ptr<Expression> lhs, std::shared_ptr<Expression> rhs)
    : m_OperatorType{operatorType}
    , m_Lhs{std::move(lhs)}
    , m_Rhs{std::move(rhs)}
{
}

#define OPERATION_GENERATE_CODE_CHECK_OPERAND(operand, operandResult, check) \
    do \
    { \
        if (check) \
        { \
            operandResult = operand->GenerateCode(); \
            RETURN_IF_NULL(operandResult); \
        } \
        else \
        { \
            operandResult = nullptr; \
        } \
    } \
    while (false)

llvm::Value* Operation::GenerateCode() const
{
    llvm::Value* lhs = nullptr;
    if (m_OperatorType != Operator::Assignment)
        OPERATION_GENERATE_CODE_CHECK_OPERAND(m_Lhs, lhs, OperatorHasLeftOperand(m_OperatorType));
    llvm::Value* rhs;
    OPERATION_GENERATE_CODE_CHECK_OPERAND(m_Rhs, rhs, OperatorHasRightOperand(m_OperatorType));

    const std::string_view operatorName = magic_enum::enum_name(m_OperatorType);
    static constexpr std::string_view NameSuffix = "Result";
    std::string name{operatorName};
    name += NameSuffix;

    switch (m_OperatorType)
    {
        case Operator::Addition:            return builder->CreateAdd(lhs, rhs, name);
        case Operator::Subtraction:         return builder->CreateSub(lhs, rhs, name);
        case Operator::Multiplication:      return builder->CreateMul(lhs, rhs, name);
        case Operator::Division:            return builder->CreateSDiv(lhs, rhs, name);
        case Operator::Modulo:              return builder->CreateSRem(lhs, rhs, name);

        case Operator::Equals:              return builder->CreateICmpEQ(lhs, rhs, name);
        case Operator::NotEquals:           return builder->CreateICmpNE(lhs, rhs, name);
        case Operator::Greater:             return builder->CreateICmpSGT(lhs, rhs, name);
        case Operator::GreaterEquals:       return builder->CreateICmpSGE(lhs, rhs, name);
        case Operator::Less:                return builder->CreateICmpSLT(lhs, rhs, name);
        case Operator::LessEquals:          return builder->CreateICmpSLE(lhs, rhs, name);

        case Operator::LogicalNot:          return builder->CreateIsNull(rhs, name);
        case Operator::LogicalAnd:          return builder->CreateLogicalAnd(lhs, rhs, name);
        case Operator::LogicalOr:           return builder->CreateLogicalOr(lhs, rhs, name);

        case Operator::BitshiftLeft:        return builder->CreateShl(lhs, rhs, name);
        case Operator::BitshiftRight:       return builder->CreateLShr(lhs, rhs, name); // TODO - Check shift type

        case Operator::BinaryNot:           return builder->CreateNot(rhs, name);
        case Operator::BinaryAnd:           return builder->CreateAnd(lhs, rhs, name);
        case Operator::BinaryOr:            return builder->CreateOr(lhs, rhs, name);
        case Operator::BinaryXor:           return builder->CreateXor(lhs, rhs, name);

        case Operator::Negation:            return builder->CreateNeg(rhs, name);

        case Operator::Assignment:          return GenerateAssignment(*m_Lhs, rhs);
        case Operator::AddEquals:           return GenerateAssignment(*m_Lhs, builder->CreateAdd(lhs, rhs, name));
        case Operator::SubtractEquals:      return GenerateAssignment(*m_Lhs, builder->CreateSub(lhs, rhs, name));
        case Operator::MultiplyEquals:      return GenerateAssignment(*m_Lhs, builder->CreateMul(lhs, rhs, name));
        case Operator::DivideEquals:        return GenerateAssignment(*m_Lhs, builder->CreateSDiv(lhs, rhs, name));
        case Operator::ModuloEquals:        return GenerateAssignment(*m_Lhs, builder->CreateSRem(lhs, rhs, name));
        case Operator::BitshiftLeftEquals:  return GenerateAssignment(*m_Lhs, builder->CreateShl(lhs, rhs, name));
        case Operator::BitshiftRightEquals: return GenerateAssignment(*m_Lhs, builder->CreateLShr(lhs, rhs, name));
        case Operator::BinaryAndEquals:     return GenerateAssignment(*m_Lhs, builder->CreateAnd(lhs, rhs, name));
        case Operator::BinaryXorEquals:     return GenerateAssignment(*m_Lhs, builder->CreateXor(lhs, rhs, name));
        case Operator::BinaryOrEquals:      return GenerateAssignment(*m_Lhs, builder->CreateOr(lhs, rhs, name));

        case Operator::PreIncrement:
            return GenerateAssignment(*m_Rhs, builder->CreateAdd(rhs, llvm::ConstantInt::get(*llvmContext, llvm::APInt{getTypeBitSize(rhs), 1}), name));
        case Operator::PreDecrement:
            return GenerateAssignment(*m_Rhs, builder->CreateSub(rhs, llvm::ConstantInt::get(*llvmContext, llvm::APInt{getTypeBitSize(rhs), 1}), name));
        case Operator::PostIncrement:
            GenerateAssignment(*m_Lhs, builder->CreateAdd(lhs, llvm::ConstantInt::get(*llvmContext, llvm::APInt{getTypeBitSize(lhs), 1}), name));
            return lhs;
        case Operator::PostDecrement:
            GenerateAssignment(*m_Lhs, builder->CreateSub(lhs, llvm::ConstantInt::get(*llvmContext, llvm::APInt{getTypeBitSize(lhs), 1}), name));
            return lhs;

        default: return RaiseError("Invalid binary operator: {}", m_OperatorType);
    }
}

#undef OPERATION_GENERATE_CODE_CHECK_OPERAND

llvm::Value* Operation::GenerateAssignment(const Expression& variable, llvm::Value* value) const
{
    const auto variableUsage = dynamic_cast<const VariableUsage*>(&variable);
    RETURN_IF_NULL(variableUsage);
    return GenerateAssignment(*variableUsage, value);
}

llvm::Value* Operation::GenerateAssignment(const VariableUsage& variable, llvm::Value* value) const
{
    return builder->CreateStore(value, LookupIdentifier(variable.GetName()));
}

FunctionCall::FunctionCall(std::string callee, std::vector<std::shared_ptr<Expression>> arguments)
    : m_Callee{std::move(callee)}
    , m_Arguments{std::move(arguments)}
{
}

llvm::Value* FunctionCall::GenerateCode() const
{
    // Look up the name in the global module table
    llvm::Function* calleeFunction = theModule->getFunction(m_Callee);
    if (!calleeFunction)
        return RaiseError("Unknown function referenced: {}", m_Callee);

    if (calleeFunction->arg_size() != m_Arguments.size())
        return RaiseError("Incorrect number of arguments passed");

    std::vector<llvm::Value*> arguments;
    for (const std::shared_ptr<Expression>& m_Argument : m_Arguments)
    {
        arguments.push_back(m_Argument->GenerateCode());
        if (!arguments.back())
            return nullptr;
    }

    const c8* resultName = calleeFunction->getReturnType() == llvm::Type::getVoidTy(*llvmContext) ? "" : "FunctionCallResult";
    return builder->CreateCall(calleeFunction, arguments, resultName);
}

FunctionPrototype::FunctionPrototype(std::shared_ptr<TypeIdentifierPair> typeNamePair, std::vector<Parameter> parameters)
    : m_TypeNamePair{std::move(typeNamePair)}
    , m_Parameters{std::move(parameters)}
{
}

llvm::Function* FunctionPrototype::GenerateCode() const
{
    // Prepare the parameters
    std::vector<llvm::Type*> parameters;
    parameters.reserve(m_Parameters.size());
    for (const auto& parameter : m_Parameters)
        parameters.push_back(parameter->GetType()->GetLlvmType());

    // Make the function type
    llvm::FunctionType* functionType = llvm::FunctionType::get(m_TypeNamePair->GetType()->GetLlvmType(), parameters, false);

    llvm::Function* function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, m_TypeNamePair->GetName(), theModule.get());

    // Set names for all arguments
    for (size_t i = 0; i < function->arg_size(); i++)
        (function->arg_begin() + i)->setName(m_Parameters[i]->GetName());

    return function;
}

FunctionDefinition::FunctionDefinition(std::shared_ptr<FunctionPrototype> prototype, std::shared_ptr<Scope> body)
    : m_Prototype{std::move(prototype)}
    , m_Body{std::move(body)}
{
}

llvm::Function* FunctionDefinition::GenerateCode() const
{
    // First, check for an existing function from a previous declaration.
    llvm::Function* function = theModule->getFunction(m_Prototype->GetTypeNamePair()->GetName());

    if (!function)
        function = m_Prototype->GenerateCode();

    RETURN_IF_NULL(function);

    if (!function->empty())
        return RaiseError<llvm::Function>("Function {} has already been defined", function->getName());

    // Create a new basic block to start insertion into
    llvm::BasicBlock* block = llvm::BasicBlock::Create(*llvmContext, "Entry", function);
    builder->SetInsertPoint(block);

    auto& identifiers = m_Body->GetIdentifiers();
    // Record the function arguments in the identifier map
    identifiers.clear();
    for (llvm::Argument& argument : function->args())
    {
        const auto allocatedVariable = CreateEntryBlockAlloca(function, argument.getType(), argument.getName());
        builder->CreateStore(&argument, allocatedVariable);
        identifiers[argument.getName().data()] = allocatedVariable;
    }

    llvm::Value* lastBodyValue = m_Body->GenerateCode();
    if (!lastBodyValue) // If body code generation failed
    {
        // Error reading body, remove function
        function->eraseFromParent();
        return nullptr;
    }

    if (!llvm::isa<llvm::ReturnInst, llvm::BranchInst>(lastBodyValue))
    {
        const auto& returnType = m_Prototype->GetTypeNamePair()->GetType();
        if (returnType->IsBuiltinType() && returnType->GetBuiltinType() == BuiltinType::Void)
            builder->CreateRetVoid();
    }

    // Validate the generated code, checking for consistency
    llvm::verifyFunction(*function);

    return function;
}

FunctionReturn::FunctionReturn(std::shared_ptr<Expression> returnValue)
    : m_ReturnValue{std::move(returnValue)}
{
}

llvm::Value* FunctionReturn::GenerateCode() const
{
    if (!m_ReturnValue)
        return builder->CreateRet(nullptr); // Return void

    llvm::Value* returnValue = m_ReturnValue->GenerateCode();
    RETURN_IF_NULL(returnValue);
    return builder->CreateRet(returnValue);
}

IfElse::IfElse(std::shared_ptr<Expression> condition, std::shared_ptr<Scope> ifBody, std::shared_ptr<Scope> elseBody)
    : m_Condition{std::move(condition)}
    , m_IfBody{std::move(ifBody)}
    , m_ElseBody{std::move(elseBody)}
{
}

llvm::Value* IfElse::GenerateCode() const
{
    llvm::Value* condition = m_Condition->GenerateCode();
    RETURN_IF_NULL(condition);

    condition = builder->CreateICmpNE(condition, llvm::ConstantInt::get(*llvmContext, llvm::APInt{getTypeBitSize(condition), 0}), "IfCondition");

    llvm::Function* function = builder->GetInsertBlock()->getParent();

    const bool hasElse = m_ElseBody != nullptr;

    // Create blocks for the if and else cases
    // Insert the 'if' block at the end of the function
    llvm::BasicBlock* ifBlock = llvm::BasicBlock::Create(*llvmContext, "IfTrue", function);
    llvm::BasicBlock* elseBlock = hasElse ? llvm::BasicBlock::Create(*llvmContext, "IfFalse") : nullptr;
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*llvmContext, "IfEnd");

    builder->CreateCondBr(condition, ifBlock, hasElse ? elseBlock : mergeBlock);

    // Emit if value
    builder->SetInsertPoint(ifBlock);

    llvm::Value* ifContents = m_IfBody->GenerateCode();
    RETURN_IF_NULL(ifContents);

    const bool ifBlockBranches = llvm::isa<llvm::ReturnInst, llvm::BranchInst>(ifContents);
    if (!ifBlockBranches)
        builder->CreateBr(mergeBlock);

    bool elseBlockBranches = true;
    llvm::Value* elseContents;
    if (hasElse)
    {
        // Emit else block
        function->insert(function->end(), elseBlock);
        builder->SetInsertPoint(elseBlock);

        elseContents = m_ElseBody->GenerateCode();
        RETURN_IF_NULL(elseContents);

        elseBlockBranches = llvm::isa<llvm::ReturnInst, llvm::BranchInst>(elseContents);
        if (!elseBlockBranches)
            builder->CreateBr(mergeBlock);
    }

    if (!ifBlockBranches || !elseBlockBranches)
    {
        // Emit merge block
        function->insert(function->end(), mergeBlock);
        builder->SetInsertPoint(mergeBlock);

        return mergeBlock;
    }

    return hasElse ? elseContents : ifContents;
}

Ternary::Ternary(std::shared_ptr<Expression> condition, std::shared_ptr<Expression> trueResult, std::shared_ptr<Expression> falseResult)
    : m_Condition{std::move(condition)}
    , m_TrueResult{std::move(trueResult)}
    , m_FalseResult{std::move(falseResult)}
{
}

llvm::Value* Ternary::GenerateCode() const
{
    llvm::Value* condition = m_Condition->GenerateCode();
    RETURN_IF_NULL(condition);

    condition = builder->CreateICmpNE(condition, llvm::ConstantInt::get(*llvmContext, llvm::APInt{getTypeBitSize(condition), 0}), "TernaryCondition");

    llvm::Function* function = builder->GetInsertBlock()->getParent();

    // Create blocks for the true and false cases
    // Insert the 'true' block at the end of the function
    llvm::BasicBlock* trueBlock = llvm::BasicBlock::Create(*llvmContext, "TernaryTrue", function);
    llvm::BasicBlock* falseBlock = llvm::BasicBlock::Create(*llvmContext, "TernaryFalse");
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*llvmContext, "TernaryEnd");

    builder->CreateCondBr(condition, trueBlock, falseBlock);

    // Emit true value
    builder->SetInsertPoint(trueBlock);

    llvm::Value* ifReturnValue = m_TrueResult->GenerateCode();
    RETURN_IF_NULL(ifReturnValue);

    builder->CreateBr(mergeBlock);
    // Codegen of 'true' can change the current block, update trueBlock for the Phi
    trueBlock = builder->GetInsertBlock();

    // Emit false block
    function->insert(function->end(), falseBlock);
    builder->SetInsertPoint(falseBlock);

    llvm::Value* elseReturnValue = m_FalseResult->GenerateCode();
    RETURN_IF_NULL(elseReturnValue);

    builder->CreateBr(mergeBlock);
    // Codegen of 'false' can change the current block, update falseBlock for the Phi
    falseBlock = builder->GetInsertBlock();

    // Emit merge block
    function->insert(function->end(), mergeBlock);
    builder->SetInsertPoint(mergeBlock);

    llvm::PHINode* phiNode = builder->CreatePHI(llvm::Type::getInt32Ty(*llvmContext), 2, "TernaryResult");
    phiNode->addIncoming(ifReturnValue, trueBlock);
    phiNode->addIncoming(elseReturnValue, falseBlock);

    return phiNode;
}

WhileLoop::WhileLoop(std::shared_ptr<Expression> condition, std::shared_ptr<Scope> body)
    : m_Condition{std::move(condition)}
    , m_Body{std::move(body)}
{
}

llvm::Value* WhileLoop::GenerateCode() const
{
    llvm::Function* function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* loopConditionBlock = llvm::BasicBlock::Create(*llvmContext, "WhileLoopCondition", function);
    llvm::BasicBlock* loopBlock = llvm::BasicBlock::Create(*llvmContext, "WhileLoopBody");
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*llvmContext, "WhileLoopEnd");

    builder->CreateBr(loopConditionBlock);

    builder->SetInsertPoint(loopConditionBlock);

    llvm::Value* condition = m_Condition->GenerateCode();
    RETURN_IF_NULL(condition);
    condition = builder->CreateICmpNE(condition, llvm::ConstantInt::get(*llvmContext, llvm::APInt{getTypeBitSize(condition), 0}), "WhileLoopConditionResult");

    builder->CreateCondBr(condition, loopBlock, mergeBlock);

    function->insert(function->end(), loopBlock);
    builder->SetInsertPoint(loopBlock);

    const llvm::Value* loopContents = m_Body->GenerateCode();
    RETURN_IF_NULL(loopContents);

    builder->CreateBr(loopConditionBlock);

    function->insert(function->end(), mergeBlock);
    builder->SetInsertPoint(mergeBlock);

    return mergeBlock;
}

DoWhileLoop::DoWhileLoop(std::shared_ptr<Scope> body, std::shared_ptr<Expression> condition)
    : m_Body{std::move(body)}
    , m_Condition{std::move(condition)}
{
}

llvm::Value* DoWhileLoop::GenerateCode() const
{
    llvm::Function* function = builder->GetInsertBlock()->getParent();

    llvm::BasicBlock* loopBlock = llvm::BasicBlock::Create(*llvmContext, "DoWhileLoopBody", function);
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*llvmContext, "DoWhileLoopEnd");

    builder->CreateBr(loopBlock);

    builder->SetInsertPoint(loopBlock);

    const llvm::Value* loopContents = m_Body->GenerateCode();
    RETURN_IF_NULL(loopContents);

    llvm::Value* condition = m_Condition->GenerateCode();
    RETURN_IF_NULL(condition);
    condition = builder->CreateICmpNE(condition, llvm::ConstantInt::get(*llvmContext, llvm::APInt{getTypeBitSize(condition), 0}), "DoWhileLoopConditionResult");

    builder->CreateCondBr(condition, loopBlock, mergeBlock);

    function->insert(function->end(), mergeBlock);
    builder->SetInsertPoint(mergeBlock);

    return mergeBlock;
}

ForLoop::ForLoop(
    InitializationType initialization,
    std::shared_ptr<Expression> condition,
    std::shared_ptr<Expression> expression,
    std::shared_ptr<Scope> body
)
    : m_Initialization{std::move(initialization)}
    , m_Condition{std::move(condition)}
    , m_Expression{std::move(expression)}
    , m_Body{std::move(body)}
{
}

llvm::Value* ForLoop::GenerateCode() const
{
    llvm::Function* function = builder->GetInsertBlock()->getParent();

    if (!std::holds_alternative<std::monostate>(m_Initialization))
    {
        const llvm::Value* initializationContents = std::holds_alternative<std::shared_ptr<VariableDeclaration>>(m_Initialization)
                                                        ? std::get<std::shared_ptr<VariableDeclaration>>(m_Initialization)->GenerateCode()
                                                        : std::get<std::shared_ptr<Expression>>(m_Initialization)->GenerateCode();
        RETURN_IF_NULL(initializationContents);
    }

    llvm::BasicBlock* loopConditionBlock = llvm::BasicBlock::Create(*llvmContext, "ForLoopCondition", function);
    llvm::BasicBlock* loopBlock = llvm::BasicBlock::Create(*llvmContext, "ForLoopBody");
    llvm::BasicBlock* mergeBlock = llvm::BasicBlock::Create(*llvmContext, "ForLoopEnd");

    builder->CreateBr(loopConditionBlock);

    builder->SetInsertPoint(loopConditionBlock);

    llvm::Value* condition = m_Condition->GenerateCode();
    RETURN_IF_NULL(condition);
    condition = builder->CreateICmpNE(condition, llvm::ConstantInt::get(*llvmContext, llvm::APInt{getTypeBitSize(condition), 0}), "ForLoopConditionResult");

    builder->CreateCondBr(condition, loopBlock, mergeBlock);

    function->insert(function->end(), loopBlock);
    builder->SetInsertPoint(loopBlock);

    const llvm::Value* loopContents = m_Body->GenerateCode();
    RETURN_IF_NULL(loopContents);

    if (m_Expression)
    {
        const llvm::Value* loopEndExpressionContents = m_Expression->GenerateCode();
        RETURN_IF_NULL(loopEndExpressionContents);
    }

    builder->CreateBr(loopConditionBlock);

    function->insert(function->end(), mergeBlock);
    builder->SetInsertPoint(mergeBlock);

    return mergeBlock;
}
