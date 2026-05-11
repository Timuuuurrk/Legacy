public class WsppEntry {
    private int entryCount;
    private int firstEntry;
    private final IntList positions;
    public int getEntryCount(){
        return entryCount;
    }
    public int getFirstEntry(){
        return firstEntry;
    }
    public IntList getPositions(){
        return positions;
    }
    public int getPositionsIndexed(int ind){
        return positions.get(ind);
    }
    public int getPositionsSize(){
        return positions.size();
    }
    public WsppEntry(int count, int first, IntList ins) {
        entryCount = count;
        firstEntry = first;
        positions = ins;
    }

    public WsppEntry merge(WsppEntry other) {
        WsppEntry res = this;
        res.entryCount += other.entryCount;
        res.firstEntry = Integer.min(res.firstEntry, other.firstEntry);
        res.positions.merge(other.positions);
        return res;
    }
}
