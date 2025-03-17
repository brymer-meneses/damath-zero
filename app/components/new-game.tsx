"use client";

import { rpc } from "@/lib/rpc";
import { redirect } from "next/navigation";
import { useEffect } from "react";

export function NewGame({ route }: { route: string }) {
  useEffect(() => {
    rpc.on("new", (state) => {
      redirect(`/${route}/${state.id}`);
    });
  }, []);

  const submit = (data: FormData) => {
    rpc.send("new", {});
  };

  return (
    <form action={submit}>
      <button className="bg-blue-500 text-white px-4 py-2 rounded-lg">
        New Game
      </button>
    </form>
  );
}
