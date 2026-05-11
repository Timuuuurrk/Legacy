
#pragma once

#include "AutoCompletionError.h"
#include "SCHEMA.h"
#include "TokenParser.h"

#include <set>
#include <string>

namespace ct::schema::query_parser::autocomplete_parser {
using namespace token_parser;

std::string
try_complete_argument(std::string& cur, std::set<std::string>& range, TokenParser& my_parser, std::string sep);

void try_to_process(char need, TokenParser& my_parser, std::string& cur);

bool autocomplete_structure(std::string_view name_, TokenParser& my_parser, Schema& schema, std::string& cur);

bool autocomplete_primitive(std::string_view type, TokenParser& my_parser, Schema& schema, std::string& cur);

std::string autocompletion(std::string from, Schema& schema);

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
