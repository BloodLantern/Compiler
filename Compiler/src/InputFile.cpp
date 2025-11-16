#include "InputFile.hpp"

#include "Utils.hpp"

InputFile::InputFile(const std::string_view filepath)
    : m_Filepath{filepath}
{
}

void InputFile::ReadContents()
{
    if (!std::filesystem::exists(m_Filepath))
    {
        std::println("Input file {} doesn't exist", m_Filepath);
        Utils::Exit(ErrorCodes::FatalError);
    }

    std::ifstream input{m_Filepath, std::ios::ate | std::ios::in};
    const std::streampos size = input.tellg();

    m_Text.resize(size);

    input.seekg(0);
    input.read(m_Text.data(), size);
    input.close();
}

std::string::value_type InputFile::operator[](const std::string::size_type offset) const
{
    return m_Text[offset];
}
