package expression;

import java.util.Map;

public class Power extends Operation {
    public Power(Operation fir, Operation sec) {
        super(fir, sec, 2, false);
    }

    public int evaluate(int val) {
        int[] values = calculate(val);
        if (values[1] < 0) {
            return 1;
        }
        return (int) (Math.pow(values[0], values[1]));
    }

    public int evaluate(int x, int y, int z) {
        int[] values = calculate(x, y, z);
        if (values[1] < 0) {
            return 1;
        }
        return binpow(values[0], values[1]);
    }

    private int binpow(int a, int p) {
        if (p == 0) {
            return 1;
        }
        int k = binpow(a, p / 2);
        k *= k;
        if (p % 2 != 0) {
            k *= a;
        }
        return k;
    }

    public double evaluateD(Map<String, Double> cur) {
        double[] values = calculateD(cur);
        return (Math.pow(values[0], values[1]));
    }

    public double evaluateD(double val) {
        double[] values = calculateD(val);
        return (Math.pow(values[0], values[1]));
    }

    public String toString() {
        String[] values = getDefaultOperation();
        return "(" + values[0] + " ** " + values[1] + ")";
    }

    public String toMiniString() {
        String[] values = getMiniOperation(OperationsSet.LOG);
        return values[0] + " ** " + values[1];
    }
}
