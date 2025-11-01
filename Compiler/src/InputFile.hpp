#pragma once

#include "Utils.hpp"

class InputFile
{
public:
    explicit InputFile(std::string_view filepath);

    void ReadContents();

    GETTER_M(const std::string&, Text)
    GETTER_M(std::string_view, Filepath)
    GETTER_M(size_t, LineCount)

    std::string::value_type operator[](std::string::size_type offset) const;

private:
    std::string m_Text;
    std::string_view m_Filepath;
    size_t m_LineCount = 0;
};
