package markup;

import java.lang.StringBuilder;
import java.util.List;

public class Emphasis extends Ancestor implements MrkDown {
    public Emphasis(List<MrkDown> c) {
        super(c);
    }

    public void toMarkdown(StringBuilder a) {
        castMarkdown(true);
        bounding(a, "*", "*");
    }

    public void toDocBook(StringBuilder a) {
        castMarkdown(false);
        bounding(a, "<emphasis>", "</emphasis>");
    }
}
