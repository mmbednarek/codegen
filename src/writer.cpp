#include <mb/codegen/writer.h>

namespace mb::codegen {

writer::writer(std::ostream &stream) : m_stream(stream) {}

void writer::indent_in() {
    ++m_indent;
}

void writer::indent_out() {
    if (m_indent == 0)
        return;
    --m_indent;
}

std::ostream &writer::raw() {
    return m_stream;
}

void writer::line() {
    m_stream << "\n";
}

void writer::put_indent() {
    for (mb::u32 i = 0; i < m_indent; ++i)
        m_stream << "   ";
}

void writer::descope_flat() {
    write("}\n");
}

void writer::line(std::string_view sv) {
    put_indent();
    write(sv);
    line();
}

void writer::descope() {
    indent_out();
    put_indent();
    write("}\n");
}

void writer::write(std::string_view sv) {
    m_stream << sv;
}

}// namespace mb::codegen