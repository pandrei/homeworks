import java.io.BufferedReader;
import java.io.FileReader;
import java.util.ArrayList;
/*
 * Clasa ReadFile citeste din fisier dictionarul dict.txt si fiecare linie a
 * dictionarului este introdusa in vectorul de tipul <obiect>.
 */
public class ReadFile {
    private String path;
    
    public ReadFile(String file_path)
    {
        path = file_path;
    }
    
    public ArrayList<obiect> OpenFile() 
    {
        ArrayList<obiect> obs = new ArrayList<>();
        //FileReader fil = null;
        FileReader fil;
        try {
            fil = new FileReader("dict.txt");
            BufferedReader br = new BufferedReader(fil);
            String line;
            //String tokens[] = new String[2];
            String tokens[];
            obiect curent;
            while ((line = br.readLine()) != null)  
                { 
                    tokens = line.split(" ");
                    curent = new obiect(tokens[0],Integer.parseInt(tokens[1]));
                    obs.add(curent);
                }                                                    
             } catch (Exception e){
                System.err.println("Error: " + e.getMessage());
                }
            return obs;
    }
}
