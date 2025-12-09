# Find external packages
find_package(LLVM REQUIRED CONFIG)
find_package(magic_enum REQUIRED CONFIG)

# Add executable target
add_executable(Compiler
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
        <algorithm>
        <cassert>
        <cctype>
        <cstdio>
        <cstdlib>
        <map>
        <memory>
        <string>
        <system_error>
        <utility>
        <vector>

        # LLVM
        <llvm/ADT/APFloat.h>
        <llvm/ADT/STLExtras.h>
        <llvm/IR/BasicBlock.h>
        <llvm/IR/Constants.h>
        <llvm/IR/DerivedTypes.h>
        <llvm/IR/Function.h>
        <llvm/IR/IRBuilder.h>
        <llvm/IR/Instructions.h>
        <llvm/IR/LLVMContext.h>
        <llvm/IR/LegacyPassManager.h>
        <llvm/IR/Module.h>
        <llvm/IR/Type.h>
        <llvm/IR/Verifier.h>
        <llvm/MC/TargetRegistry.h>
        <llvm/Support/FileSystem.h>
        <llvm/Support/TargetSelect.h>
        <llvm/Support/raw_ostream.h>
        <llvm/Target/TargetMachine.h>
        <llvm/Target/TargetOptions.h>
        <llvm/TargetParser/Host.h>

        # magic_enum
        <magic_enum/magic_enum.hpp>

        # Our headers
        "src/Utility/Attributes.hpp"
        "src/Utility/CompilerSpecific.hpp"
        "src/Common.hpp"
)

if (WIN32)
    execute_process(
            COMMAND mkdir -p "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional"
            COMMAND cp -rv "C:/Program Files/Microsoft Visual Studio/2022/Enterprise/DIA SDK" "C:/Program Files (x86)/Microsoft Visual Studio/2019/Professional/DIA SDK"
    )
endif ()

# Link against necessary LLVM libraries
llvm_map_components_to_libnames(llvm_libs
        XRay
        AArch64AsmParser
        AArch64CodeGen
        AArch64Desc
        AArch64Disassembler
        AArch64Info
        AArch64Utils
        AggressiveInstCombine
        Analysis
        ARMAsmParser
        ARMCodeGen
        ARMDesc
        ARMDisassembler
        ARMInfo
        ARMUtils
        AsmParser
        AsmPrinter
        BinaryFormat
        BitReader
        BitstreamReader
        BitWriter
        BPFAsmParser
        BPFCodeGen
        BPFDesc
        BPFDisassembler
        BPFInfo
        CFGuard
        CFIVerify
        CGData
        CodeGen
        CodeGenTypes
        Core
        Coroutines
        Coverage
        DebugInfoBTF
        DebugInfoCodeView
        Debuginfod
        DebugInfoDWARF
        DebugInfoDWARFLowLevel
        DebugInfoGSYM
        DebugInfoLogicalView
        DebugInfoMSF
        DebugInfoPDB
        Demangle
        Diff
        DlltoolDriver
        DWARFCFIChecker
        DWARFLinker
        DWARFLinkerClassic
        DWARFLinkerParallel
        DWP
        ExecutionEngine
        Exegesis
        ExegesisAArch64
        ExegesisRISCV
        ExegesisX86
        Extensions
        FileCheck
        FrontendAtomic
        FrontendDirective
        FrontendDriver
        FrontendHLSL
        FrontendOffloading
        FrontendOpenACC
        FrontendOpenMP
        FuzzerCLI
        FuzzMutate
        GlobalISel
        HipStdPar
        InstCombine
        Instrumentation
        InterfaceStub
        Interpreter
        ipo
        IRPrinter
        IRReader
        JITLink
        LibDriver
        LineEditor
        Linker
        LTO
        MC
        MCA
        MCDisassembler
        MCJIT
        MCParser
        MIRParser
        NVPTXCodeGen
        NVPTXDesc
        NVPTXInfo
        ObjCARCOpts
        ObjCopy
        Object
        ObjectYAML
        OptDriver
        Option
        OrcDebugging
        OrcJIT
        OrcShared
        OrcTargetProcess
        Passes
        ProfileData
        Remarks
        RISCVAsmParser
        RISCVCodeGen
        RISCVDesc
        RISCVDisassembler
        RISCVInfo
        RISCVTargetMCA
        RuntimeDyld
        SandboxIR
        ScalarOpts
        SelectionDAG
        Support
        Symbolize
        TableGen
        TableGenBasic
        TableGenCommon
        Target
        TargetParser
        Telemetry
        TextAPI
        TextAPIBinaryReader
        TransformUtils
        Vectorize
        WebAssemblyAsmParser
        WebAssemblyCodeGen
        WebAssemblyDesc
        WebAssemblyDisassembler
        WebAssemblyInfo
        WebAssemblyUtils
        WindowsDriver
        X86AsmParser
        X86CodeGen
        X86Desc
        X86Disassembler
        X86Info
        X86TargetMCA)

# Link all external libraries
target_link_libraries(Compiler ${llvm_libs} magic_enum::magic_enum)
