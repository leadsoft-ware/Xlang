#include "../../include/xast.cpp"

struct xit_stmt{
    enum type{ _normal_stmt,_right_stmt };
    xast::astree ast;
};