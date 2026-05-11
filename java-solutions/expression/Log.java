package expression;

import java.util.Map;

public class Log extends Operation {
    public Log(Operation fir, Operation sec) {
        super(fir, sec, 2, false);
    }

    public int evaluate(int x) {
        int[] values = calculate(x);
        if (values[0] < 0) return 0;
        return (int) (Math.log(values[0]) / Math.log(values[1]));
    }

    public int evaluate(int x, int y, int z) {
        int[] values = calculate(x, y, z);
        if (values[0] < 0) return 0;
        return (int) (Math.log(values[0]) / Math.log(values[1]));
    }

    public double evaluateD(Map<String, Double> cur) {
        double[] values = calculateD(cur);
        return (Math.log(values[0]) / Math.log(values[1]));
    }

    public double evaluateD(double val) {
        double[] values = calculateD(val);
        return (Math.log(values[0]) / Math.log(values[1]));
    }

    public String toString() {
        String[] values = getDefaultOperation();
        return "(" + values[0] + " // " + values[1] + ")";
    }

    public String toMiniString() {
        String[] values = getMiniOperation(OperationsSet.LOG);
        return values[0] + " // " + values[1];
    }
}
