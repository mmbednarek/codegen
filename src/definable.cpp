#include <algorithm>
#include <mb/codegen/definable.h>

namespace mb::codegen {

globalvar::globalvar(std::string_view type, std::string_view name, const expression &value) : m_type(type),
                                                                                              m_name(name),
                                                                                              m_value(value.copy()) {}

void globalvar::write_declaration(writer &w) const {
   w.line("extern {} {};", m_type, m_name);
}

void globalvar::write_definition(writer &w) const {
   w.put_indent();
   w.write("{} {} = ", m_type, m_name);
   m_value->write_expression(w);
   w.write(";\n");
}

definable::ptr globalvar::copy() const {
   return std::make_unique<globalvar>(m_type, m_name, *m_value);
}

void function::write_declaration(writer &w) const {
   w.put_indent();
   w.write("{} {}(", m_return_type, m_name);
   if (!m_arguments.empty()) {
      auto it_first = m_arguments.begin();
      w.write("{} {}", it_first->type, it_first->name);
      std::for_each(it_first + 1, m_arguments.end(), [&w](const arg &arg) {
         w.write(", {} {}", arg.type, arg.name);
      });
   }
   w.write(");\n");
}

void function::write_definition(writer &w) const {
   w.put_indent();
   w.write("{} {}(", m_return_type, m_name);
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
   w.write("}\n\n");
}

definable::ptr function::copy() const {
   return std::make_unique<function>(*this);
}

function::function(const function &other) : m_return_type(other.m_return_type), m_name(other.m_name),
                                            m_arguments(other.m_arguments) {
   m_statements.reserve(other.m_statements.size());
   std::transform(other.m_statements.begin(), other.m_statements.end(), std::back_inserter(m_statements),
                  [](const statement::ptr &stmt) {
                     return stmt->copy();
                  });
}

function::function(std::string_view return_type, std::string_view name, std::vector<arg> arguments,
                   std::function<void(statement::collector &)> statement_gen) : m_return_type(return_type),
                                                                                m_name(name),
                                                                                m_arguments(std::move(arguments)),
                                                                                m_statements([&statement_gen]() {
                                                                                   statement::collector col;
                                                                                   statement_gen(col);
                                                                                   return col.build();
                                                                                }()) {}

arg::arg(std::string_view type, std::string_view name) : type(type), name(name) {}

}// namespace mb::codegen