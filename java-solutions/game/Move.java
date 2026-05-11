package game;


public final class Move {
    private final int row;
    private final int column;
    private final Cell value;

    public Move(final int row, final int column, final Cell value) {
        this.row = row;
        this.column = column;
        this.value = value;
    }

    public int getRow() {
        return row;
    }

    public int getColumn() {
        return column;
    }

    public Cell getValue() {
        return value;
    }

    public boolean isSpecial() {
        return (row == -2 && column == -2) || (row == -1 && column == -1);
    }

    public boolean isLose() {
        return row == -3 && column == -3;
    }

    @Override
    public String toString() {
        return "row=" + row + ", column=" + column + ", value=" + value;
    }
}
