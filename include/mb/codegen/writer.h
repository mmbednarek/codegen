#ifndef LIBMB_SCRIPT_WRITER_H
#define LIBMB_SCRIPT_WRITER_H
#include <cstdint>
#include <fmt/core.h>
#include <mb/int.h>
#include <ostream>

namespace mb::codegen {

class writer {
    std::ostream &m_stream;
    mb::u32 m_indent = 0;

public:
    explicit writer(std::ostream &m_stream);

    void indent_in();
    void indent_out();

    void line();
    void line(std::string_view sv);

    template<typename... Args>
    void line(std::string_view sv, Args... args) {
        put_indent();
        write(sv, args...);
        line();
    }

    template<typename... Args>
    void line_ignore(std::string_view sv, Args... args) {
        write(sv, args...);
        line();
    }

    void put_indent();

    void write(std::string_view sv);

    template<typename... Args>
    void write(std::string_view sv, Args... args) {
        m_stream << fmt::format(sv, args...);
    }

    template<typename... Args>
    void scope(std::string_view sv, Args... args) {
        put_indent();
        write(sv, args...);
        write(" {\n");
        indent_in();
    }

    template<typename... Args>
    void flat_scope(std::string_view sv, Args... args) {
        put_indent();
        write(sv, args...);
        write(" {\n");
    }

    void descope();

    template<typename... Args>
    void descope(std::string_view sv, Args... args) {
        indent_out();
        put_indent();
        write("}");
        write(sv, args...);
        line();
    }

    void descope_flat();

    std::ostream &raw();
};

}// namespace mb::codegen

#endif//LIBMB_SCRIPT_WRITER_H
