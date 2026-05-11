package markup;

import java.lang.StringBuilder;
import java.util.List;

public class Strong extends Ancestor implements MrkDown {
    public Strong(List<MrkDown> c) {
        super(c);
    }

    public void toMarkdown(StringBuilder a) {
        castMarkdown(true);
        bounding(a, "__", "__");
    }

    public void toDocBook(StringBuilder a) {
        castMarkdown(false);
        bounding(a, "<emphasis role='bold'>", "</emphasis>");
    }
}
