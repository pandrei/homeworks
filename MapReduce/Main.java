
import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileReader;
import java.io.FileWriter;
import java.io.IOException;

import java.math.BigDecimal;

import java.util.Collections;
import java.util.HashMap;

import java.util.LinkedList;
import java.util.Scanner;
import java.util.Map.Entry;

public class Main {
	private int docNum;
	private int srchWrdNum;
	private int mostFreq;
	private int chunksize;
	private int retFilesNum;
	private String[] searchedWords;
	private String[] filesNames;
	private String[] filesContent;
	public LinkedList<LinkedList<HashMap<String, Float>>> partvects;
	public int[] totals;

	public Main() {
		partvects = new LinkedList<LinkedList<HashMap<String, Float>>>();
	}

	/* creeaza <nr_doc> liste de liste de hashmap-uri. */
	public void makeLists() {
		totals = new int[docNum];
		for (int i = 0; i < docNum; i++) {
			totals[i] = 0;
			LinkedList<HashMap<String, Float>> l = new LinkedList<HashMap<String, Float>>();
			partvects.add(l);
		}
		// System.out.println(partvects.size());
	}

	/* functii pentru utilitati de baza */
	public int getDocsNumber() {
		return docNum;
	}

	public String getFileContent(int index) {
		return filesContent[index];
	}

	public int getFileSize(int index) {
		return filesContent[index].length();
	}

	public int getChunkSize() {
		return chunksize;
	}
	/* functia trunk, n - am reusit sa o gasesc prin documentatia Java */
	public static float round(float unrounded, int precision, int roundingMode) {
		BigDecimal bd = new BigDecimal(unrounded);
		BigDecimal rounded = bd.setScale(precision, roundingMode);
		return rounded.floatValue();
	}
	/* doar pentru debug, nu am mai sters - o */
	public void debug(int option) {
		if (option == 1) {
			System.out.println("searched words number = " + srchWrdNum);
			System.out.print("Searched words:");
			for (int i = 0; i < srchWrdNum; i++) {
				System.out.print(" " + searchedWords[i]);
			}
			System.out.println("data chunk size = " + chunksize);
			System.out.println("most frequent words = " + mostFreq);
			System.out.println("number of documents returned:" + retFilesNum);
			System.out.println("number of files existent:" + docNum);
			// System.out.println("");
			System.out.print("filesNames:");
			for (int i = 0; i < docNum; i++) {
				System.out.print(" " + filesNames[i]);
			}
		}
		if (option == 2) {
			for (int i = 0; i < docNum; i++) {
				System.out.print(filesContent[i]);
			}
		}
	}
	/* prelucrare fisier initial si updatare membrii clasei Main */
	public void parseInputFile(String fname) throws IOException {
		try {
			FileReader f = new FileReader(fname);
			BufferedReader b = new BufferedReader(f);
			String ln = new String();
			ln = b.readLine();
			srchWrdNum = Integer.parseInt(ln);
			ln = b.readLine();
			searchedWords = ln.split(" ");
			ln = b.readLine();
			chunksize = Integer.parseInt(ln);
			ln = b.readLine();
			mostFreq = Integer.parseInt(ln);
			ln = b.readLine();
			retFilesNum = Integer.parseInt(ln);
			ln = b.readLine();
			docNum = Integer.parseInt(ln);
			filesNames = new String[docNum];
			for (int i = 0; i < docNum; i++) {
				filesNames[i] = b.readLine();
			}
			b.close();

		} catch (FileNotFoundException e) {
			// TODO Auto-generated catch block
			System.out
					.println("File input.txt not found in current directory!");
			// e.printStackTrace();
			System.exit(-1);
		}
	}
	/* Citire din fisier, regex-ul \\A face match pe inceputul fisierului
	 * pana la un "inceput ilogic urmator" deci va citi tot fisierul.
	 */
	public void readFiles() throws FileNotFoundException {
		filesContent = new String[docNum];
		for (int i = 0; i < docNum; i++) {
			filesContent[i] = new Scanner(new File(filesNames[i]))
					.useDelimiter("\\A").next();
			filesContent[i] = filesContent[i].toLowerCase();
		}
	}

