#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <math.h>
#include <cstring>
#include <vector>
#include <queue>
using namespace std;
const double PI = 3.1415926535;
/*
 * Functie care citeste din fisiere
 */
int **ScanFile(FILE* from, int& r, int& c)
{
	int temporary;
	char line[80];
	fgets(line, sizeof(line),from);
	fscanf(from,"%d",&c);
	fscanf(from,"%d",&r);
	fscanf(from,"%d",&temporary);
	int **retMat;
	retMat=(int**)calloc((r+1),sizeof(int*));
	for (int i = 0 ; i <=r; i++)
		retMat[i]=(int*)calloc(c,sizeof(int));

	for(int i = 0 ; i < r ; i ++)
		for (int j = 0 ; j < c; j++)
			fscanf(from,"%d",&retMat[i][j]);
	return retMat;
}
/*
 *  Sigma + Miu <=> SigMiu
 *  Calculeaza cele 2 valori si le intoarce "in parametri"
 *  Foloseste formulele date, cred ca se putea omite radicalul
 *  ca dupa se ridica la ^2
 */
void SigMiu(int ** mask, int **img,int r , int c, double& miu, double &sigma)
{
	int positives=0;
	double val=0;
	for (int i = 0; i <r; i++) 
		for (int j = 0; j < c; j++)
			if (mask[i][j] > 0)
			{
				positives++;
				val=val+img[i][j];
			}
	if (positives != 0 )
		val = val / positives;
	miu=val;
	val=0;
	for (int i = 0; i <r; i++)
		for (int j = 0; j < c; j++)
			if (mask[i][j] > 0)
				val =val+pow((miu - img[i][j]), 2);
	if (positives != 0 )
		val = sqrt(val / positives);
	sigma=val;
}

/*
 * BFS , varianta initiala era din laboratorul din care sunt si
 * maxFlow si min_cut. Isi reface drumul p care a mers. Daca nu mai poate
 * returneaza un drum gol.( vector gol).
 *
 */
vector<int> BFS(int *v, int** vlist, double** Caps, int src, int sink, int r , int c)
				{
	bool reached;
	reached = false;
	int nod,i;
	vector<int> father(r*c+2, -1);
	deque<int> q;
	vector<int> val;

	q.push_back(src);
	father[src] = src;

	while (q.size()>0)
	{
		if (reached == true)
			break;
		nod = q.front();
		q.pop_front();
		for (i = 0; i < v[nod]; i++)
			if(Caps[nod][i]>0 &&
					father[vlist[nod][i]] == -1)
			{
				q.push_back(vlist[nod][i]);
				father[vlist[nod][i]] = nod;
				if (vlist[nod][i] == sink)
					reached = true;
			}
	}
	/*
	 * Daca nu avem drum, deci nu mai avem
	 * niciun drum de crestere.
	 */
	if (father[sink] == -1)
		return vector<int>();
	int node=sink;
	/*
	 * Recover
	 */
	while (node!=src)
	{
		val.push_back(node);
		node=father[node];
	}
	return val;
				}

// Saturam pathul determinat de BFS.
void saturate_path(int *v, int** vlist, double** Caps,
		vector<int>& road,int r, int c)
{

	double min = 9999999;
	int size=road.size()-1;
	int pzp;
	int ps;
	int i,j;
	int t[r*c+2];
	for (i = 1; i <= size; i++)
	{
		t[road[i]] = -1;
		for (j = 0; j < v[road[i]]; j++)
			if (vlist[road[i]][j] == road[i-1])
			{
				t[road[i]] = j;
				break;
			}

	}
	for (i=1; i <= size;i++)
	{
		road[i] = road[i];
		ps = t[road[i]];
		if (t[road[i]] > -1)
			if (min >Caps[road[i]][ps])
				min = Caps[road[i]][ps];
	}
	for (i=1; i <= size;i++)
	{
		road[i] = road[i];
		ps = t[road[i]];
		if (ps != -1)
			Caps[road[i]][ps] -= min;

	}
}
/*
 * Optimizare, ca sa scutim costul bfs-urilor imediate (sursa-> nod ->drena)
 *
 */
