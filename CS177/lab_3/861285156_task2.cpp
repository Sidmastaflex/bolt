// Siddhanth Sharma 861285156

#define GNU
#include <queue>
#include <cpp.h>              /*include the CSIM C++ header file*/

using namespace std;

#if defined(GNU)
double erand48(unsigned short xsubi[3]);
#elif defined(TURBO)
int rand ();
#endif

class randStream {
// Manage one stream of random numbers.
//
// "nextrand" gives the floating-point number obtained from a
// new value in this stream. "xsubi" stores that latest value.
private:
	unsigned short xsubi[3];

public:
	randStream (unsigned short seed = 0);
	double nextrand ();
};

randStream * arrivalStream; // auto arrival times
randStream * literStream; // number of litres needed
randStream * balkingStream; // balking probability
randStream * serviceStream; // service times

double randStream::nextrand ()
// Returns the next random number in this stream.
{
#if defined(GNU)
	return erand48 (xsubi);
#elif defined(TURBO)
	return ((double) rand())/((double) RAND_MAX);
#endif
}

randStream::randStream (unsigned short seed)
// Initialize the stream of random numbers.
{
#if defined(GNU)
	xsubi[0] = 0;
	xsubi[1] = 0;
	xsubi[2] = seed;
#elif defined(TURBO)
	// Turbo C doesn't provide a random-number generator with
	// explicit seeds. We will get along with just one stream.
	return;
#endif
}


facility_ms *station;                 /*the service center*/

void car()
{
    create("car");        /*make this a process*/
    station->use(uniform(10,60));   /*obtain needed amount of service*/
}

extern "C" void sim() {
	create("sim");            /*make this a process*/
	f = new facility_ms("Gas Station", 3);      /*create the service center - f*/

	while(simtime() < 10000) {  /*loop until end of simulation*/
		double liters = uniform(10,60);

		if (!(station->status() != 0 && balkingStream->nextrand() > (40 + liters) / (25 * (3 + station->qlength())))){
			station->reserve();
			hold(uniform(12,20));
			station->release();
		}
		hold(exponential(50));  /*delay between customer arrivals*/
		car();           /*generate new customer*/
	}
	report();                /*produce statistics report*/
}



