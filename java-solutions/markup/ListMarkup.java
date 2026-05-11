package markup;

import java.util.List;

public abstract class ListMarkup extends Ancestor implements Token {
    List<ListItem> elems;

    public ListMarkup() {
        super();
        elems = null;
    }

    public ListMarkup(List<ListItem> b) {
        elems = List.copyOf(b);
        res = new StringBuilder();
        for (ListItem in : b) {
            StringBuilder storage = new StringBuilder();
            in.toDocBook(storage);
            res.append(storage);
        }
    }
}
