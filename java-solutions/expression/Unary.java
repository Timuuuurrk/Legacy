package expression;

import java.util.Map;

public class Unary extends Operation{
    private final Operation our;
    public Unary(Operation from){
        super(4,false);
        our=from;
    }
    public Operation getOur(){
        return our;
    }
    public double evaluateD(Map<String, Double> variables) {
        return -our.evaluateD(variables);
    }

    public int evaluate(int x) {
        return -our.evaluate(x);
    }

    public double evaluateD(double val) {
        return -our.evaluateD(val);
    }

    public String toString() {
        return "-("+our.toString()+")";
    }

    public String toMiniString() {
        if(our instanceof Unary||our instanceof Sqr||our instanceof Cub ||our instanceof Const||our instanceof Variable){
            return "- "+ our.toMiniString();
        }
        return "-("+our.toMiniString()+")";
    }

    public int evaluate(int x, int y, int z) {
        return -our.evaluate(x,y,z);
    }

    @Override
    public boolean equals(Object other) {
        return other!=null&&other.getClass()==getClass()&&our.equals(((Unary) other).getOur());
    }

    @Override
    public int hashCode() {
        return our.hashCode();
    }
}
