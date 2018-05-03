//AJAY RAJ
//SID: 861285158


#include <queue>
#include <cpp.h>              /*include the CSIM C++ header file*/ 

using namespace std;

facility *f;                 /*the service center*/ 

void customer() 
{ 
    create("customer");        /*make this a process*/ 
    f->use(exponential(0.5));   /*obtain needed amount of service*/ 
} 

extern "C" void sim() {
	create("sim");            /*make this a process*/ 
	f = new facility("JOE");      /*create the service center - f*/ 
	
	while(simtime() < 480) {  /*loop until end of simulation*/ 
		f->reserve();
		hold(uniform(12,20));
		f->release();
		hold(uniform(12,23));  /*delay between customer arrivals*/ 
		customer();           /*generate new customer*/ 
	}       
	report();                /*produce statistics report*/ 
} 



