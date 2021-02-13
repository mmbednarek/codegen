#include <gtest/gtest.h>
#include <iostream>
#include <mb/codegen/class.h>
#include <mb/codegen/component.h>
#include <mb/codegen/definable.h>
#include <mb/codegen/expression.h>
#include <mb/codegen/lambda.h>
#include <mb/codegen/statement.h>
#include <mb/codegen/writer.h>

TEST(codegen, call) {
   using namespace mb::codegen;

   call some("hello", call("something"), call("something_else", raw("value")));

   std::stringstream ss;
   mb::codegen::writer w(ss);
   some.write_expression(w);

   ASSERT_EQ(ss.str(), "hello(something(), something_else(value))");
}

TEST(codegen, exprstmt) {
   using namespace mb::codegen;

   expr ex(call("hello", raw("2 + 2")));

   std::stringstream ss;
   mb::codegen::writer w(ss);
   w.indent_in();
   ex.write_statement(w);

   ASSERT_EQ(ss.str(), "   hello(2 + 2);\n");
}

TEST(codegen, function) {
   using namespace mb::codegen;

   function func("std::string", "append_strings", {{"std::string", "a"}, {"std::string", "b"}}, [](statement::collector &col) {
      col << call("hello");
   });

   std::stringstream ss;
   mb::codegen::writer w(ss);
   w.indent_in();
   func.write_declaration(w);
   func.write_definition(w);

   ASSERT_EQ(ss.str(), "   std::string append_strings(std::string a, std::string b);\n   std::string append_strings(std::string a, std::string b) {\n      hello();\n   }\n");
}

TEST(codegen, classes) {
   using namespace mb::codegen;

   class_spec c("foo");
   c.add_public(method("int", "do_stuff", {{"int", "a"}, {"float", "b"}}, true, [](statement::collector &col) {
      lambda lam(
              {{"int", "v"}}, [](statement::collector &col) {
                 col << raw("return v * a * {} - 1", "b");
              },
              raw("a"), raw("b"));
      col << call("foobar", lam);
      col << raw("return a + (int)b");
      lambda for_each_lam({{"const int", "&val"}}, [](statement::collector &col) {
         col << raw("stuff");
      });
      col << call("std::transform", call("args.begin"), call("args.end"), call("target.begin"), for_each_lam);
   }));
   c.add_public(method_template("int", "do_stuff", {{"typename", "T"}}, {{"int", "a"}, {"float", "b"}}, true, [](statement::collector &col) {
      lambda lam(
              {{"int", "v"}}, [](statement::collector &col) {
                 col << raw("return v * a * {} - 1", "b");
              },
              raw("a"), raw("b"));
      col << call("foobar", lam);
      col << raw("return a + (int)b");
      lambda for_each_lam({{"const int", "&val"}}, [](statement::collector &col) {
         col << raw("stuff");
      });
      col << call("std::transform", call("args.begin"), call("args.end"), call("target.begin"), for_each_lam);
   }));

   struct_constructor constr1;
   constr1.add(raw("2"));
   constr1.add(raw("3"));
   struct_constructor constr2;
   constr2.add(raw("6"));
   constr2.add(raw("3"));

   items something;
   something.add(constr1);
   something.add(constr2);

   c.add_public(static_attribute("std::vector<std::vector<int>>", "something", something));
   c.add_private(static_method("void", "do_something_else", {}, [](statement::collector &col) {
      col << call("do_stuff", raw("3"), raw("0.4"));
      col << if_statement(
              raw("a == b"),
              [](statement::collector &col) {// if
                 col << call("foobar");
                 switch_statement some_switch(raw("test"));
                 some_switch.add_noscope(raw("5"), [](statement::collector &col) {
                    col << call("something");
                 });
                 some_switch.add(raw("4"), [](statement::collector &col) {
                    col << call("something_else");
                 });
                 col << some_switch;
              },
              [](statement::collector &col) {// else
                 col << call("something_else");
              });
   }));
   c.add_public("int", "bar");
   c.add_private("int", "bar2");

   mb::codegen::writer w(std::cerr);
   c.write_declaration(w);
   c.write_definition(w);
}

TEST(codegen, full) {
   using namespace mb::codegen;

   component cmp("mb::foo::bar", "MB_FOO_BAR_H");
   cmp.header_include("bbbb");
   cmp.header_include_local("bbbb.h");
   cmp.header_include("aaaa");
   cmp.header_include_local("aaaa.h");
   cmp.header_include("zzzz");
   cmp.header_include_local("zzzz.h");
   cmp.source_include("bbbbbb");
   cmp.source_include_local("bbbbbb.h");
   cmp.source_include("aaaaaa");
   cmp.source_include_local("aaaaaa.h");
   cmp.source_include("zzzzzz");
   cmp.source_include_local("zzzzzz.h");

   class_spec c("foo", "FOO_H");
   c.add_public(method("int", "do_stuff", {{"int", "a"}, {"float", "b"}}, [](statement::collector &col) {
      lambda lam(
              {{"int", "v"}}, [](statement::collector &col) {
                 col << raw("return v * a * {} - 1", "b");
              },
              raw("a"), raw("b"));
      col << call("foobar", lam);
      col << return_statement(raw("a + b"));
      lambda for_each_lam({{"const int", "&val"}}, [](statement::collector &col) {
         col << raw("stuff");
      });
      col << call("std::transform", call("args.begin"), call("args.end"), call("target.begin"), for_each_lam);
   }));

   struct_constructor constr1;
   constr1.add(raw("2"));
   constr1.add(raw("3"));
   struct_constructor constr2;
   constr2.add(raw("6"));
   constr2.add(raw("3"));

   items something;
   something.add(constr1);
   something.add(constr2);

   c.add_public(static_attribute("std::vector<std::vector<int>>", "something", something));
   c.add_private(method("void", "do_something_else", {}, [](statement::collector &col) {
      col << call("do_stuff", raw("3"), raw("0.4"));
      col << if_statement(
              raw("a == b"),
              [](statement::collector &col) {// if
                 col << call(lambda({}, [](statement::collector &col) {
                    col << call("foobar");
                 }));
                 switch_statement some_switch(raw("test"));
                 some_switch.add_noscope(raw("5"), [](statement::collector &col) {
                    col << call("something");
                 });
                 some_switch.add(raw("4"), [](statement::collector &col) {
                    col << call("something_else");
                 });
                 col << some_switch;
                 col << for_statement(raw("int i = 0"), raw("i < 10"), raw("++i"), [](statement::collector &col) {
                    col << call("aaaaa");
                 });
              },
              [](statement::collector &col) {// else
                 col << call("something_else");
              });
   }));
   c.add_public("int", "bar");
   c.add_private("int", "bar2");

   cmp << c;

   std::cerr << "// header.h\n\n";
   cmp.write_header(std::cerr);
   std::cerr << "// source.cpp\n\n";
   cmp.write_source(std::cerr);
}