#include "mbed.h"
#include "rtos.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"

#define THREAD_COUNT 4
#define THREAD_ACCESS_TIME 1000

void mainDraw();
void threadWork(int const *threadNum);
void drawThread(int const *threadNum);
bool touchDetected();
void init();
void drawMain();

TS_StateTypeDef TS_State;
Semaphore smphr(1);
volatile int repetition[THREAD_COUNT];

int main()
{ 
    init();  
          
    Thread threads[THREAD_COUNT];
    for (int i = 0; i < THREAD_COUNT; i++) {
        repetition[i] = 0;
    }
    
    int touchCount = 0;  
    
    while(true) { //check for screen touch every 100ms
        
        if (touchDetected()) {
            int mod = touchCount % THREAD_COUNT;
            if (touchCount++ < THREAD_COUNT) {
                threads[mod].start(callback(threadWork, (int *) mod));                  
            }
            repetition[mod]++;                       
            
            while (true) { //wait for screen touch release
                if (!touchDetected()) { break; }
                Thread::wait(100);
            }
        }    
        HAL_Delay(100);
    }  
}

bool touchDetected()
{
    BSP_TS_GetState(&TS_State);    
    if (TS_State.touchDetected != 0) { return true; } else { return false; }
}

void threadWork(int const *threadNum) 
{   
    while(true) {
        if (0 < repetition[(uint32_t)threadNum]) {   
        
            smphr.wait();
            
            drawThread(threadNum);           
            Thread::wait(THREAD_ACCESS_TIME);     
        
            repetition[(uint32_t)threadNum]--;            
            int nextId = ( (uint32_t)threadNum + 1 ) % THREAD_COUNT;
            if (repetition[nextId] == 0) { drawMain(); }
            
            smphr.release();            
        }
    }
}

void drawThread(int const *threadNum)
{
    switch ((uint32_t)threadNum) {
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

