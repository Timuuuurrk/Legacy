package markup;

import java.util.List;

public class UnorderedList extends ListMarkup {
    public UnorderedList() {
        super();
    }

    public UnorderedList(List<ListItem> c) {
        super(c);
    }

    public void toDocBook(StringBuilder a) {
        bounding(a, "<itemizedlist>", "</itemizedlist>");
    }
}
