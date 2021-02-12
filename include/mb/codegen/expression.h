#ifndef LIBMB_EXPRESSION_H
#define LIBMB_EXPRESSION_H
#include "writer.h"
#include <mb/generic.h>
#include <memory>
#include <vector>

namespace mb::codegen {

class expression {
 public:
   using ptr = std::unique_ptr<expression>;

   virtual void write_expression(writer &w) const = 0;
   [[nodiscard]] virtual expression::ptr copy() const = 0;
};

class raw : public expression {
   std::string m_contents;

 public:
   explicit raw(const std::string &contents);
   template<typename... ARGS>
   explicit raw(const std::string &format, ARGS... args) : m_contents(fmt::format(format, args...)) {}

   void write_expression(writer &w) const override;
   [[nodiscard]] ptr copy() const override;
};

class call : public expression {
   expression::ptr m_function_name;
   std::vector<expression::ptr> m_arguments;
 public:
   template<typename... ARGS>
   explicit call(const std::string &function_name, ARGS &&...args) : m_function_name(raw(function_name).copy()), m_arguments(as_vector_copy<expression::ptr, expression>(args...)) {}
   template<typename... ARGS>
   explicit call(const expression &expre, ARGS &&...args) : m_function_name(expre.copy()), m_arguments(as_vector_copy<expression::ptr, expression>(args...)) {}
   call(const call &other);

   void write_expression(writer &w) const override;
   [[nodiscard]] ptr copy() const override;
};

class assign : public expression {
   std::string_view m_variable;
   expression::ptr m_value;

 public:
   assign(std::string_view variable, const expression &value);

   void write_expression(writer &w) const override;
   [[nodiscard]] ptr copy() const override;
};

class items : public expression {
   std::vector<expression::ptr> m_items;

 public:
   items() = default;
   items(const items &other);

   void add(const expression &expr);
   void write_expression(writer &w) const override;
   [[nodiscard]] ptr copy() const override;
};

class struct_constructor : public expression {
   std::vector<expression::ptr> m_items;

 public:
   struct_constructor() = default;
   struct_constructor(const struct_constructor &other);

   void add(const expression &expr);
   void write_expression(writer &w) const override;
   [[nodiscard]] ptr copy() const override;
};

}// namespace mb::codegen

#endif//LIBMB_EXPRESSION_H
