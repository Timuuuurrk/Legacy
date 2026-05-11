package expression;

import java.util.Map;

public class Multiply extends Operation {
    public Multiply(Operation fir, Operation sec) {
        super(fir, sec, 1, true);
    }

    public int evaluate(int x) {
        int[] values = calculate(x);
        return values[0] * values[1];
    }

    public int evaluate(int x, int y, int z) {
        int[] values = calculate(x, y, z);
        return values[0] * values[1];
    }

    public double evaluateD(Map<String, Double> cur) {
        double[] values = calculateD(cur);
        return values[0] * values[1];
    }

    public double evaluateD(double x) {
        double[] values = calculateD(x);
        return values[0] * values[1];
    }

    public String toString() {
        String[] values = getDefaultOperation();
        return "(" + values[0] + " * " + values[1] + ")";
    }

    public String toMiniString() {
        String[] values = getMiniOperation(OperationsSet.MUL);
        return values[0] + " * " + values[1];
    }
}
