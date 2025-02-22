from lightbug_http import *
from lightbug_http.io.bytes import Bytes


@value
struct DamathZero(HTTPService):
    fn func(mut self, req: HTTPRequest) raises -> HTTPResponse:
        if req.uri.path == "/":
            with open("static/index.html", "r") as f:
                return OK(Bytes(f.read_bytes()), "text/html; charset=utf-8")

        return NotFound(req.uri.path)


fn main() raises:
    var server = Server()
    var handler = DamathZero()
    server.listen_and_serve("localhost:8080", handler)
