#include "Utils.hpp"

void Utils::Exit(ErrorCodes code)
{
    std::cout.flush();
    std::cerr.flush();

    std::println("Exiting early with code {0:d} ({0})", code);

    exit(static_cast<int>(code));  // NOLINT(concurrency-mt-unsafe)
}

// ReSharper disable CppClangTidyCertDcl58Cpp
size_t std::replace_all(std::string& str, const std::string_view pattern, const std::string_view replacement)  // NOLINT(cert-dcl58-cpp)
{
    std::string::size_type offset;
    size_t occurrences = 0;

    while ((offset = str.find(pattern)) != std::string::npos)
    {
        str.replace(offset, pattern.size(), replacement);
        ++occurrences;
    }

    return occurrences;
}
// ReSharper enable CppClangTidyCertDcl58Cpp
