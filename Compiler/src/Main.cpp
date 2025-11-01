#include "PrecompiledHeader.hpp"

#include "InputFile.hpp"
#include "Lexer.hpp"

// TODO - https://llvm.org/docs/tutorial/MyFirstLanguageFrontend/index.html

int main()
{
    std::println(std::cout, "Working directory: {}", std::filesystem::current_path().generic_string());
    std::cout.flush();

    InputFile inputFile{"example.txt"};
    inputFile.ReadContents();

    Lexer lexer{inputFile};
    std::vector<Token> tokens;
    lexer.ParseAll(tokens);

    for (const Token& token : tokens)
    {
        std::print(std::cout, "{}", magic_enum::enum_name(token.type));

        switch (token.type)
        {
            case TokenType::Identifier:
            case TokenType::LiteralString:
                std::print(std::cout, "(\"{}\")", std::get<std::string>(token.value.value()));
                break;

            case TokenType::LiteralInteger:
                std::print(std::cout, "({})", std::get<int>(token.value.value()));
                break;

            case TokenType::LiteralCharacter:
                std::print(std::cout, "({:?})", std::get<char>(token.value.value()));
                break;

            case TokenType::Semicolon:
                std::println(std::cout);
                break;

            default:
                break;
        }

        if (token.type != TokenType::Semicolon)
            std::print(std::cout, " ");
    }
}
