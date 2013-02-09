#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

FILE *fIn, *fOut;
int maxZero, maxOne;
char *fnameIn, *fnameOut;
int max(int a, int b) {
	if (a > b)
		return a;
	return b;
}
int main(int argv, char **argc) {

	if (argv !=4 ) {
		fprintf(stderr,"argument number not matching\n");
				return 1;
	}
	int i, j, k, i1, j1, T;
	int dim;
	int pMin, pMax, nrZero, nrOne;
	fnameIn = malloc(80 * sizeof(char));
	fnameOut = malloc(80 * sizeof(char));
	T = atoi(argc[1]);
	strcpy(fnameIn, argc[2]);
	strcpy(fnameOut, argc[3]);
	fIn = fopen(fnameIn, "rt");
	fOut = fopen(fnameOut, "w");
	if (fIn == NULL) {
		fprintf(stderr,"File could not be opened\n");
		return 1;
	}
	
	fscanf(fIn, "%d%d%d", &pMin, &pMax, &dim);
	
	/* alocari statice - pentru ca necesita mai putin timp decat cele dinamice
	 * iar memoria nu e o problema
	 */
	int b[dim][dim], r[dim][dim], cost[dim][dim], costRes[dim][dim],
			p[dim][dim];
	int Newb[dim][dim], Newp[dim][dim], Newr[dim][dim];

	for (i = 0; i < dim; i++)
		for (j = 0; j < dim; j++)
			fscanf(fIn, "%d", &r[i][j]);

	for (i = 0; i < dim; i++)
		for (j = 0; j < dim; j++)
			fscanf(fIn, "%d", &p[i][j]);

	for (i = 0; i < dim; i++)
		for (j = 0; j < dim; j++)
			fscanf(fIn, "%d", &b[i][j]);
	fclose(fIn);

	/* Calculez matricea de costuri initiale */
	for (i = 0; i < dim; i++)
		for (j = 0; j < dim; j++) {
			cost[i][j] = 999999;
			costRes[i][j] = 999999;
			for (i1 = 0; i1 < dim; i1++)
				for (j1 = 0; j1 < dim; j1++) {
					if ((i != i1 || j != j1) && (r[i][j] == (1 - r[i1][j1])))
						if (cost[i][j]
								> (p[i1][j1] + abs(i - i1) + abs(j - j1)))
							cost[i][j] = p[i1][j1] + abs(i - i1) + abs(j - j1);

					if (r[i][j] == r[i1][j1])
						if (costRes[i][j]
								> (p[i1][j1] + abs(i - i1) + abs(j - j1)))
							costRes[i][j] = p[i1][j1] + abs(i - i1)
									+ abs(j - j1);

				}
		}

	for (k = 0; k < T; k++) {
		maxZero = -99;
		maxOne = -99;
		nrZero = 0;
		for (i = 0; i < dim; i++)
			for (j = 0; j < dim; j++) {

				/* Conditiile pentru cost, Conditia de depasire de maxim am integrat - o in
				 * blocul if pentru a nu face o verificare suplimentara chiar daca nu e cazul
				 *
				 * New(r,b,p) = an +1, unde r,b,p sunt pentru anul curent.
				 * */
				if (cost[i][j] > b[i][j]) {
					Newb[i][j] = cost[i][j];
					Newp[i][j] = p[i][j] + cost[i][j] - b[i][j];
					Newr[i][j] = r[i][j];
				} else

				if (cost[i][j] < b[i][j]) {
					Newb[i][j] = cost[i][j];
					Newp[i][j] = p[i][j] + (cost[i][j] - b[i][j]) / 2;
					Newp[i][j] = max(Newp[i][j], pMin);
					Newr[i][j] = r[i][j];
				} else

				if (cost[i][j] == b[i][j]) {
					Newb[i][j] = cost[i][j];
					Newp[i][j] = costRes[i][j] + 1;
					Newr[i][j] = r[i][j];
				}

				if (Newp[i][j] > pMax) {
					Newr[i][j] = 1 - r[i][j];
					Newb[i][j] = pMax;
					Newp[i][j] = (pMin + pMax) / 2;
				}

				/* Determinarea maximului pentru colonistii "A"(0) respectiv "B"(1) */
				if (Newr[i][j] == 0)

				{
					nrZero++;
					if (Newp[i][j] > maxZero) {
						maxZero = Newp[i][j];
					}

				} else {
					if (Newp[i][j] > maxOne) {
						maxOne = Newp[i][j];
					}
				}

			}

		nrOne = dim * dim - nrZero;
		fprintf(fOut, "%d %d %d %d\n", nrZero, maxZero, nrOne, maxOne);
		/* trecerea la anul urmator, adica actualizarea marticilor */
		for (i = 0; i < dim; i++)
			for (j = 0; j < dim; j++) {
				p[i][j] = Newp[i][j];
				b[i][j] = Newb[i][j];
				r[i][j] = Newr[i][j];

			}

		/* matricile s au modificat, deci matricile de cost trebuie recalculate */
		for (i = 0; i < dim; i++)
			for (j = 0; j < dim; j++) {
				cost[i][j] = 999999;
				costRes[i][j] = 999999;
				for (i1 = 0; i1 < dim; i1++)
					for (j1 = 0; j1 < dim; j1++) {
						if ((i != i1 || j != j1)
								&& (r[i][j] == (1 - r[i1][j1])))
							if (cost[i][j]
									> (p[i1][j1] + abs(i - i1) + abs(j - j1)))
								cost[i][j] = p[i1][j1] + abs(i - i1)
										+ abs(j - j1);

						if (r[i][j] == r[i1][j1])
							if (costRes[i][j]
									> (p[i1][j1] + abs(i - i1) + abs(j - j1)))
								costRes[i][j] = p[i1][j1] + abs(i - i1)
										+ abs(j - j1);

					}

			}

	}
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++)
			fprintf(fOut, "(%d,%d,%d) ", r[i][j], p[i][j], b[i][j]);
		fprintf(fOut, "\n");
	}
	fclose(fOut);
	return 0;
}
