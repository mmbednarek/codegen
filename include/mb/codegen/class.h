#ifndef LIBMB_CLASS_H
#define LIBMB_CLASS_H
#include "definable.h"

namespace mb::codegen {

struct attribute {
   std::string_view type;
   std::string_view name;
   bool default_constr{true};
};

class class_member {
 public:
   using ptr = std::unique_ptr<class_member>;

   virtual void write_declaration(writer &w) const = 0;
   virtual void write_definition(writer &w) const = 0;
   virtual void set_class_name(std::string_view class_name) = 0;
   [[nodiscard]] virtual ptr copy() const = 0;
};

class class_spec : public definable {
   std::string_view m_name;
   std::vector<class_member::ptr> m_public_members;
   std::vector<class_member::ptr> m_private_members;
   std::vector<attribute> m_public_attributes;
   std::vector<attribute> m_private_attributes;

 public:
   explicit class_spec(std::string_view name);
   class_spec(const class_spec &other);

   void add_public(const class_member &member);
   void add_private(const class_member &member);
   void add_public(std::string_view type, std::string_view name);
   void add_private(std::string_view type, std::string_view name);

   void write_declaration(writer &w) const override;
   void write_definition(writer &w) const override;
   [[nodiscard]] ptr copy() const override;
};

class method : public class_member {
 private:
   std::string_view m_return_type;
   std::string_view m_class_name;
   std::string_view m_name;
   std::vector<arg> m_arguments;
   std::vector<statement::ptr> m_statements;
   bool m_const{};

 public:
   method(std::string_view return_type, std::string_view name, std::vector<arg> arguments, std::function<void(statement::collector &)> statement_gen);
   method(std::string_view return_type, std::string_view name, std::vector<arg> arguments, bool constant, std::function<void(statement::collector &)> statement_gen);
   method(const method &other);

   void write_declaration(writer &w) const override;
   void write_definition(writer &w) const override;
   void set_class_name(std::string_view class_name) override;
   [[nodiscard]] ptr copy() const override;
};

class static_method : public class_member {
 private:
   std::string_view m_return_type;
   std::string_view m_class_name;
   std::string_view m_name;
   std::vector<arg> m_arguments;
   std::vector<statement::ptr> m_statements;

 public:
   static_method(std::string_view return_type, std::string_view name, std::vector<arg> arguments, std::function<void(statement::collector &)> statement_gen);
   static_method(const static_method &other);

   void write_declaration(writer &w) const override;
   void write_definition(writer &w) const override;
   void set_class_name(std::string_view class_name) override;
   [[nodiscard]] ptr copy() const override;
};

class default_constructor : public class_member {
   std::string_view m_class_name;
 public:
   default_constructor() = default;
   default_constructor(const default_constructor &other);

   void write_declaration(writer &w) const override;
   void write_definition(writer &w) const override;
   void set_class_name(std::string_view class_name) override;
   ptr copy() const override;
};

class constructor : public class_member {
 private:
   std::string_view m_class_name;
   std::vector<arg> m_arguments;
   std::vector<statement::ptr> m_statements;

 public:
   constructor(std::vector<arg> arguments, std::function<void(statement::collector &)> statement_gen);
   constructor(const constructor &other);

   void write_declaration(writer &w) const override;
   void write_definition(writer &w) const override;
   void set_class_name(std::string_view class_name) override;
   [[nodiscard]] ptr copy() const override;
};

class static_attribute : public class_member {
 private:
   std::string_view m_class_name;
   std::string_view m_type;
   std::string_view m_name;
   expression::ptr m_value;

 public:
   static_attribute(std::string_view type, std::string_view name, const expression &value);
   static_attribute(const static_attribute &other);

   void write_declaration(writer &w) const override;
   void write_definition(writer &w) const override;
   void set_class_name(std::string_view class_name) override;
   [[nodiscard]] ptr copy() const override;
};

}// namespace mb::codegen

#endif//LIBMB_CLASS_H
