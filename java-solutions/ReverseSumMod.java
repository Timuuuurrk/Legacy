import java.util.Arrays;
import java.io.*;

public class ReverseSumMod {
    static public final int mult = 2;

    public static int modAr(int k, int mod) {
        return (k % mod + mod) % mod;
    }

    public static void main(String[] args) {
        StringBuilder buf = new StringBuilder();
        int mx = 0;
        int cnt1 = 0;
        int mod = 1_000_000_007;
        int[][] matr = new int[1][];
        int[] b = new int[1];
        int[] rows = new int[1];
        int cntmult = 0;
        try(Scaner cons=new Scaner()){
            while (cons.hasNextLine()) {
                while (cons.hasNextToken(false)) {
                    int have;
                    have = Integer.parseInt(cons.nextToken(false));
                    rows[cntmult] = have;
                    b[cntmult] += have % mod;
                    b[cntmult] = modAr(b[cntmult], mod);
                    ++cntmult;
                    if (cntmult == b.length) {
                        b = Arrays.copyOf(b, b.length * mult);
                    }
                    if (cntmult == rows.length) {
                        rows = Arrays.copyOf(rows, rows.length * mult);
                    }
                }
                rows = Arrays.copyOf(rows, cntmult);
                if (cntmult > mx) {
                    mx = cntmult;
                }
                matr[cnt1++] = rows;
                if (cnt1 == matr.length) {
                    matr = Arrays.copyOf(matr, matr.length * mult);
                }
                rows = new int[1];
                cntmult = 0;
            }
        } catch (IOException e) {
            System.err.println(e.getMessage());
        }
        b = Arrays.copyOf(b, mx);
        matr = Arrays.copyOf(matr, cnt1);
        int[] a = new int[matr.length];
        for (int i = 0; i < matr.length; ++i) {
            int su = 0;
            for (int j = 0; j < matr[i].length; ++j) {
                su = su + matr[i][j] % mod;
                su = modAr(su, mod);
            }
            a[i] = su;
        }

        for (int i = 0; i < matr.length; ++i) {
            for (int j = 0; j < matr[i].length; ++j) {
                int su = modAr(modAr(a[i] + b[j], mod) - matr[i][j] % mod, mod);
                System.out.print(su + " ");
            }
            System.out.println();
        }
    }
}
