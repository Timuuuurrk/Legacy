#include "AutoCompleteParser.h"

namespace ct::schema::query_parser::autocomplete_parser {
using namespace token_parser;

std::string
try_complete_argument(std::string& cur, std::set<std::string>& range, TokenParser& my_parser, std::string sep) {
  if (range.empty()) {
    return "";
  }

  if (my_parser.eof()) {
    std::string ans = *range.begin();
    cur += ans;
    range.erase(ans);
    return ans;
  }

  std::string pref = my_parser.take_while(sep);
  if (!my_parser.eof()) {
    if (!range.contains(pref)) {
      throw AutoCompletionError("Error: unknown name of function or argument!\n");
    }
    range.erase(pref);
    return pref;
  }

  auto it = range.lower_bound(pref);

  if (it != range.end() && it->starts_with(pref)) {
    std::string ans = *it;
    cur += ans.substr(pref.size());
    range.erase(ans);
    return ans;
  }

  throw AutoCompletionError("Error: unable to autocomplete function or its argument!\n");
}

void try_to_process(char need, TokenParser& my_parser, std::string& cur) {
  if (my_parser.eof()) {
    cur += need;
  } else {
    my_parser.expect(need);
  }
}

bool autocomplete_structure(std::string_view name_, TokenParser& my_parser, Schema& schema, std::string& cur) {
  bool fl;
  if (my_parser.eof()) {
    // complete without writing structure name
    cur += "{";
  } else {
    my_parser.expect('{', fl);
    if (!fl) {
      std::set<std::string> buf;
      buf.insert(std::string(name_));
      std::string cur_name = try_complete_argument(cur, buf, my_parser, "{");
      try_to_process('{', my_parser, cur);
      if (cur_name != name_) {
        throw AutoCompletionError(
            "Error: Structure name " + cur_name + "doesn't match its name " + std::string(name_) + "\n"
        );
      }
    }
  }

  const Struct* result = schema.get_struct(std::string(name_));
  if (result == nullptr) {
    throw AutoCompletionError("Error: Unknown structure name: " + std::string(name_) + "\n");
  }
  std::unordered_map<std::string, std::string, Hash, Equal> fields_names_n_types = result->set_fields_;
  std::set<std::string> names;
  for (const auto& it : fields_names_n_types) {
    names.insert(it.first);
  }
  while (!names.empty()) {
    auto cur_field = try_complete_argument(cur, names, my_parser, "=");

    std::string type = fields_names_n_types[cur_field];
    try_to_process('=', my_parser, cur);
    fl = autocomplete_primitive(type, my_parser, schema, cur);
    if (my_parser.eof()) {
      if (names.empty()) {
        if (fl) {
          cur += "}";
        }
        break;
      } else {
        if (fl) {
          cur += ", ";
        } else {
          break;
        }
      }
    } else {
      if (my_parser.match(',')) {
        continue;
      } else {
        my_parser.expect('}');
        return true;
      }
    }
  }
  return fl;
}

bool autocomplete_primitive(std::string_view type, TokenParser& my_parser, Schema& schema, std::string& cur) {
  if (type == "string") {
    if (my_parser.eof()) {
      cur += "\"";
      return false;
    } else {
      my_parser.expect('\"');
    }
    my_parser.take_while("\"");
    if (my_parser.eof()) {
      return false;
    } else {
      my_parser.expect('\"');
    }
    return true;
  } else if (schema.exist_builtin_type(type)) {
    my_parser.take_while(",)}");
    return !my_parser.eof();
  } else {
    // structure
    return autocomplete_structure(type, my_parser, schema, cur);
  }
}

std::string autocompletion(std::string from, Schema& schema) {
  TokenParser my_parser(std::move(from));
  std::string ans;
  std::set<std::string> range_functions;
  for (const auto& y : schema.funcs) {
    range_functions.insert(y.first);
  }

  std::string cur_function = try_complete_argument(ans, range_functions, my_parser, "(");
  try_to_process('(', my_parser, ans);
  std::set<std::string> func_args;
  for (auto& y : schema.funcs[cur_function].args_) {
    func_args.insert(y.first);
  }
  while (!func_args.empty()) {
    auto cur_arg = try_complete_argument(ans, func_args, my_parser, "=");

    std::string type = schema.funcs[cur_function].set_args_[cur_arg];
    try_to_process('=', my_parser, ans);
    bool fl = autocomplete_primitive(type, my_parser, schema, ans);
    if (my_parser.eof()) {
      if (!func_args.empty()) {
        if (fl) {
          ans += ", ";
          continue;
        }
      } else {
        if (fl) {
          ans += ")";
        }
      }
    } else {
      if (my_parser.match(',')) {
        continue;
      } else {
        my_parser.expect(')');
      }
    }
    break;
  }
  return ans;
}

/*                        AUTOCOMPLETION RULES
 The parser's logic can be deduced by analyzing typical cases of auto-completion.
 I will analyze cases for query getCarsQuantity(name="Dodge", from=Car{id=42, price=100})
 Possible cases:
 *   1. getCa -> getCarId(id=                             [complete prefix and before first argument]
 *   2. getCarsQuantity -> getCarsQuantity(from={id=      [complete before first argument sorted by alphabet]
 *   3. getCarsQuantity(n  -> getCarsQuantity(name="      [complete name of first argument]
 *   4. getCarsQuantity(name -> getCarsQuantity(name="    [complete equal sign]
 *   5. getCarsQuantity(name= -> getCarsQuantity(name="   [complete is empty because we can't predict user's input]

 *   6. getCarsQuantity(name="Dodge ->
        getCarsQuantity(name="Dodge                       [complete is empty because we can't predict user's input]

 *   7. getCarsQuantity(name="Dodge" ->
        getCarsQuantity(name="Dodge", from={id=           [complete second argument with open bracket]

 *   8. getCarsQuantity(name="Dodge", from= ->
        getCarsQuantity(name="Dodge", from={id=           [complete second argument with open bracket]

 *   9. getCarsQuantity(name="Dodge", from={id=42, p ->
        getCarsQuantity(name="Dodge", from={id=42, price= [complete second argument of structure]

 *   10. getCarsQuantity(name="Dodge", from={id=42, price=100} ->
         getCarsQuantity(name="Dodge", from={id=42, price=100}) [complete close brackets]

SPECIAL CASES:
      1. getCarsQuantity(name="Dodge", from=Ca ->
         getCarsQuantity(name="Dodge", from=Car{id=       [complete prefix of structure]

      2. getCarsQuantity(name= ->
         getCarsQuantity(name="                           [complete quote of string]

      3. getCarsQuantity(from={id=42, price=100} ->
         getCarsQuantity(from={id=42, price=100}, name="  [complete argument after structure]
*/

} // namespace ct::schema::query_parser::autocomplete_parser
