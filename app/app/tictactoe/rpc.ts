'use server';

const id: number = Math.floor(Math.random() * 1000);

export interface State {
    board: (number | null)[];
    to_move: number;
    winner: number;
}

export const rpc = async (method: string, params: any = {}) => {
    return fetch(`http://localhost:8080/${method}`, {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify({
            "jsonrpc": "2.0",
            "method": method,
            "params": params,
            "id": id,
        }),
    }).then((res) => {
        return res.json().then((json) => {
            if (json.error)
                throw new Error(json.error.message);

            const state: State = json.result;
            return state;
        });
    });
}