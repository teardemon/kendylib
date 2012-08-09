#include "SysTime.h"
#include "thread.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
struct system_time_mgr
{	
	volatile int32_t   current_index __attribute__((aligned(8)));	
	time_t   sec[2];
	uint32_t ms[2];
	char     str[2][64];
	
	uint32_t sleep_time;	
};


static struct system_time_mgr *stm = NULL;

static void _update()
{
	int32_t index = (stm->current_index + 1)%2;
	
	stm->ms[index] = GetSystemMs();
	//printf("_update:%u\n",stm->ms[index]);
	
	time_t _now = time(NULL);
	stm->sec[index] = _now;
	
	struct tm _tm;
	gmtime_r(&_now, &_tm);
	snprintf(stm->str[index],64,"[%04d-%02d-%02d %02d:%02d:%02d]",_tm.tm_year+1900,_tm.tm_mon+1,_tm.tm_mday,_tm.tm_hour,_tm.tm_min,_tm.tm_sec);	
	stm->current_index  = index;
}

static void *_thread_routine(void *arg)
{
	while(1)
	{
		sleepms(stm->sleep_time);
		_update();
	}
}

void init_system_time(uint32_t sleep_time)
{
	if(stm == NULL)
	{
		stm = calloc(1,sizeof(*stm));
		stm->current_index = 1;
		stm->sleep_time = sleep_time;
		_update();
		CREATE_THREAD_RUN(0,_thread_routine,NULL);
	}
}

time_t   GetCurrentSec()
{
	int32_t index = stm->current_index;
	return stm->sec[index];
}

uint32_t GetCurrentMs()
{
	int32_t index = stm->current_index;
	return stm->ms[index];
}

const char *GetCurrentTimeStr()
{
	int32_t index = stm->current_index;
	return stm->str[index];
}





