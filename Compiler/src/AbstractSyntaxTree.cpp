#include "AbstractSyntaxTree.hpp"

using namespace Ast;

Character::Character(const char value)
    : m_Value{value}
{
}

llvm::Value* Character::CodeGeneration()
{
    return llvm::ConstantInt::get(*theContext, llvm::APInt{8, static_cast<unsigned int>(m_Value)});
}

String::String(std::string value)
    : m_Value{std::move(value)}
{
}

llvm::Value* String::CodeGeneration()
{
    return nullptr;
}

Variable::Variable(std::string name)
    : m_Name{std::move(name)}
{
}

llvm::Value* Variable::CodeGeneration()
{
    // Look this variable up in the function.
    llvm::Value* v = namedValues[m_Name];
    if (!v)
        return RaiseError("Unknown variable name");
    return v;
}

Operation::Operation(const Operator operatorType, std::unique_ptr<Expression> lhs, std::unique_ptr<Expression> rhs)
    : m_OperatorType{operatorType}
    , m_Lhs{std::move(lhs)}
    , m_Rhs{std::move(rhs)}
{
}

llvm::Value* Operation::CodeGeneration()
{
    llvm::Value* lhs = m_Lhs->CodeGeneration();
    llvm::Value* rhs = m_Rhs->CodeGeneration();

    if (!lhs || !rhs)
        return nullptr;

    switch (m_OperatorType)
    {
        case Operator::Addition:
            return builder->CreateFAdd(lhs, rhs, "addtmp");
        case Operator::Subtraction:
            return builder->CreateFSub(lhs, rhs, "subtmp");
        case Operator::Multiplication:
            return builder->CreateFMul(lhs, rhs, "multmp");
        case Operator::Division:
            return builder->CreateFDiv(lhs, rhs, "divtmp");
        default:
            return RaiseError("Invalid binary operator");
    }
}

FunctionCall::FunctionCall(std::string callee, std::vector<std::unique_ptr<Expression>> arguments)
    : m_Callee{std::move(callee)}
    , m_Arguments{std::move(arguments)}
{
}

llvm::Value* FunctionCall::CodeGeneration()
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
        arguments.push_back(m_Argument->CodeGeneration());
        if (!arguments.back())
            return nullptr;
    }

    return builder->CreateCall(calleeFunction, arguments, "calltmp");
}

FunctionDefinition::FunctionDefinition(std::unique_ptr<FunctionPrototype> prototype, std::vector<std::unique_ptr<Statement>> body)
    : m_Prototype{std::move(prototype)}
    , m_Body{std::move(body)}
{
}

llvm::Function* FunctionDefinition::CodeGeneration()
{
    // First, check for an existing function from a previous 'extern' declaration.
    llvm::Function* function = theModule->getFunction(m_Prototype->GetTypeNamePair()->GetName());

    if (!function)
        function = m_Prototype->CodeGeneration();

    if (!function)
        return nullptr;

    if (!function->empty())
        return RaiseError<llvm::Function>("Function cannot be redefined.");

    // Create a new basic block to start insertion into.
    llvm::BasicBlock* block = llvm::BasicBlock::Create(*theContext, "entry", function);
    builder->SetInsertPoint(block);

    // Record the function arguments in the NamedValues map.
    namedValues.clear();
    for (llvm::Argument& argument : function->args())
        namedValues[std::string{argument.getName()}] = &argument;

    llvm::Value* returnValue = m_Body[0]->CodeGeneration();
    if (returnValue)
    {
        // Finish off the function.
        builder->CreateRet(returnValue);

        // Validate the generated code, checking for consistency.
        // verifyFunction(*function);

        return function;
    }

    // Error reading body, remove function.
    function->eraseFromParent();
    return nullptr;
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

llvm::Value* TypeNamePair::CodeGeneration()
{
    return nullptr;
}

FunctionPrototype::FunctionPrototype(std::unique_ptr<TypeNamePair> typeNamePair, std::vector<Parameter> parameters)
    : m_TypeNamePair{std::move(typeNamePair)}
    , m_Parameters{std::move(parameters)}
{
}

llvm::Function* FunctionPrototype::CodeGeneration()
{
    // Make the function type
    std::vector<llvm::Type*> parameters(m_Parameters.size(), llvm::Type::getInt32Ty(*theContext));
    llvm::FunctionType* functionType = llvm::FunctionType::get(llvm::Type::getInt32Ty(*theContext), parameters, false);

    llvm::Function* function = llvm::Function::Create(functionType, llvm::Function::ExternalLinkage, m_TypeNamePair->GetName(), theModule.get());

    // Set names for all arguments.
    for (size_t i = 0; i < function->arg_size(); i++)
        (function->arg_begin() + i)->setName(m_Parameters[i]->GetName());

    return function;
}
