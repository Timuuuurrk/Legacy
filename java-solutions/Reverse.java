import java.io.IOException;
import java.util.Arrays;

public class Reverse {
    static public final int mult = 2;

    public static void main(String[] args) {
        int mx = 0;
        int cnt1 = 0;
        int[][] matr = new int[1][];
        int[] rows = new int[1];
        int cntmult = 0;
        try(Scaner cons=new Scaner()){
            while (cons.hasNextLine()) {
                while (cons.hasNextToken(false)) {
                    rows[cntmult++] = Integer.parseInt(cons.nextToken(false));
                    //System.err.print(rows[cntmult-1]+" ");
                    if (cntmult == rows.length) {
                        rows = Arrays.copyOf(rows, rows.length * mult);
                    }
                }
                //  System.err.println();
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
            return;
        }
        matr = Arrays.copyOf(matr, cnt1);
        for (int i = matr.length - 1; i > -1; --i) {
            for (int j = matr[i].length - 1; j > -1; --j) {
                System.out.print(matr[i][j]);
                if (j != 0) {
                    System.out.print(" ");
                }
            }
            System.out.println();
        }
    }

}
