#include <iostream>
#include "cpp.h"
#include <string.h>
#include <vector>
#include <stack>

using namespace std;

#define NUM_SEATS 6      // number of seats available on shuttle
#define TINY 1.e-20      // a very small time period
#define TERMNL 0         // named constants for labelling event set
#define CARLOT 1

long T;
long S;
long M;
stack<long> resting;
stack<long> active;

facility_set *drop;
facility_set *pick;
mailbox load("loading");

facility_set *buttons;  // customer queues at each stop
facility_set *rest;           // dummy facility indicating an idle shuttle

event get_off_now ("get_off_now");  // all customers can get off shuttle

event_set *hop_on;  // invite one customer to board at this stop
event boarded ("boarded");             // one customer responds after taking a seat

event_set *shuttle_called; // call buttons at each location

void make_passengers(long whereami);       // passenger generator
string places[2] = {"Terminal","CarLot"}; // where to generate
long group_size();

void passenger(long whoami);                // passenger trajectory
string people[2] = {"arr_cust","dep_cust"}; // who was generated

void shuttle(long i);                  // trajectory of the shuttle bus consists of...
void loop_around_airport(long & seats_used, long i, long ID);      // ... repeated trips around airport
void load_shuttle(long whereami, long & on_board); // posssibly loading passengers
qtable shuttle_occ("bus occupancy");  // time average of how full is the bus

extern "C" void sim(int argc, char* argv[])      // main process
{
  T = atoi(argv[1]);
  S = atoi(argv[2]);
  M = atoi(argv[3]);
  buttons = new facility_set("Curb", 2*T);
  hop_on = new event_set("board shuttle", 2*T);
  shuttle_called = new event_set("call button", T+1);
  drop = new facility_set("pickup", T+1);
  pick = new facility_set("dropoff", T+1);
  rest = new facility_set("rest", S);

  create("sim");
  shuttle_occ.add_histogram(NUM_SEATS+1,0,NUM_SEATS);
  make_passengers(TERMNL);  // generate a stream of arriving customers
  make_passengers(CARLOT);  // generate a stream of departing customers
  for(long i = 0; i < S; ++i) 
  {                         // create a single shuttle
    resting.push(i);
    shuttle(i);
  }
  hold (1440);              // wait for a whole day (in minutes) to pass
  report();
  status_facilities();
}

// Model segment 1: generate groups of new passengers at specified location

void make_passengers(long whereami)
{
  const char* myName=places[whereami > T].c_str(); // hack because CSIM wants a char*
  create(myName);

  while(clock < 1440.)          // run for one day (in minutes)
  {
    hold(expntl(M/T));           // exponential interarrivals, mean M
    long group = group_size();
    for (long i=0;i<group;i++)  // create each member of the group
      passenger(whereami);      // new passenger appears at this location
  }
}

// Model segment 2: activities followed by an individual passenger

void passenger(long whoami)
{
  const char* myName=people[whoami].c_str(); // hack because CSIM wants a char*
  create(myName);

  load.receive(&whoami);
  (*buttons)[whoami].reserve();     // join the queue at my starting location
  (*shuttle_called)[whoami].set();  // head of queue, so call shuttle
  (*hop_on)[whoami].queue();        // wait for shuttle and invitation to board
  (*shuttle_called)[whoami].clear();// cancel my call; next in line will push 
  
  hold(uniform(0.5,1.0));        // takes time to get seated
  boarded.set();                 // tell driver you are in your seat
  (*buttons)[whoami].release();     // let next person (if any) access button
  get_off_now.wait();          // everybody off when shuttle reaches next stop

}

// Model segment 3: the shuttle bus

int isNeeded() {
  for(long i = 0; i < 2*T; ++i) {
    if((*shuttle_called)[i].state() == OCC)
	return i;
  }
  return -1;
}

void shuttle(long i) {
    create ("shuttle");
    (*rest)[i].reserve(); 
    while(1) {  // loop forever
      // start off in idle state, waiting for the first call...
      // relax at garage till called from somewhere
      long top = resting.top();
      (*rest)[top].reserve();
      (*shuttle_called)[top].wait();
      (*shuttle_called)[top].set(); // loop exit needs to see event
      (*rest)[top].release();                   // and back to work we go!
      

      long seats_used = 0;              // shuttle is initially empty
      shuttle_occ.note_value(seats_used);
      
      active.push(top);
      resting.pop();

      hold(2);  // 2 minutes to reach car lot stop

      // Keep going around the loop until there are no calls waiting
      while (isNeeded() != -1) {
        load.send(i);
        loop_around_airport(seats_used, isNeeded(), top);
      }
    }
}

long group_size() {  // calculates the number of passengers in a group
  double x = prob();
  if (x < 0.3) return 1;
  else {
    if (x < 0.7) return 2;
    else return 5;
  }
}

void loop_around_airport(long & seats_used, long i, long ID) { // one trip around the airport
  // Start by picking up departing passengers at car lot
    (*pick)[i].reserve();
    load_shuttle(CARLOT, seats_used);
    (*pick)[i].release();
    shuttle_occ.note_value(seats_used);

    hold (uniform(3,5));  // drive to airport terminal

    // drop off all departing passengers at airport terminal
    if(seats_used > 0) {
      (*drop)[i].reserve();
      get_off_now.set(); // open door and let them off
      seats_used = 0;
      (*drop)[i].release();
      shuttle_occ.note_value(seats_used);
    }

    // pick up arriving passengers at airport terminal
    (*pick)[i].reserve();
    load_shuttle(TERMNL, seats_used);
    (*pick)[i].release();
    shuttle_occ.note_value(seats_used);

    hold (uniform(3,5));  // drive to Hertz car lot
 
    // drop off all arriving passengers at car lot
    if(seats_used > 0) {
      (*drop)[i].reserve();
      get_off_now.set(); // open door and let them off
      seats_used = 0;
      (*drop)[i].release();
      shuttle_occ.note_value(seats_used);
    }
  active.pop();
  resting.push(ID);
  (*rest)[ID].reserve();
  // Back to starting point. Bus is empty. Maybe I can rest...
}

void load_shuttle(long whereami, long & on_board)  // manage passenger loading
{
  // invite passengers to enter, one at a time, until all seats are full
  while((on_board < NUM_SEATS) &&
    ((*buttons)[whereami].num_busy() + (*buttons)[whereami].qlength() > 0))
  {
    (*hop_on)[whereami].set();// invite one person to board
    boarded.wait();  // pause until that person is seated
    on_board++;
    hold(TINY);  // let next passenger (if any) reset the button
  }
}

