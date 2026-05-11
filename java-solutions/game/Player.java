package game;

import java.util.InputMismatchException;

public interface Player {
    String TIE_QUERY = "I want tie";
    String TIE_AGREE = "I agree for the tie";
    String LOSE = "I want to kill myself";

    Move move(Position position, Cell cell, boolean draw);
}
