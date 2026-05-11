package expression;

import java.util.Map;

public class Subtract extends Operation {
    public Subtract(Operation fir, Operation sec) {
        super(fir, sec, 0,false);
    }

    public int evaluate(int val) {
        int[]values=calculate(val);
        return values[0]-values[1];
    }

    public int evaluate(int x, int y, int z) {
        int[]values=calculate(x,y,z);
        return values[0]-values[1];
    }

    public double evaluateD(Map<String, Double> cur) {
        double[]values=calculateD(cur);
        return values[0]-values[1];
    }

    public double evaluateD(double val) {
        double[]values=calculateD(val);
        return values[0]-values[1];
    }

    public String toString() {
        String[] values=getDefaultOperation();
        return "("+values[0]+" - "+values[1]+")";
    }

    public String toMiniString() {
        String[]values=getMiniOperation(OperationsSet.MINUS);
        return values[0]+" - "+values[1];
    }
}
