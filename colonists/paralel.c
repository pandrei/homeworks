#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <omp.h>

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
	if (fIn == NULL) {
			fprintf(stderr,"File could not be opened\n");
			return 1;
		}
	fOut = fopen(fnameOut, "w");
	fscanf(fIn, "%d%d%d", &pMin, &pMax, &dim);
	/* alocari statice, pentru ca sunt mai rapide decat cele dinamice */
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
	/* Calculul costurilor, facut in mod paralelizat  */
#pragma omp parallel private(i,i1,j,j1)
	{
#pragma omp for
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
	/* for-ul pentru ani */
	for (k = 0; k < T; k++) {
		maxZero = -99999;
		maxOne = -99999;
		nrZero = 0;
		/* calculul variabilelor in mod paralelizat */
#pragma omp parallel  private(i,j)
		{
			/* am unit punctele 3 si 5
			 * pentru a nu face verificarea maximului
			 * de fiecare data(inutil)
			 */
#pragma omp for
			for (i = 0; i < dim; i++)
				for (j = 0; j < dim; j++) {
					/* pct. 2 */
					if (cost[i][j] > b[i][j]) {
						Newb[i][j] = cost[i][j];
						Newp[i][j] = p[i][j] + cost[i][j] - b[i][j];
						Newr[i][j] = r[i][j];
					} else
					/* pct. 3 + 5*/
					if (cost[i][j] < b[i][j]) {
						Newb[i][j] = cost[i][j];
						Newp[i][j] = p[i][j] + (cost[i][j] - b[i][j]) / 2;
						Newp[i][j] = max(Newp[i][j], pMin);
						Newr[i][j] = r[i][j];
					} else
					/* pct.4 */
					if (cost[i][j] == b[i][j]) {
						Newb[i][j] = cost[i][j];
						Newp[i][j] = costRes[i][j] + 1;
						Newr[i][j] = r[i][j];
					}
					/* pct 6*/
					if (Newp[i][j] > pMax) {
						Newr[i][j] = 1 - r[i][j];
						Newb[i][j] = pMax;
						Newp[i][j] = (pMin + pMax) / 2;
					}

				}
		}

		/* Determinarea numarului de  colonisti "A" si "B"
		 * folosesc reduction pt nuamrul de colonisti A, cei din B
		 * ii determin facand diferenta dim^2 - nr.A */
#pragma omp parallel private(i,j)   shared(maxZero, maxOne)
		{
#pragma omp for reduction(+ :nrZero)
			for (i = 0; i < dim; i++)
				for (j = 0; j < dim; j++) {

					if (Newr[i][j] == 0)

					{ /* numarare colonisti "A" */
						nrZero++;
#pragma omp critical
						{
							if (Newp[i][j] > maxZero) {
								maxZero = Newp[i][j];
							}

						}
					}
#pragma omp critical
					{
						if (Newr[i][j] == 1) {
							if (Newp[i][j] > maxOne) {
								maxOne = Newp[i][j];
							}
						}
					}
				}
		}
		nrOne = dim * dim - nrZero;
		fprintf(fOut, "%d %d %d %d\n", nrZero, maxZero, nrOne, maxOne);
		/*actualizarea matricilor, in mod paralelizat */
#pragma omp parallel  private(i,j)   
		{
#pragma omp for 
			for (i = 0; i < dim; i++)
				for (j = 0; j < dim; j++) {
					p[i][j] = Newp[i][j];
					b[i][j] = Newb[i][j];
					r[i][j] = Newr[i][j];

				}
		}

		/* recalcularea matricilor de cost si costRes pe baza matricilor actualizate ale
		 * celor patru valori agregate */
#pragma omp parallel  private(i,j,i1,j1)  
		{
#pragma omp for
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
	}
	for (i = 0; i < dim; i++) {
		for (j = 0; j < dim; j++)
			fprintf(fOut, "(%d,%d,%d) ", r[i][j], p[i][j], b[i][j]);
		fprintf(fOut, "\n");
	}
	fclose(fOut);
	return 0;
}
