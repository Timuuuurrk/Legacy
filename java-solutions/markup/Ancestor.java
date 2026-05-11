package markup;

import java.lang.StringBuilder;
import java.util.List;

public class Ancestor {
    public List<MrkDown> cur;
    public StringBuilder res;

    public Ancestor() {
        cur = null;
        res = new StringBuilder();
    }

    public Ancestor(List<MrkDown> b) {
        cur = List.copyOf(b);
        res = new StringBuilder();
    }

    public void bounding(StringBuilder a, String left, String right) {
        a.append(left);
        a.append(res);
        a.append(right);
    }

    public void castMarkdown(boolean toMark) {
        res = new StringBuilder();
        for (MrkDown in : cur) {
            StringBuilder storage = new StringBuilder();
            if (toMark) {
                in.toMarkdown(storage);
            } else {
                in.toDocBook(storage);
            }
            res.append(storage);
        }
    }

}
