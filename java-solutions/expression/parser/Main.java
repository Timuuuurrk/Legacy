package expression.parser;

import expression.TripleExpression;

public class Main {
    public static void main(String[] args) {
        ExpressionParser myDog=new ExpressionParser();
        TripleExpression kek;//=myDog.parse("(-1+0)");
        kek=myDog.parse("0");
        System.out.println(kek.toMiniString());
    }
}
