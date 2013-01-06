/*
 * Clasa holder are rolul unei structuri din C care memoreaza cele 4 campuri
 * relevante pentru criteriile de sortare. Folosesc un Array de holder  
 * in loc sa am 4 array-uri separate.
 */

public class holder {
    int dist;
    int freq;
    int word_count;
    String curent_string;        

    public holder(){
        
    }

    public holder(int d, int f, int w, String c)
    {
        this.dist = d;
        this.freq = f;
        this.word_count = w;
        this.curent_string = c;     
    }

    public void set(int d, int f, int w, String c)
    {
        this.dist = d;
        this.freq = f;
        this.word_count = w;
        this.curent_string = c;
    }

    @Override
    public String toString(){
        return "{" + " " + this.dist + " , " + this.freq + " , " + this.word_count + " , " + this.curent_string+ " }";
    }
}
