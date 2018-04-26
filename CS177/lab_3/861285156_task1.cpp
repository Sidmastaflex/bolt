//Siddhanth Sharma 861285156

#include <queue>
#include <cpp.h>

using namespace std;

facility *f; // The service center


void customer()
{
	create("customer"); //make this a process
	f->use(exponential(0.5)); //obtain needed amount of service

}

extern "C" void sim()
{
	create("sim"); //make this a process
	f = new facility("JOE"); //create the service center - f
	while(simtime() < 480.0) { //loop until end of sim
		f->reserve();
		hold(uniform(12,20)); //delay between customer arrivals
		f->release();
		hold(uniform(12,23));
		customer(); //generate new customers
		}
	report(); //produce statistics report
}

