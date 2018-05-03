// Simulation of the Hertz airport shuttle bus, which picks up passengers
// fromt he airport terminal building going to the Hertz rental car lot

#include "cpp.h"
#include <string.h>
#include <stdlib.h>

#define NUM_SEATS 6		// number of seats available on shuttle

#define TINY 1.e-20		// a very small time period

#define TERMNL 0		// named constants for labelling event set
#define CARLOT 1

facility_set *terminals;	// customer queue at airport terminal
facility carlot ("carlot");	// customer queue at rental car lot
facility rest ("rest");		// dummy facility indicating an idle shuttle

event get_off_now ("get_off_now"); // all customers can get off shuttle

event on_termnl ("on_termnl");	// one customer can get on shuttle at terminal
event on_carlot ("on_carlot");	// one customer can get on shuttle at car lot
event boarded ("boarded");	// one customer has gotten on and taken a seat

event_set shuttle_called ("call button", 2);
				// call buttons at terminal and car lot

void arrivals(int T);	// function declarations
void arr_cust(int T);
void departures();
void dep_cust();
void shuttle(int T);
long group_size();

extern "C" void sim(int argc, char* argv[])		// main process
{
	long T = atoi(argv[1]) + 1;
	long S = atoi(argv[2]);
	long M = atoi(argv[3]);
	terminals = new facility_set("termnl", T);
	create("sim");
	arrivals(T);		// start a stream of arriving customers
	departures();		// start a stream of departing customers
	shuttle(T);		// create a single shuttle
	hold (1440);		// wait for a whole day (in minutes) to pass
	report();
}

// Model segment 1a: generate groups of customers arriving at the airport

void arrivals(int T)
{
	create("arrivals");

	while(clock < 1440.)	//
	{
		hold(expntl(10)); // exponential interarrivals, mean 10 minutes
		long group = group_size();
		for (long i=0;i<group;i++)
			arr_cust(T);	// new customer appears at the airport
	}
}

// Model segment 1b: activities followed by individual  airport customers

void arr_cust(int T)
{
	create("arr_cust");
	int tnum = uniform(1,T);
	(*terminals)[tnum].reserve();	// join the queue at the airport terminal
	shuttle_called[TERMNL].set();	// head of queue, so call shuttle
	on_termnl.queue();	// wait for shuttle and invitation to board
	shuttle_called[TERMNL].clear();	// clear call; next in line will push 
	boarded.set();		// tell driver you are in your seat
	(*terminals)[tnum].release();	// now the next person (if any) is head of queue
	get_off_now.wait();	// everybody off when shuttle reaches car lot
}

// Model segment 2a: generate groups of customers at car lot, heading to airport

void departures()		// this model segment spawns departing customers
{
	create("departures");

	while(clock < 1440.)	//
	{
		hold(expntl(10)); // exponential interarrivals, mean 10 minutes
		long group = group_size();
		for (long i=0;i<group;i++)
			dep_cust();	// new customer appears at the car lot
	}
}

// Model segment 2b: activities followed by individual car lot customers

void dep_cust()
{
	create("dep_cust");

	carlot.reserve();	// join the queue at the car lot
	shuttle_called[CARLOT].set();	// head of queue, so call shuttle
	on_carlot.queue();	// wait for shuttle and invitation to board
	shuttle_called[CARLOT].clear();	// clear call; next in line will push 
	boarded.set();		// tell driver you are in your seat
	carlot.release();	// now the next person (if any) is head of queue
	get_off_now.wait();	// everybody off when shuttle reaches car lot
}

// Model segment 3: the shuttle bus

void shuttle(int T)
{
	create ("shuttle");

	while(1)		// loop forever
	{
		// start off in idle state, waiting for the first call...
		// should check here that bus is indeed empty!
		rest.reserve();
		long who_pushed = shuttle_called.wait_any();
			// relax at garage till called from either location
		shuttle_called[who_pushed].set();
			// hack to satisfy loop exit, below
		rest.release();
		hold(2);	// 2 minutes to get to car lot stop

		// go around the loop until there are no calls waiting

		long seats_used = 0;	// shuttle is initially empty

		while ((shuttle_called[TERMNL].state()==OCC)||
			(shuttle_called[CARLOT].state()==OCC))
		{
			// ready to load customers at car lot
			// invite them in, one by one, till all seats are full

			while((seats_used < NUM_SEATS) &&
				(carlot.num_busy() + carlot.qlength() > 0))
			{
				on_carlot.set();// invite one person to board
				boarded.wait();	// that person is now on board
				seats_used++;
				hold(TINY);
			}

			hold (uniform(3,5));	// drive to airport terminal

			// drop off any passengers on board

			if(seats_used > 0)
			{
				get_off_now.set(); // open door and let them off
				seats_used = 0;
			}

			// ready to load customers at airport terminal
			// invite them in, one by one, till all seats are full
			
			for(int i = 1; i < T; ++i) {
				while((seats_used < NUM_SEATS) &&
					((*terminals)[i].num_busy() + (*terminals)[i].qlength() > 0))
				{
					on_termnl.set();// invite one person to board
					boarded.wait();	// that person is now on board
					seats_used++;
					hold(TINY);
				}
			}

			hold (uniform(3,5));	// drive to Hertz car lot

			// drop off any passengers on board

			if(seats_used > 0)
			{
				get_off_now.set(); // open door and let them off
				seats_used = 0;
			}
		}
	}
}

long group_size()	// function gives the number of customers in a group
{
	double x = prob();
	if (x < 0.3) return 1;
	else
	{
		if (x < 0.7) return 2;
		else
			return 4;
	}
}

