"use client";

import { useEffect, useState } from "react";
import { rpc, State } from "@/lib/rpc";
import { NewGame } from "@/components/new-game";

export default function Board({ id }: { id: number }) {
  const [state, setState] = useState<State>();

  useEffect(() => {
    rpc.on("get", setState);
    rpc.on("move", setState);

    rpc.send("get", { id });
  }, []);

  if (!state) return <div>Loading...</div>;

  const move = (cell: number) => {
    rpc.send("move", { id: state.id, cell });
  };

  return (
    <main className="flex flex-col justify-center items-center p-10 gap-4">
      {state.result === -2 && (
        <div>Player {state.player === "first" ? "X" : "O"} turn...</div>
      )}
      {state.result === 0 && <div>Draw!</div>}
      {state.result === 1 && <div>X wins!</div>}
      {state.result === -1 && <div>O wins!</div>}
      <div className="grid grid-cols-3 w-full max-w-[50%] border-gray-200 border-2 rounded-lg overflow-hidden">
        {state.board?.map((cell, i) => {
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
                {cell === 1 ? "X" : "O"}
              </div>
            );
          }
        })}
      </div>
      <NewGame route="tictactoe" />
    </main>
  );
}
