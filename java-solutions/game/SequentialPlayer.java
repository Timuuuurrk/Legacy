package game;

public class SequentialPlayer implements Player {
    private final int n;
    private final int m;

    public SequentialPlayer(final int mainN, final int mainM, final boolean mainSquare) {
        if (mainSquare) {
            //isSquare
            n = 2 * mainN - 1;
            m = 2 * mainM - 1;
        } else {
            n = mainN;
            m = mainM;
        }
    }

    @Override
    public Move move(final Position position, final Cell cell, boolean draw) {
        for (int r = 0; r < n; r++) {
            for (int c = 0; c < m; c++) {
                final Move move = new Move(r, c, cell);
                if (position.isValid(move, Cell.E)) {
                    return move;
                }
            }
        }
        throw new IllegalStateException("No valid moves");
    }
}
