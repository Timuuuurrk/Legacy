#include "query_parser.h"

#include <ctpg/ctpg.hpp>

#include <sstream>

namespace ct::schema::query_parser {
using namespace types_;
using namespace ctpg;

static Schema schema_;
using name_n_value = std::pair<std::string, std::unique_ptr<Basic_type>>;
using argument_list = std::vector<name_n_value>;

constexpr char id_alph[] = "[a-zA-Z_][a-zA-Z0-9_]*";
constexpr regex_term<id_alph> ID("ID");
constexpr char digits[] = "-?[0-9]+";
constexpr regex_term<digits> num("num");

constexpr char mask[] = "\"[^\"]*\"";
constexpr regex_term<mask> str("str");

constexpr nterm<std::unique_ptr<FuncType>> QUERY_RESULT("QUERY_RESULT");
constexpr nterm<argument_list> args("args");
constexpr nterm<name_n_value> arg_definition("arg_definition");

constexpr nterm<std::unique_ptr<StructType>> struct_result("struct_result");
constexpr nterm<argument_list> fields("fields");

std::unique_ptr<Basic_type> preparse_struct(auto&& fields_) {
  auto ans = std::make_unique<StructType>();
  ans->name = "";
  std::unordered_map<std::string, std::unique_ptr<Basic_type>> occured_fields;
  for (auto&& cur : fields_) {
    const std::string cur_field = std::move(cur.first);
    occured_fields[cur_field] = std::move(cur.second);
  }
  for (const auto& cur_fields : occured_fields) {
    std::pair<std::string, std::unique_ptr<Basic_type>> bufe = {
        cur_fields.first,
        std::move(occured_fields[cur_fields.first])
    };
    ans->seq_fields.push_back(std::move(bufe));
  }
  return ans;
}

std::unique_ptr<Basic_type> parse_number(const std::string& type, const std::string& cur_number_) {
  if (type == "uint32") {
    auto ans = std::make_unique<Uint32>();
    try {
      int64_t buf = std::stoul(cur_number_);
      if (buf > std::numeric_limits<uint32_t>::max()) {
        throw ParsingError("Error: uint32 type overflow!\n");
      }
      if (!cur_number_.empty() && cur_number_[0] == '-') {
        throw ParsingError("Error: uint32 type underflow!\n");
      }
      ans->val = buf;
    } catch (...) {
      throw ParsingError("Expected uint32 type, found: " + cur_number_ + "\n");
    }
    return ans;
  } else if (type == "uint64") {
    auto ans = std::make_unique<Uint64>();
    try {
      uint64_t buf = std::stoull(cur_number_);
      if (!cur_number_.empty() && cur_number_[0] == '-') {
        throw ParsingError("Error: uint64 type underflow!\n");
      }
      ans->val = buf;
    } catch (...) {
      throw ParsingError("Error: Expected uint64 type, found: " + cur_number_ + "\n");
    }
    return ans;
  } else if (type == "int32") {
    auto ans = std::make_unique<Int32>();
    try {
      int64_t buf = std::stoi(cur_number_);
      ans->val = buf;
    } catch (...) {
      throw ParsingError("Error: Expected int32 type, found: " + cur_number_ + "\n");
    }
    return ans;
  } else {
    auto ans = std::make_unique<Int64>();
    try {
      int64_t buf = std::stoll(cur_number_);
      ans->val = buf;
    } catch (...) {
      throw ParsingError("Error: Expected int64 type, found: " + cur_number_ + "\n");
    }
    return ans;
  }
}

void parse_struct(const std::string& type, std::unique_ptr<Basic_type>& from) {
  Struct* result = schema_.get_struct(type);
  if (result == nullptr) {
    throw ParsingError("Error: Unknown structure: " + type + "\n");
  }
  auto buf = dynamic_cast<StructType*>(from.release());
  std::unique_ptr<StructType> ans(buf);
  std::unordered_map<std::string, std::string, Hash, Equal> fields_names_n_types = result->set_fields_;
  std::unordered_set<std::string> names;
  for (const auto& it : fields_names_n_types) {
    names.insert(it.first);
  }

  ans->name = type;
  std::unordered_map<std::string, std::unique_ptr<Basic_type>> buf2;
  for (auto& cur_field : ans->seq_fields) {
    if (!fields_names_n_types.contains(cur_field.first)) {
      throw ParsingError("Error: Unknown field: " + cur_field.first + "\n");
    }
    if (!names.contains(cur_field.first)) {
      throw ParsingError("Error: Duplicate initialization: " + cur_field.first + "\n");
    }
    names.erase(cur_field.first);
    buf2[cur_field.first] = std::move(cur_field.second);
  }
  if (!names.empty()) {
    throw ParsingError("Error: Missing fields in structure " + type + "\n");
  }
  std::vector<std::pair<std::string, std::unique_ptr<Basic_type>>> seq_buf;
  for (auto& cur_field : result->fields_) {
    parse_primitive(result->set_fields_[cur_field.first], buf2[cur_field.first]);
    std::pair<std::string, std::unique_ptr<Basic_type>> bufe = {cur_field.first, std::move(buf2[cur_field.first])};
    seq_buf.push_back(std::move(bufe));
  }
  ans->seq_fields = std::move(seq_buf);
  from = std::move(ans);
}

void parse_primitive(const std::string& type, std::unique_ptr<Basic_type>& from) {
  // for structure - fill it's name and cast it's args
  // for number - cast it to the right type from  uint64
  // for string - nothing
  if (type == "string") {
    return;
  }
  if (schema_.exist_builtin_type(type) && type != "string") {
    // number
    auto cur_num = dynamic_cast<Uint64*>(from.get());
    std::string val_str = cur_num->val_str;
    std::unique_ptr<Basic_type> ans_num = parse_number(type, val_str);
    from = std::move(ans_num);
  } else {
    // struct
    parse_struct(type, from);
  }
}

constexpr parser query_parser(
    QUERY_RESULT,
    terms(str, num, '=', ',', '(', ')', '{', '}', ID),
    nterms(QUERY_RESULT, fields, struct_result, arg_definition, args),
    rules(

        QUERY_RESULT() >= [] { return std::make_unique<FuncType>(); },
        QUERY_RESULT(ID, '(', args, ')') >=
            [](auto&& func_name, ctpg::term_value<char>, auto&& func_args, ctpg::term_value<char>) {
              std::string func_name_ = std::string(std::forward<decltype(func_name)>(func_name));
              auto ans = std::make_unique<FuncType>();
              ans->name = std::forward<decltype(func_name)>(func_name);
              Function* func = schema_.get_func(func_name);
              if (func == nullptr) {
                throw ParsingError("Error: Missing query token: " + func_name_ + "\n");
              }
              std::unordered_map<std::string, std::unique_ptr<Basic_type>> cur_args;
              for (auto& cur : func_args) {
                if (func->set_args_.find(cur.first) == func->set_args_.end()) {
                  throw ParsingError("Error: Unknown argument of function " + ans->name + " found " + cur.first + "\n");
                }
                if (cur_args.contains(cur.first)) {
                  throw ParsingError("Error: Duplicate arg of function\n");
                }
                parse_primitive(func->set_args_[cur.first], cur.second);
                cur_args[cur.first] = std::move(cur.second);
              }
              for (const auto& it_arg : func->args_) {
                auto it = cur_args.find(it_arg.first);
                if (it == cur_args.end()) {
                  throw ParsingError("Error: Missing argument " + it_arg.first + "in function " + ans->name + "\n");
                }
                std::pair<std::string, std::unique_ptr<Basic_type>> bufe = {
                    it_arg.first,
                    std::move(cur_args[it_arg.first])
                };
                ans->args.emplace_back(std::move(bufe));
              }
              return ans;
            },

        args() >= [] { return argument_list{}; },

        args(arg_definition) >=
            [](name_n_value&& cur) {
              argument_list ans;
              ans.emplace_back(std::move(cur));
              return ans;
            },

        args(args, ',', arg_definition) >=
            [](argument_list&& from, ctpg::term_value<char>, name_n_value&& cur) {
              from.emplace_back(std::move(cur));
              return from;
            },

        arg_definition(ID, '=', num) >=
            [](auto&& cur_name, ctpg::term_value<char>, auto&& cur_number) {
              std::string cur_name_ = std::string(std::forward<decltype(cur_name)>(cur_name));
              std::string cur_number_ = std::string(std::forward<decltype(cur_number)>(cur_number));
              name_n_value result;
              result.first = cur_name_;
              auto ans = std::make_unique<Uint64>();
              ans->val_str = cur_number_;
              result.second = std::unique_ptr<Basic_type>(std::move(ans));
              return result;
            },

        arg_definition(ID, '=', str) >=
            [](auto&& cur_str, ctpg::term_value<char>, auto&& cur) {
              std::string buf = std::string(cur);
              return name_n_value{
                  std::string(std::forward<decltype(cur_str)>(cur_str)),
                  std::make_unique<String>(buf.substr(1, buf.size() - 2))
              };
            },

        arg_definition(ID, '=', '{', args, '}') >=
            [](auto&& cur_arg, ctpg::term_value<char>, ctpg::term_value<char>, auto&& fields_, ctpg::term_value<char>) {
              auto ans = preparse_struct(std::forward<decltype(fields_)>(fields_));
              return name_n_value{
                  std::string(std::forward<decltype(cur_arg)>(cur_arg)),
                  std::unique_ptr<Basic_type>(std::move(ans))
              };
            },

        arg_definition(ID, '=', ID, '{', args, '}') >=
            [](auto&& cur_arg,
               ctpg::term_value<char>,
               auto&& checker,
               ctpg::term_value<char>,
               auto&& fields_,
               ctpg::term_value<char>) {
              std::string cur_arg_ = std::string(std::forward<decltype(cur_arg)>(cur_arg));
              std::string checker_ = std::string(std::forward<decltype(checker)>(checker));
              auto ans = preparse_struct(std::forward<decltype(fields_)>(fields_));
              return name_n_value{cur_arg_, std::unique_ptr<Basic_type>(std::move(ans))};
            },

        fields() >= [] { return argument_list{}; },
        fields(arg_definition) >=
            [](auto&& cur) {
              argument_list ans;
              ans.emplace_back(std::forward<decltype(cur)>(cur));
              return ans;
            },
        fields(fields, ',', arg_definition) >=
            [](auto&& from, ctpg::term_value<char>, auto&& cur_field) {
              from.emplace_back(std::forward<decltype(cur_field)>(cur_field));
              return from;
            }
    )
);

QueryParser::QueryParser(Schema schema) {
  schema_ = (std::move(schema));
}

std::unique_ptr<FuncType> QueryParser::parse(std::string& query) {
  using namespace ctpg::buffers;
  using namespace ctpg::ftors;
  std::stringstream parsing_error;
  const char* buf = query.c_str();
  std::optional<std::unique_ptr<FuncType>> ans = query_parser.parse(string_buffer(buf), parsing_error);
  if (!ans.has_value()) {
    throw ParsingError("Error: Failed to parse query " + parsing_error.str() + "\n");
  }
  return std::move(ans.value());
}

} // namespace ct::schema::query_parser
