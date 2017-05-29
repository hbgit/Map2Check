#pragma once
#include <string>
#include <memory>
namespace Map2Check::Parser {
    enum class Operator {
        PLUS,
        MINUS,
        TIMES,
        QUOCIENT
    };

    struct Argument {

    };

    struct ValueArgument : public Argument {
        double value;
    };

    struct IdentifierArgument : public Argument {
        std::string name;
    };

    struct Operation : public Argument {
        std::unique_ptr<Argument> lhs;
        std::unique_ptr<Argument> rhs;
        Operator op;
    };

    struct DynamicAlloc {
        std::unique_ptr<Argument> addr;
        std::unique_ptr<Argument> size;
        std::string name;
    };

    struct StaticAlloc {
        std::unique_ptr<Argument> addr;
        std::unique_ptr<Argument> size;
        std::string name;
    };

    struct DynamicRelease {
        std::unique_ptr<Argument> addr;
        std::string name;
    };

}

