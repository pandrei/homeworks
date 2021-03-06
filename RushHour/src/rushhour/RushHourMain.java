package rushhour;

import java.io.File;
import java.io.IOException;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.Writer;
import java.util.HashSet;
import java.util.List;
import java.util.PriorityQueue;
import java.util.Scanner;
import java.util.Set;

public class RushHourMain {

	public static void main(String[] args) {
		try {
			State initialState = new State(new Scanner(new File("in.txt")));
			aStar(initialState);
		} catch (IOException e) {
			e.printStackTrace();
		}
	}

	// functia de generare a numarului minim de mutari
	public static void aStar(State initialState) {
		Set<State> closedSet = new HashSet<State>();
		PriorityQueue<State> openSet = new PriorityQueue<State>(50, State.getComparator());
		openSet.add(initialState);
		initialState.g = 0;
		initialState.f = State.heuristicDistance(initialState);
		
		while (!openSet.isEmpty()) {
			State current = openSet.poll();
			
			if (current.isGoal()) {
				List<State> path = State.reconstructPath(current);
				String text = "";
				
				text = "Steps: " + path.size() + "\n\n";
				for (State s : path)
					text += s + "\n";
				
				try {
					File file = new File("out.txt");
					Writer output = null;
					output = new BufferedWriter(new FileWriter(file));
					output.write(text);
					output.close();
				} catch (IOException e) {
					e.printStackTrace();
					System.exit(1);
				}
				return;
			}
			
			closedSet.add(current);
			
			for (State neighbour : current.getNeighbours()) {
				if (closedSet.contains(neighbour))
					continue;
				int tentativeGScore = current.g + 1;
				
				if (!openSet.contains(neighbour) || tentativeGScore < neighbour.g) {
					openSet.remove(neighbour);
					neighbour.parent = current;
					neighbour.g = tentativeGScore;
					neighbour.f = neighbour.g + State.heuristicDistance(neighbour);
					openSet.add(neighbour);
				}
			}
		}
		
		System.out.println("Failed to find solution");
	}
}
