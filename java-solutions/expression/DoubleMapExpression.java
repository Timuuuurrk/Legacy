package expression;

import base.Asserts;
import base.ExtendedRandom;
import base.Pair;
import base.TestCounter;
import expression.common.ExpressionKind;
import expression.common.Type;

import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;
import java.util.stream.Collectors;
import java.util.stream.IntStream;
import java.util.stream.Stream;

/**
 * @author Georgiy Korneev (kgeorgiy@kgeorgiy.info)
 */
@FunctionalInterface
@SuppressWarnings("ClassReferencesSubclass")
public interface DoubleMapExpression extends ToMiniString {
    double evaluateD(Map<String, Double> variables);

    // Tests follow. You may temporarily remove everything til the end.

    Add EXAMPLE = new Add(
            new Subtract(new Variable("xx"), new Const(1.1)),
            new Multiply(new Variable("yy"), new Const(10.1))
    );

    Type<Double> TYPE = new Type<>(Double::valueOf, random -> random.getRandom().nextGaussian() * 1000, double.class);
    String LETTERS = ExtendedRandom.ENGLISH + ExtendedRandom.ENGLISH.toUpperCase().substring(1, 20);
    ExpressionKind<DoubleMapExpression, Double> KIND = new ExpressionKind<>(
            TYPE,
            DoubleMapExpression.class,
            (r, c) -> Stream.generate(() -> r.randomString(LETTERS))
                    .filter(name -> name.chars().anyMatch(Character::isLowerCase))
                    .filter(name -> name.chars().anyMatch(Character::isUpperCase))
                    .distinct()
                    .limit(c)
                    .map(name -> Pair.<String, DoubleMapExpression>of(name, new Variable(name)))
                    .collect(Collectors.toUnmodifiableList()),
            (expr, variables, values) -> expr.evaluateD(
                    IntStream.range(0, variables.size())
                            .boxed()
                            .collect(Collectors.toMap(variables::get, values::get, (a, b) -> a, LinkedHashMap::new)))
    );

