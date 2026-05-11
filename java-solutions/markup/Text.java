package markup;

import java.lang.StringBuilder;

public class Text extends Ancestor implements MrkDown {
    public Text(String a) {
        res = new StringBuilder(a);
    }

    public void toDocBook(StringBuilder a) {
        a.append(res);
    }

    public void toMarkdown(StringBuilder a) {
        a.append(res);
    }
}
