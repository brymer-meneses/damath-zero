export interface State {
  id: number;
  board: number[];
  player: number;
  result: number;
}

type Callback = (state: any) => void;

const ws = new WebSocket("ws://localhost:8080");

class RPC {
  private methods: Map<string, Callback>;
  private messages: Map<number, string>;

  constructor() {
    this.methods = new Map();
    this.messages = new Map();

    ws.onmessage = (event) => {
      const response = JSON.parse(event.data);

      const method = this.messages.get(response.id);
      if (!method) {
        console.error(`No method for id: ${response.id}`);
        return;
      }

      const cb = this.methods.get(method);
      if (!cb) {
        console.error(`No callback for method: ${method}`);
        return;
      }
      cb(response.result);
    };
  }

  async on(method: string, cb: Callback) {
    this.methods.set(method, cb);
  }

  async send(method: string, params: any) {
    const id = Math.floor(Math.random() * 1000000);
    this.messages.set(id, method);
    ws.send(
      JSON.stringify({
        jsonrpc: "2.0",
        method,
        params,
        id: Number(id),
      }),
    );
  }
}

export const rpc = new RPC();
