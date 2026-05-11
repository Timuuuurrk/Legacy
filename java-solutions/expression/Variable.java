package expression;

import java.util.Map;
import java.util.Objects;

public class Variable extends Operation {
    private final String x;

    public Variable(String name) {
        super(4, true);
        x = name;
    }

    public int evaluate(int x) {
        return x;
    }

    public double evaluateD(double x) {
        return x;
    }

    public String toString() {
        return x;
    }

    public String toMiniString() {
        return x;
    }

    public int evaluate(int xx, int y, int z) {
        if (x.charAt(x.length() - 1) == 'x') {
            return xx;
        } else if (x.charAt(x.length() - 1) == 'y') {
            return y;
        } else {
            return z;
        }
    }

    public double evaluate(double xx, double y, double z) {
        if (x.charAt(x.length() - 1) == 'x') {
            return xx;
        } else if (x.charAt(x.length() - 1) == 'y') {
            return y;
        } else {
            return z;
        }
    }

    @Override
    public int hashCode() {
        return Objects.hash(x);
    }

    public double evaluateD(Map<String, Double> variables) {
        return variables.get(x.substring(x.length() - 1));
    }

    public boolean equals(Object other) {
        return other != null && other.getClass() == Variable.class && this.x.equals(((Variable) other).x);
    }
}
