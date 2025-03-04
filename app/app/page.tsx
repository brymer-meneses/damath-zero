import Link from "next/link";

export default function Home() {
  return <div className="container mx-auto flex gap-4">
    <Link href="/tictactoe">TicTacToe</Link>
    <Link href="/damath">Damath</Link>
  </div>
}
