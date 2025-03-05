'use server'

import { redirect } from 'next/navigation'

export async function newGame(data: FormData) {
    const state = await rpc("new");
    redirect(`/tictactoe/${state.id}`);
};

export interface State {
    id: number;
    board: number[];
    player: number;
    result: number;
}

export const rpc = async (method: string, params: any = {}) => {
    const id: number = Math.floor(Math.random() * 1000);
    const res = await fetch(`http://localhost:8080/tictactoe`, {
        method: "POST",
        headers: {
            "Content-Type": "application/json",
        },
        body: JSON.stringify({
            "jsonrpc": "2.0",
            "method": method,
            "params": params,
            "id": Number(id),
        }),
    });

    const json = await res.json();
    console.log(json)
    if (json.error) throw new Error(json.error.message);

    const state: State = json.result;
    return state;
};