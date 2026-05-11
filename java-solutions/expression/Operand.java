package expression;

public interface Operand extends Expression, TripleExpression, DoubleMapExpression {
    boolean equals(Object other);

    String toString();
}
