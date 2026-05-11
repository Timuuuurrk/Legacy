package game;

public interface Board {
    Position getPosition();

    Cell getCell();

    Result makeMove(Move move);

    boolean isValid(Move move, Cell need);

    Cell getCell(final int r, final int c);
}
