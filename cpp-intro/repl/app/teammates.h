#pragma once

#include "Schema/autocompletion/AutoCompleteParser.h"
#include "Schema/query_parser/TokenParser.h"
#include "Schema/query_parser/deserializer.h"
#include "Schema/query_parser/query_parser.h"
#include "Schema/schema_preparation/SyntaxError.h"
#include "Schema/schema_preparation/scheme_parser.h"
#include "rpc/client.h"

#include <CLI/CLI.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <replxx.hxx>
#include <string>
#include <string_view>

using replxx::Replxx;

std::string read_file_helper(std::string_view file_path);

std::string deserialize_helper(const std::vector<std::byte>& response, ct::schema::Schema& my_schema, std::string_view);

void autocomplete_helper(replxx::Replxx& my_client, ct::schema::Schema& my_schema, char32_t);

void setup_cli(
    CLI::App& app,
    std::string& path_to_schema,
    std::string& rpc_path,
    std::string& rpc_host,
    int32_t& rpc_port,
    bool& no_tty
);

void setup_client(replxx::Replxx& my_client, ct::schema::Schema& my_schema, const char* history_file);

std::string parse_query(
    replxx::Replxx& my_client,
    ct::schema::Schema& my_schema,
    std::unique_ptr<ct::schema::types_::FuncType>& parsed_query,
    std::string& line,
    bool no_tty
);
