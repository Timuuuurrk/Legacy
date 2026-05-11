import java.util.Arrays;
import java.io.*;

public class ReverseMaxModOctDec {
    static public final int mult = 2;

    public static int modAr(int k, int mod) {
        return (k % mod + mod) % mod;
    }

    public static void main(String[] args) {
        int mx = 0;
        int cnt1 = 0;
        int mod = 1_000_000_007;
        int[][] matr = new int[1][];
        int[] b = new int[1];
        //  cons=new Scaner("3o 2o",false);
        int[] rows = new int[1];
        int cntMult = 0;
        try (Scaner cons = new Scaner()) {
            while (cons.hasNextLine()) {
                while (cons.hasNextToken(false)) {
                    rows[cntMult] = Integer.parseInt(cons.nextToken(false));
                    //System.err.print(rows[cntMult]+" ");
                    if (b[cntMult] == 0) {
                        b[cntMult] = cnt1 + 1;
                    } else {
                        if (modAr(rows[cntMult], mod) > modAr(matr[b[cntMult] - 1][cntMult], mod)) {
                            b[cntMult] = cnt1 + 1;
                        }
                    }
                    ++cntMult;
                    if (cntMult == b.length) {
                        b = Arrays.copyOf(b, b.length * mult);
                    }
                    if (cntMult == rows.length) {
                        rows = Arrays.copyOf(rows, rows.length * mult);
                    }
                }
                //System.err.println();
                rows = Arrays.copyOf(rows, cntMult);
                if (cntMult > mx) {
                    mx = cntMult;
                }
                matr[cnt1++] = rows;
                if (cnt1 == matr.length) {
                    matr = Arrays.copyOf(matr, matr.length * mult);
                }
                rows = new int[1];
                cntMult = 0;
            }
        } catch (IOException e) {
            System.err.println(e.getMessage());
            return;
        }
        b = Arrays.copyOf(b, mx);
        matr = Arrays.copyOf(matr, cnt1);
        int[] a = new int[matr.length];
        for (
                int i = 0;
                i < matr.length; ++i) {
            a[i] = 1;
            for (int j = 1; j < matr[i].length; ++j) {
                if (modAr(matr[i][j], mod) > modAr(matr[i][(int) a[i] - 1], mod)) {
                    a[i] = j + 1;
                }
            }
        }

        for (
                int i = 0;
                i < matr.length; ++i) {
            for (int j = 0; j < matr[i].length; ++j) {
                if (modAr(matr[i][(int) a[i] - 1], mod) > modAr(matr[b[j] - 1][j], mod)) {
                    System.out.print(Integer.toOctalString(matr[i][a[i] - 1]) + "o ");
                } else {
                    System.out.print(Integer.toOctalString(matr[b[j] - 1][j]) + "o ");
                }
            }
            System.out.println();
        }
    }
}
