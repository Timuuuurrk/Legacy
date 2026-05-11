package expression;

import java.util.Map;

public class Cub extends Operation {
    private final Operation our;

    public Cub(Operation from) {
        super(4, false);
        our = from;
    }

    public Operation getOur() {
        return our;
    }

    public double evaluateD(Map<String, Double> variables) {
        double val = our.evaluateD(variables);
        return val * val * val;
    }

    public int evaluate(int x) {
        int val = our.evaluate(x);
        return val * val * val;
    }

    public double evaluateD(double x) {
        double val = our.evaluateD(x);
        return val * val * val;
    }

    public String toString() {
        return "(" + our.toString() + ")³";
    }

    public String toMiniString() {
        if (our instanceof Cub || our instanceof Sqr || our instanceof Const || our instanceof Variable) {
            return our.toMiniString() + "³";
        }
        return "(" + our.toMiniString() + ")³";
    }

    public int evaluate(int x, int y, int z) {
        int val = our.evaluate(x, y, z);
        return val * val * val;
    }

    @Override
    public boolean equals(Object other) {
        return other != null && other.getClass() == getClass() && our.equals(((Cub) other).getOur());
    }

    @Override
    public int hashCode() {
        return our.hashCode();
    }
}
