# Find external packages
find_package(LLVM REQUIRED CONFIG)
find_package(magic_enum REQUIRED CONFIG)

# Add executable target
add_executable (Compiler
        src/AbstractSyntaxTree.hpp
        src/AbstractSyntaxTree.cpp
        src/Utility/Attributes.hpp
        src/Utility/CompilerSpecific.hpp
        src/ErrorCodes.hpp
        src/Globals.hpp
        src/InputFile.cpp
        src/InputFile.hpp
        src/Lexer.cpp
        src/Lexer.hpp
        src/Main.cpp
        src/Parser.cpp
        src/Parser.hpp
        src/Position.hpp
        src/Token.hpp
        src/Utility/Utils.cpp
        src/Utility/Utils.hpp
        src/Keyword.hpp
        src/Operator.hpp
        src/BuiltinType.hpp
        src/Common.hpp
)

include_directories(${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})

include_directories("src")

target_precompile_headers(Compiler PRIVATE
        # STL
        <filesystem>
        <fstream>
        <functional>
        <iostream>
        <print>
        <optional>
        <unordered_map>
        <variant>
        <vector>
        <memory>

        # magic_enum
        <magic_enum/magic_enum.hpp>

        # LLVM
        <llvm/IR/IRBuilder.h>
        <llvm/IR/LLVMContext.h>
        <llvm/IR/Module.h>
        <llvm/IR/Value.h>
        <llvm/IR/Verifier.h>

        # Our headers
        "src/Utility/Attributes.hpp"
        "src/Utility/CompilerSpecific.hpp"
        "src/Common.hpp"
)

# Link against necessary LLVM libraries
llvm_map_components_to_libnames(llvm_libs Core Support IRReader)

# Link all external libraries
target_link_libraries(Compiler ${llvm_libs} magic_enum::magic_enum)
