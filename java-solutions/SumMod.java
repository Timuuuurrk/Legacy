//
// Source code recreated from a .class file by IntelliJ IDEA
// (powered by FernFlower decompiler)
//

import java.util.Arrays;
import java.util.Scanner;

public class SumMod {

    public static void main(String[] var0) {
        Scanner var1 = new Scanner(System.in);
        StringBuilder var2 = new StringBuilder();
        int var6 = 0;

        while(true) {
            boolean var7;
            do {
                Scanner var3;
                int var4;
                if (!var1.hasNextLine()) {
                    int var8 = 0;
                    boolean var9 = false;
                    var1 = new Scanner(var2.toString());
                    int[][] var10 = new int[1][];

                    int[] var11;
                    while(var1.hasNextLine()) {
                        var3 = new Scanner(var1.nextLine());
                        var11 = new int[1];
                        int var17 = 0;

                        while(var3.hasNextInt()) {
                            var4 = var3.nextInt();
                            var11[var17++] = var4;
                            if (var17 == var11.length) {
                                var11 = Arrays.copyOf(var11, var11.length * 2);
                            }
                        }

                        var11 = Arrays.copyOf(var11, var17);
                        if (var17 > var6) {
                            var6 = var17;
                        }

                        var10[var8++] = var11;
                        if (var8 == var10.length) {
                            var10 = (int[][])Arrays.copyOf(var10, var10.length * 2);
                        }
                    }

                    var11 = new int[var10.length];
                    int[] var12 = new int[var6];
                    int var13 = 1000000007;

                    int var14;
                    int var15;
                    int var16;
                    for(var15 = 0; var15 < var10.length; ++var15) {
                        var14 = 0;

                        for(var16 = 0; var16 < var10[var15].length; ++var16) {
                            var14 += var10[var15][var16];
                            if (var14 >= var13) {
                                var14 %= var13;
                            }
                        }

                        var11[var15] = var14;
                    }

                    for(var15 = 0; var15 < var6; ++var15) {
                        var14 = 0;

                        for(var16 = 0; var16 < var10.length; ++var16) {
                            if (var15 < var10[var16].length) {
                                var14 += var10[var16][var15];
                                if (var14 >= var13) {
                                    var14 %= var13;
                                }
                            }
                        }

                        var12[var15] = var14;
                    }

                    for(var15 = 0; var15 < var10.length; ++var15) {
                        for(var16 = 0; var16 < var10[var15].length; ++var16) {
                            System.out.print(var11[var15] + var12[var16]);
                            System.out.print(" ");
                        }

                        System.out.println();
                    }

                    var10 = (int[][])Arrays.copyOf(var10, var8);
                    return;
                }

                var7 = false;
                var3 = new Scanner(var1.nextLine());

                while(var3.hasNextInt()) {
                    var7 = true;
                    var4 = var3.nextInt();
                    var2.append(Integer.toString(var4)).append(" ");
                }
            } while(!var1.hasNextLine() && var7);

            var2.append("\n");
        }
    }
}
