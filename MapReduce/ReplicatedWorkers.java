import java.util.HashMap;
import java.util.LinkedList;
import java.util.Map.Entry;

/**
 * Clasa ce reprezinta o solutie partiala pentru problema de rezolvat. Aceste
 * solutii partiale constituie task-uri care sunt introduse in workpool.
 */
class PartialSolution {
	String str;
	private int findex;
	private boolean type;

	public PartialSolution() {
	}

	public PartialSolution(String s, int i, boolean t) {
		str = new String(s);
		findex = i;
		type = t;
	}

	public String getString() {
		return str;
	}

	public int getIndex() {
		return findex;
	}

	public boolean getType() {
		return type;
	}
}

/**
 * Clasa ce reprezinta un thread worker.
 */
class Worker extends Thread {
	WorkPool wp;
	Main data;
	LinkedList<HashMap<String, Integer>> list;

	public Worker(WorkPool workpool, Main m) {
		this.wp = workpool;
		this.data = m;

	}

	/**
	 * Procesarea unei solutii partiale. Aceasta poate implica generarea unor
	 * noi solutii partiale care se adauga in workpool folosind putWork(). Daca
	 * s-a ajuns la o solutie finala, aceasta va fi afisata.
	 */
	void processPartialSolution(PartialSolution ps) {
		/*
		 * type se refera la tipul solutiei. Adica : false = map | true = reduce
		 */
		if (ps.getType() == false) {
			HashMap<String, Float> partial = new HashMap<String, Float>();
			int wordstotal = 0;
			String s = ps.getString();
			/*
			 * split dupa orice nu e format din a-z( nu mai trebuie A-Z pt ca am
			 * facut lowercase )
			 */
			String[] words = s.split("[^a-z]");
			for (String word : words) {
				if (word.length() > 0) {
					wordstotal++;
					/* daca n am gasit cuvantul in v. partial */
					if (!partial.containsKey(word)) {
						partial.put(word, 1f);
					} else {
						/*
						 * daca am gasit cuvantul in v. partial updatam valoarea
						 */
						float val = partial.get(word);
						val = val + 1;
						partial.put(word, val);
					}
				}
			}
			/* updatam vectorii partiali din main si numarul total
			 * de aparitii ( in mod sincronizat)
			 */
			synchronized (data) {
				data.partvects.get(ps.getIndex()).push(partial);
				data.totals[ps.getIndex()] = data.totals[ps.getIndex()]
						+ wordstotal;
			}
			/* am avut nevoie de bucata asta pentru a opri workerii */
			if (wp.tasks.size() == 0)
				wp.ready = true;
		}
		/* pentru reduce */
		if (ps.getType() == true) {
			/* luam vectorii din main */
			HashMap<String, Float> fst = data.partvects.get(ps.getIndex())
					.poll();
			HashMap<String, Float> snd = data.partvects.get(ps.getIndex())
					.poll();
			/* facem "merge" la hashmap-uri si punem rezultatul inapoi
			 * in vectorul de vectori partiali 
			 */
			for (Entry<String, Float> itr : fst.entrySet()) {
				if (snd.containsKey(itr.getKey())) {
					snd.put(itr.getKey(),
							itr.getValue() + snd.get(itr.getKey()));
				} else {
					snd.put(itr.getKey(), itr.getValue());
				}
			}
			data.partvects.get(ps.getIndex()).add(snd);
		}
	}

	public void run() {
		// System.out.println("Thread-ul worker " + this.getName() +
		// " a pornit...");
		while (true) {
			PartialSolution ps = wp.getWork();
			if (ps == null)
				break;

			processPartialSolution(ps);

		}
		// System.out.println("Thread-ul worker " + this.getName() +
		// " s-a terminat...");
	}

}

public class ReplicatedWorkers {

	public static void main(String args[]) {
		// ..
	}

}
