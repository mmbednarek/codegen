#include <algorithm>
#include <mb/codegen/expression.h>

namespace mb::codegen {

call::call(const call &other) : m_function_name(other.m_function_name->copy()),
                                m_arguments(other.m_arguments.size()) {
   std::transform(other.m_arguments.begin(), other.m_arguments.end(), m_arguments.begin(), [](const expression::ptr &arg) {
      return arg->copy();
   });
}

void call::write_expression(writer &w) const {
   m_function_name->write_expression(w);
   w.write("(");
   if (!m_arguments.empty()) {
      auto it_first = m_arguments.begin();
      (*it_first)->write_expression(w);
      std::for_each(it_first + 1, m_arguments.end(), [&w](const expression::ptr &ex) {
         w.write(", ");
         ex->write_expression(w);
      });
   }
   w.write(")");
}

expression::ptr call::copy() const {
   return std::make_unique<call>(*this);
}

raw::raw(const std::string &contents) : m_contents(contents) {}

void raw::write_expression(writer &w) const {
   w.write(m_contents);
}

expression::ptr raw::copy() const {
   return std::make_unique<raw>(m_contents);
}

assign::assign(std::string_view variable, const expression &value) : m_variable(variable), m_value(value.copy()) {}

void assign::write_expression(writer &w) const {
   w.write("{} = ", m_variable);
   m_value->write_expression(w);
}

expression::ptr assign::copy() const {
   return std::make_unique<assign>(m_variable, *m_value);
}

void items::add(const expression &expr) {
   m_items.emplace_back(expr.copy());
}

void items::write_expression(writer &w) const {
   w.write("\n");
   w.indent_in();
   std::for_each(m_items.begin(), m_items.end(), [&w](const expression::ptr &expr) {
      w.put_indent();
      expr->write_expression(w);
      w.write(",\n");
   });
   w.indent_out();
}

items::items(const items &other) : m_items(other.m_items.size()) {
   std::transform(other.m_items.begin(), other.m_items.end(), m_items.begin(), [](const expression::ptr &expr) {
      return expr->copy();
   });
}

expression::ptr items::copy() const {
   return std::make_unique<items>(*this);
}

struct_constructor::struct_constructor(const struct_constructor &other) : m_items(other.m_items.size()) {
   std::transform(other.m_items.begin(), other.m_items.end(), m_items.begin(), [](const expression::ptr &expr) {
      return expr->copy();
   });
}

void struct_constructor::add(const expression &expr) {
   m_items.emplace_back(expr.copy());
}

void struct_constructor::write_expression(writer &w) const {
   w.write("{");
   auto first = m_items.begin();
   (*first)->write_expression(w);
   std::for_each(first + 1, m_items.end(), [&w](const expression::ptr &expr) {
      w.write(", ");
      expr->write_expression(w);
   });
   w.write("}");
}

expression::ptr struct_constructor::copy() const {
   return std::make_unique<struct_constructor>(*this);
}

method_call::method_call(const method_call &other) : m_object(other.m_object->copy()),
                                                     m_method_name(other.m_method_name),
                                                     m_arguments(other.m_arguments.size()) {
   std::transform(other.m_arguments.begin(), other.m_arguments.end(), m_arguments.begin(), [](const expression::ptr &arg) {
      return arg->copy();
   });
}

void method_call::write_expression(writer &w) const {
   m_object->write_expression(w);
   w.write(".");
   w.write(m_method_name);
   w.write("(");
   if (!m_arguments.empty()) {
      auto it_first = m_arguments.begin();
      (*it_first)->write_expression(w);
      std::for_each(it_first + 1, m_arguments.end(), [&w](const expression::ptr &ex) {
        w.write(", ");
        ex->write_expression(w);
      });
   }
   w.write(")");
}

expression::ptr method_call::copy() const {
   return std::make_unique<method_call>(*this);
}

}// namespace mb::codegen
