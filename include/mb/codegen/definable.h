#ifndef LIBMB_OBJECT_H
#define LIBMB_OBJECT_H
#include "expression.h"
#include "statement.h"
#include "writer.h"
#include <memory>

namespace mb::codegen {

class definable {
 public:
   using ptr = std::unique_ptr<definable>;

   virtual ~definable() noexcept = default;

   virtual void write_declaration(writer &w) const = 0;
   virtual void write_definition(writer &w) const = 0;
   [[nodiscard]] virtual definable::ptr copy() const = 0;
};

class globalvar : public definable {
   std::string_view m_type;
   std::string_view m_name;
   expression::ptr m_value;

 public:
   globalvar(std::string_view type, std::string_view name, const expression &value);

   void write_declaration(writer &w) const override;
   void write_definition(writer &w) const override;
   [[nodiscard]] definable::ptr copy() const override;
};

struct arg {
   std::string_view type;
   std::string_view name;
   arg(std::string_view type, std::string_view name);
};

class function : public definable {
 private:
   std::string_view m_return_type;
   std::string_view m_name;
   std::vector<arg> m_arguments;
   std::vector<statement::ptr> m_statements;

 public:
   function(std::string_view return_type, std::string_view name, std::vector<arg> arguments, std::function<void(statement::collector &)> statement_gen);
   function(const function &other);

   void write_declaration(writer &w) const override;
   void write_definition(writer &w) const override;
   [[nodiscard]] ptr copy() const override;
};

}// namespace mb::codegen

#endif//LIBMB_OBJECT_H
