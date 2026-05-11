import java.io.*;
import java.nio.charset.StandardCharsets;

public class Scaner implements AutoCloseable {
    private final int len = 1024;
    private Reader scn;
    private final StringBuilder buf;
    private char[] slice;
    private boolean saveNext;
    private int saveResult;
    private int endResult;
    private String saveWord;
    private boolean end = false;
    private boolean digit = false;

    public Scaner() {
        saveNext = false;
        buf = new StringBuilder();
        slice = new char[len];
        scn = new InputStreamReader(System.in, StandardCharsets.UTF_8);
    }

    public Scaner(String stream, boolean fl) {
        buf = new StringBuilder();
        saveNext = false;
        if (!fl) {
            InputStream is = new ByteArrayInputStream(stream.getBytes(StandardCharsets.UTF_8));
            scn = new InputStreamReader(is);
        } else {
            slice = new char[len];
            try {
                scn = new InputStreamReader(new FileInputStream(stream), StandardCharsets.UTF_8);
            } catch (IOException e) {
                System.err.println(e.getMessage());
            }
        }
    }
    public boolean getEnd() {
        return end;
    }
    public void setDigit(boolean value){
        digit=value;
    }
    private void readInput(boolean one) throws IOException {
        char[] oneCh = new char[1];
        int countReadChars;
        if (one) {
            countReadChars = scn.read(oneCh);
        } else {
            countReadChars = scn.read(slice);
            buf.delete(0, buf.length());
        }
        if (countReadChars == -1) {
            end = true;
            return;
        }
        if (one) {
            buf.append(oneCh);
        } else {
            buf.append(new String(slice), 0, countReadChars);
        }
    }

    public void fillBuffer(boolean fl, boolean fl2) throws IOException {
        if (!end && fl) {
            slice = new char[len];
            readInput(fl2);
        }
    }

    public boolean hasNextLine() throws IOException {
        fillBuffer(buf.isEmpty(), false);
        return !buf.isEmpty();
    }

    public boolean hasNextToken(boolean Word) throws IOException {
        fillBuffer(buf.isEmpty(), false);
        StringBuilder b = new StringBuilder();
        int i = 0;
        boolean isOct = false;
        saveNext = false;
        while (i != buf.length() && !isValid(buf.charAt(i), Word)) {
            if (buf.charAt(i) == '\n' || buf.charAt(i) == '\r' || System.lineSeparator().indexOf(buf.charAt(i)) != -1) {
                ++i;
                if (i == buf.length()) {
                    fillBuffer(true, true);
                    if (i == buf.length()) {
                        buf.delete(0, i);
                        return false;
                    }
                }
                if (!(buf.charAt(i) == '\n' && buf.charAt(i - 1) == '\r')) {
                    --i;
                }
                buf.delete(0, i + 1);
                return false;
            }
            ++i;
            if (i == buf.length()) {
                fillBuffer(true, false);
                if (buf.isEmpty() && end) {
                    return false;
                }
                i = 0;
            }
        }
        while (i != buf.length() && isValid(buf.charAt(i), Word)) {
            b.append(buf.charAt(i));
            ++i;
            if (i == buf.length()) {
                fillBuffer(true, false);
                i = 0;
            }
        }
        if (!Word && !b.isEmpty() && (b.charAt(b.length() - 1) == 'o' || b.charAt(b.length() - 1) == 'O')) {
            isOct = true;
            b.delete(b.length() - 1, b.length());
        }
        endResult = i;
        if (!Word) {
            try {
                saveNext = true;
                int osn = 10;
                if (isOct) {
                    osn = 8;
                }
                if (osn == 8) {
                    saveResult = Integer.parseUnsignedInt(b.toString(), osn);
                } else {
                    saveResult = Integer.parseInt(b.toString());
                }
            } catch (NumberFormatException e) {
                saveNext = false;
                return false;
            }
        } else {
            saveNext = !b.isEmpty();
            saveWord = b.toString();
            return !b.isEmpty();
        }
        return true;
    }

    public String nextToken(boolean Word) throws IOException {
        if (saveNext || hasNextToken(Word)) {
            saveNext = false;
            buf.delete(0, endResult);
        }
        if (Word) {
            return saveWord.toLowerCase();
        }
        return Integer.toString(saveResult);
    }

    private boolean isValid(char e, boolean isWord) {
        if (isWord) {
            return isValidChar(e);
        } else {
            return isValidDig(e);
        }
    }

    private boolean isValidDig(char e) {
        return Character.isDigit(e) || e == 'o' || e == '-' || e == '+' || e == 'O';
    }

    private boolean isValidChar(char e) {
        return e == '\'' || Character.isLetter(e) || Character.getType(e) == Character.DASH_PUNCTUATION || (digit && Character.isDigit(e));
    }

    public void close() throws IOException {
        if (scn != null) {
            scn.close();
        }
    }
}