package markup;

import java.util.List;

public class OrderedList extends ListMarkup {
    public OrderedList() {
        super();
    }

    public OrderedList(List<ListItem> c) {
        super(c);
    }

    public void toDocBook(StringBuilder a) {
        bounding(a, "<orderedlist>", "</orderedlist>");
    }
}
