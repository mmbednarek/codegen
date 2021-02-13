#include <mb/codegen/statement.h>

#include <utility>

namespace mb::codegen {

expr::expr(const expression &expr) : m_expr(expr.copy()) {}

void expr::write_statement(writer &w) const {
   w.put_indent();
   m_expr->write_expression(w);
   w.write(";\n");
}

statement::ptr expr::copy() const {
   return std::make_unique<expr>(*m_expr);
}

statement::collector &statement::collector::operator<<(const statement &stmt) {
   m_statements.emplace_back(stmt.copy());
   return *this;
}


statement::collector &statement::collector::operator<<(const expression &expre) {
   m_statements.emplace_back(expr(expre).copy());
   return *this;
}

std::vector<statement::ptr> statement::collector::build() {
   return std::move(m_statements);
}

if_statement::if_statement(const expression &condition, std::function<void(statement::collector &)> if_then) : m_condition(condition.copy()),
                                                                                                               m_if_then([&if_then]() {
                                                                                                                  statement::collector col;
                                                                                                                  if_then(col);
                                                                                                                  return col.build();
                                                                                                               }()) {}

if_statement::if_statement(const expression &condition, std::function<void(statement::collector &)> if_then, std::function<void(statement::collector &)> if_else) : m_condition(condition.copy()),
                                                                                                                                                                    m_if_then([&if_then]() {
                                                                                                                                                                       statement::collector col;
                                                                                                                                                                       if_then(col);
                                                                                                                                                                       return col.build();
                                                                                                                                                                    }()),
                                                                                                                                                                    m_if_else([&if_else]() {
                                                                                                                                                                       statement::collector col;
                                                                                                                                                                       if_else(col);
                                                                                                                                                                       return col.build();
                                                                                                                                                                    }()) {}
if_statement::if_statement(const if_statement &other) : m_condition(other.m_condition->copy()),
                                                        m_if_then(other.m_if_then.size()),
                                                        m_if_else(other.m_if_else.size()) {
   std::transform(other.m_if_then.begin(), other.m_if_then.end(), m_if_then.begin(), [](const statement::ptr &stmt) {
      return stmt->copy();
   });
   std::transform(other.m_if_else.begin(), other.m_if_else.end(), m_if_else.begin(), [](const statement::ptr &stmt) {
      return stmt->copy();
   });
}

void if_statement::write_statement(writer &w) const {
   if (m_if_then.empty()) {
      if (!m_if_else.empty()) {
         w.put_indent();
         w.write("if (!");
         m_condition->write_expression(w);
         w.write(") {\n");
         w.indent_in();
         std::for_each(m_if_else.begin(), m_if_else.end(), [&w](const statement::ptr &stmt) {
            stmt->write_statement(w);
         });
         w.indent_out();
         w.put_indent();
         w.write("}\n");
      }
      return;
   }
   w.put_indent();
   w.write("if (");
   m_condition->write_expression(w);
   w.write(") {\n");
   w.indent_in();
   std::for_each(m_if_then.begin(), m_if_then.end(), [&w](const statement::ptr &stmt) {
      stmt->write_statement(w);
   });
   w.indent_out();
   w.put_indent();
   w.write("}");
   if (!m_if_else.empty()) {
      w.write(" else {\n");
      w.indent_in();
      std::for_each(m_if_else.begin(), m_if_else.end(), [&w](const statement::ptr &stmt) {
         stmt->write_statement(w);
      });
      w.indent_out();
      w.put_indent();
      w.write("}\n");
   } else {
      w.write("\n");
   }
}

statement::ptr if_statement::copy() const {
   return std::make_unique<if_statement>(*this);
}

switch_statement::switch_statement(const expression &value) : m_value(value.copy()) {}

switch_statement::switch_statement(const switch_statement &other) : m_value(other.m_value->copy()),
                                                                    m_cases(other.m_cases),
                                                                    m_default_case(other.m_default_case.size()),
                                                                    m_default_case_scope(other.m_default_case_scope) {
   std::transform(other.m_default_case.begin(), other.m_default_case.end(), m_default_case.begin(), [](const statement::ptr &stmt) {
      return stmt->copy();
   });
}

void switch_statement::add(const expression &case_expr, std::function<void(statement::collector &)> statements) {
   m_cases.emplace_back(case_statement(case_expr, [&statements]() {
      statement::collector col;
      statements(col);
      return col.build();
   }()));
}

void switch_statement::add_noscope(const expression &case_expr, std::function<void(statement::collector &)> statements) {
   m_cases.emplace_back(case_statement(
           case_expr, [&statements]() {
              statement::collector col;
              statements(col);
              return col.build();
           }(),
           false));
}

void switch_statement::write_statement(writer &w) const {
   if (m_cases.empty()) {
      return;
   }
   w.put_indent();
   w.write("switch (");
   m_value->write_expression(w);
   w.write(") {\n");
   std::for_each(m_cases.begin(), m_cases.end(), [&w](const case_statement &stmt) {
      w.put_indent();
      w.write("case ");
      stmt.m_case->write_expression(w);
      if (stmt.m_scope) {
         w.write(": {\n");
      } else {
         w.write(":\n");
      }
      w.indent_in();
      std::for_each(stmt.m_statements.begin(), stmt.m_statements.end(), [&w](const statement::ptr &stmt) {
         stmt->write_statement(w);
      });
      w.indent_out();
      if (stmt.m_scope) {
         w.put_indent();
         w.write("}\n");
      }
   });
   if (!m_default_case.empty()) {
      w.put_indent();
      if (m_default_case_scope) {
         w.write("default: {\n");
      } else {
         w.write("default: \n");
      }
      w.indent_in();
      std::for_each(m_default_case.begin(), m_default_case.end(), [&w](const statement::ptr &stmt) {
         stmt->write_statement(w);
      });
      w.indent_out();
      if (m_default_case_scope) {
         w.put_indent();
         w.write("}\n");
      }
   }
   w.put_indent();
   w.write("}\n");
}

statement::ptr switch_statement::copy() const {
   return std::make_unique<switch_statement>(*this);
}

void switch_statement::add_default(std::function<void(statement::collector &)> statements) {
   m_default_case = [&statements]() {
      statement::collector col;
      statements(col);
      return col.build();
   }();
}

void switch_statement::add_default_noscope(std::function<void(statement::collector &)> statements) {
   m_default_case_scope = false;
   add_default(std::move(statements));
}

switch_statement::case_statement::case_statement(const switch_statement::case_statement &other) : m_case(other.m_case->copy()),
                                                                                                  m_statements(other.m_statements.size()),
                                                                                                  m_scope(other.m_scope) {
   std::transform(other.m_statements.begin(), other.m_statements.end(), m_statements.begin(), [](const statement::ptr &stmt) {
      return stmt->copy();
   });
}

switch_statement::case_statement::case_statement(const expression &case_expr, std::vector<statement::ptr> statements) : m_case(case_expr.copy()),
                                                                                                                        m_statements(std::move(statements)) {}

switch_statement::case_statement::case_statement(const expression &case_expr, std::vector<statement::ptr> statements, bool scope) : m_case(case_expr.copy()),
                                                                                                                                    m_statements(std::move(statements)),
                                                                                                                                    m_scope(scope) {}

return_statement::return_statement(const expression &expre) : m_value(expre.copy()) {}

return_statement::return_statement(const return_statement &other) : m_value(other.m_value == nullptr ? nullptr : other.m_value->copy()) {}

void return_statement::write_statement(writer &w) const {
   w.put_indent();
   w.write("return");
   if (m_value != nullptr) {
      w.write(" ");
      m_value->write_expression(w);
   }
   w.write(";\n");
}

statement::ptr return_statement::copy() const {
   return std::make_unique<return_statement>(*this);
}

for_statement::for_statement(const expression &start, const expression &condition, const expression &progress, std::function<void(statement::collector &)> body) : m_start(start.copy()),
                                                                                                                                                                   m_condition(condition.copy()),
                                                                                                                                                                   m_progress(progress.copy()),
                                                                                                                                                                   m_body([&body]() {
                                                                                                                                                                      statement::collector col;
                                                                                                                                                                      body(col);
                                                                                                                                                                      return col.build();
                                                                                                                                                                   }()) {}
for_statement::for_statement(const for_statement &other) : m_start(other.m_start->copy()),
                                                           m_condition(other.m_condition->copy()),
                                                           m_progress(other.m_progress->copy()),
                                                           m_body(other.m_body.size()) {
   std::transform(other.m_body.begin(), other.m_body.end(), m_body.begin(), [](const statement::ptr &stmt) {
      return stmt->copy();
   });
}

void for_statement::write_statement(writer &w) const {
   w.put_indent();
   w.write("for (");
   m_start->write_expression(w);
   w.write("; ");
   m_condition->write_expression(w);
   w.write("; ");
   m_progress->write_expression(w);
   w.write(") {\n");
   w.indent_in();
   std::for_each(m_body.begin(), m_body.end(), [&w](const statement::ptr &stmt) {
      stmt->write_statement(w);
   });
   w.indent_out();
   w.put_indent();
   w.write("}\n");
}

statement::ptr for_statement::copy() const {
   return std::make_unique<for_statement>(*this);
}

}// namespace mb::codegen