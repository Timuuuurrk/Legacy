import java.io.*;
import java.util.LinkedHashMap;

public class WordStatInput {
    static void upd_map(LinkedHashMap<String, Integer> lhm, String toad) {
        if (!toad.isEmpty()) {
            if (lhm.containsKey(toad)) {
                lhm.put(toad, lhm.get(toad) + 1);
            } else {
                lhm.put(toad, 1);
            }
        }
    }

    public static void main(String[] args) {
        String in = args[0];
        String out = args[1];
        //String in="src/in.txt";
        //String out="src/out.txt";
        int length = 2000;
        int chars_read;
        char[] curs = new char[length];
        LinkedHashMap<String, Integer> lhm = new LinkedHashMap<>();
        String buf;
        try (Scaner InF = new Scaner(in, true)) {
            while (InF.hasNextLine()) {
                try {
                    while (InF.hasNextToken(true)) {
                        buf = InF.nextToken(true);
                        upd_map(lhm, buf);
                    }
                } catch (IOException e) {
                    System.err.println(e.getMessage() + "Input error.\n");
                    return;
                }
            }
        } catch (IOException e) {
            System.err.println(e.getMessage() + "NextLine error\n");
            return;
        }
        try (BufferedWriter OU = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(out), "utf-8"), length)) {
            for (String e : lhm.keySet()) {
                String br = e + " " + lhm.get(e);
                OU.write(br, 0, br.length());
                OU.newLine();
            }
        } catch (IOException e) {
            System.err.println(e.getMessage());
        }
    }
}
