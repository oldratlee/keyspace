#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "System/Containers/SortedList.h"
#include "Timer.h"

class Scheduler
{
public:
	
	static SortedList<Timer*>	timers;
		
	static void Add(Timer* timer);
	
	static void Remove(Timer* timer);
	
	static void Reset(Timer* timer);
	
	static void Shutdown()					{ /* todo: deallocate timers? */ }
};

#endif
