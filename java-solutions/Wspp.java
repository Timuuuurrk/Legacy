import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.Map;
import java.util.LinkedHashMap;
import java.util.TreeMap;

public class Wspp {
    public static void main(String[] args) {
        String in = args[0];
        String out = args[1];
        //String in = "src/in.txt";
        //String out = "src/out.txt";
        int length = 1024;
        Map<String, Integer> gg = new LinkedHashMap<>();
        Map<String, IntList> lhm = new LinkedHashMap<>();
        int numWord = 1;
        try (Scaner inp = new Scaner(in, true)) {
            while (inp.hasNextLine()) {
                try {
                    while (inp.hasNextToken(true)) {
                        String k = inp.nextToken(true);
                        gg.merge(k, 1, Integer::sum);
                        lhm.merge(k, new IntList(numWord), IntList::merge);
                        numWord++;
                    }
                } catch (IOException e) {
                    System.err.println(e.getMessage());
                }
            }
        } catch (IOException e) {
            System.err.println(e.getMessage());
            return;
        }
        try (BufferedWriter OU = new BufferedWriter(new FileWriter(out), length)) {
            for (Map.Entry<String, IntList> entry : lhm.entrySet()) {
                OU.write(entry.getKey() + " " + gg.get(entry.getKey()) + " ");
                for (int i = 0; i < entry.getValue().size(); ++i) {
                    OU.write(entry.getValue().get(i) + "");
                    if (i != entry.getValue().size() - 1) {
                        OU.write(" ");
                    }
                }
                OU.write("\n");
            }


        } catch (IOException e) {
            System.err.println(e.getMessage());
        }


    }

}
