import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;
import java.util.*;

public class WsppCountEvenDigits {
    public static void main(String[] args) {
        String in = args[0];
        String out = args[1];
        //String in = "src/in.txt";
        //String out = "src/out.txt";

        int length = 1024;
        Map<String, WsppEntry> lhm = new LinkedHashMap<>();
        int numAll = 1;
        try (Scaner inp = new Scaner(in, true)) {
            inp.setDigit(true);

            while (inp.hasNextLine()) {
                Map<String, Integer> line = new LinkedHashMap<>();
                int numWord = 1;
                while (inp.hasNextToken(true)) {
                    String k = inp.nextToken(true);
                    //System.err.print(k+" ");
                    IntList update = new IntList();
                    if (line.get(k) != null && line.get(k) % 2 != 0) {
                        update.add(numWord);
                    }
                    line.merge(k, 1, Integer::sum);
                    WsppEntry upd = new WsppEntry(1, numAll, update);
                    lhm.merge(k, upd, WsppEntry::merge);
                    ++numAll;
                    ++numWord;
                }
            }
        } catch (IOException e) {
            System.err.println("Error in opening file.");
            return;
        }

        try (BufferedWriter writer = new BufferedWriter(new FileWriter(out), length)) {
            ArrayList<Map.Entry<String, WsppEntry>> list = getEntries(lhm);
            for (Map.Entry<String, WsppEntry> entry : list) {
                writer.write(entry.getKey() + " " + entry.getValue().getEntryCount());
                // System.err.print(entry.getKey()+" "+first.get(entry.getKey())+" "+gg.get(entry.getKey())+" ");
                for (int i = 0; i < entry.getValue().getPositionsSize(); ++i) {
                    // System.err.print(entry.getValue().get(i)+" ");
                    writer.write(" " + entry.getValue().getPositionsIndexed(i));
                }
                //System.err.println();
                writer.write(System.lineSeparator());
            }


        } catch (IOException e) {
            System.err.println(e.getMessage());
        }


    }

    private static ArrayList<Map.Entry<String, WsppEntry>> getEntries(Map<String, WsppEntry> lhm) {
        ArrayList<Map.Entry<String, WsppEntry>> list = new ArrayList<>(lhm.entrySet());
        list.sort(new Comparator<Map.Entry<String, WsppEntry>>() {
            @Override
            public int compare(Map.Entry<String, WsppEntry> o1, Map.Entry<String, WsppEntry> o2) {
                if (!Objects.equals(o1.getValue().getEntryCount(), o2.getValue().getEntryCount())) {
                    return o1.getValue().getEntryCount() - o2.getValue().getEntryCount();
                }
                return o1.getValue().getFirstEntry() - o2.getValue().getFirstEntry();
            }
        });

        return list;
    }
}
