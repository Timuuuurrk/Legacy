package expression;

import java.util.Map;

public class Sqr extends Operation {
    private final Operation our;

    public Sqr(Operation from) {
        super(4, false);
        our = from;
    }

    public Operation getOur() {
        return our;
    }

    public double evaluateD(Map<String, Double> variables) {
        double val = our.evaluateD(variables);
        return val * val;
    }

    public int evaluate(int x) {
        int val = our.evaluate(x);
        return val * val;
    }

    public double evaluateD(double x) {
        double val = our.evaluateD(x);
        return val * val;
    }

    public String toString() {
        return "(" + our.toString() + ")²";
    }

    public String toMiniString() {
        if (our instanceof Cub || our instanceof Sqr || our instanceof Const || our instanceof Variable) {
            return our.toMiniString() + "²";
        }
        return "(" + our.toMiniString() + ")²";
    }

    public int evaluate(int x, int y, int z) {
        int val = our.evaluate(x, y, z);
        return val * val;
    }

    @Override
    public boolean equals(Object other) {
        return other != null && other.getClass() == getClass() && our.equals(((Sqr) other).getOur());
    }

    @Override
    public int hashCode() {
        return our.hashCode();
    }
}
