#include "Utils.hpp"

void Utils::Exit(ErrorCodes code)
{
    std::cout.flush();
    std::cerr.flush();

    std::println("Exiting early with code {0:d} ({0})", code);

    exit(static_cast<i32>(code));  // NOLINT(concurrency-mt-unsafe)
}

std::string Utils::FormatFileLink(const std::filesystem::path& filepath, const Position& position)
{
    const auto absolutePath = absolute(filepath);
    return std::format(ANSI_FILE_LINK("{}", "{}:{}:{}"), absolutePath.generic_string(), absolutePath.string(), position.line, position.column);
}

// ReSharper disable CppClangTidyCertDcl58Cpp
size_t std::replace_all(std::string& str, const std::string_view pattern, const std::string_view replacement)  // NOLINT(cert-dcl58-cpp)
{
    std::string::size_type offset;
    size_t occurrences = 0;

    while ((offset = str.find(pattern)) != std::string::npos)
    {
        str.replace(offset, pattern.size(), replacement);
        occurrences++;
    }

    return occurrences;
}
// ReSharper enable CppClangTidyCertDcl58Cpp

unsigned llvm::getTypeBitSize(const Value* value)
{
    return static_cast<u32>(value->getType()->getPrimitiveSizeInBits().getFixedValue());
}
