#pragma once

#include "Token.hpp"

/// @brief Operators sorted by precedence, with a higher precedence being executed first.
enum class Operator : uint8_t
{
    None,

    Precedence0,
    Ternary,
    Assignment,
    AddEquals,
    SubtractEquals,
    MultiplyEquals,
    DivideEquals,
    ModuloEquals,
    BitshiftLeftEquals,
    BitshiftRightEquals,
    BinaryAndEquals,
    BinaryXorEquals,
    BinaryOrEquals,
    NullCoalescingEquals,
    WideArrow,

    Precedence1,
    NullCoalescing,

    Precedence2,
    LogicalOr,

    Precedence3,
    LogicalAnd,

    Precedence4,
    BinaryOr,

    Precedence5,
    BinaryXor,

    Precedence6,
    BinaryAnd,

    Precedence7,
    Equals,
    NotEquals,

    Precedence8,
    Less,
    LessEquals,
    Greater,
    GreaterEquals,

    Precedence9,
    BitshiftLeft,
    BitshiftRight,

    Precedence10,
    Addition,
    Subtraction,

    Precedence11,
    Multiplication,
    Division,
    Modulo,

    Precedence12,
    PointerToMemberDot,
    PointerToMemberArrow,

    Precedence13,
    PreIncrement,
    PreDecrement,
    Negation,
    LogicalNot,
    BinaryNot,
    Cast,
    Dereference,
    AddressOf,

    Precedence14,
    PostIncrement,
    PostDecrement,
    FunctionCall,
    Subscript,
    MemberAccessDot,
    MemberAccessArrow,

    Precedence15,
    ScopeResolution,
};

constexpr int OperatorGetPrecedence(Operator op);
constexpr bool OperatorHasLeftOperand(Operator op);
constexpr bool OperatorHasRightOperand(Operator op);

inline std::unordered_map BaseOperatorConversionLookup
{
    std::make_pair(TokenType::Equals, Operator::Assignment),

    std::make_pair(TokenType::DoubleColon, Operator::ScopeResolution),

    std::make_pair(TokenType::Dot, Operator::MemberAccessDot),
    std::make_pair(TokenType::MinusGreaterThan, Operator::MemberAccessArrow),

    std::make_pair(TokenType::EqualsGreaterThan, Operator::WideArrow),

    std::make_pair(TokenType::Plus, Operator::Addition),
    std::make_pair(TokenType::Minus, Operator::Subtraction),
    std::make_pair(TokenType::Star, Operator::Multiplication),
    std::make_pair(TokenType::Slash, Operator::Division),
    std::make_pair(TokenType::Percent, Operator::Modulo),

    std::make_pair(TokenType::DoublePlus, Operator::PreIncrement),
    std::make_pair(TokenType::DoubleMinus, Operator::PreDecrement),

    std::make_pair(TokenType::DoubleEquals, Operator::Equals),
    std::make_pair(TokenType::ExclamationMarkEquals, Operator::NotEquals),
    std::make_pair(TokenType::LessThan, Operator::Less),
    std::make_pair(TokenType::LessThanEquals, Operator::LessEquals),
    std::make_pair(TokenType::GreaterThan, Operator::Greater),
    std::make_pair(TokenType::GreaterThanEquals, Operator::GreaterEquals),

    std::make_pair(TokenType::DoubleAmpersand, Operator::LogicalAnd),
    std::make_pair(TokenType::DoubleVerticalBar, Operator::LogicalOr),
    std::make_pair(TokenType::ExclamationMark, Operator::LogicalNot),

    std::make_pair(TokenType::Ampersand, Operator::BinaryAnd),
    std::make_pair(TokenType::VerticalBar, Operator::BinaryOr),
    std::make_pair(TokenType::Caret, Operator::BinaryXor),
    std::make_pair(TokenType::Tilde, Operator::BinaryNot),

    std::make_pair(TokenType::DoubleLessThan, Operator::BitshiftLeft),
    std::make_pair(TokenType::DoubleGreaterThan, Operator::BitshiftRight),

    std::make_pair(TokenType::QuestionMark, Operator::Ternary),
    std::make_pair(TokenType::DoubleQuestionMark, Operator::NullCoalescing),

    std::make_pair(TokenType::PlusEquals, Operator::AddEquals),
    std::make_pair(TokenType::MinusEquals, Operator::SubtractEquals),
    std::make_pair(TokenType::StarEquals, Operator::MultiplyEquals),
    std::make_pair(TokenType::SlashEquals, Operator::DivideEquals),
    std::make_pair(TokenType::PercentEquals, Operator::ModuloEquals),
    std::make_pair(TokenType::AmpersandEquals, Operator::BinaryAndEquals),
    std::make_pair(TokenType::VerticalBarEquals, Operator::BinaryOrEquals),
    std::make_pair(TokenType::CaretEquals, Operator::BinaryXorEquals),
    std::make_pair(TokenType::DoubleLessThanEquals, Operator::BitshiftLeftEquals),
    std::make_pair(TokenType::DoubleGreaterThanEquals, Operator::BitshiftRightEquals),
    std::make_pair(TokenType::DoubleQuestionMarkEquals, Operator::NullCoalescingEquals),
};

// Start of Operator.inl

#define GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, precedence) if ((op) > Operator::Precedence##precedence) return (precedence) * 10;

