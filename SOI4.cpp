#include "monitor.h"
#include <cstdlib>
#include <vector>
#include <time.h>
#include <pthread.h>
#include <iostream>

using namespace std;


class MyMonitor: Monitor{
	public:
		void printBuffer();
		bool canPutEven();
		bool canPopEven();
		bool canPutOdd();
		bool canPopOdd();
		void putEven(int numEven);
		void popEven();
		void putOdd(int numOdd);
		void popOdd();
	private:
		vector<int> buffer;
		unsigned int nrEven = 0, nrOdd = 0;
		Condition evenProdCond, evenConsCond, oddProdCond, oddConsCond;
		unsigned int numOfProdEvenWaiting = 0, numOfProdOddWaiting = 0, numOfConsEvenWaiting = 0, numOfConsOddWaiting = 0;
};

// buffer printing

void MyMonitor::printBuffer(){
	for(int i = 0; i < buffer.size(); i++){
		cout << buffer[i] << " ";
	}
	cout << endl << endl;
}

// checkers

// checkers - even numbers

bool MyMonitor::canPutEven(){
	return nrEven < 10;
}

bool MyMonitor::canPopEven(){
	return (buffer.size() >= 3) && (buffer[0] % 2 == 0);
}

// checkers - odd numbers

bool MyMonitor::canPutOdd(){
	return nrEven > nrOdd;
}

bool MyMonitor::canPopOdd(){
	return (buffer.size() >= 7) && (buffer[0] % 2 == 1);
}

// manipulators

// manipulators - even numbers

void MyMonitor::putEven(int numEven){
	enter();
	
	if(!canPutEven()){
		numOfProdEvenWaiting ++;
		leave();
		wait(evenProdCond);
	}
	
	printf("Appending even number: [%d]\n", numEven);
	buffer.push_back(numEven);
	nrEven ++;
	printBuffer();
	
	if(numOfProdOddWaiting > 0 && canPutOdd()){
		numOfProdOddWaiting --;
		signal(oddProdCond);
	}
	else if(numOfConsEvenWaiting > 0 && canPopEven()){
		numOfConsEvenWaiting --;
		signal(evenConsCond);
	}
	else if(numOfConsOddWaiting > 0 && canPopOdd()){
		numOfConsOddWaiting --;
		signal(oddConsCond);
	}
	else leave();
}

void MyMonitor::popEven(){
	enter();
	
	if(!canPopEven()){
		numOfConsEvenWaiting ++;
		leave();
		wait(evenConsCond);
	}
	
	printf("Popping even number: [%d]\n", buffer[0]);
	buffer.erase(buffer.begin());
	nrEven --;
	printBuffer();
	
	if(numOfProdEvenWaiting > 0 && canPutEven()){
		numOfProdEvenWaiting --;
		signal(evenProdCond);
	}
	else if(numOfProdOddWaiting > 0 && canPutOdd()){
		numOfProdOddWaiting --;
		signal(oddProdCond);
	}
	else if(numOfConsOddWaiting > 0 && canPopOdd()){
		numOfConsOddWaiting --;
		signal(oddConsCond);
	}
	else leave();
}

// manipulators - odd numbers

void MyMonitor::putOdd(int numOdd){
	enter();
	
	if(!canPutOdd()){
		numOfProdOddWaiting ++;
		leave();
		wait(oddProdCond);
	}
	
	printf("Appending odd number: [%d]\n", numOdd);
	buffer.push_back(numOdd);
	nrOdd ++;
	printBuffer();
	
	if(numOfProdEvenWaiting > 0 && canPutEven()){
		numOfProdEvenWaiting --;
		signal(evenProdCond);
	}
	else if(numOfConsEvenWaiting > 0 && canPopEven()){
		numOfConsEvenWaiting --;
		signal(evenConsCond);
	}
	else if(numOfConsOddWaiting > 0 && canPopOdd()){
		numOfConsOddWaiting --;
		signal(oddConsCond);
	}
	else leave();
}

void MyMonitor::popOdd(){
	enter();
	
	if(!canPopOdd()){
		numOfConsOddWaiting ++;
		leave();
		wait(oddConsCond);
	}
	
	printf("Popping odd number: [%d]\n", buffer[0]);
	buffer.erase(buffer.begin());
	nrOdd --;
	printBuffer();
	
	if(numOfProdEvenWaiting > 0 && canPutEven()){
		numOfProdEvenWaiting --;
		signal(evenProdCond);
	}
	else if(numOfProdOddWaiting > 0 && canPutOdd()){
		numOfProdOddWaiting --;
		signal(oddProdCond);
	}
	else if(numOfConsEvenWaiting > 0 && canPopEven()){
		numOfConsEvenWaiting --;
		signal(evenConsCond);
	}
	else leave();
}

/* ######################################################################################################### */

MyMonitor monitor;

/* ######################################################################################################### */

// producers and consumers

// producers and consumers - even numbers

void* evenProducer(void *threadid){
	while(1){
		int numEven = (rand() % 25) * 2;
		monitor.putEven(numEven);
		sleep(1 + rand() % 2);
	}
}

void* evenConsumer(void *threadid){
	while(1){
		monitor.popEven();
		sleep(1 + rand() % 2);
	}
}

// producers and consumers - odd numbers

void* oddProducer(void *threadid){
	while(1){
		int numOdd = (rand() % 25) * 2 + 1;
		monitor.putOdd(numOdd);
		sleep(1 + rand() % 2);
	}
}

void* oddConsumer(void *threadid){
	while(1){
		monitor.popOdd();
		sleep(1 + rand() % 2);
	}
}

// main

int main(){
	srand(time(NULL));

   	pthread_t threads[4];
   	int rc;

   	printf("Stworzono wątek even Producer\n");
	rc = pthread_create(&threads[0], NULL, evenProducer, (void *)0);

	printf("Stworzono wątek odd Producer\n");
	rc = pthread_create(&threads[1], NULL, oddProducer, (void *)1);

	printf("Stworzono wątek even Consumer\n");
	rc = pthread_create(&threads[2], NULL, evenConsumer, (void *)2);

	printf("Stworzono wątek odd Consumer\n");
	rc = pthread_create(&threads[3], NULL, oddConsumer, (void *)3);

    	pthread_exit(NULL);
    	return 0;
}
