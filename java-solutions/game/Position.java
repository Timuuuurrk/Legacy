package game;

public interface Position {
    boolean isValid(Move move, Cell need);

    String toString();

}
