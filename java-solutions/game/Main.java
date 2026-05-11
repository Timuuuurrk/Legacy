package game;

import java.util.Scanner;
import java.util.*;

import static java.util.Collections.shuffle;

public class Main {
    private static int n, m, k, all;
    private static boolean isSquare = false;

    public static int playGame(Game game, InfoPlayer first, InfoPlayer second) {
        System.out.println("Playing game: " + "Player " + first.num + "  vs  " + "Player " + second.num);
        int result;
        do {
            result = game.play(new TicTacToeBoard(n, m, k, isSquare));
            System.out.println("Game result: " + result);
        } while (result != 1 && result != 2);
        return result;
    }

    public static ArrayList<InfoPlayer> tournamentNet(ArrayList<InfoPlayer> init, boolean isDown, boolean leftUp) {
        int ind = 0;
        ArrayList<InfoPlayer> repl = new ArrayList<>();
        shuffle(init);    //random choice
        boolean fl = !leftUp || !isDown || init.size() != 2;
        for (; ind + 1 < init.size() && fl; ind += 2) {
            Random rnd = new Random();
            int rn = rnd.nextInt(2);
            Game game = new Game(false, init.get(ind + rn).head, init.get(ind + 1 - rn).head, init.get(ind + rn).num, init.get(ind + 1 - rn).num);
            int result = playGame(game, init.get(ind + rn), init.get(ind + 1 - rn));
            int numb;
            numb = init.get(ind + rn).num;
            if (result == 2) {
                numb = init.get(ind + 1 - rn).num;
            }
            System.out.println("Won Player " + numb);
            if (result == 1) {
                repl.add(init.get(ind + rn));
            } else {
                repl.add(init.get(ind + 1 - rn));
            }
        }
        if (ind + 1 == init.size()) {
            repl.add(init.get(ind));
        }
        return repl;
    }

    public static void playTournament() {
        int curAlive = all;
        ArrayList<InfoPlayer> up = new ArrayList<>();
        ArrayList<InfoPlayer> down = new ArrayList<>();
        for (int i = 0; i < all; ++i) {
            up.add(new InfoPlayer(i + 1, new HumanPlayer(n, isSquare)));
        }
        while (up.size() > 1 || down.size() > 2) {
            //new level
            ArrayList<InfoPlayer> replaceDown;
            ArrayList<InfoPlayer> replaceUp;
            replaceDown = tournamentNet(down, true, up.size() == 1);
            replaceUp = tournamentNet(up, false, false);
            int[] mask = new int[up.size()];
            for (InfoPlayer winner : replaceUp) {
                if (up.contains(winner)) {
                    mask[up.indexOf(winner)] = 1;
                }
            }
            for (int j = 0; j < up.size(); ++j) {
                if (mask[j] == 0) {
                    replaceDown.add(up.get(j));
                }
            }
            up = replaceUp;
            down = replaceDown;
            curAlive = up.size() + down.size();
            System.out.println("Have left alive: " + curAlive);
        }
        if (down.size() == 2) {
            ArrayList<InfoPlayer> replaceDown;
            int nu1 = down.getFirst().num;
            int nu2 = down.get(1).num;
            replaceDown = tournamentNet(down, false, false);
            up.add(replaceDown.getFirst());
            int third = nu1;
            if (replaceDown.getFirst().num == nu1) {
                third = nu2;
            }
            nu1 = up.getFirst().num;
            nu2 = up.get(1).num;
            replaceDown = tournamentNet(up, false, false);
            int second = nu1;
            int first = nu2;
            if (replaceDown.getFirst().num == nu1) {
                second = nu2;
                first = nu1;
            }
            System.out.println("First Place: " + first);
            System.out.println("Second Place: " + second);
            System.out.println("Third Place: " + third);
        } else {
            System.out.println("First Place: " + up.getFirst().num);
            if (!down.isEmpty()) {
                System.out.println("Second Place:" + down.getFirst().num);
            }
        }
    }

    public static void main(String[] args) {
        n = 4;
        m = 4;
        all = 4;
        boolean isTournament = true;
        try (Scanner cons = new Scanner(System.in)) {
            System.out.println("Введите размеры поля и количество клеток для выигрыша.");
            n = cons.nextInt();
            m = cons.nextInt();
            k = cons.nextInt();
            System.out.println("Играем на ромбе? true/false");
            isSquare = cons.nextBoolean();
            System.out.println("Играем турнир? true/false");
            isTournament = cons.nextBoolean();
            if (isTournament) {
                System.out.println("Сколько игроков в турнире?");
                all = cons.nextInt();
                playTournament();
            } else {
                all = 2;
                playTournament();
            }
        }
    }
}
