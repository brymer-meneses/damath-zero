'use client'

import { newGame } from './actions'

export function NewGame() {
    return (
        <form action={newGame}>
            <button className="bg-blue-500 text-white px-4 py-2 rounded-lg">
                New Game
            </button>
        </form>
    )
}