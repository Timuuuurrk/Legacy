import java.util.*;

public class IntList {
    private int[] storage;
    private int len;
    private int sz;
    private int curpos;

    public IntList() {
        storage = new int[1];
        sz = 0;
        len = 1;
        curpos = 0;
    }

    public IntList(int val) {
        storage = new int[1];
        sz = 1;
        len = 1;
        curpos = 1;
        storage[0] = val;
    }

    public int get(int pos) {
        return storage[pos];
    }

    public IntList merge(IntList other) {
        IntList res = this;
        for (int i = 0; i < other.size(); ++i) {
            res.add(other.get(i));
        }
        return res;
    }

    public void set(int pos, int val) {
        storage[pos] = val;
    }

    public void add(int val) {
        if (curpos == len) {
            int mult = 2;
            storage = Arrays.copyOf(storage, mult * len);
            len *= mult;
        }
        storage[curpos++] = val;
        ++sz;
    }

    public int size() {
        return sz;
    }

    public int capacity() {
        return len;
    }

    public boolean isEmpt() {
        return sz == 0;
    }

}
