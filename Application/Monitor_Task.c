#include "Monitor_Task.h"
#include "SerialLogger.h"
#include "FreeRTOS.h"
#include "task.h"

#include "tinyprintf.h"

static void sendMessage(void);
static int timeToSendMessage(void);

volatile int idleTickCountOfLastCycle;
volatile int monitorTickCount;
volatile int monitoringActive;
volatile int globalTickCount = 0;
volatile int ticksSinceLastMessage;

void Monitor_ResetTickCount(void)
{
  idleTickCountOfLastCycle = monitorTickCount;

  //monitoring active will stay zero until this task unblocks from the yield
  monitoringActive = 0;
  monitorTickCount = 0;
}

int idleHookReached = 0;

void Monitor_Task(void * argument)
{
  (void)argument;

  ticksSinceLastMessage = 0;
  monitoringActive = 0;

  while(1){
    taskYIELD();
    monitoringActive = 1;

    if(timeToSendMessage())
      sendMessage();
  }
}

static int timeToSendMessage(void){
  //every second
  if(ticksSinceLastMessage > 1000){
    ticksSinceLastMessage = 0;
    return 1;
  }
  return 0;
}

static void sendMessage(void)
{
  static char msg[100];
  int size = tfp_snprintf(msg, 100, "Idle Tick Count: %d\n", idleTickCountOfLastCycle);
  SerialLogger_Log(LOGTYPE_IDLE_MONITOR, msg, size);
}

void vApplicationIdleHook(void);
void vApplicationTickHook (void);

void vApplicationIdleHook(void)
{
  idleHookReached = 1;
}

void vApplicationTickHook (void)
{
  globalTickCount++;
  ticksSinceLastMessage++;

  if(monitoringActive)
    monitorTickCount++;
}
