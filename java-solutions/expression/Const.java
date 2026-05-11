package expression;

import java.util.Map;
import java.util.Objects;

public class Const extends Operation {
    private final Number value;
    private final String fromParser;

    public Const(String from) {
        super(3, true);
        if (from.charAt(0) == '-' && Character.isWhitespace(from.charAt(1))) {
            value = Integer.parseInt("-" + from.substring(2));
        } else {
            value = Integer.parseInt(from);
        }
        fromParser = from;
    }

    public Const(int x) {
        super(3, true);
        fromParser = "";
        value = x;
    }

    public Const(double x) {
        super(3, true);
        fromParser = "";
        value = x;
    }

    public int evaluate(int x) {
        return value.intValue();
    }

    public double evaluateD(double x) {
        return value.doubleValue();
    }

    public int evaluate(int x, int y, int z) {
        return value.intValue();
    }

    public String toString() {
        if (!fromParser.isEmpty()) {
            return fromParser;
        }
        return value.toString();
    }

    public String toMiniString() {
        if (!fromParser.isEmpty()) {
            return fromParser;
        }
        return value.toString();
    }

    public double evaluateD(Map<String, Double> variables) {
        return value.doubleValue();
    }

    @Override
    public boolean equals(Object other) {
        return other != null && other.getClass() == Const.class && Objects.equals(((Const) other).value, this.value);
    }

    @Override
    public int hashCode() {
        return Objects.hash(value);
    }
}
