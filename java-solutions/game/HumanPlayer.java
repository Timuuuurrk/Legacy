package game;

import java.io.PrintStream;
import java.util.InputMismatchException;
import java.util.Scanner;

public class HumanPlayer implements Player {
    private final PrintStream out;
    private final Scanner in;
    private final int n;
    private final boolean isSquare;

    public HumanPlayer(final PrintStream out, final Scanner in, final int mainN, final boolean mainSquare) {
        this.out = out;
        this.in = in;
        n = mainN;
        isSquare = mainSquare;
    }

    public HumanPlayer(final int mainN, final boolean mainSquare) {
        this(System.out, new Scanner(System.in), mainN, mainSquare);
    }

    @Override
    public Move move(final Position position, final Cell cell, boolean draw) throws InputMismatchException, NumberFormatException {
        while (true) {
            out.println("Position");
            out.println(position);
            out.println(cell + "'s move");
            out.println("Enter row and column");
            int row, column;
            if (in.hasNextInt()) {
                row = in.nextInt();
                column = in.nextInt();
            } else {
                String line = "";
                while (line.isEmpty()) {
                    line = in.nextLine();
                }
                if (line.equals(TIE_QUERY)) {
                    if (draw) {
                        return new Move(-3, -3, cell);
                    }
                    return new Move(-2, -2, cell);
                } else if (line.equals(TIE_AGREE) && draw) {
                    return new Move(-1, -1, cell);
                } else if (line.equals(LOSE)) {
                    return new Move(-3, -3, cell);
                } else {
                    out.println("Move " + line + " is invalid");
                    continue;
                }
            }
            int cx = column;
            if (isSquare) {
                if (row <= n - 1) {
                    column = (n - 1 - row) + column;
                } else {
                    column = row - (n - 1) + column;
                }
            }
            final Move move = new Move(row, column, cell);
            if (position.isValid(move, Cell.E)) {
                return move;
            }
            out.println("Move " + new Move(row, cx, Cell.E) + " is invalid");
        }
    }
}
