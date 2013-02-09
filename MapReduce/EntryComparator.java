import java.util.Comparator;
import java.util.Map.Entry;


public class EntryComparator implements Comparator{
   /* Clasa folosita pentru a sorta Entry-uri */
    public int compare(Object emp1, Object emp2){
   

       
    	Entry<String,Float> Entry1 = ((Entry<String,Float>)emp1);        
    	Entry<String,Float> Entry2 = ((Entry<String,Float>)emp2);
       
        if(Entry1.getValue() < Entry2.getValue())
            return 1;
        else if(Entry1.getValue() > Entry2.getValue())
            return -1;
        else
            return 0;    
    }
   
}