void Optimize(int *v, int** vlist, double** Caps, int src, int sink,int r,int c)
{
	vector<int> road;
	int crt_nod=0;
	while(crt_nod < r*c)
	{
		road.push_back(sink);
		road.push_back(crt_nod);
		road.push_back(src);
		saturate_path (v, vlist, Caps, road,r,c);
		road.clear();
		crt_nod++;
	}
}
/*
 * Functia e asemanatoare cu cea din laborator,
 * cicleaza pana cand nu mai sunt drumuri de crestere
 */
void MAXFlow(int *v, int** vlist, double** Caps, int src, int sink,int r,int c)
{
	vector<int> road;

	Optimize(v,vlist,Caps,src,sink,r,c);
	while (1)
	{
		road = BFS(v, vlist, Caps, src, sink,r,c);

		if (road.size() == 0)
			return;
		saturate_path (v, vlist, Caps, road,r,c);
	}
}
/*
 * Functie triviala, initial am crezut c-o voi folosi de mai multe ori
 * ca sa plimb antete, n-am mai sters-o, chit ca o folosesc doar o data.
 */
void copy(FILE* from, FILE* to)
{
	char line[80];
	int lines_count=3;
	while(lines_count>0){
		fgets(line, sizeof(line), from);
		fprintf(to, "%s", line);
		lines_count--;
	}
	return;
}
/*
 * Am incercat sa dau tipuri si nume
 * cat mai sugestive pentru a fi mai usor de urmarit
 * Practic e o parcurgere BFS din nodul sursa care
 * adauga toate nodurile vizitate in source_set
 * si afiseaza, fara sa mai retina ceva.
 */
void Mincut(int *v, int** vlist, double** Caps,
		int src,int r ,int c)
{
	FILE* g,*f;
	int nod;
	int i;

	vector<bool> hasVisited((r*c+2), false);
	hasVisited[src] = true;
	deque<int> q;
	/*
	 * Bfs
	 */
	q.push_back(src);
	while (q.size() != 0)
	{
		nod = q.front();
		q.pop_front();
		for (i = 0; i < v[nod]; i++)
			if (hasVisited[vlist[nod][i]] == false)
				if(Caps[nod][i] > 0)
				{
					hasVisited[vlist[nod][i]] = true;
					q.push_back(vlist[nod][i]);

				}
	}
	g = fopen("./segment.pgm", "w");
	f = fopen("./imagine.pgm", "r");

	copy(f,g);

	for (i = 0; i < r*c; i++)
		if (hasVisited[i] == true)
			fprintf(g, "%d \r\n", 0);
		else
			fprintf(g, "%d \r\n", 255);
	fclose(g);
	fclose(f);
}
/*
 * functia ComputeCaps e defapt programul in sine. Creeaza o lista cu numarul de vecini ai
 * fiecarui nod (cred ca era mai optim sa presupun toti fiind 6 si sa i modific doar pe cei
 * de pe margini cu 5 respectiv 4.
 */
