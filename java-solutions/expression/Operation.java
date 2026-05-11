package expression;

import java.util.Map;
import java.util.Objects;

public abstract class Operation implements Operand {
    private final Operation first;
    private final Operation second;
    private final int priority;
    private final boolean associative;

    public Operation(Operation fir, Operation sec, int type, boolean assoc) {
        first = fir;
        second = sec;
        priority = type;
        associative = assoc;
    }

    public Operation(int type, boolean assoc) {
        first = null;
        second = null;
        priority = type;
        associative = assoc;
    }

    public int[] calculate(int val) {
        assert first != null && second != null;
        int[] result = new int[2];
        result[0] = first.evaluate(val);
        result[1] = second.evaluate(val);
        return result;
    }

    public double[] calculateD(double v) {
        assert first != null && second != null;
        double[] result = new double[2];
        result[0] = first.evaluateD(v);
        result[1] = second.evaluateD(v);
        return result;
    }

    public double[] calculateD(Map<String, Double> cur) {
        assert first != null && second != null;
        double[] result = new double[2];
        result[0] = first.evaluateD(cur);
        result[1] = second.evaluateD(cur);
        return result;
    }

    public int[] calculate(int x, int y, int z) {
        assert first != null && second != null;
        int[] result = new int[2];
        result[0] = first.evaluate(x, y, z);
        result[1] = second.evaluate(x, y, z);
        return result;
    }


    public String[] getDefaultOperation() {
        assert first != null && second != null;
        String[] result = new String[2];
        result[0] = first.toString();
        result[1] = second.toString();
        return result;
    }

    public String[] getMiniOperation(OperationsSet type) {
        assert first != null && second != null;
        String[] result = new String[2];
        result[0] = first.toMiniString();
        result[1] = second.toMiniString();
        if (first.priority < priority) {
            result[0] = "(" + result[0] + ")";
        }
        if (second.priority < priority || (second.priority == priority && (!associative || (!second.associative && priority == 1)))) {
            result[1] = "(" + result[1] + ")";
        }
        return result;
    }

    public boolean equals(Object other) {
        return other != null && other.getClass() == getClass() && ((Operation) other).first.equals(first) && ((Operation) other).second.equals(second);
    }

    @Override
    public int hashCode() {
        return Objects.hash(first, second, priority, associative);
    }

    public abstract int evaluate(int val);

    public abstract double evaluateD(double val);

    public abstract String toString();

    public abstract String toMiniString();
}
