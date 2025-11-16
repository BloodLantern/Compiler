#pragma once

#include "Utils.hpp"

class InputFile
{
public:
    explicit InputFile(std::string_view filepath);

    void ReadContents();

    GETTER_M(const std::string&, Text)
    GETTER_M(const std::filesystem::path&, Filepath)

    std::string::value_type operator[](std::string::size_type offset) const;

private:
    std::string m_Text;
    std::filesystem::path m_Filepath;
};
