import { NewGame } from "./new-game";

export default async function Page() {
    return (
        <main className="flex flex-col justify-center items-center p-10">
            <div className="mb-4">Tic-Tac-Toe</div>
            <NewGame />
        </main>
    );
}
