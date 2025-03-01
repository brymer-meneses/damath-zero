import { Board } from "./board";

export default function Page() {
  const board = new Board();

  const operationToSymbol = {
    ["multiply"]: "×",
    ["minus"]: "—",
    ["add"]: "+",
    ["divide"]: "÷",
  };

  return (
    <main className="flex flex-col justify-center items-center p-10">
      <div className="grid grid-cols-8 w-full max-w-[50%] border-gray-200 border-2 rounded-lg overflow-hidden">
        {board.cells.map((cell, i) => {
          if (!cell) {
            return (
              <div
                className="aspect-square bg-gray-300 justify-center"
                key={i}
              ></div>
            );
          } else {
            return (
              <div
                className="aspect-square flex items-center justify-center text-4xl font-bold"
                key={i}
              >
                {operationToSymbol[cell.operation]}
              </div>
            );
          }
        })}
      </div>
    </main>
  );
}
