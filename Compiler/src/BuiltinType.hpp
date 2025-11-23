#pragma once

#define BUILTIN_TYPE_ENUM_CONTENTS \
    Void, /* No value */ \
    Bool, /* Either 'true' or 'false' */ \
    I8, /* Signed 8-bit integer */ \
    I16, /* Signed 16-bit integer */ \
    I32, /* Signed 32-bit integer */ \
    I64, /* Signed 64-bit integer */ \
    I128, /* Signed 128-bit integer */ \
    U8, /* Unsigned 8-bit integer */ \
    U16, /* Unsigned 16-bit integer */ \
    U32, /* Unsigned 32-bit integer */ \
    U64, /* Unsigned 64-bit integer */ \
    U128, /* Unsigned 128-bit integer */ \
    F16, /* 16-bit floating-point */ \
    F32, /* 32-bit floating-point */ \
    F64, /* 64-bit floating-point */ \
    F128, /* 128-bit floating-point */ \
    ISize, /* Signed integer with a bit-width equivalent to that of a pointer */ \
    USize, /* Unsigned integer with a bit-width equivalent to that of a pointer */ \
    C8, /* UTF-8 character */ \
    C16, /* UTF-16 character */ \
    C32, /* UTF-32 character */ \
    S8, /* UTF-8 string */ \
    S16, /* UTF-16 string */ \
    S32, /* UTF-32 string */

enum class BuiltinType : u8
{
    BUILTIN_TYPE_ENUM_CONTENTS
};
