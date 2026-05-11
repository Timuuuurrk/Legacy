package game;

public class PrivacyBoard implements Position {
    private final Board cur;

    PrivacyBoard(Board ticTac) {
        cur = ticTac;
    }

    @Override
    public boolean isValid(final Move move, Cell need) {
        return cur.isValid(move, need);
    }

    @Override
    public String toString() {
        return cur.toString();
    }

}
