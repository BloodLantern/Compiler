#pragma once

inline std::unique_ptr<llvm::LLVMContext> theContext;
inline std::unique_ptr<llvm::IRBuilder<>> builder;
inline std::unique_ptr<llvm::Module> theModule;
