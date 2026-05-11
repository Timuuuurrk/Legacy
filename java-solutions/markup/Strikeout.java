package markup;

import java.lang.StringBuilder;
import java.util.List;

public class Strikeout extends Ancestor implements MrkDown {
    public Strikeout(List<MrkDown> c) {
        super(c);
    }

    public void toMarkdown(StringBuilder a) {
        castMarkdown(true);
        bounding(a, "~", "~");
    }

    public void toDocBook(StringBuilder a) {
        castMarkdown(false);
        bounding(a, "<emphasis role='strikeout'>", "</emphasis>");
    }
}
