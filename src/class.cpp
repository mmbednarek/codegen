#include <mb/codegen/class.h>
#include <utility>

namespace mb::codegen {

attribute::attribute(std::string type, std::string name, bool default_const) : type(std::move(type)),
                                                                               name(std::move(name)),
                                                                               default_constr(default_const) {}

attribute::attribute(const attribute &other) : type(other.type),
                                               name(other.name),
                                               default_constr(other.default_constr) {}

void class_spec::write_declaration(writer &w) const {
   if (!m_class_constant.empty()) {
      w.write("#ifndef {}\n#define {}\n", m_class_constant, m_class_constant);
   }
   w.put_indent();
   w.write("class {} {}\n", m_name, "{");
   w.indent_in();
   std::for_each(m_private_attributes.begin(), m_private_attributes.end(), [&w](const attribute &attr) {
      w.put_indent();
      if (attr.default_constr) {
         w.write("{} {}{};\n", attr.type, attr.name, "{}");
      } else {
         w.write("{} {};\n", attr.type, attr.name);
      }
   });
   std::for_each(m_private_members.begin(), m_private_members.end(), [&w](const class_member::ptr &attirb) {
      attirb->write_declaration(w);
   });
   w.indent_out();
   w.write(" public:\n");
   w.indent_in();
   std::for_each(m_public_attributes.begin(), m_public_attributes.end(), [&w](const attribute &attr) {
      w.put_indent();
      if (attr.default_constr) {
         w.write("{} {}{};\n", attr.type, attr.name, "{}");
      } else {
         w.write("{} {};\n", attr.type, attr.name);
      }
   });
   std::for_each(m_public_members.begin(), m_public_members.end(), [&w](const class_member::ptr &attirb) {
      attirb->write_declaration(w);
   });
   w.indent_out();
   w.put_indent();
   w.write("};\n");
   if (!m_class_constant.empty()) {
      w.write("#endif//{}\n", m_class_constant);
   }
   w.write("\n");
}

void class_spec::write_definition(writer &w) const {
   std::for_each(m_public_members.begin(), m_public_members.end(), [&w](const class_member::ptr &attirb) {
      attirb->write_definition(w);
   });
   std::for_each(m_private_members.begin(), m_private_members.end(), [&w](const class_member::ptr &attirb) {
      attirb->write_definition(w);
   });
}

definable::ptr class_spec::copy() const {
   return std::make_unique<class_spec>(*this);
}

class_spec::class_spec(std::string name) : m_name(std::move(name)) {}

class_spec::class_spec(std::string name, std::string constant) : m_name(std::move(name)),
                                                                 m_class_constant(std::move(constant)) {}

class_spec::class_spec(const class_spec &other) : m_name(other.m_name),
                                                  m_public_members(other.m_public_members.size()),
                                                  m_private_members(other.m_private_members.size()),
                                                  m_public_attributes(other.m_public_attributes.size()),
                                                  m_private_attributes(other.m_private_attributes.size()),
                                                  m_class_constant(other.m_class_constant) {
   std::transform(other.m_public_members.begin(), other.m_public_members.end(), m_public_members.begin(), [](const class_member::ptr &mem) {
      return mem->copy();
   });
   std::transform(other.m_private_members.begin(), other.m_private_members.end(), m_private_members.begin(), [](const class_member::ptr &mem) {
      return mem->copy();
   });
   std::copy(other.m_public_attributes.begin(), other.m_public_attributes.end(), m_public_attributes.begin());
   std::copy(other.m_private_attributes.begin(), other.m_private_attributes.end(), m_private_attributes.begin());
}

void class_spec::add_public(const class_member &member) {
   auto copied = member.copy();
   copied->set_class_name(m_name);
   m_public_members.emplace_back(std::move(copied));
}

void class_spec::add_private(const class_member &member) {
   auto copied = member.copy();
   copied->set_class_name(m_name);
   m_private_members.emplace_back(std::move(copied));
}

void class_spec::add_public(std::string_view type, std::string_view name) {
   m_public_attributes.emplace_back(attribute(std::string(type), std::string(name)));
}

void class_spec::add_private(std::string_view type, std::string_view name) {
   m_private_attributes.emplace_back(attribute(std::string(type), std::string(name)));
}

method::method(const method &other) : m_return_type(other.m_return_type),
                                      m_name(other.m_name),
                                      m_class_name(other.m_class_name),
                                      m_arguments(other.m_arguments),
                                      m_const(other.m_const) {
   m_statements.reserve(other.m_statements.size());
   std::transform(other.m_statements.begin(), other.m_statements.end(), std::back_inserter(m_statements), [](const statement::ptr &stmt) {
      return stmt->copy();
   });
}

method::method(std::string_view return_type,
               std::string_view name,
               std::vector<arg> arguments,
               std::function<void(statement::collector &)> statement_gen) : m_return_type(return_type),
                                                                            m_name(name),
                                                                            m_arguments(std::move(arguments)),
                                                                            m_statements([&statement_gen]() {
                                                                               statement::collector col;
                                                                               statement_gen(col);
                                                                               return col.build();
                                                                            }()),
                                                                            m_const(false) {}

method::method(std::string_view return_type,
               std::string_view name,
               std::vector<arg> arguments,
               bool constant,
               std::function<void(statement::collector &)> statement_gen) : m_return_type(return_type),
                                                                            m_name(name),
                                                                            m_arguments(std::move(arguments)),
                                                                            m_statements([&statement_gen]() {
                                                                               statement::collector col;
                                                                               statement_gen(col);
                                                                               return col.build();
                                                                            }()),
                                                                            m_const(constant) {}

void method::write_declaration(writer &w) const {
   w.put_indent();
   w.write("{} {}(", m_return_type, m_name);
   if (!m_arguments.empty()) {
      auto it_first = m_arguments.begin();
      w.write("{} {}", it_first->type, it_first->name);
      std::for_each(it_first + 1, m_arguments.end(), [&w](const arg &arg) {
         w.write(", {} {}", arg.type, arg.name);
      });
   }
   if (m_const) {
      w.write(") const;\n");
   } else {
      w.write(");\n");
   }
}

void method::write_definition(writer &w) const {
   w.put_indent();
   w.write("{} {}::{}(", m_return_type, m_class_name, m_name);
   if (!m_arguments.empty()) {
      auto it_first = m_arguments.begin();
      w.write("{} {}", it_first->type, it_first->name);
      std::for_each(it_first + 1, m_arguments.end(), [&w](const arg &arg) {
         w.write(", {} {}", arg.type, arg.name);
      });
   }
   if (m_const) {
      w.write(") const {\n");
   } else {
      w.write(") {\n");
   }
   w.indent_in();
   std::for_each(m_statements.begin(), m_statements.end(), [&w](const statement::ptr &stmt) {
      stmt->write_statement(w);
   });
   w.indent_out();
   w.put_indent();
   w.write("}\n\n");
}

class_member::ptr method::copy() const {
   return std::make_unique<method>(*this);
}

void method::set_class_name(std::string class_name) {
   m_class_name = class_name;
}

constructor::constructor(std::vector<arg> arguments, std::function<void(statement::collector &)> statement_gen) : m_arguments(std::move(arguments)),
                                                                                                                  m_statements([&statement_gen]() {
                                                                                                                     statement::collector col;
                                                                                                                     statement_gen(col);
                                                                                                                     return col.build();
                                                                                                                  }()) {}

constructor::constructor(const constructor &other) : m_class_name(other.m_class_name),
                                                     m_arguments(other.m_arguments) {
   m_statements.reserve(other.m_statements.size());
   std::transform(other.m_statements.begin(), other.m_statements.end(), std::back_inserter(m_statements), [](const statement::ptr &stmt) {
      return stmt->copy();
   });
}

void constructor::write_declaration(writer &w) const {
   w.put_indent();
   w.write("{}(", m_class_name);
   if (!m_arguments.empty()) {
      auto it_first = m_arguments.begin();
      w.write("{} {}", it_first->type, it_first->name);
      std::for_each(it_first + 1, m_arguments.end(), [&w](const arg &arg) {
         w.write(", {} {}", arg.type, arg.name);
      });
   }
   w.write(");\n");
}

void constructor::write_definition(writer &w) const {
   w.put_indent();
   w.write("{}::{}(", m_class_name, m_class_name);
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

void constructor::set_class_name(std::string class_name) {
   m_class_name = class_name;
}

class_member::ptr constructor::copy() const {
   return std::make_unique<constructor>(*this);
}

static_attribute::static_attribute(std::string_view type, std::string_view name, const expression &value) : m_type(type),
                                                                                                            m_name(name),
                                                                                                            m_value(value.copy()) {}

static_attribute::static_attribute(const static_attribute &other) : m_class_name(other.m_class_name),
                                                                    m_type(other.m_type),
                                                                    m_name(other.m_name),
                                                                    m_value(other.m_value->copy()) {}

void static_attribute::write_declaration(writer &w) const {
   w.put_indent();
   w.write("static {} {};\n", m_type, m_name);
}

void static_attribute::write_definition(writer &w) const {
   w.put_indent();
   w.write("{} {}::{} {}", m_type, m_class_name, m_name, "{");
   m_value->write_expression(w);
   w.write("};\n\n");
}

void static_attribute::set_class_name(std::string class_name) {
   m_class_name = class_name;
}

class_member::ptr static_attribute::copy() const {
   return std::make_unique<static_attribute>(*this);
}

default_constructor::default_constructor(const default_constructor &other) : m_class_name(other.m_class_name) {}

void default_constructor::write_declaration(writer &w) const {
   w.put_indent();
   w.write("{}() = default;\n", m_class_name);
}

void default_constructor::write_definition(writer &w) const {
   // nothing here
}

void default_constructor::set_class_name(std::string class_name) {
   m_class_name = class_name;
}

class_member::ptr default_constructor::copy() const {
   return std::make_unique<default_constructor>(*this);
}

static_method::static_method(std::string_view return_type,
                             std::string_view name,
                             std::vector<arg> arguments,
                             std::function<void(statement::collector &)> statement_gen) : m_return_type(return_type),
                                                                                          m_name(name),
                                                                                          m_arguments(std::move(arguments)),
                                                                                          m_statements([&statement_gen]() {
                                                                                             statement::collector col;
                                                                                             statement_gen(col);
                                                                                             return col.build();
                                                                                          }()) {}

static_method::static_method(const static_method &other) : m_return_type(other.m_return_type),
                                                           m_name(other.m_name),
                                                           m_class_name(other.m_class_name),
                                                           m_arguments(other.m_arguments) {
   m_statements.reserve(other.m_statements.size());
   std::transform(other.m_statements.begin(), other.m_statements.end(), std::back_inserter(m_statements), [](const statement::ptr &stmt) {
      return stmt->copy();
   });
}

void static_method::write_declaration(writer &w) const {
   w.put_indent();
   w.write("static {} {}(", m_return_type, m_name);
   if (!m_arguments.empty()) {
      auto it_first = m_arguments.begin();
      w.write("{} {}", it_first->type, it_first->name);
      std::for_each(it_first + 1, m_arguments.end(), [&w](const arg &arg) {
         w.write(", {} {}", arg.type, arg.name);
      });
   }
   w.write(");\n");
}

void static_method::write_definition(writer &w) const {
   w.put_indent();
   w.write("{} {}::{}(", m_return_type, m_class_name, m_name);
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

void static_method::set_class_name(std::string class_name) {
   m_class_name = class_name;
}

class_member::ptr static_method::copy() const {
   return std::make_unique<static_method>(*this);
}

method_template::method_template(std::string_view return_type,
                                 std::string_view name,
                                 std::vector<arg> template_arguments,
                                 std::vector<arg> arguments,
                                 std::function<void(statement::collector &)> statement_gen) : m_return_type(return_type),
                                                                                              m_name(name),
                                                                                              m_template_arguments(std::move(template_arguments)),
                                                                                              m_arguments(std::move(arguments)),
                                                                                              m_statements([&statement_gen]() {
                                                                                                 statement::collector col;
                                                                                                 statement_gen(col);
                                                                                                 return col.build();
                                                                                              }()),
                                                                                              m_const(false) {}

method_template::method_template(std::string_view return_type,
                                 std::string_view name,
                                 std::vector<arg> template_arguments,
                                 std::vector<arg> arguments,
                                 bool constant,
                                 std::function<void(statement::collector &)> statement_gen) : m_return_type(return_type),
                                                                                              m_name(name),
                                                                                              m_template_arguments(std::move(template_arguments)),
                                                                                              m_arguments(std::move(arguments)),
                                                                                              m_statements([&statement_gen]() {
                                                                                                 statement::collector col;
                                                                                                 statement_gen(col);
                                                                                                 return col.build();
                                                                                              }()),
                                                                                              m_const(constant) {}

method_template::method_template(const method_template &other) : m_return_type(other.m_return_type),
                                                                 m_name(other.m_name),
                                                                 m_arguments(other.m_arguments),
                                                                 m_template_arguments(other.m_template_arguments),
                                                                 m_const(other.m_const) {
   m_statements.reserve(other.m_statements.size());
   std::transform(other.m_statements.begin(), other.m_statements.end(), std::back_inserter(m_statements), [](const statement::ptr &stmt) {
      return stmt->copy();
   });
}

void method_template::write_declaration(writer &w) const {
   w.write("\n");
   w.put_indent();
   w.write("template<");
   if (!m_template_arguments.empty()) {
      auto it_first = m_template_arguments.begin();
      w.write("{} {}", it_first->type, it_first->name);
      std::for_each(it_first + 1, m_template_arguments.end(), [&w](const arg &arg) {
         w.write(", {} {}", arg.type, arg.name);
      });
   }
   w.write(">\n");
   w.put_indent();
   w.write("{} {}(", m_return_type, m_name);
   if (!m_arguments.empty()) {
      auto it_first = m_arguments.begin();
      w.write("{} {}", it_first->type, it_first->name);
      std::for_each(it_first + 1, m_arguments.end(), [&w](const arg &arg) {
         w.write(", {} {}", arg.type, arg.name);
      });
   }
   if (m_const) {
      w.write(") const {\n");
   } else {
      w.write(") {\n");
   }
   w.indent_in();
   std::for_each(m_statements.begin(), m_statements.end(), [&w](const statement::ptr &stmt) {
      stmt->write_statement(w);
   });
   w.indent_out();
   w.put_indent();
   w.write("}\n\n");
}

void method_template::write_definition(writer &w) const {
   // nothing here
}

void method_template::set_class_name(std::string class_name) {
   // nothing here
}

class_member::ptr method_template::copy() const {
   return std::make_unique<method_template>(*this);
}

}// namespace mb::codegen
