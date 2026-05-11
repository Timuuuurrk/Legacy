package game;

import java.util.Arrays;
import java.util.Map;

public class TicTacToeBoard implements Board {
    private final int n;
    private final int m;
    private final int k;
    private int su = 0;
    private final boolean isSquare;
    private static final Map<Cell, Character> SYMBOLS = Map.of(
            Cell.X, 'X',
            Cell.O, 'O',
            Cell.E, '.'
    );

    private final Cell[][] cells;
    private Cell turn;

    public TicTacToeBoard(final int mainN, final int mainM, final int mainK, final boolean mainSquare) {
        k = mainK;
        isSquare = mainSquare;
        if (isSquare) {
            //isSquare
            n = 2 * mainN - 1;
            m = 2 * mainM - 1;
        } else {
            n = mainN;
            m = mainM;
        }
        this.cells = new Cell[n][m];
        for (Cell[] row : cells) {
            Arrays.fill(row, Cell.E);
        }
        turn = Cell.X;
    }

    public Position getPosition() {
        return new PrivacyBoard(this);
    }

    @Override
    public Cell getCell() {
        return turn;
    }

    @Override
    public Result makeMove(final Move move) {
        if (!move.isLose() && !move.isSpecial()) {
            if (!isValid(move, Cell.E)) {
                return Result.LOSE;
            }
            cells[move.getRow()][move.getColumn()] = move.getValue();
            su++;
            int rowFilled = 1;
            int colFilled = 1;
            int diag1Filled = 1;
            int diag2Filled = 1;
            rowFilled += maxShift(move, 2) + maxShift(move, 6);
            colFilled += maxShift(move, 0) + maxShift(move, 4);
            diag1Filled += maxShift(move, 1) + maxShift(move, 5);
            diag2Filled += maxShift(move, 3) + maxShift(move, 7);
            if (rowFilled >= k || colFilled >= k || diag1Filled >= k || diag2Filled >= k) {
                return Result.WIN;
            }
            if (su == overall()) {
                return Result.DRAW;
            }
        }
        turn = turn == Cell.X ? Cell.O : Cell.X;
        return Result.UNKNOWN;
    }

    private int overall() {
        if (isSquare) {
            return n * (2 + 2 * (n - 1)) - (1 + 2 * (n - 1));//n*(2a1+d(n-1))/2;
        }
        return n * m;
    }

    private int maxShift(final Move move, int direction) {
        int[] dy = new int[]{-1, -1, 0, 1, 1, 1, 0, -1};
        int[] dx = new int[]{0, -1, -1, -1, 0, 1, 1, 1};
        int curx = move.getColumn() + dx[direction];
        int cury = move.getRow() + dy[direction];
        int ans = 0;
        while (ans < k && isValid(new Move(cury, curx, move.getValue()), move.getValue())) {
            ans++;
            curx += dx[direction];
            cury += dy[direction];
        }
        return ans;
    }

    public boolean isValid(final Move move, Cell need) {
        return 0 <= move.getRow() && move.getRow() < n
                && 0 <= move.getColumn() && move.getColumn() < m
                && (!isSquare || inSquare(move))
                && (need == Cell.S || cells[move.getRow()][move.getColumn()] == need)
                && turn == getCell();
    }

    public boolean inSquare(final Move move) {
        int y = move.getRow();
        int x = move.getColumn();
        int realN = (n + 1) / 2;
        return y >= (realN - 1) - x && y >= x - (realN - 1) && y <= x + (realN - 1) && y <= 3 * (realN - 1) - x;
    }

    @Override
    public Cell getCell(final int r, final int c) {
        return cells[r][c];
    }

    @Override
    public String toString() {
        final StringBuilder sb = new StringBuilder("    ");
        for (int i = 0; i < m; ++i) {
            sb.append(i).append(" ");
        }
        for (int r = 0; r < n; r++) {
            sb.append(System.lineSeparator());
            StringBuilder es = new StringBuilder(Integer.toString(r));
            while (es.length() < 4) {
                es.append(" ");
            }
            sb.append(es);
            for (int c = 0; c < m; c++) {
                if (isValid(new Move(r, c, Cell.E), Cell.S)) {
                    sb.append(SYMBOLS.get(getCell(r, c)));
                } else {
                    sb.append(" ");
                }
                sb.append(" ");
            }
        }
        return sb.toString();
    }
}
