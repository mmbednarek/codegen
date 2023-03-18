#include <mb/codegen/component.h>
#include <mb/codegen/writer.h>
#include <utility>

namespace mb::codegen {

component::component(std::string ns) : m_namespace(std::move(ns)) {}

component::component(std::string ns, std::string header_constant) : m_namespace(std::move(ns)), m_header_constant(std::move(header_constant)) {}

void component::operator<<(const definable &def) {
   m_elements.emplace_back(def.copy());
}

void component::source_include(const std::string &inc) {
   m_source_includes.emplace_back(include{inc, false});
}

void component::source_include_local(const std::string &inc) {
   m_source_includes.emplace_back(include{inc, true});
}

void component::header_include(const std::string &inc) {
   m_header_includes.emplace_back(include{inc, false});
}

void component::header_include_local(const std::string &inc) {
   m_header_includes.emplace_back(include{inc, true});
}

void component::write_header(std::ostream &stream) {
   writer w(stream);
   std::sort(m_header_includes.begin(), m_header_includes.end(), [](const include &left, const include &right) {
      if (left.local && !right.local)
         return true;
      if (!left.local && right.local)
         return false;
      return left.path.compare(right.path) < 0;
   });

   if (!m_header_constant.empty()) {
      w.write("#ifndef {}\n#define {}\n", m_header_constant, m_header_constant);
   } else {
      w.write("#pragma once\n");
   }

   std::for_each(m_header_includes.begin(), m_header_includes.end(), [&w](const include &inc) {
      if (inc.local) {
         w.write("#include \"{}\"\n", inc.path);
      } else {
         w.write("#include <{}>\n", inc.path);
      }
   });

   w.write("\n");
   if (!m_namespace.empty()) {
      w.write("namespace {} {}\n\n", m_namespace, "{");
   }
   std::for_each(m_elements.begin(), m_elements.end(), [&w](const definable::ptr &def) {
      def->write_declaration(w);
   });
   if (!m_namespace.empty()) {
      w.write("}\n");
   }

   if (!m_header_constant.empty()) {
      w.write("#endif//{}\n", m_header_constant);
   }
}

void component::write_source(std::ostream &stream) {
   writer w(stream);
   std::sort(m_source_includes.begin(), m_source_includes.end(), [](const include &left, const include &right) {
     if (left.local && !right.local)
        return true;
     if (!left.local && right.local)
        return false;
     return left.path.compare(right.path) < 0;
   });

   std::for_each(m_source_includes.begin(), m_source_includes.end(), [&w](const include &inc) {
      if (inc.local) {
         w.write("#include \"{}\"\n", inc.path);
      } else {
         w.write("#include <{}>\n", inc.path);
      }
   });

   w.write("\n");
   if (!m_namespace.empty()) {
      w.write("namespace {} {}\n\n", m_namespace, "{");
   }
   std::for_each(m_elements.begin(), m_elements.end(), [&w](const definable::ptr &def) {
      def->write_definition(w);
   });
   if (!m_namespace.empty()) {
      w.write("}");
   }
}
}// namespace mb::codegen