
#include "InputFile.hpp"
#include "Lexer.hpp"
#include "Parser.hpp"

// TODO - https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html

static std::unique_ptr<llvm::LLVMContext> TheContext;
static std::unique_ptr<llvm::IRBuilder<>> Builder;
static std::unique_ptr<llvm::Module> TheModule;
static std::unordered_map<std::string, llvm::Value*> NamedValues;

int main()
{
    std::println("Working directory: {}", std::filesystem::current_path().generic_string());
    std::cout.flush();

    InputFile inputFile{"example.txt"};
    inputFile.ReadContents();

    Lexer lexer{inputFile};
    std::vector<Token> tokens;
    lexer.ParseAll(tokens);

    if (lexer.GetFailure())
        Utils::Exit(ErrorCodes::FatalError);

    Parser parser{tokens};

    for (const Token& token : tokens)
    {
        std::print("{}", magic_enum::enum_name(token.type));

        switch (token.type)
        {
            case TokenType::Identifier:
            case TokenType::LiteralString:
                std::print("(\"{}\")", std::get<std::string>(token.value.value()));
                break;

            case TokenType::LiteralInteger:
                std::print("({})", std::get<int>(token.value.value()));
                break;

            case TokenType::LiteralCharacter:
                std::print("({:?})", std::get<char>(token.value.value()));
                break;

            case TokenType::Keyword:
                std::print("({})", magic_enum::enum_name(std::get<Keyword>(token.value.value())));
                break;

            case TokenType::Semicolon:
            case TokenType::CurlyBraceOpen:
            case TokenType::CurlyBraceClose:
                std::println(std::cout);
                break;

            default:
                break;
        }

        if (token.type != TokenType::Semicolon)
            std::print(" ");
    }

    std::println(std::cout);
    std::cout.flush();

    std::vector<std::unique_ptr<Ast::Statement>> statements;
    parser.ParseAll(statements);

    std::println(std::cout);
    std::cout.flush();

    if (parser.GetFailure())
        Utils::Exit(ErrorCodes::SyntaxError);
}
