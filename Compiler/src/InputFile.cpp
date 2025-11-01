#include "PrecompiledHeader.hpp"
#include "InputFile.hpp"

#include "Utils.hpp"

InputFile::InputFile(const std::string_view filepath)
    : m_Filepath{filepath}
{
}

void InputFile::ReadContents()
{
    std::ifstream input{std::string{m_Filepath}, std::ios::ate | std::ios::in};
    const std::streampos size = input.tellg();

    m_Text.resize(size);

    input.seekg(0);
    input.read(m_Text.data(), size);
    input.close();

#ifdef ENVIRONMENT_WINDOWS
    m_LineCount = replace_all(m_Text, "\r\n", "\n");
#else
    std::string::size_type offset;
    while ((offset = m_Text.find('\n', offset)) != std::string::npos)
    {
        m_LineCount++;
        offset++; // Skip the newline character
    }
#endif
}

std::string::value_type InputFile::operator[](const std::string::size_type offset) const
{
    return m_Text[offset];
}
