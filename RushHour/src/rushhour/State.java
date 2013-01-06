package rushhour;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.HashSet;
import java.util.List;
import java.util.Scanner;
import java.util.Set;

public class State {

	private static final int UP = 0;
	private static final int DOWN = 1;
	private static final int LEFT = 2;
	private static final int RIGHT = 3;
	
	public enum StateValue {
		BORDER,
		CAR_MARGIN_LEFT,
		CAR_MARGIN_RIGHT,
		CAR_MARGIN_TOP,
		CAR_MARGIN_BOTTOM,
		CAR_HORIZONTAL_BODY,
		CAR_VERTICAL_BODY,
		RED_CAR,
		EXIT,
		EMPTY
	}
	
	StateValue[][] map;
	int rows, columns;
	static StateComparator comparator = new StateComparator();
	int exit_row, exit_col;
	int red_car_row, red_car_col;
	
	public int f, g;
	State parent = null;
	
	public State(StateValue[][] map) {
		this.map = map;
		rows = map.length;
		columns = map[0].length;
		initVariables();
	}
	
	//constructorul
	public State(Scanner source) {
		rows = source.nextInt();
		columns = source.nextInt();
		source.nextLine();
		
		map = new StateValue[rows][columns];
		for (int i = 0; i < rows; i ++) {
			boolean car = false;
			for (int j = 0; j < columns; j ++) {
				map[i][j] = getStateValue(source.next(), car);
				if (map[i][j] == StateValue.CAR_MARGIN_LEFT)
					car = true;
				if (map[i][j] == StateValue.CAR_MARGIN_RIGHT)
					car = false;
				
			}
		}
		
		initVariables();
	}
	
	// initializarea variabilelor
	private void initVariables() {
		for (int i = 0; i < rows; i ++) {
			for (int j = 0; j < columns; j ++) {
				if (map[i][j] == StateValue.EXIT) {
					exit_row = i;
					exit_col = j;
				}
				if (map[i][j] == StateValue.RED_CAR) {
					red_car_row = i;
					red_car_col = j;
				}
			}
		}
		
	}
	
	// verifica daca 2 stari sunt egale
	@Override
	public boolean equals(Object o) {
		State otherState;
		if (!(o instanceof State))
			return false;
		otherState = (State)o;
		
		if (rows != otherState.rows || columns != otherState.columns)
			return false;
		
		for (int i = 0; i < rows; i ++)
			for (int j = 0; j < columns; j ++)
				if (map[i][j] != otherState.map[i][j])
					return false;
		return true;
	}
	

	@Override
	public int hashCode() {
		int hashValue = 0;
		for (int i = 0; i < rows; i ++)
			for (int j = 0; j < columns; j ++)
				hashValue += map[i][j].hashCode() * i + j;
		return hashValue;
	}
	
	// verifica ce tip are un caracter din matricea starii (harta)
	private static StateValue getStateValue(String symbol, boolean car) {
		if (symbol.equals("0"))
			return StateValue.BORDER;
		if (symbol.equals("<"))
			return StateValue.CAR_MARGIN_LEFT;
		if (symbol.equals(">"))
			return StateValue.CAR_MARGIN_RIGHT;
		if (symbol.equals("^"))
			return StateValue.CAR_MARGIN_TOP;
		if (symbol.equals("v"))
			return StateValue.CAR_MARGIN_BOTTOM;
		if (symbol.equals("-")) {
			if (car)
				return StateValue.CAR_HORIZONTAL_BODY;
			else
				return StateValue.EMPTY;
		}
		if (symbol.equals("|"))
			return StateValue.CAR_VERTICAL_BODY;
		if (symbol.equals("?"))
			return StateValue.RED_CAR;
		if (symbol.equals("*"))
			return StateValue.EXIT;
		
		System.out.println("Unrecognized symbol: " + symbol);
		System.exit(1);
		return null;
	}
	
