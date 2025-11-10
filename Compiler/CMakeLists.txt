# Find external packages
find_package(LLVM REQUIRED CONFIG)
find_package(magic_enum REQUIRED CONFIG)

# Add source files
aux_source_directory("src" SOURCES)

# Add executable target
add_executable (Compiler ${SOURCES})

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

        # magic_enum
        <magic_enum/magic_enum.hpp>

        # LLVM
        <llvm/IR/IRBuilder.h>
        <llvm/IR/LLVMContext.h>
        <llvm/IR/Module.h>
        <llvm/IR/Value.h>

        # Our headers
        "src/Attributes.hpp"
        "src/CompilerSpecific.hpp"
)

# Link against necessary LLVM libraries
llvm_map_components_to_libnames(llvm_libs core support irreader)

# Link all external libraries
target_link_libraries(Compiler ${llvm_libs} magic_enum::magic_enum)

# TODO: Add tests and install targets if needed.