    @SuppressWarnings({"PointlessArithmeticExpression", "Convert2MethodRef"})
    static ExpressionTester<?, ?> tester(final TestCounter counter) {
        Asserts.assertEquals("Example toString()", "((xx - 1.1) + (yy * 10.1))", EXAMPLE.toString());
        Asserts.assertEquals(EXAMPLE + " at (2.1, 3.1)", 32.31, EXAMPLE.evaluateD(Map.of("xx", 2.1, "yy", 3.1)), 1e-6);

        final ConstWrapper one = w(1.1);
        final ConstWrapper two = w(2.2);
        final ConstWrapper three = w(3.3);
        final ConstWrapper ten = w(10.1);

        return new ExpressionTester<>(
                counter, KIND, c -> v -> c,
                (op, a, b) -> v -> op.apply(a.evaluateD(v), b.evaluateD(v)),
                (a, b) -> a + b, (a, b) -> a - b, (a, b) -> a * b, (a, b) -> a / b,
                Map.of("@one", one.v, "@two", two.v, "@three", three.v, "@ten", ten.v)
        )
                .basicF("@ten", "@ten", v -> ten.v, v -> ten.c)
                .basicF("$x", "$x", DoubleMapExpression::x, DoubleMapExpression::vx)
                .basicF("$y", "$y", DoubleMapExpression::y, DoubleMapExpression::vy)
                .basicF("($x + $y)", "$x + $y", v -> x(v) + y(v), v -> new Add(vx(v), vy(v)))
                .basicF("($x + @two)", "$x + @two", v -> x(v) + two.v, v -> new Add(vx(v), two.c))
                .basicF("(@two - $x)", "@two - $x", v -> two.v - x(v), v -> new Subtract(two.c, vx(v)))
                .basicF("(@three * $x)", "@three * $x", v -> three.v * x(v), v -> new Multiply(three.c, vx(v)))
                .basicF("($x + $x)", "$x + $x", v -> x(v) + x(v), v -> new Add(vx(v), vx(v)))
                .basicF("($x / -@two)", "$x / -@two", v -> -x(v) / two.v, v -> new Divide(vx(v), c(-two.v)))
                .basicF("(@two + $x)", "@two + $x", v -> two.v + x(v), v -> new Add(two.c, vx(v)))
                .basicF(
                        "((@one + @two) + @three)",
                        "@one + @two + @three",
                        v -> one.v + two.v + three.v,
                        v -> new Add(new Add(one.c, two.c), three.c)
                )
                .basicF(
                        "(@one + (@two * @three))",
                        "@one + @two * @three",
                        v -> one.v + two.v * three.v,
                        v -> new Add(one.c, new Multiply(two.c, three.c))
                )
                .basicF(
                        "(@one - (@ten * @three))",
                        "@one - @ten * @three",
                        v -> one.v - ten.v * three.v,
                        v -> new Subtract(one.c, new Multiply(ten.c, three.c))
                )
                .basicF(
                        "(@one + (@two + @three))",
                        "@one + @two + @three",
                        v -> one.v + two.v + three.v,
                        v -> new Add(one.c, new Add(two.c, three.c))
                )
                .basicF(
                        "((@one - @two) - @three)",
                        "@one - @two - @three",
                        v -> one.v - two.v - three.v,
                        v -> new Subtract(new Subtract(one.c, two.c), three.c)
                )
                .basicF(
                        "(@one - (@two - @three))",
                        "@one - (@two - @three)",
                        v -> one.v - (two.v - three.v),
                        v -> new Subtract(one.c, new Subtract(two.c, three.c))
                )
                .basicF(
                        "((@one * @two) * @three)",
                        "@one * @two * @three",
                        v -> one.v * two.v * three.v,
                        v -> new Multiply(new Multiply(one.c, two.c), three.c)
                )
                .basicF(
                        "(@one * (@two * @three))",
                        "@one * @two * @three",
                        v -> one.v * two.v * three.v,
                        v -> new Multiply(one.c, new Multiply(two.c, three.c))
                )
                .basicF(
                        "((@ten / @two) / @three)",
                        "@ten / @two / @three",
                        v -> ten.v / two.v / three.v,
                        v -> new Divide(new Divide(ten.c, two.c), three.c)
                )
                .basicF(
                        "(@ten / (@three / @two))",
                        "@ten / (@three / @two)",
                        v -> ten.v / (three.v / two.v),
                        v -> new Divide(ten.c, new Divide(three.c, two.c))
                )
                .basicF(
                        "(@ten * (@three / @two))",
                        "@ten * (@three / @two)",
                        v -> ten.v * (three.v / two.v),
                        v -> new Multiply(ten.c, new Divide(three.c, two.c))
                )
                .basicF(
                        "(@ten + (@three - @two))",
                        "@ten + @three - @two",
                        v -> ten.v + (three.v - two.v),
                        v -> new Add(ten.c, new Subtract(three.c, two.c))
                )
                .basicF(
                        "(($x * $x) + (($x - @one) / @ten))",
                        "$x * $x + ($x - @one) / @ten",
                        v -> x(v) * x(v) + (x(v) - one.v) / ten.v,
                        v -> new Add(new Multiply(vx(v), vx(v)), new Divide(new Subtract(vx(v), one.c), ten.c))
                )
                .basicF("($x * -1000000.0)", "$x * -1000000.0", v -> x(v) * v(-1e6), v -> new Multiply(vx(v), c(-1e6)))
                .basicF("($x * -1.0E12)", "$x * -1.0E12", v -> x(v) * v(-1e12), v -> new Multiply(vx(v), c(v(-1e12))))
                .basicF("(@ten / $x)", "@ten / $x", v -> ten.v / x(v), v -> new Divide(ten.c, vx(v)))
                .basicF("($x / $x)", "$x / $x", v -> x(v) / x(v), v -> new Divide(vx(v), vx(v)))

                .advancedF("($x - $x)", "$x - $x", v -> x(v) - x(v), v -> new Subtract(vx(v), vx(v)))
                .advancedF("(@one * $x)", "@one * $x", v -> one.v * x(v), v -> new Multiply(one.c, vx(v)))
                .advancedF("(@one / @two)", "@one / @two", v -> one.v / two.v, v -> new Divide(one.c, two.c))
                .advancedF("(@two + @one)", "@two + @one", v -> two.v + one.v, v -> new Add(two.c, one.c))
                .advancedF("($x - @one)", "$x - @one", v -> x(v) - one.v, v -> new Subtract(vx(v), one.c))
                .advancedF("(@one * @two)", "@one * @two", v -> one.v * two.v, v -> new Multiply(one.c, two.c))
                .advancedF("($x / @one)", "$x / @one", v -> x(v) / one.v, v -> new Divide(vx(v), one.c))
                .advancedF(
                        "(@one + (@two + @one))",
                        "@one + @two + @one",
                        v -> one.v + two.v + one.v,
                        v -> new Add(one.c, new Add(two.c, one.c))
                )
                .advancedF(
                        "($x - ($x - @one))",
                        "$x - ($x - @one)",
                        v -> x(v) - (x(v) - one.v),
                        v -> new Subtract(vx(v), new Subtract(vx(v), one.c))
                )
                .advancedF(
                        "(@two * ($x / @one))",
                        "@two * ($x / @one)",
                        v -> two.v * (x(v) / one.v),
                        v -> new Multiply(two.c, new Divide(vx(v), one.c))
                )
                .advancedF(
                        "(@two / ($x - @one))",
                        "@two / ($x - @one)",
                        v -> two.v / (x(v) - one.v),
                        v -> new Divide(two.c, new Subtract(vx(v), one.c))
                )
                .advancedF(
                        "((@one * @two) + $x)",
                        "@one * @two + $x",
                        v -> one.v * two.v + x(v),
                        v -> new Add(new Multiply(one.c, two.c), vx(v))
                )
                .advancedF(
                        "(($x - @one) - @two)",
                        "$x - @one - @two",
                        v -> x(v) - one.v - two.v,
                        v -> new Subtract(new Subtract(vx(v), one.c), two.c)
                )
                .advancedF(
                        "(($x / @one) * @two)",
                        "$x / @one * @two",
                        v -> x(v) / one.v * two.v,
                        v -> new Multiply(new Divide(vx(v), one.c), two.c)
                )
                .advancedF(
                        "((@two + @one) / @one)",
                        "(@two + @one) / @one",
                        v -> (two.v + one.v) / one.v,
                        v -> new Divide(new Add(two.c, one.c), one.c)
                )
                .advancedF(
                        "(@one + (@one + (@two + @one)))",
                        "@one + @one + @two + @one",
                        v -> one.v + one.v + two.v + one.v,
                        v -> new Add(one.c, new Add(one.c, new Add(two.c, one.c)))
                )
                .advancedF(
                        "($x - ((@one * @two) + $x))",
                        "$x - (@one * @two + $x)",
                        v -> x(v) - (one.v * two.v + x(v)),
                        v -> new Subtract(vx(v), new Add(new Multiply(one.c, two.c), vx(v)))
                )
                .advancedF(
                        "($x * (@two / ($x - @one)))",
                        "$x * (@two / ($x - @one))",
                        v -> x(v) * (two.v / (x(v) - one.v)),
                        v -> new Multiply(vx(v), new Divide(two.c, new Subtract(vx(v), one.c)))
                )
                .advancedF(
                        "($x / (@one + (@two + @one)))",
                        "$x / (@one + @two + @one)",
                        v -> x(v) / (one.v + two.v + one.v),
                        v -> new Divide(vx(v), new Add(one.c, new Add(two.c, one.c)))
                )
                .advancedF(
                        "((@one * @two) + (@two + @one))",
                        "@one * @two + @two + @one",
                        v -> one.v * two.v + two.v + one.v,
                        v -> new Add(new Multiply(one.c, two.c), new Add(two.c, one.c))
                )
                .advancedF(
                        "((@two + @one) - (@two + @one))",
                        "@two + @one - (@two + @one)",
                        v -> two.v + one.v - (two.v + one.v),
                        v -> new Subtract(new Add(two.c, one.c), new Add(two.c, one.c))
                )
                .advancedF(
                        "(($x - @one) * ($x / @one))",
                        "($x - @one) * ($x / @one)",
                        v -> (x(v) - one.v) * (x(v) / one.v),
                        v -> new Multiply(new Subtract(vx(v), one.c), new Divide(vx(v), one.c))
                )
                .advancedF(
                        "(($x - @one) / (@one * @two))",
                        "($x - @one) / (@one * @two)",
                        v -> (x(v) - one.v) / (one.v * two.v),
                        v -> new Divide(new Subtract(vx(v), one.c), new Multiply(one.c, two.c))
                )
                .advancedF(
                        "((($x - @one) - @two) + $x)",
                        "$x - @one - @two + $x",
                        v -> x(v) - one.v - two.v + x(v),
                        v -> new Add(new Subtract(new Subtract(vx(v), one.c), two.c), vx(v))
                )
                .advancedF(
                        "(((@one * @two) + $x) - @one)",
                        "@one * @two + $x - @one",
                        v -> one.v * two.v + x(v) - one.v,
                        v -> new Subtract(new Add(new Multiply(one.c, two.c), vx(v)), one.c)
                )
                .advancedF(
                        "(((@two + @one) / @one) * $x)",
                        "(@two + @one) / @one * $x",
                        v -> (two.v + one.v) / one.v * x(v),
                        v -> new Multiply(new Divide(new Add(two.c, one.c), one.c), vx(v))
                )
                .advancedF(
                        "($x / ($x - $x))",
                        "$x / ($x - $x)",
                        v -> x(v) / (x(v) - x(v)),
                        v -> new Divide(vx(v), new Subtract(vx(v), vx(v)))
                )
                .advancedF(
                        "(($x - $x) + @one)",
                        "$x - $x + @one",
                        v -> x(v) - x(v) + one.v,
                        v -> new Add(new Subtract(vx(v), vx(v)), one.c)
                )
                .advancedF(
                        "(($x - $x) / @two)",
                        "($x - $x) / @two",
                        v -> (x(v) - x(v)) / two.v,
                        v -> new Divide(new Subtract(vx(v), vx(v)), two.c)
                )
                .advancedF(
                        "(@two - (@two - (@one * $x)))",
                        "@two - (@two - @one * $x)",
                        v -> two.v - (two.v - one.v * x(v)),
                        v -> new Subtract(two.c, new Subtract(two.c, new Multiply(one.c, vx(v))))
                )
                .advancedF(
                        "(@one * (($x - $x) + @one))",
                        "@one * ($x - $x + @one)",
                        v -> one.v * (x(v) - x(v) + one.v),
                        v -> new Multiply(one.c, new Add(new Subtract(vx(v), vx(v)), one.c))
                )
                .advancedF(
                        "($x / (@two - (@one * $x)))",
                        "$x / (@two - @one * $x)",
                        v -> x(v) / (two.v - one.v * x(v)),
                        v -> new Divide(vx(v), new Subtract(two.c, new Multiply(one.c, vx(v))))
                )
                .advancedF(
                        "((@one * $x) + (@one / @two))",
                        "@one * $x + @one / @two",
                        v -> one.v * x(v) + one.v / two.v,
                        v -> new Add(new Multiply(one.c, vx(v)), new Divide(one.c, two.c))
                )
                .advancedF(
                        "(($x + $x) - (@one * $x))",
                        "$x + $x - @one * $x",
                        v -> x(v) + x(v) - one.v * x(v),
                        v -> new Subtract(new Add(vx(v), vx(v)), new Multiply(one.c, vx(v)))
                )
                .advancedF(
                        "((@one * $x) * (@one / @two))",
                        "@one * $x * (@one / @two)",
                        v -> one.v * x(v) * (one.v / two.v),
                        v -> new Multiply(new Multiply(one.c, vx(v)), new Divide(one.c, two.c))
                )
                .advancedF(
                        "((@one * $x) / ($x + $x))",
                        "@one * $x / ($x + $x)",
                        v -> one.v * x(v) / (x(v) + x(v)),
                        v -> new Divide(new Multiply(one.c, vx(v)), new Add(vx(v), vx(v)))
                )
                .advancedF(
                        "((($x - $x) / @two) + @two)",
                        "($x - $x) / @two + @two",
                        v -> (x(v) - x(v)) / two.v + two.v,
                        v -> new Add(new Divide(new Subtract(vx(v), vx(v)), two.c), two.c)
                )
                .advancedF(
                        "(($x / ($x - $x)) - @one)",
                        "$x / ($x - $x) - @one",
                        v -> x(v) / (x(v) - x(v)) - one.v,
                        v -> new Subtract(new Divide(vx(v), new Subtract(vx(v), vx(v))), one.c)
                )
                .advancedF(
                        "((@two - (@one * $x)) * @one)",
                        "(@two - @one * $x) * @one",
                        v -> (two.v - one.v * x(v)) * one.v,
                        v -> new Multiply(new Subtract(two.c, new Multiply(one.c, vx(v))), one.c)
                )
                .advancedF(
                        "(($x / ($x - $x)) / $x)",
                        "$x / ($x - $x) / $x",
                        v -> x(v) / (x(v) - x(v)) / x(v),
                        v -> new Divide(new Divide(vx(v), new Subtract(vx(v), vx(v))), vx(v))
                )
                .advancedF(
                        "((1.1E10 * $x) * (1.1E10 / 2.3E12))",
                        "1.1E10 * $x * (1.1E10 / 2.3E12)",
                        v -> v(1.1E10) * x(v) * (v(1.1E10) / v(2.3E12)),
                        v -> new Multiply(new Multiply(c(1.1E10), vx(v)), new Divide(c(1.1E10), c(2.3E12)))
                )
                .advancedF(
                        "((1.1E10 * $x) / ($x + $x))",
                        "1.1E10 * $x / ($x + $x)",
                        v -> v(1.1E10) * x(v) / (x(v) + x(v)),
                        v -> new Divide(new Multiply(c(1.1E10), vx(v)), new Add(vx(v), vx(v)))
                )
                .advancedF(
                        "((($x - $x) / 2.3E12) + 2.3E12)",
                        "($x - $x) / 2.3E12 + 2.3E12",
                        v -> (x(v) - x(v)) / v(2.3E12) + v(2.3E12),
                        v -> new Add(new Divide(new Subtract(vx(v), vx(v)), c(2.3E12)), c(2.3E12))
                )
                .advancedF(
                        "(($x / ($x - $x)) - 1.1E10)",
                        "$x / ($x - $x) - 1.1E10",
                        v -> x(v) / (x(v) - x(v)) - v(1.1E10),
                        v -> new Subtract(new Divide(vx(v), new Subtract(vx(v), vx(v))), c(1.1E10))
                )
                .advancedF(
                        "((2.3E12 - (1.1E10 * $x)) * 1.1E10)",
                        "(2.3E12 - 1.1E10 * $x) * 1.1E10",
                        v -> (v(2.3E12) - v(1.1E10) * x(v)) * v(1.1E10),
                        v -> new Multiply(new Subtract(c(2.3E12), new Multiply(c(1.1E10), vx(v))), c(1.1E10))
                )
                .advancedF(
                        "((@two / ($x - @one)) / @two)",
                        "@two / ($x - @one) / @two",
                        v -> two.v / (x(v) - one.v) / two.v,
                        v -> new Divide(new Divide(two.c, new Subtract(vx(v), one.c)), two.c)
                );
    }

    private static Variable vx(final List<String> vars) {
        return new Variable(vars.get(0));
    }

    private static Variable vy(final List<String> vars) {
        return new Variable(vars.get(1));
    }

    private static double x(final Map<String, Double> vars) {
        return vars.values().iterator().next();
    }

    private static double y(final Map<String, Double> vars) {
        return vars.values().stream().skip(1).findFirst().orElseThrow();
    }


    private static Const c(final double v) {
        return TYPE.constant(v(v));
    }

    private static double v(final double v) {
        return v;
    }

    private static ConstWrapper w(final double v) {
        return new ConstWrapper(v);
    }

    class ConstWrapper {
        private final Const c;
        private final double v;

        public ConstWrapper(final double v) {
            this.c = c(v);
            this.v = v;
        }
    }

    static void main(final String... args) {
        TripleExpression.SELECTOR
                .variant("DoubleMap", ExpressionTest.v(DoubleMapExpression::tester))
                .main(args);
    }
}
