#include "teammates.h"

std::string read_file_helper(std::string_view file_path) {
  std::string ans;
  std::filesystem::path input_path = {file_path};
  input_path = std::filesystem::absolute(input_path);
  std::ifstream fin(input_path, std::ios::binary);
  if (!fin) {
    std::string s = "Error: ";
    s += file_path;
    throw SyntaxError(s);
  }
  std::ostringstream sstream;
  sstream << fin.rdbuf();
  ans = sstream.str();
  return ans;
}

std::string deserialize_helper(
    const std::vector<std::byte>& response,
    ct::schema::Schema& my_schema,
    std::string_view return_type
) {
  std::size_t ind = 0;
  auto res = ct::schema::deserializer::deserialize_primitive(response, return_type, my_schema, ind);
  std::string ans;
  res->str(ans);
  return ans;
}

void autocomplete_helper(replxx::Replxx& my_client, ct::schema::Schema& my_schema, char32_t) {
  auto buf = my_client.get_state();
  std::string from = buf.text();
  try {
    from += ct::schema::query_parser::autocomplete_parser::autocompletion(from, my_schema);
  } catch (AutoCompletionError& e) {
    std::cout << "Error: failed to autocomplete!\n";
    return;
  } catch (ParsingError& p) {
    // In case when TokenParser throws exception
    std::cout << "Error: failed to autocomplete!\n";
    return;
  }
  const char* ans = from.c_str();
  my_client.set_state({ans, static_cast<int>(from.size())});
}

void setup_cli(
    CLI::App& app,
    std::string& path_to_schema,
    std::string& rpc_path,
    std::string& rpc_host,
    int32_t& rpc_port,
    bool& no_tty
) {
  app.add_option("--schema", path_to_schema, "Path to file which includes schema")
      ->required()
      ->check(CLI::ExistingFile);
  app.add_flag("--no-tty", no_tty, "Turn off interactive mode");
  app.add_option("--rpc-host", rpc_host, "RPC host(by default is set to: 127.0.0.1");
  app.add_option("--rpc-port", rpc_port, "RPC host (by default is 8080")->check(CLI::Range(1, 65535));
  app.add_option("--rpc-path", rpc_path, "RPC path (by default is empty)");
}

void setup_client(replxx::Replxx& my_client, ct::schema::Schema& my_schema, const char* history_file) {
  my_client.history_load(history_file);
  my_client.bind_key(replxx::Replxx::KEY::TAB, [&](char32_t cur) -> Replxx::ACTION_RESULT {
    autocomplete_helper(my_client, my_schema, cur);
    return replxx::Replxx::ACTION_RESULT::CONTINUE;
  });
}

std::string parse_query(
    replxx::Replxx& my_client,
    ct::schema::Schema& my_schema,
    std::unique_ptr<ct::schema::types_::FuncType>& parsed_query,
    std::string& line,
    bool no_tty
) {
  if (!no_tty) {
    my_client.history_add(line);
  }
  ct::schema::query_parser::QueryParser query_parser(my_schema);
  parsed_query = query_parser.parse(line);
  return my_schema.get_func(parsed_query->name)->return_type_;
}
