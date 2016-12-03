#include <stdio.h>
#include <list>
#include "simlib.h"
#include <cstdlib>
#define POCET_POKLADEN 6
using namespace std;

list<int> otevrene;
list<int> zavrene;



Facility Uzeniny("Uzeniny");
Facility Pokladna[POCET_POKLADEN];

Histogram celk_cas("Celkova doba v system", 0, 80, 20);
Histogram ote("Otevirani pokladen", 0, 1000, 1000);
Histogram clos("Zavirani pokladen", 0, 1000, 1000);
int aktualni_pocet = 0;
int pokladny[5];
int prichod = 27;

class Zakaznik : public Process {
    public: int prerusen;
	void Behavior(){
		double start_time = Time;
		int minimum;
		int pozice;
		prerusen = 0;
		if(Random() < 0.1111){
			Seize(Uzeniny);
			Wait(Uniform(30, 80));
			Release(Uzeniny);
			if(Random() <20){
				goto pokladna;
			}
		}
		Wait(Uniform(60,90)+Exponential(400));
	pokladna:
            //int kam = (int)(Random()*aktualni_pocet + 0.5);
            minimum = 999;
            //printf("%d\n",minimum);
            pozice = -1;

            for(list<int>::iterator iter = otevrene.begin(); iter != otevrene.end(); iter++){
                if(Pokladna[(*iter)].QueueLen() < minimum ){
                    minimum = Pokladna[(*iter)].QueueLen();
                    pozice = (*iter);
                }

            }

            if(Pokladna[pozice].QueueLen() >= 8){
                if(otevrene.size() != 6){
                    otevrene.push_back(zavrene.front());
                    zavrene.pop_front();
                    ote(Time);
                    goto pokladna;
                }
            }
        opak:
            Seize(Pokladna[pozice]);
            if(prerusen) goto opak;
            Wait(10+Exponential(110));
            Release(Pokladna[pozice]);

            for(list<int>::iterator iter = otevrene.begin(); iter != otevrene.end();){
                if(Pokladna[(*iter)].QueueLen() == 0 && otevrene.size() > 1){
                    zavrene.push_back((*iter));
                    iter = otevrene.erase(iter);
                    clos(Time);
                }
                else{
                    iter++;
                }

            }
    konec:
		celk_cas(Time - start_time);
	}
};

class Generator : public Event {
	void Behavior(){
		(new Zakaznik)->Activate();
		//printf("pocet otevrenych: %d\n",otevrene.size());
		Activate(Time+Exponential(prichod));
	}
};
class Oprava : public Process {
    void Behavior(){
        int kde = (int)(Random()*5 + 0.5);
        printf("%d\n",kde);
        Seize(Pokladna[kde], 1);
        if(Pokladna[kde].Q2->Length() > 0){
            Zakaznik *zak = (Zakaznik*)Pokladna[kde].Q2->GetFirst();
            zak->prerusen = 1;
            Wait(120);
            zak->prerusen = 0;
            Release(Pokladna[kde]);
        }
        else{
            Wait(120);
            Release(Pokladna[kde]);
        }
    }
};
class Chyba : public Event{
    void Behavior(){
        (new Oprava)->Activate();
        Activate(Time+13*60);
    }
};

int main(int argc, char *argv[]){
    otevrene.push_back(0);
    zavrene.push_back(1);
    zavrene.push_back(2);
    zavrene.push_back(3);
    zavrene.push_back(4);
    zavrene.push_back(5);
    Pokladna[0].SetName("Pokladna1");
    Pokladna[1].SetName("Pokladna2");
    Pokladna[2].SetName("Pokladna3");
    Pokladna[3].SetName("Pokladna4");
    Pokladna[4].SetName("Pokladna5");
    Pokladna[5].SetName("Pokladna6");


    if(argc >= 2){
        prichod = atol(argv[1]);
    }
    SetOutput("out.txt");

	Init(0,1000000);
	(new Generator)->Activate();
	(new Chyba)->Activate();
	Run();

	Uzeniny.Output();

    for(int i = 0; i < POCET_POKLADEN; i++){
        Pokladna[i].Output();
    }

	celk_cas.Output();
	ote.Output();
	clos.Output();
	printf("Vysledky odpovidaji generovani zakazniku s exponencialnim rozlezeni: %d\n", prichod);
}
