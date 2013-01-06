#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include "helpers.h"
#define DRUMAX 10000

/* Aditional defines
 */
#include <iostream>
#include <queue>
#include <deque>
#include <list>
using namespace std;
typedef queue<msg> smqueue;
int min (int a, int b){
	if (a>b)return b;
	return a;
}
int main (int argc, char ** argv)
{

	int pipeout = atoi(argv[1]);
	int pipein = atoi(argv[2]);
	int nod_id = atoi(argv[3]);
	//procesul curent participa la simulare numai dupa ce nodul
	//cu id-ul lui este adaugat in topologie
	int timp =-1 ;
	int i=0;
	int end;
	int father[KIDS], dist[KIDS], nodeque[KIDS], size, dist_min,minc, alt;
	int j=0;
	int topologie[KIDS][KIDS];
	int gata = FALSE;
	msg mesaj;
	msg process;
	msg LSADb[KIDS];
	int cit, k,crtscv;
	smqueue oldq,newq;
	for (i = 0; i < KIDS; i++) {
		LSADb[i].type = -1; // Nu exista informatii despre ruterul "i"
	}

	for (i = 0; i < KIDS; i++) {
		for (j = 0; j < KIDS; j++) {
			topologie[i][j] = 10000; // Nu exista legaturi intre rutere la inceput
			if (i == j) {
				topologie[i][j] = 0; // De la un ruter la el insusi am considerat costul ca fiind 0.
			}
		}
	}

	//nu modificati numele, modalitatea de alocare si initializare a tabelei de rutare
	//- se foloseste la mesajele de tip 8/10, deja implementate si la logare
	int tab_rutare [KIDS][2]; //tab_rutare[k][0] reprezinta costul drumului minim de la ruterul curent (nod_id) la ruterul k
	//tab_rutare[k][1] reprezinta next_hop pe drumul minim de la ruterul curent (nod_id) la ruterul k
	for (k = 0; k < KIDS; k++) {
		tab_rutare[k][0] = 10000;
		// drum =DRUMAX daca ruterul k nu e in retea sau informatiile despre
		//el nu au ajuns la ruterul curent
		tab_rutare[k][1] = -1;
		//in cadrul protocolului(pe care il veti implementa), next_hop =-1
		//inseamna ca ruterul k nu e (inca) cunoscut de ruterul nod_id (vezi mai sus)
	}

	//	for (i = 0 ; i < KIDS; i++)
	//	for (j = 0 ; j < KIDS; j++)
	//		topologie[i][j]=-1;
	printf ("Nod %d, pid %u alive & kicking\n", nod_id, getpid());

	if (nod_id == 0) { //sunt deja in topologie
		timp = -1; //la momentul 0 are loc primul eveniment
		mesaj.type = 5; //finish procesare mesaje timp -1
		mesaj.sender = nod_id;
		write (pipeout, &mesaj, sizeof(msg)); 
		printf ("TRIMIS Timp %d, Nod %d, msg tip 5 - terminare procesa"
				"re mesaje vechi din coada\n", timp, nod_id);

	}

	while (!gata) {
		/*for(i = 0 ; i < KIDS ; i++){
			printf("\n");
				for(j = 0 ; j< KIDS; j++)
					if (topologie[i][j]!=DRUMAX)
					printf("%d   ",topologie[i][j]);
					else printf("    ");
		}
		printf("\n");
		printf("\n");
		printf("\n");*/
		cit = read(pipein, &mesaj, sizeof(msg));

		if (cit <= 0) {
			printf ("Adio, lume cruda. Timp %d, Nod %d, msg tip %d"
					" cit %d\n", timp, nod_id, mesaj.type, cit);
			exit (-1);
		}

		switch (mesaj.type) {

		//1,2,3,4 sunt mesaje din protocolul link state;
		//actiunea imediata corecta la primirea unui pachet de tip 1,2,3,4 este
		//buffer-area (punerea in coada /coada new daca sunt 2 cozi - vezi enunt)
		//mesajele din coada new se vor procesa atunci cand ea devine coada old
		// (cand am intrat in urmatorul pas de timp)
		case 1:
			newq.push(mesaj);
			printf ("Timp %d, Nod %d, msg tip 1 - LSA\n", timp, nod_id);
			break;

		case 2:
			newq.push(mesaj);
			printf ("Timp %d, Nod %d, msg tip 2 - Database Request\n", timp, nod_id);
			break;

		case 3:
		{
			newq.push(mesaj);
			printf ("Timp %d, Nod %d, msg tip 3 - Database Reply\n", timp, nod_id);
			break;
		}

		case 4:
			newq.push(mesaj);
			printf ("Timp %d, Nod %d, msg tip 4 - pachet de date (de rutat)\n", timp, nod_id);
			break;

		case 6://complet in ceea ce priveste partea cu mesajele de control
			//puteti inlocui conditia de coada goala, ca sa corespunda cu implementarea personala
			//aveti de implementat procesarea mesajelor ce tin de protocolul de rutare
		{
			timp++;
			printf ("Timp %d, Nod %d, msg tip 6 - incepe procesarea mesajelor puse din coada la timpul anterior (%d)\n", timp, nod_id, timp-1);

			//veti modifica ce e mai jos -> in scheletul de cod nu exista nicio coada


			//daca NU mai am de procesat mesaje venite la timpul anterior
			//(dar mai pot fi mesaje venite in acest moment de timp, pe care le procesez la t+1)
			//trimit mesaj terminare procesare pentru acest pas (tip 5)
			//altfel, procesez mesajele venite la timpul anterior si apoi trimit mesaj de tip 5




			while (oldq.size()!=0) {
				//	procesez tote mesajele din coada old
				//	(sau toate mesajele primite inainte de inceperea
				// timpului curent - marcata de mesaj de tip 6)
				//	la acest pas/timp NU se vor procesa mesaje venite DUPA inceperea timpului curent
				//cand trimiteti mesaje de tip 4 nu uitati sa setati
				// (inclusiv) campurile, necesare pt logare:  mesaj.timp, mesaj.creator,
				//mesaj.nr_secv, mesaj.sender, mesaj.next_hop
				//la tip 4 - creator este sursa initiala a pachetului rutat

				process=oldq.front();
				//	printf("\nProcess Details\n\n:");
				//	printf("process creator=%d\n",process.creator);
				//	printf("process type=%d\n",process.type);
				//printf("process sender=%d\n",process.sender);
				oldq.pop();
				switch(process.type){
				// Procesez mesajele de tip 1 - LSA. Daca LSA-ul este mai nou il updatez si nu e duplicat
				case 1:
				{
					int creator = process.creator;
					bool valid = false;
					int nrv1,nrv2;
					int vecin,cost;
					char toint1[sizeof(int)];
					char toint2[sizeof(int)];
					msg tbs;
					if (LSADb[creator].type==-1) valid = true;
					else if(LSADb[creator].nr_secv<process.nr_secv) valid = true;
					//printf("secv  creator = %d\n\n",LSADb[creator].nr_secv);
					//printf("secv primita = %d\n\n",process.nr_secv);
					// Daca valid e true retrimit LSA-ul catre toti vecinii, fara cel de care am primit.
					if (valid){
						//actualizez topologia
						LSADb[creator]=process;
						tbs=process;
						strncpy(toint1,process.payload,sizeof(int));
						strncpy(toint2,LSADb[creator].payload,sizeof(int));
						memcpy(&nrv1,toint1,sizeof(int));
						memcpy(&nrv2,toint2,sizeof(int));
						for(j = 0 ; j < KIDS ; j++)
							if(nrv2<nrv1 && LSADb[j].timp<=process.timp && i!=j)
							{
								topologie[i][j]=DRUMAX;
								topologie[j][i]=DRUMAX;
							}
					
						

						int nr_neigh=nrv1;
						for(i= 0 ;i < nr_neigh;i++)
						{
							strncpy(toint1,process.payload+(1+2*i)*sizeof(int),sizeof(int));
							memcpy(&vecin,toint1,sizeof(int));
							strncpy(toint2,process.payload+(2+2*i)*sizeof(int),sizeof(int));
							memcpy(&cost,toint2,sizeof(int));
							//	printf("DEBUG - vecin %d\n",vecin);
							//	printf("DEBUG - cost %d\n",cost);
							// imi formez topologia pe baza LSADatabase ( LSADb)
							if (LSADb[vecin].timp<=process.timp)
							{
								topologie[vecin][creator]=cost;
								topologie[creator][vecin]=cost;
							}

						}
						tbs.sender=nod_id;
						for(i = 0 ; i < KIDS ; i++)
							if (topologie[nod_id][i]!=DRUMAX && i!=nod_id){
								tbs.next_hop=i;
								write(pipeout,&tbs,sizeof(msg));
							}

					}
					break;
				}
				//Procesez mesajele de tip 2 - adaugare link

				case 2:
				{

					//Database req
					msg tbs;
					int vecin,cost;
					int count;
					char toint1[sizeof(int)];
					char toint2[sizeof(int)];
					char towrite[1400];
					char intbuffer[sizeof(int)];
					for(i = 0 ; i < KIDS; i++)
						if(LSADb[i].type!=-1){
							tbs=LSADb[i];
							tbs.sender=nod_id;
							tbs.type=3;
							tbs.next_hop=process.sender;
							write(pipeout,&tbs,sizeof(msg));
						}
					strncpy(toint1,process.payload,sizeof(int));
					memcpy(&vecin,toint1,sizeof(int));
					strncpy(toint2,process.payload+sizeof(int),sizeof(int));
					memcpy(&cost,toint2,sizeof(int));
					//	printf("Case 2 \n");
					//ok	printf("DEBUG111 - vecin %d\n",vecin);
					//ok	printf("DEBUG111 - cost %d\n",cost);
					//modific topologia
					topologie[vecin][process.creator]=cost;
					topologie[process.creator][vecin]=cost;

					for ( i = 0 ; i < KIDS; i++){
						if (topologie[nod_id][i]!=DRUMAX){
							count++;
						}
					}
					memcpy(intbuffer,&count,sizeof(int));
					strcpy(towrite,intbuffer);
					for(i = 0 ; i < KIDS; i++)
						if(topologie[nod_id][i]!=DRUMAX){
							memcpy(intbuffer,&i,sizeof(int));
							strcpy(towrite+(1+2*i)*sizeof(int),intbuffer);
							memcpy(intbuffer,&topologie[nod_id][i],sizeof(int));
							strcpy(towrite+(2+2*i)*sizeof(int),intbuffer);
						}
					memcpy(tbs.payload,towrite,1400);
					// Trimit LSA catre toti vecinii, inclusiv cel de la care am primit.
					tbs.creator=nod_id;
					tbs.sender=nod_id;
					tbs.timp=timp;
					tbs.type=1;
					tbs.nr_secv=crtscv;
					crtscv++;
					for(i = 0 ; i < KIDS ; i++)
						if (topologie[nod_id][i]!=DRUMAX && i!=nod_id){
							tbs.next_hop=i;
							write(pipeout,&tbs,sizeof(msg));
						}

					break;
				}
				case 3:
				{
					//Identic type 1, cu exceptia ca nu mai forwardez mesajele
					int creator = process.creator;
					bool valid = false;
					int nrv1,nrv2;
					int vecin,cost;
					char toint1[sizeof(int)];
					char toint2[sizeof(int)];
					if (LSADb[creator].type==-1) valid = true;
					else if(LSADb[creator].nr_secv<process.nr_secv) valid = true;
					if (valid){
						LSADb[creator]=process;
						strncpy(toint1,process.payload,sizeof(int));
						strncpy(toint2,LSADb[creator].payload,sizeof(int));
						memcpy(&nrv1,toint1,sizeof(int));
						memcpy(&nrv2,toint2,sizeof(int));
						//	printf("CASE 3\n");
						//	printf("DEBUG - NRV1 %d\n",nrv1);
						//	printf("DEBUG - NRV2 %d\n",nrv2);
						for(j = 0 ; j < KIDS ; j++)
							if(nrv1<nrv2 && LSADb[j].timp<=process.timp && i!=j)
							{
								topologie[i][j]=DRUMAX;
								topologie[j][i]=DRUMAX;
							}



						int nr_neigh=nrv1;
						for(i= 0 ;i < nr_neigh;i++)
						{
							strncpy(toint1,process.payload+(1+2*i)*sizeof(int),sizeof(int));
							memcpy(&vecin,toint1,sizeof(int));
							strncpy(toint2,process.payload+(2+2*i)*sizeof(int),sizeof(int));
							memcpy(&cost,toint2,sizeof(int));
							//	printf("DEBUG - vecin %d\n",vecin);
							//	printf("DEBUG - cost %d\n",cost);
							if (LSADb[vecin].timp<=process.timp)
							{
								topologie[vecin][creator]=cost;
								topologie[creator][vecin]=cost;
							}

						}
					}
					break;
				}
				case 4:
				{   // Trimit pachetul mai departe in cazul in care n-a ajuns la destinatie.
					int indr2;

					char intbuffer[sizeof(int)];
					strncpy(intbuffer,process.payload,sizeof(int));
					memcpy(&indr2,intbuffer,sizeof(int));
					if(nod_id!=indr2){
						process.sender=nod_id;
						process.timp=timp;
						process.next_hop=tab_rutare[indr2][1];
					}
					if(tab_rutare[indr2][1]!=-1)
					{
						write (pipeout, &process, sizeof(msg));
						printf("Ok! Have sent %d to %d\n",process.creator,indr2);
					}
					else printf("Failed to send %d to %d\n",process.creator,indr2);
					break;
				}
				default:break;
				}
			}


			// Noua coada devine veche, golesc noua coada in cea veche.
			// Avand grija sa pastrez ordinea elementelor
			// in coada
			while(newq.size()!=0){
				oldq.push(newq.back());
				newq.pop();
			}
			//Actualizez tab_rutare folosind Dijkstra
			size = KIDS - 1;
			for (i = 0; i < KIDS; i++) {

				if(i==nod_id)
					nodeque[i]=-1;
				else
					nodeque[i] = 1;
				dist[i] = topologie[nod_id][i];
				if (dist[i] != DRUMAX)
					father[i] = nod_id;
				else
					father[i] = -1;
			}

			while (size != 0) {
				minc = -1;
				dist_min = DRUMAX;
				for (i = 0; i < KIDS; i++) {
					if (nodeque[i] == 1) {
						if (dist[i] < dist_min) {
							dist_min = dist[i];
							minc = i;
						}
					}
				}

				if (dist_min >= DRUMAX) {
					break;
				}
				nodeque[minc] = -1;
				size--;
				for (i = 0; i < KIDS; i++) {
					if (nodeque[i] == 1 && i != minc) {
						alt = dist[minc] + topologie[minc][i];
						if (alt < dist[i]) {
							dist[i] = alt;
							father[i] = minc;
						}
					}
				}
			}
			for (i = 0; i < KIDS; i++) {
				end = i;
				while (father[end] != nod_id && father[end] != -1) {
					end = father[end];
				}
				if (father[end] == nod_id) {
					tab_rutare[i][0] = dist[i];
					tab_rutare[i][1] = end;
				} else {
					tab_rutare[i][0] = DRUMAX;
					tab_rutare[i][1] = -1;
				}
			}


			//acum coada_old e goala, trimit mesaj de tip 5
			mesaj.type = 5;
			mesaj.sender = nod_id;
			write (pipeout, &mesaj, sizeof(msg));
			break;
		}
		case 7:
		{
			//complet in ceea ce priveste partea cu mesajele de control
			//aveti de implementat tratarea evenimentelor si trimiterea mesajelor ce tin de protocolul de rutare
			//in campul payload al mesajului de tip 7 e linia de fisier (%s) corespunzatoare respectivului eveniment
			//vezi multiproc.c, liniile 88-115 (trimitere mes tip 7) si liniile 184-194 (parsare fisiere evenimente)

			//rutere direct implicate in evenimente, care vor primi mesaje de tip 7 de la simulatorul central:
			//eveniment tip 1: ruterul nou adaugat la retea  (ev.d1  - vezi liniile indicate)
			//eveniment tip 2: capetele noului link (ev.d1 si ev.d2)
			//eveniment tip 3: capetele linkului suprimat (ev.d1 si ev.d2)
			//evenimet tip 4:  ruterul sursa al pachetului (ev.d1)

			char backup[1400];
			char towrite[1400];
			strcpy(backup,mesaj.payload);
			char* iterator;
			char intbuffer[sizeof(int)];
			iterator=strtok(backup, " ");
			int type;
			type =atoi(iterator);
			//printf("\n\nDEBUG- TIP MSG %d\n\n",type);
			if (mesaj.join == TRUE)
			{
				// prelucrez mesajele de tip 1.
				int indrut;
				iterator=strtok(NULL, " ");
				indrut = atoi(iterator);
				//printf("DEBUG- ID RUTER %d\n",indrut);
				msg tbs; // to be sent
				timp = mesaj.timp;
				printf ("Nod %d, msg tip eveniment "
						"- voi adera la topologie la"
						" pasul %d\n", nod_id, timp+1);
				if(nod_id==indrut){
					// Daca sunt ruterul nou adaugat trimit DB Request tuturor vecinilor;
					iterator=strtok(NULL, " ");
					int nr_neigh=atoi(iterator);

					//printf("DEBUG- NR_NEIGH %d\n",nr_neigh);
					int k;
					int n_index;
					int n_cost;
					tbs.creator=nod_id;
					tbs.sender=nod_id;
					tbs.type=2;
					tbs.timp=timp+1;
					tbs.nr_secv=crtscv;
					crtscv++;
					//printf("test %s\n",mesaj.payload);
					for (k = 0 ; k < nr_neigh; k++){
						iterator=strtok(NULL, " ");
						//	printf("iterator =%s\n",iterator);
						n_index=atoi(iterator);
						iterator=strtok(NULL, " ");
						//	printf("iterator =%s\n",iterator);
						n_cost=atoi(iterator);
						//	printf("\nDebug neighbors case 1 - 7 n=%d cost=%d\n",n_index,n_cost);
						tbs.next_hop=n_index;
						memset(towrite,0,1400);
						memcpy(intbuffer,&n_index,sizeof(int));
						strcpy(towrite,intbuffer);
						memcpy(intbuffer,&n_cost,sizeof(int));
						strcpy(towrite+sizeof(int),intbuffer);
						memcpy(tbs.payload,towrite,1400);
						//printf ("DEBUG BOX\n");
						//printf("\n\n------------------------------------------------------------\n\n");
						//printf("AFTER PROCESS\n");
						//printf("tbs.payload %s\n",mesaj.payload);
						//	strncpy(toint1,tbs.payload,sizeof(int));
						//memcpy(&vecin,toint1,sizeof(int));
						//strncpy(toint2,tbs.payload+sizeof(int),sizeof(int));
						//	memcpy(&cost,toint2,sizeof(int));
						//printf("DEBUG11 - vecin %d\n",vecin);
						//printf("DEBUG11- cost %d\n",cost);
						//printf("\n\n------------------------------------------------------------\n\n");
						write (pipeout, &tbs, sizeof(msg));
						// Trimit vecinului legatura si costul;
					}
				}
			}
			else {
			/*else {
				printf ("Timp %d, Nod %d, msg tip 7 - eveniment\n", timp+1, nod_id);
				//acest tip de mesaj (7) se proceseaza imediat - nu se pune in nicio coada (vezi enunt)
			 */
				switch(type){
			case 2:
			{
				// Adaugare Link
				int indr1,indr2;
				int cost;
				msg tbs; // to be sent
				//	printf("mesaj.payload = %s\n",mesaj.payload);
				iterator=strtok(NULL, " ");
				indr1=atoi(iterator);
				iterator=strtok(NULL, " ");
				indr2=atoi(iterator);
				iterator=strtok(NULL, " ");
				cost=atoi(iterator);
				//printf("\n\nMSG type 2 debug ruter1=%d ruter2=%d cost=%d\n\n",indr1,indr2,cost);
				tbs.creator=nod_id;
				tbs.sender=nod_id;
				tbs.type=2;
				tbs.timp=timp+1;
				tbs.nr_secv=crtscv;
				crtscv++;
				memset(towrite,0,1400);
				if (nod_id==indr1){
					tbs.next_hop=indr2;
					memcpy(intbuffer,&indr2,sizeof(int));
					strcpy(towrite,intbuffer);
					memcpy(intbuffer,&cost,sizeof(int));
					strcpy(towrite+sizeof(int),intbuffer);
					memcpy(tbs.payload,towrite,1400);
					write (pipeout, &tbs, sizeof(msg));
				}
				else if (nod_id==indr2){
					tbs.next_hop=indr1;
					memcpy(intbuffer,&indr1,sizeof(int));
					strcpy(towrite,intbuffer);
					memcpy(intbuffer,&cost,sizeof(int));
					strcpy(towrite+sizeof(int),intbuffer);
					memcpy(tbs.payload,towrite,1400);
					write (pipeout, &tbs, sizeof(msg));
				}
				break;
			}
			case 3:
				//Suprimare Link
			{   int indr1,indr2;
			iterator=strtok(NULL, " ");
			indr1=atoi(iterator);
			iterator=strtok(NULL, " ");
			indr2=atoi(iterator);
			int count =0 ;
			msg tbs;
			memset(towrite,0,1400);
			char towrite[1400];

			//printf("MSG %d ruter1 %d ruter2 %d\n",process.type,indr1,indr2);

			if(nod_id==indr2 || nod_id==indr1){
				tbs.creator=nod_id;
				tbs.sender=nod_id;
				tbs.timp=timp+1;
				tbs.nr_secv=crtscv;
				crtscv++;
				topologie[indr1][indr2]=DRUMAX;
				topologie[indr2][indr1]=DRUMAX;

				for (i = 0 ; i < KIDS; i++)
					if(topologie[nod_id][i]!=DRUMAX) count++;
				memcpy(intbuffer,&count,sizeof(int));
				strcpy(towrite,intbuffer);
				for(i = 0 ; i < KIDS; i++)
					if(topologie[nod_id][i]!=DRUMAX){
						memcpy(intbuffer,&i,sizeof(int));
						strcpy(towrite+(1+2*i)*sizeof(int),intbuffer);
						memcpy(intbuffer,&topologie[nod_id][i],sizeof(int));
						strcpy(towrite+(2+2*i)*sizeof(int),intbuffer);
					}
				memcpy(tbs.payload,towrite,1400);
				LSADb[nod_id]= tbs;
				for(i = 0 ; i < KIDS; i++)
					if(i!=nod_id && topologie[nod_id][i]!=DRUMAX){
						tbs.next_hop=i;
						write (pipeout, &tbs, sizeof(msg));
					}
			}

			size = KIDS - 1;
			for (i = 0; i < KIDS; i++) {

				if(i==nod_id)
					nodeque[i]=-1;
				else
					nodeque[i] = 1;
				dist[i] = topologie[nod_id][i];
				if (dist[i] != DRUMAX)
					father[i] = nod_id;
				else
					father[i] = -1;
			}

			while (size != 0) {
				minc = -1;
				dist_min = DRUMAX;
				for (i = 0; i < KIDS; i++) {
					if (nodeque[i] == 1) {
						if (dist[i] < dist_min) {
							dist_min = dist[i];
							minc = i;
						}
					}
				}

				if (dist_min >= DRUMAX) {
					break;
				}
				nodeque[minc] = -1;
				size--;
				for (i = 0; i < KIDS; i++) {
					if (nodeque[i] == 1 && i != minc) {
						alt = dist[minc] + topologie[minc][i];
						if (alt < dist[i]) {
							dist[i] = alt;
							father[i] = minc;
						}
					}
				}
			}
			for (i = 0; i < KIDS; i++) {
				end = i;
				while (father[end] != nod_id && father[end] != -1) {
					end = father[end];
				}
				if (father[end] == nod_id) {
					tab_rutare[i][0] = dist[i];
					tab_rutare[i][1] = end;
				} else {
					tab_rutare[i][0] = DRUMAX;
					tab_rutare[i][1] = -1;
				}
			}




			break;
			}

			/*
			 * Dijsktra
			 */

			case 4:
			{
				int indr1,indr2;
				char intbuffer[sizeof(int)];
				msg tbs;
				iterator=strtok(NULL, " ");
				indr1=atoi(iterator);
				iterator=strtok(NULL, " ");
				indr2=atoi(iterator);
				memset(tbs.payload,0,1400);
				if(nod_id==indr1){
					tbs.creator=nod_id;
					tbs.sender=nod_id;
					tbs.timp=timp+1;
					tbs.nr_secv=crtscv;
					tbs.type=4;
					crtscv++;
					memcpy(intbuffer,&indr2,sizeof(int));
					strcpy(tbs.payload,intbuffer);
					tbs.next_hop=tab_rutare[indr2][1];
					if(tab_rutare[indr2][1]!=-1)
						write (pipeout, &tbs, sizeof(msg));
					else
						printf("DEBUG - Failed to send %d to %d\n",tbs.creator,indr2);
				}

				break;
			}
			default:break;
			}
			}
			break;
		}
		case 8: {//complet implementat - nu modificati! (exceptie afisari on/off)

			printf ("Timp %d, Nod %d, msg tip 8 - cerere tabela de rutare\n", timp+1, nod_id);
			mesaj.type = 10;  //trimitere tabela de rutare
			mesaj.sender = nod_id;
			memcpy (mesaj.payload, &tab_rutare, sizeof (tab_rutare));
			//Observati ca acest tip de mesaj (8) se proceseaza imediat - nu se pune in nicio coada (vezi enunt)
			write (pipeout, &mesaj, sizeof(msg));

			break;
		}
		case 9:{ //complet implementat - nu modificati! (exceptie afisari on/off)

			//Aici poate sa apara timp -1 la unele "noduri"
			//E ok, e vorba de procesele care nu reprezentau rutere in retea, deci nu au de unde sa ia valoarea corecta de timp
			//Alternativa ar fi fost ca procesele neparticipante la simularea propriu-zisa sa ramana blocate intr-un apel de read()
			printf ("Timp %d, Nod %d, msg tip 9 - terminare simulare\n", timp, nod_id);
			gata = TRUE;
			break;
		}

		default:
			printf ("\nEROARE: Timp %d, Nod %d, msg tip %d - NU PROCESEZ ACEST TIP DE MESAJ\n", timp, nod_id, mesaj.type);
			exit (-1);
		}

	}
	return 0;
}

