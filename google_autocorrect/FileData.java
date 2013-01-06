import java.util.ArrayList;
import java.util.Scanner;

public class FileData {
    
    public static String solve(ArrayList<obiect> dict, String input)
    {
        /*
         * Functia solve rezolva efectiv problema, foloseste un vector de holder
         * pentru a efectua dinamica. Se puteau folosi 4 matrici (sau o matrice
         * de holder, dar mi s-a parut mai eficient asa, cel putin d.p.d.v.
         * al spatiului)
         * Incep cu stringul initial pentru care calculez distanta levenshtein
         * fata de toate prefixele cuvintelor din dictionar, dupa care "sterg" 
         * mereu primul caracter din stringul initial; 
		 * Probabil era mai eficient sa incep intai cu stringul cu un caracter si
		 * apoi sa adaug.
		 * Ca idee, algoritmul seamana intr-o oarecare masura cu knapsack 0/1.
         */
      String output = null;
      String cnt;
      int ln = input.length();
      int sz = dict.size();
      holder vect[] = new holder [2 * ln];
      
      for (int i = 0; i < 2 * ln; i++){
        vect[i] = new holder(127, 0, 0, "");
      }
      
      vect[0].dist = 0;
      int[] anterior = new int[2 * ln];
      int[] poz = new int[2 * ln];
      for (int i = 0 ; i <ln ;  i++)
      {
        cnt = input.subSequence(i, input.length()).toString();
        for (int j = 0; j < sz; j++)
        {
          int[] lv = levenshtein(dict.get(j).cuvant, cnt);
          for (int k = 1; k < lv.length; k++)
          {
            /*
             * In functie de cazurile problemei,gasesc cuvantul cel mai potrivit
             * Cazul 1. se alege cuvantul cu abaterea totala minima
             */
            if ((vect[i].dist + lv[k]) < vect[i + k].dist)
            {
              vect[i + k].set(vect[i].dist + lv[k],
                vect[i].freq + dict.get(j).freq, 
                vect[i].word_count + 1,
                vect[i].curent_string.concat(dict.get(j).cuvant));
               poz[i + k] = j;
              anterior[i + k] = i;               
            }
            else if ((vect[i].dist + lv[k]) == vect[i + k].dist)
                 {
                     /*
                      * Cazul 2. dintre posibilitatile cu abaterea minima
                      * se alege cea cu un numar minim de cuvinte
                      */
                   if ((vect[i].word_count + 1) < vect[i + k].word_count)
                   {
                     vect[i + k].set(vect[i].dist + lv[k],
                       vect[i].freq+dict.get(j).freq,
                       vect[i].word_count + 1,
                       vect[i].curent_string.concat(dict.get(j).cuvant));
                              
                     poz[i + k] = j;
                     anterior[i + k] = i;
                   }
                   else if ((vect[i].word_count + 1) == vect[i + k].word_count)
                      {
                          /*
                           * Cazul 3. dintre variantele ramase de la cazul 2
                           * se alege cea cu frecventa totala maxima
                           */
                          if ((vect[i].freq+dict.get(j).freq) > vect[i+k].freq)
                          {
                            vect[i + k].set(vect[i].dist + lv[k], 
                              vect[i].freq+dict.get(j).freq,
                              vect[i].word_count + 1,
                              vect[i].curent_string.concat(dict.get(j).cuvant));
                            poz[i + k] = j;
                            anterior[i + k] = i;
                          }
                          else 
                            if ((vect[i].freq+dict.get(j).freq)==vect[i+k].freq)
                            {
                                /*
                                 * Cazul 4. ultima posibilitate se alege cea in 
                                 * care sirul obtinut este minim lexicografic
                                 */
                              if (vect[i].curent_string.
                                      concat(dict.get(j).cuvant).
                                      compareTo(vect[i+k].curent_string) <= 0)
                              {
                                vect[i+k].set(vect[i].dist+lv[k],
                                  vect[i].freq+dict.get(j).freq,
                                  vect[i].word_count+1,
                                  vect[i].curent_string.concat
                                        (dict.get(j).cuvant));
                                poz[i + k] = j;
                                anterior[i + k] = i;
                              }
                            }
                      }
                 }
              }
          }
      }
      int itr = ln;
      output = "";
      // alcatuiesc stringul din cuvintele memorate
      while (itr != 0)
      {
          output = dict.get(poz[itr]).cuvant + " " + output;  
          itr = anterior[itr];
      }    
      return output;
  }
    
    public static int[] levenshtein (String t, String s)
    {   
        /*
         * Fata de algoritmul dat ca pseudocod pe wikipedia, am facut doua optimizari
         * 1) Nu sunt necesare decat randul curent si randul precedent pentru a calcula 
         * dinamica , deci in loc de toata matricea retin 2 array-uri
         * 2) pe ultimul rand in matrice, se gaseste distanta de la stringul dat ca input
         * la toate prefixele celui cu care se compara. 
         * Astfel, in loc de a parcurge dictionarul in O(n^2); (n=dictionar.size) 
         * il parcurg in O(n);
         */
        int n = s.length();
        int m = t.length();		
        int p[] = new int[n+1];
        int d[] = new int[n+1];
        int _d[], i, j, cost; 
        char t_j;
        for (i = 0; i <= n; i++)
        {
            p[i] = i;
        }		
        for (j = 1; j <= m; j++)
        {
            t_j = t.charAt(j-1);
            d[0] = j;		
            for (i = 1; i <= n; i++)
            {
                cost = s.charAt(i-1) == t_j ? 0 : 1;				
                d[i] = Math.min(Math.min(d[i-1]+1, p[i]+1),  p[i-1]+cost);  
            }
            _d = p;
            p = d;
            d = _d;
        } 
    return p;
    }
    
    public static void main(String[] args)
    {
        String file_name = "dict.txt";
        ReadFile file = new ReadFile(file_name);
        ArrayList<obiect> o = file.OpenFile();
        String solv;
        
        // Citire de la tastatura
        Scanner in = new Scanner(System.in);
        String input;
        input = in.nextLine();
        in.close();
        //String input = "schaprobl mcanbe easilisolvedofcourseitcanitisofutmostimportanc";
        
        input = input.replaceAll(" ", "");
        solv = solve(o, input);
        System.out.println(solv.substring(0, solv.length() - 1));
    } 
}
