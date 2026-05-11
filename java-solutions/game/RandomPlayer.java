package game;

import java.util.Random;


public class RandomPlayer implements Player {
    private final Random random;
    private final int n;
    private final int m;

    public RandomPlayer(final Random random, final int mainN, final int mainM, final boolean mainSquare) {
        this.random = random;
        if (mainSquare) {
            //isSquare
            n = 2 * mainM - 1;
            m = 2 * mainM - 1;
        } else {
            n = mainN;
            m = mainM;
        }
    }

    public RandomPlayer(final int mainN, final int mainM, boolean mainSquare) {
        this(new Random(), mainN, mainM, mainSquare);
    }

    @Override
    public Move move(final Position position, final Cell cell, boolean Draw) {
        while (true) {
            if (Draw && random.nextInt(2) == 1) {
                return new Move(-1, -1, cell);
            }
            int r, c;
            r = random.nextInt(n);
            c = random.nextInt(m);
            final Move move = new Move(r, c, cell);
            if (position.isValid(move, Cell.E)) {
                return move;
            }
        }
    }
}
