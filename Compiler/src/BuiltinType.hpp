#pragma once

#define BUILTIN_TYPE_ENUM_CONTENTS \
    Void, \
    Bool, \
    Char, \
    SByte, \
    Byte, \
    Short, \
    UShort, \
    Int, \
    UInt, \
    Long, \
    ULong, \
    String,

enum class BuiltinType : uint8_t
{
    BUILTIN_TYPE_ENUM_CONTENTS
};
