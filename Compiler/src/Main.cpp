#include "InputFile.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

// TODO - https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html

int main()
{
    std::println("Working directory: {}", std::filesystem::current_path().generic_string());
    std::cout.flush();

    InputFile inputFile{"example.txt"};
    inputFile.ReadContents();

    Lexer lexer{inputFile};
    std::vector<Token> tokens;
    lexer.ParseAll(tokens);

    Lexer::PrintDebug(tokens);

    if (lexer.GetFailure())
        Utils::Exit(ErrorCodes::FatalError);

    Parser parser{tokens};

    std::println(std::cout);
    std::cout.flush();

    std::vector<std::unique_ptr<Ast::Statement>> statements;
    parser.ParseAll(statements);

    std::println(std::cout);
    std::cout.flush();

    if (parser.GetFailure())
        Utils::Exit(ErrorCodes::SyntaxError);

    theContext = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*theContext);
    theModule = std::make_unique<llvm::Module>("Example module", *theContext);

    for (const std::unique_ptr<Ast::Statement>& statement : statements)
    {
        if (statement->CodeGeneration() == nullptr)
            Utils::Exit(ErrorCodes::FatalError);
    }

    theModule->print(llvm::outs(), nullptr);
}
