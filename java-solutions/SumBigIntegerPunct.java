import java.math.BigInteger;

public class SumBigIntegerPunct {
    public static void main(String[] args) {
        BigInteger value = new BigInteger("0");
        long csost;
        StringBuilder buf = new StringBuilder();
        char fe;
        for (String cur_s : args) {
            csost = 0;
            buf = new StringBuilder();
            for (int i = 0; i < cur_s.length(); ++i) {
                fe = cur_s.charAt(i);
                if (Character.isDigit(fe) || fe == '-' || fe == '+') {
                    csost = 1;
                    buf.append(Character.toString(fe));
                }
                if (!(Character.isDigit(fe) || fe == '-' || fe == '+') || i == cur_s.length() - 1) {
                    if (csost != 0) {
                        BigInteger buga = new BigInteger(buf.toString());
                        value = value.add(buga);
                        buf = new StringBuilder();
                    }
                    csost = 0;
                }
            }
        }
        System.out.println(value);

    }
}
