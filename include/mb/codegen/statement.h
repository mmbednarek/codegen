#ifndef LIBMB_STATEMENT_H
#define LIBMB_STATEMENT_H
#include "expression.h"
#include "writer.h"
#include <memory>
#include <functional>

namespace mb::codegen {

class statement {
 public:
   using ptr = std::unique_ptr<statement>;

   virtual ~statement() noexcept = default;

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
   bool m_constexpr = false;

 public:
   if_statement(const expression &condition, std::function<void(statement::collector &)> if_then);
   if_statement(const expression &condition, std::function<void(statement::collector &)> if_then, std::function<void(statement::collector &)> if_else);
   if_statement(const if_statement &other);

   constexpr if_statement &with_constexpr() {
      m_constexpr = true;
      return *this;
   }

   void write_statement(writer &w) const override;
   ptr copy() const override;
};

class if_switch_statement : public statement {
   struct if_case {
      expression::ptr condition;
      std::vector<statement::ptr> block;
   };

   std::vector<if_case> m_cases;

 public:
   if_switch_statement() = default;
   if_switch_statement(const if_switch_statement &other);
   void add_case(const expression &condition, std::function<void(statement::collector &)> block);

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
   std::vector<statement::ptr> m_default_case;
   bool m_default_case_scope{true};

 public:
   explicit switch_statement(const expression &value);
   switch_statement(const switch_statement &other);

   void add(const expression &case_expr, std::function<void(statement::collector &)> statements);
   void add_noscope(const expression &case_expr, std::function<void(statement::collector &)> statements);
   void add_default(std::function<void(statement::collector &)> statements);
   void add_default_noscope(std::function<void(statement::collector &)> statements);

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

class for_statement : public statement {
   expression::ptr m_start;
   expression::ptr m_condition;
   expression::ptr m_progress;
   std::vector<statement::ptr> m_body;
 public:
   for_statement(const expression& start, const expression &condition, const expression &progress, std::function<void(statement::collector &)> body);
   for_statement(const for_statement &other);

   void write_statement(writer &w) const override;
   ptr copy() const override;
};

class ranged_for_statement : public statement {
   std::string m_item_type;
   std::string m_value_name;
   expression::ptr m_range;
   std::vector<statement::ptr> m_body;
 public:
   ranged_for_statement(std::string_view item_type, std::string_view value_name,  const expression& range, std::function<void(statement::collector &)> body);
   ranged_for_statement(const ranged_for_statement &other);

   void write_statement(writer &w) const override;
   [[nodiscard]] ptr copy() const override;
};

}// namespace mb::codegen

#endif//LIBMB_STATEMENT_H
