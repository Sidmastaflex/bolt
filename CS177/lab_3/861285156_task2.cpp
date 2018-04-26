// Siddhanth Sharma 861285156

#define GNU

#include <queue>
#include <iomanip>
#include <stdio.h>
#include <math.h>
#include <stack>
#include <vector>
#include <cpp.h>

using namespace std;

double erand48(unsigned short xsubi[3]);
class randStream {
private:
	unsigned short xsubi[3];

public:
	randStream()
	double nextrand();
};

randStream * arrivalStream;
randStream * litreStream;
randStream * balkingStream;
randStream * serviceStream;

double randStream::nextrand ()
{
#if defined(GNU)
	return erand48 (xsubi);
#elif defined (TURBO)
	return ((double) rand())/((double) RAND_MAX);
#endif
}

randStream::randStream ()
{
#if defined(GNU)
	xsubi[0] = 0;
	xsubi[1] = 0;
	xsubi[2] = 4;
#elif defined (TURBO)
	return;
#endif
}

facility_ms station("pumps", 10);

void car()
{
	create("car"); //make this a process
	station.use(exponential(50.0)); //obtain needed amount of service
	
}

extern "C" void sim() 
{
	create("sim"); //make this a process
	//facility_ms *station;
	//station = new facility_set("pumps",10); //create a pump - p
	while(simtime() < 10000) { //loop until end of sim 		
		station.reserve();
		hold(uniform(12,20)); //delay between customer arrivals
		station.release();
		hold(exponential(50.0));
		car(); //generate new customers
		}
	report(); //produce statistics report
}

