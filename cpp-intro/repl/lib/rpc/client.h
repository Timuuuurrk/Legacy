#pragma once

#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>
#include <stdexcept>
#include <vector>

namespace ct::rpc {

class Exception : public std::runtime_error {
public:
  explicit Exception(const std::string& what);
};

class Client {
public:
  using Response = std::vector<std::byte>;
  using Request = std::span<const std::byte>;
  using Callback = std::function<Response(Request)>;

  Client(std::string host, std::uint16_t port, std::string path);

  template <typename Cbk>
    requires std::convertible_to<Cbk, Callback>
  explicit Client(Cbk&& cbk)
      : client(std::forward<Cbk>(cbk)) {}

  Response send(Request request);

private:
  Callback client;
};

} // namespace ct::rpc
