import java.io.IOException;
import java.util.Scanner;

public class Sum {
    public static void main(String[] args) {
        int su=0;
        for(String cur:args){
            try(Scanner cons=new Scanner(cur)){
                while(cons.hasNextInt()){
                    su+=cons.nextInt();
                }
            }
        }
        System.out.println(su);
    }
}