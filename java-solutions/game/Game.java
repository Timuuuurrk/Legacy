package game;

import java.util.InputMismatchException;

public class Game {
    private final boolean log;
    private final Player player1;
    private final Player player2;
    private final int nu1;
    private final int nu2;
    private boolean tryDraw;

    public Game(final boolean log, final Player player1, final Player player2, final int num1, final int num2) {
        this.log = log;
        this.player1 = player1;
        this.player2 = player2;
        this.tryDraw = false;
        this.nu1 = num1;
        this.nu2 = num2;
    }

    public int play(Board board) {
        while (true) {
            try {
                final int result1 = move(board, player1, 1);
                if (result1 != -1) {
                    return result1;
                }
                final int result2 = move(board, player2, 2);
                if (result2 != -1) {
                    return result2;
                }
            } catch (NumberFormatException | InputMismatchException e) {
                System.out.println("Invalid format of move, try again." + e.getMessage() + System.lineSeparator());
            }
        }
    }

    private int move(final Board board, final Player player, final int no) throws InputMismatchException, NumberFormatException {
        final Move move = player.move(board.getPosition(), board.getCell(), tryDraw);
        final Result result;
        int num = nu1;
        if (no == 2) {
            num = nu2;
        }
        if (move.isLose()) {
            board.makeMove(move);
            System.out.println("Player " + num + " decided to lose.");
            result = Result.LOSE;
        } else if (tryDraw && !move.isSpecial()) {
            board.makeMove(new Move(-1, -1, Cell.E));
            System.out.println("Ok, " + "Player " + num + " don't agree for the tie.");
            return -1;
        } else if (move.isSpecial()) {
            board.makeMove(move);
            if (tryDraw) {
                System.out.println("Verdict of game: tie with decision.");
                result = Result.DRAW;
            } else {
                System.out.println("Player " + num + " wants a tie. If you agree, type 'I agree for the tie'.");
                tryDraw = true;
                return -1;
            }
        } else {
            result = board.makeMove(move);
        }
        tryDraw = false;
        log("Player " + num + " move: " + move);
        log("Position:" + System.lineSeparator() + board);
        if (result == Result.WIN) {
            log("Player " + num + " won");
            return no;
        } else if (result == Result.LOSE) {
            log("Player " + num + " lose");
            return 3 - no;
        } else if (result == Result.DRAW) {
            log("Draw");
            return 0;
        } else {
            return -1;
        }
    }

    private void log(final String message) {
        if (log) {
            System.out.println(message);
        }
    }
}
