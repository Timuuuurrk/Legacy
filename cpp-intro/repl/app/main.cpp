#include "teammates.h"

ct::schema::Schema my_schema;

signed main(int argc, char** argv) {
  using namespace CLI;
  App app{"REPL app"};

  std::string path_to_schema;
  std::string rpc_host = "127.0.0.1";
  std::string rpc_path;
  int32_t rpc_port = 8080;
  bool no_tty = false;
  setup_cli(app, path_to_schema, rpc_path, rpc_host, rpc_port, no_tty);
  try {
    app.parse(argc, argv);
  } catch (CLI::ParseError& pars_error) {
    std::cerr << "Error: " << pars_error.what() << '\n';
    return 1;
  }

  std::string file_text;
  try {
    file_text = read_file_helper(path_to_schema);
  } catch (SyntaxError& e) {
    std::cerr << e.what();
    return 1;
  }

  const char* buf = file_text.c_str();
  try {
    my_schema = ct::schema::schema_parser::SchemaParser::parse(buf);
  } catch (SyntaxError& e) {
    std::cerr << e.what() << "\n";
    return 1;
  }

  ct::rpc::Client rpc_client(rpc_host, rpc_port, rpc_path);
  std::string line;
  replxx::Replxx my_client;
  const char* history_file = ".repl_history";
  if (!no_tty) {
    setup_client(my_client, my_schema, history_file);
  }

  while (true) {
    // std::cout << ">> " << std::flush;
    if (!no_tty) {
      const char* input = my_client.input("");
      line = input;
      if (line == "exit") {
        std::cout << "Goodbye!\n";
        break;
      }
    } else {
      if (!std::getline(std::cin, line)) {
        std::cout << "Goodbye!\n";
        break;
      }
    }

    if (line.empty()) {
      continue;
    }

    std::unique_ptr<ct::schema::types_::FuncType> parsed_query;
    std::string return_type;

    try {
      return_type = parse_query(my_client, my_schema, parsed_query, line, no_tty);
    } catch (const ParsingError& e) {
      std::cout << "Error: parsing error" << '\n';
      continue;
    } catch (const std::out_of_range& e) {
      std::cout << "Error: overflow error" << '\n';
      continue;
    }

    std::vector<std::byte> serialized_query;
    parsed_query->serialize(serialized_query);
    try {
      std::vector<std::byte> response = rpc_client.send(serialized_query);
      std::string ans_query = deserialize_helper(response, my_schema, return_type);
      std::cout << ans_query << "\n";
    } catch (std::runtime_error& e) {
      std::string a = "Error: ";
      std::cout << a + e.what() << '\n';
    }
  }

  if (!no_tty) {
    my_client.history_save(history_file);
  }
}