	/**
	 * @param args
	 * @throws IOException
	 * @throws InterruptedException
	 */
	@SuppressWarnings("deprecation")
	public static void main(String[] args) throws IOException,
			InterruptedException {
		Main m = new Main();
		m.parseInputFile(args[1]);
		m.readFiles();
		m.makeLists();

		// m.debug(1);
		WorkPool wp = new WorkPool(100);
		
		/* Main-ul ( master-ul ) determina chunksize-urile efective si adauga toate
		 * taskurile in workpool. Urmand ca workerii sa isi ia taskurile fara a mai
		 * cree altele.
		 * (adica : daca substringul se opreste in mijlocul unui cuvant va merge pana 
		 * la sfarsitul cuvantului.
		 * Asta exclude in mod implicit verificarea inceputului  */
		for (int i = 0; i < m.getDocsNumber(); i++) {
			int posS = 0;
			int posF = posS + m.getChunkSize();
			while ((posF < m.getFileSize(i) - 1)
					&& Character.isLetter(m.getFileContent(i).charAt(posF))) {
				posF++;
			}
			while (posF < m.getFileSize(i) && posS != posF) {
				String part = new String(m.getFileContent(i).substring(posS,
						posF));
				PartialSolution p = new PartialSolution(part, i, false);
				wp.putWork(p);
				posS = posF;
				posF = posS + m.getChunkSize();
				while ((posF < m.getFileSize(i) - 1)
						&& Character.isLetter(m.getFileContent(i).charAt(posF))) {
					posF++;
				}
				posF = Math.min(posF, m.getFileSize(i) - 1);
			}
		}
		/* alocare workeri */
		Worker wrks[] = new Worker[Integer.parseInt(args[0])];
		for (int i = 0; i < Integer.parseInt(args[0]); i++) {
			wrks[i] = new Worker(wp, m);

		}
		for (int i = 0; i < Integer.parseInt(args[0]); i++) {
			wrks[i].start();

		}

		/* blocam masterul pana cand toti workerii au terminat 
		 * de efectuat map 
		 */
		boolean rdy = false;
		while (rdy == false) {
			rdy = true;
			for (int i = 0; i < Integer.parseInt(args[0]); i++) {
				if (wrks[i].wp.ready == false)
					rdy = false;
			}
			Thread.sleep(1);
		}

		WorkPool rwp = new WorkPool(100);

		Worker rwrks[] = new Worker[Integer.parseInt(args[0])];
		for (int i = 0; i < Integer.parseInt(args[0]); i++) {
			rwrks[i] = new Worker(rwp, m);

		}

		for (int i = 0; i < Integer.parseInt(args[0]); i++) {
			rwrks[i].start();

		}

		boolean done = false;
		/* blocam main-ul(masteru) pana cand se termina de facut operatiile
		 * de reduce( adica pana ramane un singur vector partial )
		 */
		while (done != true) {
			done = true;
			for (int i = 0; i < m.getDocsNumber(); i++) {

				if (m.partvects.get(i).size() > 1) {
					PartialSolution p = new PartialSolution("", i, true);
					rwp.putWork(p);
					done = false;
				}
			}
			while (!rwp.tasks.isEmpty()) {
				Thread.sleep(1);
			}

		}
		/* oprim threadurile workeri */
		for (int i = 0; i < Integer.parseInt(args[0]); i++) {
			rwrks[i].stop();

		}

	    /* Pun toate elementele din Hashmapuri in Liste, sortez listele, le adaug la o lista de liste 
	     * de perechi<String, Float> ;
	     * */
		LinkedList<LinkedList<Entry<String, Float>>> sorted = new LinkedList<LinkedList<Entry<String, Float>>>();
		for (int i = 0; i < m.getDocsNumber(); i++) {
			LinkedList<Entry<String, Float>> l = new LinkedList<Entry<String, Float>>();
			l.addAll(m.partvects.get(i).getFirst().entrySet());
			for (Entry<String, Float> itr : l) {
				itr.setValue(Main.round(((itr.getValue() / m.totals[i]) * 100), 2,
						BigDecimal.ROUND_DOWN));
			}
			Collections.sort(l, new EntryComparator());
			sorted.add(l);
		}
		LinkedList<LinkedList<Entry<String, Float>>> FilteredSorted = new LinkedList<LinkedList<Entry<String, Float>>>();

		/* pastrez cel mult N + nr de cuvinte cu aceeasi frecventa ca al N-lea 
		 * pe care le pun in FilteredSorted, eliberez sorted.
		 * */
		for (int i = 0; i < m.getDocsNumber(); i++) {
			int total = 0;
			LinkedList<Entry<String, Float>> l = new LinkedList<Entry<String, Float>>();
			while (total < m.mostFreq
					|| (sorted.get(i).get(0).getValue() == sorted.get(i).get(1)
							.getValue())) {
				l.add(sorted.get(i).poll());
				total++;
			}
			FilteredSorted.add(l);
		}
		sorted.clear();
		
		FileWriter fstream = new FileWriter(args[2]);
		  BufferedWriter out = new BufferedWriter(fstream);

		
		 /* afisez in fisier conform cu formatul cerut
		  * asa cum am vazut in postul de pe forum, afisez
		  * primele X documente in ordinerea in care apar in input.txt */
		out.write("Rezultate pentru: (");
		for (int i = 0; i < m.searchedWords.length - 1; i++) {
			out.write(m.searchedWords[i] + ", ");
		}
		out.write(m.searchedWords[m.searchedWords.length - 1] + ")" + "\n");
		for (int i = 0; i < m.retFilesNum; i++) {

			out.write(m.filesNames[i] + " (");
			lp: for (String s : m.searchedWords) {
				boolean found = false;
				float val = 0;
				for (Entry<String, Float> it : FilteredSorted.get(i)) {
					if (it.getKey().equals(s)) {
						found = true;
						val = it.getValue();
					}
				}
				if (found == true) {
					if (s.equals(m.searchedWords[m.searchedWords.length - 1]))
						out.write(val + "");
					else
						out.write(val + ", ");
				} else
					break lp;
			}
			out.write(")\n");
		}
		  //Close the output stream
		  out.close();
	}
}
