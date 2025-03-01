interface PlaceableCell {
  operation: "add" | "multiply" | "divide" | "minus";
  chip: Chip | null;
}

interface Chip {
  value: number;
  isKing: boolean;
}

type Cell = PlaceableCell | null;

export class Board {
  public cells: Cell[];

  constructor() {
    this.cells = Array.from({ length: 64 }, () => null);

    this.set(0, 0, { operation: "multiply", chip: null });
    this.set(2, 0, { operation: "divide", chip: null });
    this.set(4, 0, { operation: "minus", chip: null });
    this.set(6, 0, { operation: "add", chip: null });

    this.set(1, 1, { operation: "divide", chip: null });
    this.set(3, 1, { operation: "multiply", chip: null });
    this.set(5, 1, { operation: "add", chip: null });
    this.set(7, 1, { operation: "minus", chip: null });

    this.set(0, 2, { operation: "minus", chip: null });
    this.set(2, 2, { operation: "add", chip: null });
    this.set(4, 2, { operation: "multiply", chip: null });
    this.set(6, 2, { operation: "divide", chip: null });

    this.set(1, 3, { operation: "add", chip: null });
    this.set(3, 3, { operation: "minus", chip: null });
    this.set(5, 3, { operation: "divide", chip: null });
    this.set(7, 3, { operation: "multiply", chip: null });

    this.set(0, 4, { operation: "multiply", chip: null });
    this.set(2, 4, { operation: "divide", chip: null });
    this.set(4, 4, { operation: "minus", chip: null });
    this.set(6, 4, { operation: "add", chip: null });

    this.set(1, 5, { operation: "divide", chip: null });
    this.set(3, 5, { operation: "multiply", chip: null });
    this.set(5, 5, { operation: "add", chip: null });
    this.set(7, 5, { operation: "minus", chip: null });

    this.set(0, 6, { operation: "minus", chip: null });
    this.set(2, 6, { operation: "add", chip: null });
    this.set(4, 6, { operation: "multiply", chip: null });
    this.set(6, 6, { operation: "divide", chip: null });

    this.set(1, 7, { operation: "add", chip: null });
    this.set(3, 7, { operation: "minus", chip: null });
    this.set(5, 7, { operation: "divide", chip: null });
    this.set(7, 7, { operation: "multiply", chip: null });
  }

  public set(posX: number, posY: number, cell: PlaceableCell) {
    this.cells[(posX % 8) + posY * 8] = cell;
  }
}
