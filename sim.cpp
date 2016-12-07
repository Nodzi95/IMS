#include <stdio.h>
#include <list>
#include "simlib.h"
#include <cstdlib>
#include <string>
#include <ctime>
#define POCET_POKLADEN 6

#define DEBUG 1

using namespace std;

list<int> otevrene;
list<int> zavrene;



Facility Uzeniny("Uzeniny");
Facility Pokladna[POCET_POKLADEN];

int prichod = 14;
int sekund = 10000000;
int dvacetina = 10000000/20;
int delka = 6;

//histogram celkoveho casu v systemu
Histogram celk_cas("Celkova doba v system", 0, 6000/20, 20);

//histogrami otevirani pokladen
Histogram pokO1("Otevreni pokladny1", 0, dvacetina, 20);
Histogram pokO2("Otevreni pokladny2", 0, dvacetina, 20);
Histogram pokO3("Otevreni pokladny3", 0, dvacetina, 20);
Histogram pokO4("Otevreni pokladny4", 0, dvacetina, 20);
Histogram pokO5("Otevreni pokladny5", 0, dvacetina, 20);
Histogram pokO6("Otevreni pokladny6", 0, dvacetina, 20);

//histogrami zavirani pokladen
Histogram pokZ1("Zavreni pokladny1", 0, dvacetina, 20);
Histogram pokZ2("Zavreni pokladny2", 0, dvacetina, 20);
Histogram pokZ3("Zavreni pokladny3", 0, dvacetina, 20);
Histogram pokZ4("Zavreni pokladny4", 0, dvacetina, 20);
Histogram pokZ5("Zavreni pokladny5", 0, dvacetina, 20);
Histogram pokZ6("Zavreni pokladny6", 0, dvacetina, 20);

//histogrami doby na pokladne
Histogram cas_na_pokladne("Doba na pokladne", 0, 100, 20);


int otevreno = 0;
int zavreno = 0;
int chyba = 0;
int uzeniny = 0;
int clovek = 0;

class Zakaznik : public Process {
    public: int prerusen;
	void Behavior(){
		double start_time = Time;
		int pozice;
		int minimum;
		double pokl_time;
		prerusen = 0;

		if(Random() < 0.1111){  //11.11% jde k uzeninam
			Seize(Uzeniny);
			Wait(Uniform(30, 80));
			Release(Uzeniny);
#if DEBUG
			uzeniny++;
#endif
			if(Random() <20){   //20% jde z uzenin rovnou k pokladne
                //rovnou k pokladne
				goto pokladna;
			}
		}
		//nakupuji
		Wait(Uniform(100,200)+Exponential(400));
	pokladna:
            minimum = 999;
            pozice = -1;
            //vyber pokladnu s nejkratsi frontou
            for(list<int>::iterator iter = otevrene.begin(); iter != otevrene.end(); iter++){
                if(Pokladna[(*iter)].QueueLen() < minimum ){
                    minimum = Pokladna[(*iter)].QueueLen();
                    pozice = (*iter);
                }
            }
            //pokud ma vybrana pokladna frontu vetsi nebo rovno 8 a celkovy pocet pokladen neni vetsi jak 6, tak se otevre nova pokladna
            if(Pokladna[pozice].QueueLen() >= delka){
                if(otevrene.size() != 6){
                    otevrene.push_back(zavrene.front());
                    zavrene.pop_front();
#if DEBUG
                    otevreno++;
                    if(otevrene.back() == 0) pokO1(Time);
                    else if(otevrene.back() == 1) pokO2(Time);
                    else if(otevrene.back() == 2) pokO3(Time);
                    else if(otevrene.back() == 3) pokO4(Time);
                    else if(otevrene.back() == 4) pokO5(Time);
                    else if(otevrene.back() == 5) pokO6(Time);
#endif // DEBUG
                    goto pokladna;
                }
            }
        opak:
            //zaber pokladnu
		pokl_time = Time;
            Seize(Pokladna[pozice]);
            if(prerusen){
		 goto opak;
	    }
            //obsluha na pokladne
            Wait(Uniform(5,15)+Exponential(55));
            //vraceni prodavacky
            Release(Pokladna[pozice]);
		cas_na_pokladne(Time - pokl_time);
            //projde otevrene pokladny a zavrou se ty, ktere maji frontu o delce 0
            for(list<int>::iterator iter = otevrene.begin(); iter != otevrene.end();){
                if(Pokladna[(*iter)].QueueLen() == 0 && otevrene.size() > 1){
#if DEBUG
                    zavreno++;
                    if(otevrene.back() == 0) pokZ1(Time);
                    else if(otevrene.back() == 1) pokZ2(Time);
                    else if(otevrene.back() == 2) pokZ3(Time);
                    else if(otevrene.back() == 3) pokZ4(Time);
                    else if(otevrene.back() == 4) pokZ5(Time);
                    else if(otevrene.back() == 5) pokZ6(Time);
#endif // DEBUG
                    zavrene.push_back((*iter));
                    iter = otevrene.erase(iter);
                }
                else{
                    iter++;
                }
            }
    konec:
        //doba stravena v systemu
		celk_cas(Time - start_time);
	}
};

class Generator : public Event {
	void Behavior(){
		clovek++;
		(new Zakaznik)->Activate();
		Activate(Time+Exponential(prichod));
	}
};
class Oprava : public Process {
    void Behavior(){
#if DEBUG
	chyba++;
#endif
        int kde = (int)(Random()*5 + 0.5);
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
        Activate(Time+Uniform(7,14)*60);
    }
};

int main(int argc, char *argv[]){
	RandomSeed(time(0));
	
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
    string file = "output.out";

    if(argc >= 2){  //nastaveni prichodu zakazniku
        prichod = atol(argv[1]);
    }
    if(argc >= 3){  //nastaveni delky fronty pro otevreni pokladny
        delka = atol(argv[2]);
    }
    if(argc >= 4){  //nastaveni vystupniho souboru
        file = argv[3];
    }

    SetOutput(file.c_str());

	Init(0,sekund);

	(new Generator)->Activate();
	(new Chyba)->Activate();

	Run();
    //vypisy statistik front
	Uzeniny.Output();
    for(int i = 0; i < POCET_POKLADEN; i++){
        Pokladna[i].Output();
    }
    //vypisy histogramu
    //celkovy cas v systemu
	celk_cas.Output();
#if DEBUG
	Print("\nDebug\n");
	Print("Pocet otevreni pokladen: %d\n", otevreno);
	Print("Pocet uzavreni pokladen: %d\n", zavreno);
	Print("Pocet pristupu k uzeninam: %d\n", uzeniny);
	Print("Pocet chyb u pokladen: %d\n", chyba);
	Print("Prumerny prichod: %g\n", 10000000.0/clovek);
	SIMLIB_statistics.Output();
    //otevirani pokladen
	/*pokO1.Output();
	pokO2.Output();
	pokO3.Output();
	pokO4.Output();
	pokO5.Output();
	pokO6.Output();
	//zavreni pokladen
	pokZ1.Output();
	pokZ2.Output();
	pokZ3.Output();
	pokZ4.Output();
	pokZ5.Output();
	pokZ6.Output();*/
#endif // DEBUG
	cas_na_pokladne.Output();

#if DEBUG
	printf("Vysledky odpovidaji generovani zakazniku s exponencialnim rozlezeni: %d\n", prichod);
#endif // DEBUG
}
