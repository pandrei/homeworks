#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Pentru a putea trimite un array 2d cu mpi trebuia sa fie un block de memorie continua
 * functia alloc2d realizeaza asta, astfel reprezentarea este matrice, dar memoria este
 * alocata similar unui vector.
 */
int **malloc2d(int rows, int cols) {
	int i = 0;
	int *vec = malloc(rows * cols * sizeof(int));
	int **matrix = malloc(rows * sizeof(int *));
	for (i = 0; i < rows; i++)
		matrix[i] = &(vec[i * cols]);

	return matrix;
}
int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);
	int rank, size;
	double nr1, nr2;
	int i, j, k;
	int type, maxSteps;
	double pace;
	int width, height;
	MPI_Status stat;

	/*
	 * Date relevante parsate de master. Structura info este trimisa
	 * fiecarui worker pentru a isi face mai departe calculele.
	 */
	struct info {
		double xmin, xmax, ymin, ymax;
		int maxstep;
		int tip;
		double rez;
		double nr1, nr2;
	};
	struct imag {
		double real;
		double img;
	};
	//TODO: add rest
	double xmin, xmax, ymin, ymax;
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	int step;
	double temp;
	int color;
	if (rank == 0) {
		/*
		 * Master
		 */

		FILE *f, *o;
		int **mat;

		f = fopen(argv[1], "r");
		o = fopen(argv[2], "w");
		fscanf(f, "%d", &type);
		fscanf(f, "%lf %lf %lf %lf", &xmin, &xmax, &ymin, &ymax);
		fscanf(f, "%lf", &pace);
		fscanf(f, "%d", &maxSteps);
		if (type == 1) {
			fscanf(f, "%lf %lf", &nr1, &nr2);
		}
		fclose(f);

		width = (int) ((xmax - xmin) / pace + 0.5);
		height = (int) ((ymax - ymin) / pace + 0.5);

		int chunk;
		/*
		 * cu chunk determin exact ce dimensiune au bucatile pe care si le i-au procesele
		 * (inclusiv master). Masterul va prelucra si restul impartirii.
		 */
		chunk = height / size;
		struct info inf;
		inf.xmin = xmin;
		inf.xmax = xmax;
		inf.rez = pace;
		inf.ymin = ymin;
		inf.tip = type;
		inf.ymax = ymax;
		inf.maxstep = maxSteps;
		if (type == 1) {
			inf.nr1 = nr1;
			inf.nr2 = nr2;
		}
		/* aloc matricea mare < = > poza */
		mat = (int**) malloc(height * sizeof(int*));
		for (i = 0; i < height; i++) {
			mat[i] = (int*) malloc(width * sizeof(int));
		}

		/* trimit fiecarui worker datele relevante */
		for (i = 1; i < size; i++) {
			MPI_Send(&inf, sizeof(struct info), MPI_BYTE, i, 0, MPI_COMM_WORLD);
		}
		int **recmat;

		/* algoritmul asa cum apare in pseudocod, pentru mandelbrot respectiv julia */
		struct imag z;
		if (type == 0) {
			for (i = 0; i < chunk; i++) {
				for (j = 0; j < width; j++) {
					z.real = 0;
					z.img = 0;
					step = 0;
					while ((sqrt(z.real * z.real + z.img * z.img) < 2)
							&& (step < maxSteps)) {
						temp = z.real * z.real - z.img * z.img
								+ (xmin + pace * j);
						z.img = 2 * z.real * z.img + (ymin + pace * i);
						z.real = temp;
						step++;
					}
					color = step % 256;
					mat[i][j] = color;
				}
			}
			if (height % size != 0) {
				for (i = chunk * size; i < height; i++) {
					for (j = 0; j < width; j++) {
						z.real = 0;
						z.img = 0;
						step = 0;
						while ((sqrt(z.real * z.real + z.img * z.img) < 2)
								&& (step < maxSteps)) {
							temp = z.real * z.real - z.img * z.img
									+ (xmin + pace * j);
							z.img = 2 * z.real * z.img + (ymin + pace * i);
							z.real = temp;
							step++;
						}
						color = step % 256;
						mat[i][j] = color;
					}
				}
			}
		} else {

			struct imag z;
			for (i = 0; i < chunk; i++) {
				for (j = 0; j < width; j++) {
					step = 0;
					z.real = xmin + pace * j;
					z.img = ymin + pace * i;
					while ((sqrt(z.real * z.real + z.img * z.img) < 2)
							&& (step < maxSteps)) {
						temp = z.real * z.real - z.img * z.img + nr1;
						z.img = 2 * z.real * z.img + nr2;
						z.real = temp;
						step++;
					}
					color = step % 256;
					mat[i][j] = color;
				}
			}
		}
		/*
		 * Stim sigur ca trebuie sa primim atatea matrici cati workeri sunt.
		 * Nu stim in ce ordine le vom primi, deci, va fi necesar un array de
		 * recv-uri blocante, dar care pot primi del a oricine.
		 */
		MPI_Status st;
		int worker;
		recmat = malloc2d(chunk, width);
		for (i = 1; i < size; i++) {
			MPI_Recv(&(recmat[0][0]), chunk * width, MPI_INT, MPI_ANY_SOURCE, 1,
					MPI_COMM_WORLD, &st);
			worker = st.MPI_SOURCE;
			/*
			 * determina pozitia "submatricei" in matricea mare pe baza
			 * rank-ului worker-ului.
			 */
			for (k = worker * chunk; k < (worker + 1) * chunk; k++) {
				for (j = 0; j < width; j++) {
					mat[k][j] = recmat[k - worker * chunk][j];
				}
			}
		}
		/*
		 * Afiseaza matricea in fisier, conform formatului pgm
		 */
		fprintf(o, "P2\n");
		fprintf(o, "%d %d\n", width, height);
		fprintf(o, "255\n");
		for (i = height - 1; i >= 0; i--) {
			for (j = 0; j < width; j++) {
				fprintf(o, "%d ", mat[i][j]);
			}
			fprintf(o, "\n");
		}
	} else {
		/*
		 * Workers
		 */

		/*
		 * Primesc datele relevante, imi determin ce bucata calculez din matrice
		 */
		struct info inf;
		MPI_Recv(&inf, sizeof(struct info), MPI_BYTE, 0, 0, MPI_COMM_WORLD,
				&stat);

		width = (int) ((inf.xmax - inf.xmin) / inf.rez + 0.5);
		height = (int) ((inf.ymax - inf.ymin) / inf.rez + 0.5);
		int chunk;
		chunk = height / size;
		int **recmat;
		struct imag z;
		recmat = malloc2d(chunk, width);
		/*
		 * aplic algoritmii pentru mandelbrot si julia conform pseudocodului
		 */
		if (inf.tip == 0) {
			for (i = chunk * rank; i < chunk * (rank + 1); i++) {
				for (j = 0; j < width; j++) {
					z.real = 0;
					z.img = 0;
					step = 0;
					while ((sqrt(z.real * z.real + z.img * z.img) < 2)
							&& (step < inf.maxstep)) {
						temp = z.real * z.real - z.img * z.img
								+ (inf.xmin + inf.rez * j);
						z.img = 2 * z.real * z.img + (inf.ymin + inf.rez * i);
						z.real = temp;
						step++;
					}
					color = step % 256;
					recmat[i - chunk * rank][j] = color;
				}
			}
		} else {
			struct imag z;
			for (i = chunk * rank; i < chunk * (rank + 1); i++) {
				for (j = 0; j < width; j++) {
					step = 0;
					z.real = inf.xmin + inf.rez * j;
					z.img = inf.ymin + inf.rez * i;
					while ((sqrt(z.real * z.real + z.img * z.img) < 2)
							&& (step < inf.maxstep)) {
						temp = z.real * z.real - z.img * z.img + inf.nr1;
						z.img = 2 * z.real * z.img + inf.nr2;
						z.real = temp;
						step++;
					}
					color = step % 256;
					recmat[i - chunk * rank][j] = color;
				}
			}
		}
		/*
		 * Trimit inapoi la master submatricea pe care am calculat - o in workerul curent
		 */
		MPI_Send(&(recmat[0][0]), chunk * width, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}
	MPI_Finalize();
	return 0;
}
