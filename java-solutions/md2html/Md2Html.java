package md2html;

import java.io.BufferedWriter;
import java.io.File;
import java.io.FileWriter;
import java.io.IOException;
import java.nio.charset.StandardCharsets;
import java.util.Scanner;
import java.util.List;

public class Md2Html {
    final static int length = 1024;
    static boolean isDelimited = true;
    static String nameParagraph = "";
    static int linesInParagraph = 0;
    final static List<Character> screen = List.of('[', ']', '*', '_');

    public static boolean checkParagraph(String from) {
        for (int i = 0; i < from.length(); ++i) {
            if (from.charAt(i) != '#') {
                return false;
            }
        }
        return true;
    }

    public static int[] getWord(StringBuilder from) {
        int[] to = new int[2];
        int i = 0;
        while (i < from.length() && Character.isWhitespace(from.charAt(i))) {
            ++i;
        }
        to[0] = i;
        while (i < from.length() && !Character.isWhitespace(from.charAt(i))) {
            ++i;
        }
        to[1] = i;
        return to;
    }

    public static StringBuilder pars(StringBuilder words) {
        StringBuilder processed = new StringBuilder();
        for (int i = 0; i < words.length(); ++i) {
            if (i != words.length() - 1 && words.charAt(i) == words.charAt(i + 1) && (words.charAt(i) == '*' || words.charAt(i) == '_' || words.charAt(i) == '-')) {
                String finder = "**";
                if (words.charAt(i) == '_') {
                    finder = "__";
                } else if (words.charAt(i) == '-') {
                    finder = "--";
                }
                int ind = words.indexOf(finder, i + 2);
                if (ind != -1) {
                    StringBuilder temp = pars(new StringBuilder(words.substring(i + 2, ind)));
                    i = ind + 1;
                    StringBuilder mark = new StringBuilder("<strong>");
                    if (words.charAt(i) == '-') {
                        mark = new StringBuilder("<s>");
                    }
                    processed.append(mark);
                    processed.append(temp);
                    mark.insert(1, "/");
                    processed.append(mark);
                    continue;
                }
            }
            if (words.charAt(i) == '*' || words.charAt(i) == '_' || words.charAt(i) == '`' || words.charAt(i) == '[') {
                char left = ' ';
                char right = ' ';
                if (i > 0) {
                    left = words.charAt(i - 1);
                }
                if (i + 1 != words.length()) {
                    right = words.charAt(i + 1);
                }
                if (!Character.isWhitespace(left) || !Character.isWhitespace(right) || words.charAt(i) == '`' || words.charAt(i) == '[') {
                    if (words.charAt(i) == '[') {
                        int ind = words.indexOf(Character.toString(']'), i + 1);
                        while (ind != -1 && (words.charAt(ind - 1) == '\\' || (ind < words.length() && words.charAt(ind + 1) != '<'))) {
                            ind = words.indexOf(Character.toString(']'), ind + 1);
                        }
                        if (ind != -1) {
                            int pos = ind;
                            ind = words.indexOf(Character.toString('>'), ind + 1);
                            while (ind != -1 && words.charAt(ind - 1) == '\\') {
                                ind = words.indexOf(Character.toString(']'), ind + 1);
                            }
                            if (ind != -1) {
                                processed.append("<a href=");
                                processed.append("'").append(words, pos + 2, ind).append("'");
                                processed.append(">");
                                processed.append(pars(new StringBuilder(words.substring(i + 1, pos))));
                                processed.append("</a>");
                                i = ind;
                                continue;
                            }
                        }
                    } else {
                        String template = Character.toString(words.charAt(i));
                        if (words.charAt(i) == '[') {
                            template = Character.toString(']');
                        }
                        int ind = words.indexOf(template, i + 1);
                        while (ind != -1 && words.charAt(i) != '`' && (words.charAt(ind - 1) == '\\' || words.charAt(ind - 1) == words.charAt(ind) || (ind + 1 != words.length() && words.charAt(ind + 1) == words.charAt(ind)))) {
                            ind = words.indexOf(Character.toString(words.charAt(i)), ind + 1);
                        }
                        if (ind != -1) {
                            StringBuilder temp = pars(new StringBuilder(words.substring(i + 1, ind)));
                            i = ind;
                            StringBuilder markup = new StringBuilder("<em>");
                            if (words.charAt(i) == '`') {
                                markup = new StringBuilder("<code>");
                            }
                            processed.append(markup);
                            processed.append(temp);
                            markup.insert(1, "/");
                            processed.append(markup);
                            continue;
                        }
                    }
                }
            }
            if (words.charAt(i) == '<' || words.charAt(i) == '>' || words.charAt(i) == '&') {
                if (words.charAt(i) == '<') {
                    processed.append("&lt;");
                } else if (words.charAt(i) == '>') {
                    processed.append("&gt;");
                } else {
                    processed.append("&amp;");
                }
                continue;
            }
            if (i != words.length() - 1 && words.charAt(i) == '\\' && screen.contains(words.charAt(i + 1))) {
                processed.append(words.charAt(i + 1));
                i++;
                continue;
            }
            if (words.charAt(i) != '\\') {
                processed.append(words.charAt(i));
            }
        }
        return processed;
    }

    public static StringBuilder parser(StringBuilder res) {
        StringBuilder to = new StringBuilder();
        int[] cur = getWord(res);
        if (isDelimited && nameParagraph.isEmpty()) {
            int level = cur[1] - cur[0];
            boolean isPar = checkParagraph(res.substring(0, cur[1]));
            StringBuilder par = new StringBuilder("<p>");
            if (isPar) {
                par = new StringBuilder("<h" + level + ">");
            }
            to.append(par);
            par.insert(1, "/");
            nameParagraph = par.toString();
            if (isPar) {
                res.delete(0, cur[1]);
                cur = getWord(res);
                res.delete(0, cur[0]);
            }
        }
        to.append(pars(new StringBuilder(res)));
        return to;
    }

    public static void main(String[] args) {
        String in = args[0];
        String out = args[1];
        //String in = "in.txt";
        //String out = "out.txt";
        try (BufferedWriter writer = new BufferedWriter(new FileWriter(out, StandardCharsets.UTF_8), length)) {
            try (Scanner cons = new Scanner(new File(in))) {
                StringBuilder curParagraph = new StringBuilder();
                while (true) {
                    String line = "";
                    if (cons.hasNextLine()) {
                        line = cons.nextLine();
                        //System.err.println(line);
                    }
                    if (line.isEmpty()) {
                        StringBuilder ans = new StringBuilder();
                        ans.append(parser(curParagraph));
                        linesInParagraph = 0;
                        if (isDelimited) {
                            ans.append(nameParagraph);
                            ans.append(System.lineSeparator());
                            isDelimited = false;
                            nameParagraph = "";
                        }
                        writer.write(ans.toString());
                        if (!cons.hasNextLine()) {
                            break;
                        }
                        curParagraph = new StringBuilder();
                    } else {
                        if (linesInParagraph > 0) {
                            curParagraph.append(System.lineSeparator());
                        }
                        isDelimited = true;
                        curParagraph.append((line));
                        linesInParagraph++;
                    }
                }
            } catch (IOException e) {
                System.err.println("Error in opening input file." + e.getMessage());
            }
        } catch (IOException e) {
            System.err.println("Error in opening output file." + e.getMessage());
        }
    }
}
