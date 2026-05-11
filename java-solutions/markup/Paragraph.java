package markup;

import java.lang.StringBuilder;
import java.util.List;

public class Paragraph extends Ancestor implements Token {
    public Paragraph(List<MrkDown> c) {
        super(c);
    }

    public void toDocBook(StringBuilder a) {
        castMarkdown(false);
        bounding(a, "<para>", "</para>");
    }

    public void toMarkdown(StringBuilder a) {
        castMarkdown(true);
        a.append(res);
    }
}
