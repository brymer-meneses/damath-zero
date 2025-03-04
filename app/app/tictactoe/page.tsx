"use client";

import { useEffect, useState } from "react";
import { State, rpc } from "./rpc";


export default function Page() {
    const [state, setState] = useState<State>();

    useEffect(() => { rpc("new").then(setState) }, []);

    const move = (cell: number) => {
        rpc("move", { id: state?.id, cell }).then(setState);
    };

    const reset = () => {
        rpc("new").then(setState);
    };

    if (!state) return <div>Loading...</div>;

    console.log(state);

    return (
        <main className="flex flex-col justify-center items-center p-10">
            {state.winner !== undefined && (
                <div className="mb-4">
                    {state.winner === 0 ? "Draw" : `Player ${state.winner} wins`}
                </div>
            )}
            <div className="grid grid-cols-3 w-full max-w-[50%] border-gray-200 border-2 rounded-lg overflow-hidden">
                {state.board?.map((cell, i) => {
                    console.log(cell)
                    if (cell === 0) {
                        return (
                            <div
                                className="aspect-square bg-gray-300 justify-center"
                                key={i}
                                onClick={() => move(i)}
                            ></div>
                        );
                    } else {
                        return (
                            <div
                                className="aspect-square flex items-center justify-center text-4xl font-bold"
                                key={i}
                            >
                                {cell}
                            </div>
                        );
                    }
                })}
            </div>

            <button
                className="mt-4 bg-blue-500 text-white px-4 py-2 rounded-lg"
                onClick={reset}
            >
                Reset
            </button>
        </main>
    );
}
