#include <mb/codegen/lambda.h>

#include <utility>

namespace mb::codegen {

lambda::lambda(std::vector<arg> arguments, std::function<void(statement::collector &)> statement_gen) : m_arguments(std::move(arguments)),
                                                                                                        m_statements([&statement_gen]() {
                                                                                                           statement::collector col;
                                                                                                           statement_gen(col);
                                                                                                           return col.build();
                                                                                                        }()) {}
lambda::lambda(const lambda &other) : m_captures(other.m_captures.size()),
                                      m_arguments(other.m_arguments),
                                      m_statements(other.m_statements.size()) {
   std::transform(other.m_captures.begin(), other.m_captures.end(), m_captures.begin(), [](const expression::ptr &cap) {
      return cap->copy();
   });
   std::transform(other.m_statements.begin(), other.m_statements.end(), m_statements.begin(), [](const statement::ptr &stmt) {
      return stmt->copy();
   });
}

void lambda::add_capture(const expression& cap) {
   m_captures.emplace_back(cap.copy());
}

void lambda::write_expression(writer &w) const {
   w.write("[");
   if (!m_captures.empty()) {
      auto it_first_cap = m_captures.begin();
      (*it_first_cap)->write_expression(w);
      std::for_each(it_first_cap+1, m_captures.end(), [&w](const expression::ptr &cap) {
        w.write(", ");
        cap->write_expression(w);
      });
   }
   w.write("] (");
   if (!m_arguments.empty()) {
      auto it_first = m_arguments.begin();
      w.write("{} {}", it_first->type, it_first->name);
      std::for_each(it_first + 1, m_arguments.end(), [&w](const arg &arg) {
        w.write(", {} {}", arg.type, arg.name);
      });
   }
   w.write(") {\n");
   w.indent_in();
   std::for_each(m_statements.begin(), m_statements.end(), [&w](const statement::ptr &stmt) {
     stmt->write_statement(w);
   });
   w.indent_out();
   w.put_indent();
   w.write("}");
}

expression::ptr lambda::copy() const {
   return std::make_unique<lambda>(*this);
}

}// namespace mb::codegen