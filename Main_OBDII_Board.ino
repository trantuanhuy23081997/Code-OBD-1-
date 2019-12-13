#include "var.h"
#include "A9G_OS.h"
#include "HC_OS.h"
#include "BLE_OS.h"
#include "OBDII_OS.h"
void setup(){
  Serial.begin(250000);
  Serial1.begin(115200);
  Serial2.begin(9600);
  Serial3.begin(9600);
  Wire.begin();
//  HC01.Setting_Defauts();
  Conf_Board();
  HC01.ReadDataBoard();
  HC01.ConfigGPIO();
  xEvent_Receive_Group    =  xEventGroupCreate();
  xTimer2  = xTimerCreate("Auto-Reload",AUTO_RELOAD_TIMER_PERIOD,pdTRUE,0,prvAutoReloadCallback);
  if(xTimer2 !=NULL){
   xTimer2Started =  xTimerStart(xTimer2,0);
  }
  while( xTimer2Started !=pdPASS){} 
  
  xTimer3  = xTimerCreate("Auto-Reload-T3",AUTO_RELOAD_TIMER_PERIOD3,pdTRUE,0,prvAutoReloadCallback3);
  if(xTimer3 != NULL){
   xTimer3Started =  xTimerStart(xTimer3,0);
  }
  while( xTimer3Started !=pdPASS){} 
  
  if ( xSerialSemaphore == NULL ){
    xSerialSemaphore = xSemaphoreCreateMutex();
    if (( xSerialSemaphore ) != NULL ) xSemaphoreGive( ( xSerialSemaphore ) );
  };  
  xTimer4  = xTimerCreate("Auto-Reload", AUTO_RELOAD_TIMER_PERIOD4, pdTRUE, 0, prvAutoReloadCallback_OBD);
  if(xTimer4 !=NULL) xTimer4Started =  xTimerStart(xTimer4,0);
  while( xTimer4Started != pdPASS){} 
  
  OBDQueue      = xQueueCreate(2,70);
  OBDQueue_DTC  = xQueueCreate(1,60);
  OBDQueue_Data = xQueueCreate(1,sizeof(StateSys));
  BLEQueue      = xQueueCreate(10,12);
  A9GQueue      = xQueueCreate(1,50);
  
  //xTaskCreate(Task_Kill_Machine, "Task_kill", 150, NULL, 1,  &TaskHandle_Kill_Machine);
  xTaskCreate(Task_SetupA9G, "Task Setup A9G", 200, NULL, 2,  &TaskHandle_A9G);
  xTaskCreate(Task_Handle_Local, "Task 5", 250, NULL, 1,  &TaskHandle_Local);    
  //xTaskCreate(Task_SetupOBD, "Task OBD", 400, NULL, 1,  &TaskHandle_OBD);
  //xTaskCreate(Task_Check_Sys, "Task Check Systems", 300, NULL, 1,&TaskHandle_HC); 
  //xTaskCreate(Task_Warning, "Task Warning", 150, NULL, 1,  &TaskHandle_Warn);   
  //xTaskCreate(Task_Ble, "Task Serial BLE", 200, NULL, 1, &TaskHandle_Ble);
  #if DEBUG_HC_OS
    xTaskCreate(Task_Serial_Data, "Task3", 150, NULL, 1,  &TaskHandle_3);
  #endif  
//  xTaskCreate(Task_Caculator_Time, "Check time", 150, NULL, 1,  &TaskHandle_CTime);
//  xTaskCreate(Task_CLD, "Check CLK", 150, NULL, 1,  &TaskHandle_Door);
//  xTaskCreate(Task_Odometers, "Task_ODO", 200, NULL, 1,  &TaskHandle_Distance);
//  xTaskCreate(Task_Sleep, "Task_Sleep", 150, NULL, 1,  &TaskHandle_Sleep);
 
 }
void loop(){
  Serial.println(F("Task idle"));
}
