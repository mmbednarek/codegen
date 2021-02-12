#ifndef LIBMB_STATEMENT_H
#define LIBMB_STATEMENT_H
#include "expression.h"
#include "writer.h"
#include <memory>

namespace mb::codegen {

class statement {
 public:
   using ptr = std::unique_ptr<statement>;

   virtual void write_statement(writer &w) const = 0;
   [[nodiscard]] virtual statement::ptr copy() const = 0;

   class collector {
      std::vector<statement::ptr> m_statements;

    public:
      collector &operator<<(const statement &stmt);
      collector &operator<<(const expression &expre);
      std::vector<statement::ptr> build();
   };
};

// expr - an expression statement
class expr : public statement {
   expression::ptr m_expr;

 public:
   explicit expr(const expression &expr);

   void write_statement(writer &w) const override;
   [[nodiscard]] ptr copy() const override;
};

class if_statement : public statement {
   expression::ptr m_condition;
   std::vector<statement::ptr> m_if_then;
   std::vector<statement::ptr> m_if_else;

 public:
   if_statement(const expression &condition, std::function<void(statement::collector &)> if_then);
   if_statement(const expression &condition, std::function<void(statement::collector &)> if_then, std::function<void(statement::collector &)> if_else);
   if_statement(const if_statement &other);

   void write_statement(writer &w) const override;
   ptr copy() const override;
};

class switch_statement : public statement {
   struct case_statement {
      expression::ptr m_case;
      std::vector<statement::ptr> m_statements;
      bool m_scope{true};

      case_statement(const expression &case_expr, std::vector<statement::ptr> statements);
      case_statement(const expression &case_expr, std::vector<statement::ptr> statements, bool scope);
      case_statement(const case_statement &other);
   };
   expression::ptr m_value;
   std::vector<case_statement> m_cases;

 public:
   explicit switch_statement(const expression &value);
   switch_statement(const switch_statement &other);

   void add(const expression &case_expr, std::function<void(statement::collector &)> statements);
   void add_noscope(const expression &case_expr, std::function<void(statement::collector &)> statements);

   void write_statement(writer &w) const override;
   ptr copy() const override;
};

class return_statement : public statement {
   expression::ptr m_value;
 public:
   return_statement() = default;
   explicit return_statement(const expression &expre);
   return_statement(const return_statement &other);

   void write_statement(writer &w) const override;
   ptr copy() const override;
};

}// namespace mb::codegen

#endif//LIBMB_STATEMENT_H
