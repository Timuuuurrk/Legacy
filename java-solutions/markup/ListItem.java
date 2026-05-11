package markup;

import java.util.List;

public class ListItem {
    StringBuilder res;
    List<Token> cu;

    public ListItem() {
        cu = null;
        res = new StringBuilder();
    }

    public ListItem(List<Token> c) {
        res = new StringBuilder();
        cu = List.copyOf(c);
        for (Token in : c) {
            StringBuilder storage = new StringBuilder();
            in.toDocBook(storage);
            res.append(storage);
        }
    }

    public void toDocBook(StringBuilder a) {
        a.append("<listitem>");
        a.append(res);
        a.append("</listitem>");
    }
}
