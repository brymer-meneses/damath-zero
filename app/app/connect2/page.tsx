import { NewGame } from "@/components/new-game";

export default async function Page() {
  return (
    <main className="flex flex-col justify-center items-center p-10">
      <div className="mb-4">Connect2</div>
      <NewGame route="connect2" />
    </main>
  );
}
