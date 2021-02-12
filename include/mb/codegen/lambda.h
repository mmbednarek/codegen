#ifndef CODEGEN_LAMBDA_H
#define CODEGEN_LAMBDA_H
#include "definable.h"
#include "expression.h"
#include <vector>

namespace mb::codegen {

class lambda : public expression {
   std::vector<expression::ptr> m_captures;
   std::vector<arg> m_arguments;
   std::vector<statement::ptr> m_statements;

 public:
   lambda(std::vector<arg> arguments, std::function<void(statement::collector &)> statement_gen);
   lambda(const lambda &other);

   template<typename... ARGS>
   lambda(std::vector<arg> arguments, std::function<void(statement::collector &)> statement_gen, ARGS... captures) : m_captures(as_vector_copy<expression::ptr, expression>(captures...)),
                                                                                                                     m_arguments(std::move(arguments)),
                                                                                                                     m_statements([&statement_gen]() {
                                                                                                                        statement::collector col;
                                                                                                                        statement_gen(col);
                                                                                                                        return col.build();
                                                                                                                     }()) {}

   void add_capture(const expression &cap);
   void write_expression(writer &w) const override;
   [[nodiscard]] ptr copy() const override;
};

}// namespace mb::codegen

#endif//CODEGEN_LAMBDA_H
