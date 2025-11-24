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

    if (lexer.GetFailure())
        Utils::Exit(ErrorCodes::FatalError);

    Parser parser{tokens};

    std::shared_ptr<Ast::Scope> topLevelScope = parser.ParseAll();

    if (parser.GetFailure())
        Utils::Exit(ErrorCodes::SyntaxError);

    llvmContext = std::make_shared<llvm::LLVMContext>();
    builder = std::make_shared<llvm::IRBuilder<>>(*llvmContext);
    theModule = std::make_shared<llvm::Module>(inputFile.GetFilepath().generic_string(), *llvmContext);

    for (const std::shared_ptr<Ast::Statement>& statement : topLevelScope->GetBody())
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

    /*int lliResult = Utils::SystemCommand("lli {}", IrOut);
    if (lliResult == 1)
    {
        std::println(std::cerr, "An error occurred during JIT compilation: lli exited with code {}", lliResult);
        Utils::Exit(ErrorCodes::FatalError);
    }

    std::println("JIT compiled program exited with code {}", lliResult);

    if (int llcResult = Utils::SystemCommand("llc {}", IrOut))
    {
        std::println(std::cerr, "An error occurred during static compilation: llc exited with code {}", llcResult);
        Utils::Exit(ErrorCodes::FatalError);
    }*/

    llvm::Triple targetTriple{llvm::sys::getDefaultTargetTriple()};
    std::string errorMessage;
    auto target = llvm::TargetRegistry::lookupTarget(targetTriple, errorMessage);
    if (!target)
    {
        std::println(std::cerr, "Could not find target for triple {}: {}", targetTriple.getOSAndEnvironmentName(), errorMessage);
        Utils::Exit(ErrorCodes::FatalError);
    }

    const c8* cpu = "generic";
    const c8* features = "";

    llvm::TargetOptions opt;
    auto targetMachine = target->createTargetMachine(targetTriple, cpu, features, opt, llvm::Reloc::PIC_);

    theModule->setDataLayout(targetMachine->createDataLayout());
    theModule->setTargetTriple(targetTriple);

    auto filepath = inputFile.GetFilepath();
    auto filename = filepath.replace_extension("o").generic_string();
    std::error_code errorCode;
    llvm::raw_fd_ostream dest{filename, errorCode, llvm::sys::fs::OF_None};

    if (errorCode)
    {
        std::println(std::cerr, "Could not open file: {}", errorCode.message());
        Utils::Exit(ErrorCodes::FatalError);
    }

    llvm::legacy::PassManager pass;
    auto FileType = llvm::CodeGenFileType::ObjectFile;

    if (targetMachine->addPassesToEmitFile(pass, dest, nullptr, FileType))
    {
        std::println(std::cerr, "TargetMachine can't emit a file of this type");
        Utils::Exit(ErrorCodes::FatalError);
    }

    pass.run(*theModule);
    dest.flush();
}