	// metoda ce realizeaza mutarea unei masini pe harta
	private static StateValue[][] moveCar(StateValue[][] map, int direction,
			int source_row, int source_col, int dest_row, int dest_col) {
		StateValue[][] newMap = new StateValue[map.length][map[0].length];
		boolean isRedCar = map[source_row][source_col] == StateValue.RED_CAR;
		
		for (int i = 0; i < map.length; i ++)
			for (int j = 0; j < map[i].length; j ++)
				newMap[i][j] = map[i][j];
		
		switch (direction) {
			case DOWN:
				while (true) {
					newMap[dest_row][source_col] = newMap[source_row][source_col];
					newMap[source_row][source_col] = StateValue.EMPTY;
					if (newMap[dest_row][source_col] == StateValue.CAR_MARGIN_TOP)
						break;
					source_row --;
					dest_row --;
					if (newMap[source_row][source_col] == StateValue.BORDER)
						break;
				}
				return newMap;
			case UP:
				while (true) {
					newMap[dest_row][source_col] = newMap[source_row][source_col];
					newMap[source_row][source_col] = StateValue.EMPTY;
					if (newMap[dest_row][source_col] == StateValue.CAR_MARGIN_BOTTOM)
						break;
					source_row ++;
					dest_row ++;
					if (newMap[source_row][source_col] == StateValue.BORDER)
						break;
				}
				return newMap;
			case LEFT:
				while (true) {
					newMap[dest_row][dest_col] = newMap[source_row][source_col];
					newMap[source_row][source_col] = StateValue.EMPTY;
					if (newMap[dest_row][dest_col] == StateValue.CAR_MARGIN_RIGHT)
						break;
					source_col ++;
					dest_col ++;
					if (newMap[source_row][source_col] == StateValue.BORDER)
						break;
					if (newMap[source_row][source_col] != StateValue.RED_CAR && isRedCar)
						break;
				}
				return newMap;
			case RIGHT:
				while (true) {
					newMap[dest_row][dest_col] = newMap[source_row][source_col];
					newMap[source_row][source_col] = StateValue.EMPTY;
					if (newMap[dest_row][dest_col] == StateValue.CAR_MARGIN_LEFT)
						break;
					source_col --;
					dest_col --;
					if (newMap[source_row][source_col] == StateValue.BORDER)
						break;
					if (newMap[source_row][source_col] != StateValue.RED_CAR && isRedCar)
						break;
				}
				return newMap;
			default:
				return null;
		}
	}
	
	// metoda ce realizeaza expandarea starilor (genereaza stari in care se
	// poate ajunge din starea curenta)
	public Set<State> getNeighbours() {
		Set<State> neighbours = new HashSet<State>();
		for (int i = 0; i < rows; i ++) {
			for (int j = 0; j < columns; j ++) {
				if (map[i][j] == StateValue.EMPTY) {
					// Check the other 4 directions if any car can move its
					// front into this empty cell
					for (int k = i - 1; k > 0; k --) {
						// UP
						if (map[k][j] == StateValue.CAR_MARGIN_BOTTOM) {
							neighbours.add(new State(moveCar(map, DOWN, k, j, i, j)));
						}
						if (map[k][j] != StateValue.EMPTY)
							break;
					}
					for (int k = i + 1; k < rows - 1; k ++) {
						// DOWN
						if (map[k][j] == StateValue.CAR_MARGIN_TOP) {
							neighbours.add(new State(moveCar(map, UP, k, j, i, j)));
						}
						if (map[k][j] != StateValue.EMPTY)
							break;
					}
					for (int k = j - 1; k > 0; k --) {
						// LEFT
						if (map[i][k] == StateValue.CAR_MARGIN_RIGHT ||
								map[i][k] == StateValue.RED_CAR) {
							neighbours.add(new State(moveCar(map, RIGHT, i, k, i, j)));
						}
						if (map[i][k] != StateValue.EMPTY)
							break;
					}
					for (int k = j + 1; k < columns - 1; k ++) {
						// RIGHT
						if (map[i][k] == StateValue.CAR_MARGIN_LEFT ||
								map[i][k] == StateValue.RED_CAR)
							neighbours.add(new State(moveCar(map, LEFT, i, k, i, j)));
						if (map[i][k] != StateValue.EMPTY)
							break;
					}
				}
			}
		}
		return neighbours;
	}
	
	public static Comparator<State> getComparator() {
		return comparator;
	}
	
	// reconstructia caii
	public static List<State> reconstructPath(State state) {
		List<State> path = new ArrayList<State>();
		State currentState = state;
		while (currentState != null) {
			path.add(0, currentState);
			currentState = currentState.parent;
		}
		return path;
	}
	
	public static int heuristicDistance(State state) {
		return -1;
	}
	
	// verifica daca s-a ajuns la solutie
	public boolean isGoal() {
		for (int i = red_car_col + 1; i < exit_col; i ++)
			if (map[red_car_row][i] != StateValue.EMPTY)
				return false;
		return true;
	}
	
	
	private static class StateComparator implements Comparator<State> {
		@Override
		public int compare(State o1, State o2) {
			return o1.f - o2.f;
		}
	}

	
	@Override
	public String toString() {
		String mapAsString = "";
		for (int i = 0; i < rows; i ++) {
			for (int j = 0; j < columns; j ++) {
				switch (map[i][j]) {
					case BORDER:
						mapAsString += '0';
						break;
					case CAR_HORIZONTAL_BODY:
						mapAsString += '-';
						break;
					case CAR_VERTICAL_BODY:
						mapAsString += '|';
						break;
					case CAR_MARGIN_BOTTOM:
						mapAsString += 'v';
						break;
					case CAR_MARGIN_LEFT:
						mapAsString += '<';
						break;
					case CAR_MARGIN_RIGHT:
						mapAsString += '>';
						break;
					case CAR_MARGIN_TOP:
						mapAsString += '^';
						break;
					case EMPTY:
						mapAsString += '-';
						break;
					case EXIT:
						mapAsString += '*';
						break;
					case RED_CAR:
						mapAsString += '?';
						break;
				}
				mapAsString += ' ';
			}
			mapAsString += '\n';
		}
		return mapAsString;
	}
}
