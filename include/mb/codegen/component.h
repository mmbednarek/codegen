#ifndef CODEGEN_COMPONENT_H
#define CODEGEN_COMPONENT_H
#include "definable.h"
#include <set>
#include <string>
#include <string_view>

namespace mb::codegen {

struct include {
   std::string path;
   bool local{false};

   include(std::string_view path, bool local) : path{std::string{path}},
                                                local{local} {
   }

   std::strong_ordering operator<=>(const include &other) const {
      if (local && !other.local)
         return std::strong_ordering::less;
      if (!local && other.local)
         return std::strong_ordering::greater;
      return path <=> other.path;
   }
};

class component {
   std::string m_namespace;
   std::string m_header_constant;
   std::set<include> m_header_includes;
   std::set<include> m_source_includes;
   std::vector<definable::ptr> m_elements;

 public:
   explicit component(std::string ns);
   explicit component(std::string ns, std::string header_constant);

   void source_include(const std::string &inc);
   void source_include_local(const std::string &inc);
   void header_include(const std::string &inc);
   void header_include_local(const std::string &inc);

   void operator<<(const definable &def);

   void write_header(std::ostream &stream);
   void write_source(std::ostream &stream);
};

}// namespace mb::codegen

#endif//CODEGEN_COMPONENT_H
