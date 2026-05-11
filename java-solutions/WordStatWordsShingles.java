import java.io.*;
import java.util.*;

public class WordStatWordsShingles {
    static void upd_map(Map<String, Integer> lhm, String toad) {
        if (!toad.isEmpty()) {
            if (toad.length() < 3) {
                lhm.merge(toad, 1, Integer::sum);
            }
            for (int j = 0; j < toad.length() - 2; ++j) {
                lhm.merge(toad.substring(j, j + 3), 1, Integer::sum);
            }
        }
    }

    public static void main(String[] args) {
        String in = args[0];
        String out = args[1];
        //String in = "src/in.txt";
        //String out = "src/out.txt";
        int length = 1024;
        Map<String, Integer> lhm = new TreeMap<>(Collections.reverseOrder());
        String buf;
        try(Scaner InF=new Scaner(in,true)){
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
        }
        catch(IOException e){
            System.err.println(e.getMessage()+"NextLine error\n");
            return;
        }
        try (BufferedWriter OU = new BufferedWriter(new FileWriter(out), length)) {
            for (Map.Entry<String, Integer> entry : lhm.entrySet()) {
                OU.write(entry.getKey() + " " + entry.getValue() + "\n");
            }
        } catch (IOException e) {
            System.err.println(e.getMessage());
        }


    }
}