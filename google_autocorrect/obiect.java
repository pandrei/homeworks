/*
 * obiect reprezinta o pereche < cuvant, freq > echivalenta cu o linie din dictionar.
 */

public class obiect {
    String cuvant;
    int freq;
    
    public obiect()
    {
        this.cuvant = null;
        this.freq = 0;
    }
    
    public obiect(String cuv, int frecv)
    {
        this.cuvant = cuv;
        this.freq = frecv;
    }
    @Override
    public String toString(){
        return this.cuvant + "  "+ this.freq;
    }
}