void ComputeCaps(double lambda, double treshold, int** fmask, int **bmask, int** img,
		int r, int c, double fmiu, double bmiu , double fsigma, double bsigma)
{
	int *v;
	double **Caps;
	int **vlist;
	int pos;
	int poz;
	int p;
	/*
	 * Alocari memorie pentru folosire ulterioara
	 */
	vlist=(int**)calloc(r*c+2,sizeof(int*));
	v=(int*)calloc((r*c+2),sizeof(int));
	Caps=(double**)calloc((r*c+2),sizeof(double*));
	int src = r*c;
	int	snk = r*c+1;
	int last= r * c;
	/*
	 * Da valori vecinilor conform celor mentionate mai sus.
	 */
	for (int i = 0; i < r; i++)
		for (int j = 0; j < c; j++)
		{
			p = i * c + j;
			v[p] = 6;
			if ((i == 0 || i == r - 1) && (j == 0 || j == c - 1))
				v[p] = 4;
			else if ((i == 0 || i == r - 1) || (j == 0 || j == c - 1))
				v[p] = 5;

		}
	v[r * c] = r * c;
	for (int i = 0; i < r * c + 2; i++)
		Caps[i] = (double*)calloc(v[i],sizeof(double));
	for(int i = 0 ; i < r*c+2; i++)
		vlist[i]=(int*)calloc(v[i],sizeof(int));
	/*
	 * Se calculeaza capacitatile muchiilor din graf cu formula de mai jos
	 *
	 */
	for (int i = 0; i < r; i++)
		for (int j = 0; j < c; j++) {
			vlist[pos][0] = r * c;
			vlist[pos][1] = r * c + 1;
			poz = 2;
			pos = i * c + j;
			Caps[pos][0] = 0.5 *  pow(((img[i][j] - fmiu) / fsigma), 2) +
					log(sqrt(2 * PI * pow(fsigma, 2)));
			if (Caps[pos][0]>10)
				Caps[pos][0]=10;
			Caps[pos][1] =  0.5 *  pow(((img[i][j] - bmiu) / bsigma), 2) +
					log(sqrt(2 * PI * pow(bsigma, 2)));
			if(Caps[pos][1]>10)
				Caps[pos][1]=10;

			if (i != 0)
			{
				Caps[pos][poz] = (fabs(img[pos] - img[vlist[pos][poz]]) <= treshold) ? lambda : 0;
				vlist[pos][poz] = (i - 1) * c + j;
				poz++;
			}
			if (j != 0)
			{
				Caps[pos][poz] = (fabs(img[pos] - img[vlist[pos][poz]]) <= treshold) ? lambda : 0;
				vlist[pos][poz] = i * c + j - 1;
				poz++;
			}
			if (j != c - 1)
			{
				Caps[pos][poz] = (fabs(img[pos] - img[vlist[pos][poz]]) <= treshold) ? lambda : 0;
				vlist[pos][poz] = i * c + j + 1;
				poz++;
			}
			if (i != r - 1)
			{
				Caps[pos][poz] = (fabs(img[pos] - img[vlist[pos][poz]]) <= treshold) ? lambda : 0;
				vlist[pos][poz] = (i + 1) * c + j;
				poz++;
			}
		}
	for (int i = 0; i < r * c; i++)
	{
		vlist[last][i] = i;
		Caps[last][i] = Caps[i][0];
	}
	/*
	 * Rezolvarea cerintei efective
	 */
	MAXFlow(v,vlist,Caps,src,snk,r,c);
	Mincut(v,vlist,Caps,src,r,c);
	return;
}

int main()
{
	FILE* image;
	FILE* maskfg;
	FILE* maskbg;
	FILE* param;
	double lambda, treshold;
	double fmiu,fsig,bmiu,bsig;
	int **im;
	int r_img;
	int c_img;
	int *vecini;
	image=fopen("imagine.pgm","r");

	im=ScanFile(image,r_img,c_img);
	fclose(image);

	int **fmask;
	int r_fm;
	int c_fm;
	maskfg=fopen("mask_fg.pgm","r");
	fmask=ScanFile(maskfg,r_fm,c_fm);
	fclose(maskfg);

	int **bmask;
	int r_bm;
	int c_bm;
	maskbg=fopen("mask_bg.pgm","r");
	bmask=ScanFile(maskbg,r_bm,c_bm);
	fclose(maskbg);

	param=fopen("parametri.txt","r");
	fscanf(param, "%lf", &lambda);
	fscanf(param, "%lf", &treshold);
	fclose(param);
	SigMiu(fmask,im,r_img,c_img,fmiu,fsig);
	SigMiu(bmask,im,r_img,c_img,bmiu,bsig);
	ComputeCaps(lambda,treshold,fmask,bmask,im,r_img,c_img,fmiu,bmiu,fsig,bsig);

	return 0;
}

