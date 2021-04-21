#include "mbed.h"
#include "rtos.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"

#define THREAD_COUNT 4
#define THREAD_ACCESS_TIME 1000

void mainDraw();
void threadWork(int const *threadNum);
void drawThread(int threadNum);
bool touchDetected();
void init();
void drawMain();

TS_StateTypeDef TS_State;
Semaphore smphr(0);
bool switchThread = false;
bool mainShouldDraw = true;

int main()
{ 
    init();      
    int fi = 0;
    
    Thread threads[THREAD_COUNT];
    for(int i = 0; i < THREAD_COUNT; i++) {
        threads[i].start(callback(threadWork, (int *)i));
    }
    
    while(true) { //check for screen touch every 100ms
        if (mainShouldDraw) {
            drawMain();
            mainShouldDraw = false;
        }
        
        if (touchDetected()) {   
        
            if (fi++ == 0) { smphr.release(); } else { switchThread = true; }   //release semaphore on first iteration        
            
            while (true) { //wait for screen touch release
                if (!touchDetected()) { break; }
                Thread::wait(100);
            }
        }    
        Thread::wait(100);
    }  
}

bool touchDetected()
{
    BSP_TS_GetState(&TS_State);    
    if (TS_State.touchDetected != 0) { return true; } else { return false; }
}

void drawThread(int threadNum)
{
    switch (threadNum) {
        case 0:
            BSP_LCD_Clear(LCD_COLOR_RED);
            BSP_LCD_SetBackColor(LCD_COLOR_RED);   
            break;
        case 1:
            BSP_LCD_Clear(LCD_COLOR_BLUE);
            BSP_LCD_SetBackColor(LCD_COLOR_BLUE);  
            break;
        case 2:
            BSP_LCD_Clear(LCD_COLOR_BROWN); 
            BSP_LCD_SetBackColor(LCD_COLOR_BROWN);   
            break;    
        case 3:
            BSP_LCD_Clear(LCD_COLOR_BLACK);
            BSP_LCD_SetBackColor(LCD_COLOR_BLACK);   
            break; 
    }   
     
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);  
    uint8_t str[20];
    sprintf((char*) str, "Current thread: %d",(uint32_t) threadNum);
    BSP_LCD_DisplayStringAt(0, LINE(5), (uint8_t *)str, CENTER_MODE);
}

void threadWork(int const *threadNum) 
{   
    while (true)
    {
        smphr.wait();       
        mainShouldDraw = false;
        drawThread((int)threadNum);           
        Thread::wait(THREAD_ACCESS_TIME);         
        mainShouldDraw = true;
        while (true) {                    
            if (switchThread) { 
                switchThread = false;
                smphr.release();
                break;
            } 
            Thread::wait(100); 
        }          
    }
}

void init() {
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FB_START_ADDRESS);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);    
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    HAL_Delay(1000);
 
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) != TS_OK) 
    {
        BSP_LCD_Clear(LCD_COLOR_RED);
        BSP_LCD_SetBackColor(LCD_COLOR_RED);
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_DisplayStringAt(0, LINE(5), (uint8_t *)"TOUCHSCREEN INIT FAIL", CENTER_MODE);
        HAL_Delay(1000);
        NVIC_SystemReset();
    }    
      
    drawMain();
}

void drawMain() {
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE); 
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);          
    
    uint8_t str[20];
    sprintf((char*) str, "Current thread: MAIN");
    BSP_LCD_DisplayStringAt(0, LINE(5), (uint8_t *)str, CENTER_MODE);
}

