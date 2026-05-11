package expression;

public class Main {
    public static void main(String[] args) {
        int val = 6;
        int ans = new Add(new Variable("x"), new Const(2)).evaluate(-10, -10, -10);
        System.out.println(ans);
        System.out.println(ans);
//        String parser=new Subtract(
//                new Multiply(
//                        new Const(2),
//                        new Variable("x")
//                ),
//                new Const(3)
//        ).toString();
//        String pars2=new Subtract(
//                new Multiply(
//                        new Const(2),
//                        new Variable("x")
//                ),
//                new Const(3)
//        ).toMiniString();
//        //System.out.println(pars2);
        boolean fl=new Add(new Const(2),new Variable("x")).equals(new Add(new Const(2),new Variable("x"))) ;
        System.out.println(fl);
    }
}