constexpr int OperatorGetPrecedence(const Operator op)
{
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 15)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 14)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 13)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 12)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 11)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 10)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 9)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 8)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 7)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 6)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 5)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 4)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 3)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 2)
    GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE(op, 1)

    return 0;
}

constexpr bool OperatorHasLeftOperand(const Operator op)
{
    switch (op)
    {
        case Operator::None:
        case Operator::Precedence0:
        case Operator::Precedence1:
        case Operator::Precedence2:
        case Operator::Precedence3:
        case Operator::Precedence4:
        case Operator::Precedence5:
        case Operator::Precedence6:
        case Operator::Precedence7:
        case Operator::Precedence8:
        case Operator::Precedence9:
        case Operator::Precedence10:
        case Operator::Precedence11:
        case Operator::Precedence12:
        case Operator::Precedence13:
        case Operator::Precedence14:
        case Operator::Precedence15:
            return false;

        case Operator::Ternary:
        case Operator::Assignment:
        case Operator::AddEquals:
        case Operator::SubtractEquals:
        case Operator::MultiplyEquals:
        case Operator::DivideEquals:
        case Operator::ModuloEquals:
        case Operator::BitshiftLeftEquals:
        case Operator::BitshiftRightEquals:
        case Operator::BinaryAndEquals:
        case Operator::BinaryXorEquals:
        case Operator::BinaryOrEquals:
        case Operator::NullCoalescingEquals:
        case Operator::WideArrow:
        case Operator::NullCoalescing:
        case Operator::LogicalOr:
        case Operator::LogicalAnd:
        case Operator::BinaryOr:
        case Operator::BinaryXor:
        case Operator::BinaryAnd:
        case Operator::Equals:
        case Operator::NotEquals:
        case Operator::Less:
        case Operator::LessEquals:
        case Operator::Greater:
        case Operator::GreaterEquals:
        case Operator::BitshiftLeft:
        case Operator::BitshiftRight:
        case Operator::Addition:
        case Operator::Subtraction:
        case Operator::Multiplication:
        case Operator::Division:
        case Operator::Modulo:
        case Operator::PointerToMemberDot:
        case Operator::PointerToMemberArrow:
        case Operator::FunctionCall:
        case Operator::Subscript:
        case Operator::MemberAccessDot:
        case Operator::MemberAccessArrow:
        case Operator::ScopeResolution:
        case Operator::PostIncrement:
        case Operator::PostDecrement:
            return true;

        case Operator::PreIncrement:
        case Operator::PreDecrement:
        case Operator::Negation:
        case Operator::LogicalNot:
        case Operator::BinaryNot:
        case Operator::Cast:
        case Operator::Dereference:
        case Operator::AddressOf:
            return false;
    }

    return false;
}

constexpr bool OperatorHasRightOperand(const Operator op)
{
    switch (op)
    {
        case Operator::None:
        case Operator::Precedence0:
        case Operator::Precedence1:
        case Operator::Precedence2:
        case Operator::Precedence3:
        case Operator::Precedence4:
        case Operator::Precedence5:
        case Operator::Precedence6:
        case Operator::Precedence7:
        case Operator::Precedence8:
        case Operator::Precedence9:
        case Operator::Precedence10:
        case Operator::Precedence11:
        case Operator::Precedence12:
        case Operator::Precedence13:
        case Operator::Precedence14:
        case Operator::Precedence15:
            return false;

        case Operator::Ternary:
        case Operator::Assignment:
        case Operator::AddEquals:
        case Operator::SubtractEquals:
        case Operator::MultiplyEquals:
        case Operator::DivideEquals:
        case Operator::ModuloEquals:
        case Operator::BitshiftLeftEquals:
        case Operator::BitshiftRightEquals:
        case Operator::BinaryAndEquals:
        case Operator::BinaryXorEquals:
        case Operator::BinaryOrEquals:
        case Operator::NullCoalescingEquals:
        case Operator::WideArrow:
        case Operator::NullCoalescing:
        case Operator::LogicalOr:
        case Operator::LogicalAnd:
        case Operator::BinaryOr:
        case Operator::BinaryXor:
        case Operator::BinaryAnd:
        case Operator::Equals:
        case Operator::NotEquals:
        case Operator::Less:
        case Operator::LessEquals:
        case Operator::Greater:
        case Operator::GreaterEquals:
        case Operator::BitshiftLeft:
        case Operator::BitshiftRight:
        case Operator::Addition:
        case Operator::Subtraction:
        case Operator::Multiplication:
        case Operator::Division:
        case Operator::Modulo:
        case Operator::PointerToMemberDot:
        case Operator::PointerToMemberArrow:
        case Operator::FunctionCall:
        case Operator::Subscript:
        case Operator::MemberAccessDot:
        case Operator::MemberAccessArrow:
        case Operator::ScopeResolution:
        case Operator::PreIncrement:
        case Operator::PreDecrement:
        case Operator::Negation:
        case Operator::LogicalNot:
        case Operator::BinaryNot:
        case Operator::Cast:
        case Operator::Dereference:
        case Operator::AddressOf:
            return true;

        case Operator::PostIncrement:
        case Operator::PostDecrement:
            return false;
    }

    return false;
}

#undef GET_OPERATOR_PRECEDENCE_CHECK_PRECEDENCE
