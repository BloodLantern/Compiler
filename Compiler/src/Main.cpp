#include "InputFile.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

int main()
{
    std::println("Working directory: {}", std::filesystem::current_path().generic_string());
    std::cout.flush();

    const auto start = std::chrono::high_resolution_clock::now();

    InputFile inputFile{"example.txt"};
    inputFile.ReadContents();

    Lexer lexer{inputFile};
    std::vector<Token> tokens;
    lexer.ParseAll(tokens);

    // Lexer::PrintDebug(tokens);
    // std::println(std::cout);
    // std::cout.flush();

    if (lexer.GetFailure())
        Utils::Exit(ErrorCodes::FatalError);

    Parser parser{tokens};

    std::unique_ptr<Ast::Scope> topLevelScope = parser.ParseAll();

    std::println();
    std::cout.flush();

    if (parser.GetFailure())
        Utils::Exit(ErrorCodes::SyntaxError);

    theContext = std::make_unique<llvm::LLVMContext>();
    builder = std::make_unique<llvm::IRBuilder<>>(*theContext);
    theModule = std::make_unique<llvm::Module>(inputFile.GetFilepath().generic_string(), *theContext);

    for (const std::unique_ptr<Ast::Statement>& statement : topLevelScope->GetBody())
    {
        if (statement->GenerateCode() == nullptr)
            Utils::Exit(ErrorCodes::FatalError);
    }

    constexpr std::string_view IrOut = "out.ll";

    std::error_code error;
    llvm::raw_fd_ostream out{IrOut, error};
    if (error)
    {
        std::println(std::cerr, "Couldn't open file {} for writing", IrOut);
        Utils::Exit(ErrorCodes::FatalError);
    }
    theModule->print(out, nullptr);

    std::println("Compilation took {} ms",
                 std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - start).count());

    int lliResult = Utils::SystemCommand("lli {}", IrOut);
    if (lliResult == 1)
    {
        std::println(std::cerr, "An error occurred during JIT compilation: lli exited with code {}", lliResult);
        Utils::Exit(ErrorCodes::FatalError);
    }

    std::println("JIT compiled program exited with code {}", lliResult);

    /*if (int llcResult = Utils::SystemCommand("llc {}", IrOut))
    {
        std::println(std::cerr, "An error occurred during static compilation: llc exited with code {}", llcResult);
        Utils::Exit(ErrorCodes::FatalError);
    }*/
}
