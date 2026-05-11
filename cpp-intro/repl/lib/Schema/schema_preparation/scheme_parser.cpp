#include "scheme_parser.h"

namespace ct::schema::schema_parser {
using namespace ctpg;
using namespace ctpg::buffers;
using namespace ctpg::ftors;

void Accumulate::ins_elem(std::string type, std::string location) {
  types_.push_back(std::move(type));
  locations_.push_back(std::move(location));
}

void Accumulate::check_types_correctness() {
  for (const auto& cur_struct : schema_.structures) {
    // check if structures fields are existing types
    for (const auto& cur_struct_field : cur_struct.second.fields_) {
      if (!schema_.exist_type(cur_struct_field.second)) {
        throw SyntaxError(
            "Error: structure " + cur_struct.first + " have field with unknown type " + cur_struct_field.second + "\n"
        );
      }
      if (cur_struct.second.name_ == cur_struct_field.second) {
        throw SyntaxError("Error: recursive structure " + cur_struct.first + "\n");
      }
    }
  }

  for (const auto& cur_func : schema_.funcs) {
    // check function arguments are existing types
    for (const auto& cur_func_arg : cur_func.second.args_) {
      if (!schema_.exist_type(cur_func_arg.second)) {
        throw SyntaxError(
            "Error: function " + cur_func.first + " have argument with unknown type " + cur_func_arg.second + "\n"
        );
      }
    }
  }
}

constexpr char id_alph[] = "[a-zA-Z_][a-zA-Z0-9_]*";
constexpr regex_term<id_alph> ID("ID");

constexpr nterm<Accumulate> SCHEMA("SCHEMA");

constexpr string_term fn("fn");
constexpr string_term structure("struct");
constexpr string_term arrow("->");

constexpr nterm<Struct> struct_declare("struct_declare");
constexpr nterm<name_n_type> field_declare("field_declare");
constexpr nterm<name_n_type_list> fields("fields");
constexpr nterm<Function> func_declare("func_declare");
constexpr nterm<name_n_type> arg_declare("arg_declare");
constexpr nterm<name_n_type_list> args("args");
constexpr parser SCHEMA_PARSER(
    SCHEMA,
    terms(fn, structure, arrow, '{', '}', ';', ID),
    nterms(SCHEMA, struct_declare, field_declare, fields, func_declare, arg_declare, args),
    rules(

        SCHEMA() >= [] { return Accumulate{}; },
        SCHEMA(SCHEMA, struct_declare) >=
            [](auto&& cur_ans, auto&& new_elem) {
              for (auto& cur_field : new_elem.fields_) {
                const auto& name_field = cur_field.first;
                const auto& type_field = cur_field.second;
                cur_ans.ins_elem(type_field, "struct " + std::string(new_elem.name_) + "." + name_field);
              }
              cur_ans.schema_.ins_struct(new_elem);
              return std::forward<decltype(cur_ans)>(cur_ans);
            },

        SCHEMA(SCHEMA, func_declare) >=
            [](auto&& cur_ans, auto&& new_elem) {
              cur_ans.ins_elem(
                  std::string(new_elem.return_type_),
                  "fn " + std::string(new_elem.name_) + " " + " return type"
              );
              cur_ans.schema_.ins_func(new_elem);
              return std::forward<decltype(cur_ans)>(cur_ans);
            },

        struct_declare("struct", ID, '{', fields, '}') >=
            [](auto,
               auto&& name_struct,
               ctpg::term_value<char>,
               const name_n_type_list& fields_,
               ctpg::term_value<char>) {
              Struct ans;
              ans.name_ = name_struct;
              ans.fields_ = fields_;
              ans.set_fields_.clear();
              for (auto& cur_field : ans.fields_) {
                const auto& name_field = cur_field.first;
                const auto& type_field = cur_field.second;
                if (ans.set_fields_.find(name_field) != ans.set_fields_.end()) {
                  throw SyntaxError(
                      "Error: Duplicate field " + name_field + " in struct " + std::string(ans.name_) + "\n"
                  );
                }
                ans.set_fields_[name_field] = type_field;
              }
              return ans;
            },

        fields() >= [] { return name_n_type_list{}; },
        fields(fields, field_declare) >=
            [](auto&& cur, auto&& new_elem) {
              cur.push_back((new_elem));
              return std::forward<decltype(cur)>(cur);
            },

        field_declare(ID, ID, ';') >=
            [](auto&& type, auto&& name, ctpg::term_value<char>) { return name_n_type{name, type}; },

        func_declare("fn", ID, "->", ID, '{', args, '}') >=
            [](auto,
               auto&& func_name,
               ctpg::term_value<std::string_view>,
               auto&& return_type,
               ctpg::term_value<char>,
               const name_n_type_list& args_,
               auto) {
              Function ans;
              ans.name_ = func_name;
              ans.args_ = args_;
              ans.return_type_ = return_type;
              std::unordered_set<std::string> seen_args;
              for (auto& cur : ans.args_) {
                if (seen_args.find(cur.first) != seen_args.end()) {
                  throw SyntaxError("Error: Duplicate argument " + cur.first + "in function " + std::string(func_name));
                }
                seen_args.insert(cur.first);
                ans.set_args_[cur.first] = cur.second;
              }
              return ans;
            },

        args() >= [] { return name_n_type_list{}; },
        args(args, arg_declare) >=
            [](auto&& cur, auto&& new_elem) {
              cur.push_back(new_elem);
              return std::forward<decltype(cur)>(cur);
            },

        arg_declare(ID, ID, ';') >= [](auto&& type, auto&& name, auto) { return name_n_type{(name), (type)}; }
    )
);

Schema SchemaParser::parse(const char*& src) {
  std::stringstream syntax_error;
  auto ans = SCHEMA_PARSER.parse(string_buffer(src), syntax_error);
  if (!ans.has_value()) {
    throw SyntaxError("Error: Failed to parse schema " + syntax_error.str() + "\n");
  }
  ans->check_types_correctness();
  return std::move(ans->schema_);
}

} // namespace ct::schema::schema_parser
