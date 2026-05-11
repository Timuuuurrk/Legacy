package markup;

import java.lang.StringBuilder;

public interface MrkDown {
    void toDocBook(StringBuilder a);

    void toMarkdown(StringBuilder a);
}
