package expression.parser;

import expression.*;

public class ExpressionParser implements TripleParser {
    private int curInd;

    public TripleExpression parse(String from) {
        String stripped = strip(from);
        //System.err.println(from);
        curInd = 0;
        return parseExpression(stripped);
    }

    public String strip(String from) {
        StringBuilder ans = new StringBuilder();
        for (int i = 0; i < from.length(); ++i) {
            if (i > 0 && Character.isDigit(from.charAt(i)) && Character.isWhitespace(from.charAt(i - 1))) {
                ans.append(from.charAt(i - 1));
            }
            if (!Character.isWhitespace(from.charAt(i))) {
                ans.append(from.charAt(i));
            }
        }
        return ans.toString();
    }

    public Operation parseExpression(String from) {
        return parsePlus(from);
    }

    public Operation parsePlus(String from) {
        int ind;
        Operation first = parseMul(from);
        ind = curInd;
        for (int i = ind; i < from.length(); ++i) {
            if (from.charAt(i) == '+') {
                curInd = i + 1;
                first = new Add(first, parseMul(from));
                i = curInd - 1;
            } else if (from.charAt(i) == '-') {
                curInd = i + 1;
                first = new Subtract(first, parseMul(from));
                i = curInd - 1;
            } else {
                curInd = i;
                return first;
            }
        }
        curInd = from.length();
        return first;
    }

    public Operation parseMul(String from) {
        int ind;
        Operation first = parsePow(from);
        ind = curInd;
        for (int i = ind; i < from.length(); ++i) {
            if (from.charAt(i) == '*' && ((i + 1 == from.length() || from.charAt(i + 1) != '*'))) {
                curInd = i + 1;
                first = new Multiply(first, parsePow(from));
                i = curInd - 1;
            } else if (from.charAt(i) == '/' && ((i + 1 == from.length() || from.charAt(i + 1) != '/'))) {
                curInd = i + 1;
                first = new Divide(first, parsePow(from));
                i = curInd - 1;
            } else {
                curInd = i;
                return first;
            }
        }
        curInd = from.length();
        return first;
    }

    public Operation parsePow(String from) {
        int ind;
        Operation first = parseUnary(from);
        ind = curInd;
        for (int i = ind; i < from.length(); ++i) {
            if (from.charAt(i) == '*' && i + 1 != from.length() && from.charAt(i + 1) == '*') {
                curInd = i + 2;
                first = new Power(first, parseUnary(from));
                i = curInd - 1;
            } else if (from.charAt(i) == '/' && i + 1 != from.length() && from.charAt(i + 1) == '/') {
                curInd = i + 2;
                first = new Log(first, parseUnary(from));
                i = curInd - 1;
            } else {
                curInd = i;
                return first;
            }
        }
        curInd = from.length();
        return first;
    }

    public Operation parseUnary(String from) {
        int ind = curInd;
        if (from.charAt(ind) == '-') {
            if (Character.isWhitespace(from.charAt(ind + 1))) {
                curInd = ind + 2;
                return new Unary(parsePrimitive(from));
            } else if (Character.isDigit(from.charAt(ind + 1))) {
                curInd = ind;
                return parsePrimitive(from);
            }
            curInd = ind + 1;
            return new Unary(parsePrimitive(from));
        }
        return parsePrimitive(from);
    }

    public Operation parsePrimitive(String from) {
        int ind = curInd;
        for (int i = ind; i < from.length(); ++i) {
            if (from.charAt(i) == '(') {
                StringBuilder result = new StringBuilder();
                int j = i + 1;
                int bal = 1;
                while (j < from.length() && (bal > 1 || from.charAt(j) != ')')) {
                    if (from.charAt(j) == ')') {
                        bal--;
                    }
                    if (from.charAt(j) == '(') {
                        bal++;
                    }
                    result.append(from.charAt(j));
                    j++;
                }
                curInd = 0;
                Operation res = parseExpression(result.toString());
                curInd = Math.min(j + 1, from.length());
                return tryPower(from, res);
            } else if (Character.isDigit(from.charAt(i)) || (from.charAt(i) == '-' && Character.isDigit(from.charAt(i + 1)))) {
                StringBuilder result = new StringBuilder();
                result.append(from.charAt(i));
                int j = i + 1;
                while (j < from.length() && Character.isDigit(from.charAt(j))) {
                    result.append(from.charAt(j));
                    j++;
                }
                curInd = j;
                Operation res = new Const(result.toString());
                return tryPower(from, res);
            } else if (Character.isLetter(from.charAt(i))) {
                StringBuilder result = new StringBuilder();
                int j = i;
                while (j < from.length() && Character.isLetter(from.charAt(j))) {
                    result.append(from.charAt(j));
                    j++;
                }
                curInd = j;
                Operation res = new Variable(result.toString());
                return tryPower(from, res);
            } else if (from.charAt(i) == '-') {
                curInd = ind + 1;
                return new Unary(parsePrimitive(from));
            }
        }
        return null;
    }

    private Operation tryPower(String from, Operation res) {
        while (curInd < from.length() && (from.charAt(curInd) == '²' || from.charAt(curInd) == '³')) {
            if (from.charAt(curInd) == '²') {
                res = new Sqr(res);
            } else {
                res = new Cub(res);
            }
            ++curInd;
        }
        return res;
    }

}
