#pragma once

#include <concepts>
#include <glaze/ext/jsonrpc.hpp>
#include <glaze/glaze.hpp>
#include <glaze/util/expected.hpp>
#include <glaze/util/string_literal.hpp>
#include <memory>
#include <string>
#include <type_traits>

namespace DamathZero::Rpc {

template <typename H, typename Context>
concept HandlerType = requires(std::shared_ptr<Context> context,
                               const H::Request& req, const H::Response& res) {
  typename H::Request;
  typename H::Response;
  std::is_convertible_v<decltype(H::method), std::string_view>;
  {
    H::handle(context, req)
  } -> std::same_as<glz::expected<typename H::Response, glz::rpc::error>>;
};

template <typename Context, HandlerType<Context>... Handlers>
class Rpc {
 public:
  constexpr Rpc(Context&& context)
      : context_(std::make_shared<Context>(std::move(context))) {
    (register_handler<Handlers>(), ...);
  }

  constexpr auto call(std::string_view data) -> std::string {
    return server_.call(data);
  }

 private:
  template <typename H>
  constexpr auto register_handler() -> void {
    server_.template on<H::method>(
        [this](const typename H::Request& req)
            -> glz::expected<typename H::Response, glz::rpc::error> {
          return H::handle(context_, req);
        });
  }

  glz::rpc::server<
      glz::rpc::method<Handlers::method, typename Handlers::Request,
                       typename Handlers::Response>...>
      server_;
  std::shared_ptr<Context> context_ = {};
};

};  // namespace DamathZero::Rpc
