#include <PDLib.h>
#include <ArduinoJson.h>
#include <Arduino_FreeRTOS.h>
#include <portmacro.h>
#include <timers.h>
#include <queue.h>
#include <event_groups.h>
#include <task.h>

#include <avr/power.h>

#ifdef __AVR__
#include <avr/io.h>
#include <avr/pgmspace.h> 
#endif

#include <stdlib.h>

State_Vehicle State_M;
Buffer_DangerState Buffer_Danger;
StateSys State_Car;

#define RECEIVE_COMPLETE_BIT  (1U << 0U)
#define KILL_MACHINE_BIT      (1U << 1U)
#define ENGINE_OFF_BIT        (1U << 2U)
#define NOT_WARNING_BIT       (1U << 3U)

#define DEBUG                 1
#define DEBUG_TIMER           0
#define DEBUG_HC_OS           0

EventGroupHandle_t  xEvent_Receive_Group;

#define AUTO_RELOAD_TIMER_PERIOD  (pdMS_TO_TICKS(150))
#define AUTO_RELOAD_TIMER_PERIOD3 (pdMS_TO_TICKS(80))
#define AUTO_RELOAD_TIMER_PERIOD4 (pdMS_TO_TICKS(120))

TimerHandle_t  xTimer4;
BaseType_t  xTimer4Started;
char rxData[70];
byte index2 = 0;

#define RECEIVE_COMPLETE_BIT_OBD        (1U << 4U)
QueueHandle_t OBDQueue;
QueueHandle_t OBDQueue_DTC;
QueueHandle_t OBDQueue_Data;
TaskHandle_t TaskHandle_OBD;
volatile byte Cmplete_Sent = 0;// Biến tăng đếm gửi dữ liệu qua Stn1110.
char * Data_Buffer_DTCsJson = (char*)pvPortMalloc(60);

static char RxData[12];
volatile byte index = 0;
volatile byte FLAGDOR = 0;
volatile byte FLAG_POWER_DOWN  = 0;
volatile byte Time_repeat = 0;

typedef enum {
  DoNothing,
  Idle,
  SendedTCPStart,
  SendedTCPsend,
  SendedData
} SendTCP_State;

char * RxDataA9G = (char*) malloc(70);
volatile byte index_receive_a9g = 0;

unsigned long Time_Start_Engine = 0;

TaskHandle_t  TaskHandle_Ble;
TaskHandle_t TaskHandle_HC;
TaskHandle_t TaskHandle_Warn;
TaskHandle_t TaskHandle_A9G;
TaskHandle_t TaskHandle_Local;
TaskHandle_t TaskHandle_SendWeb;
TaskHandle_t TaskHandle_JS;
TaskHandle_t TaskHandle_3;
TaskHandle_t TaskHandle_Kill_Machine;
TaskHandle_t TaskHandle_CTime;
TaskHandle_t TaskHandle_Door;
TaskHandle_t TaskHandle_Sleep;
TaskHandle_t TaskHandle_Distance;


QueueHandle_t     BLEQueue;
QueueHandle_t     A9GQueue;
TimerHandle_t     xTimer2;
BaseType_t        xTimer2Started;


TimerHandle_t     xTimer3;
BaseType_t        xTimer3Started;
SemaphoreHandle_t xSerialSemaphore;
SemaphoreHandle_t xSerial1Semaphore;

char Jsonstring[300];  // send this String

volatile SendTCP_State state  = DoNothing;  

bool EventSerialA9G = true;

char DataGPSLat[11];
char DataGPSLon[11];

char _DataGPSLat[11] = "10.774340\0";
char _DataGPSLon[11] = "106.656117\0";

#define ASCII_A 'A'
#define ASCII_F 'F'
#define ASCII_0 '0'
#define ASCII_9 '9'

byte sum_DTCs = 0;

void Conf_Board (void)
{
  /*
  // RST & PWR of A9G in Version 1.01.
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  digitalWrite(10, HIGH);// reset.
  digitalWrite(11, LOW);//pwr
  delay(2000);
  digitalWrite(11, HIGH);
  */
  // Enable power for  OBDII and IO Block.
  pinMode(34, OUTPUT);
  pinMode(35, OUTPUT);
  digitalWrite(34, HIGH); // 5.0V
  digitalWrite(35, HIGH); // 3.0V
  // RST & PWR of A9G in Version 1.01.
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  digitalWrite(6, LOW);// reset.
  digitalWrite(7, HIGH); //pwr
  digitalWrite(BUZZER,HIGH);
  delay(200);
  digitalWrite(6,HIGH);
  digitalWrite(7,LOW);
  digitalWrite(BUZZER,LOW);
  delay(2000);
  digitalWrite(7,HIGH);
  
}
