#include <damath-zero/core/mcts.h>
#include <httplib.h>

auto main(int argc, char** argv) -> int {
  auto srv = httplib::Server{};

  if (!srv.set_mount_point("/", "./"))
    return 1;

  if (!srv.listen("0.0.0.0", 80))
    return 1;

  return 0;
}