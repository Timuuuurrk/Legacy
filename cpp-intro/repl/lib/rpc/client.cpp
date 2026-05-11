#include "client.h"

#include <httplib.h>

#include <chrono>
#include <sstream>
#include <utility>

ct::rpc::Exception::Exception(const std::string& what)
    : std::runtime_error(what) {}

ct::rpc::Client::Client(std::string host, std::uint16_t port, std::string path)
    : client([host_ = std::move(host), port, path_ = std::move(path)](Request request) -> Response {
      httplib::Client cli(host_, port);
      cli.set_connection_timeout(std::chrono::seconds(1));
      if (auto res = cli.Post(
              path_,
              reinterpret_cast<const char*>(request.data()),
              request.size(),
              "application/octet-stream"
          )) {
        if (res->status == httplib::StatusCode::OK_200) {
          auto str = res->body;
          std::vector<std::byte> bytes(
              reinterpret_cast<const std::byte*>(str.data()),
              reinterpret_cast<const std::byte*>(str.data()) + str.size()
          );
          return bytes;
        } else if (res->status == httplib::StatusCode::BadRequest_400) {
          throw Exception("RPC error: " + res->body);
        } else {
          std::stringstream ss;
          ss << "Unexpected server answer detected (code: " << res->status << ").  Please report to the course staff.";
          throw Exception(std::move(ss).str());
        }
      } else {
        throw Exception("RPC response was not received");
      }
    }) {}

ct::rpc::Client::Response ct::rpc::Client::send(ct::rpc::Client::Request request) {
  return client(request);
}
