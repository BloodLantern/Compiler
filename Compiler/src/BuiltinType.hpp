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

enum class BuiltinType : u8
{
    BUILTIN_TYPE_ENUM_CONTENTS
};
