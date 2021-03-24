#include "mbed.h"
#include "rtos.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"

#define ADD_TIME 1
#define SUB_TIME 1
#define UPDATE_TIME 100
#define RESET_AFTER_TIME 5000

void drawTimebomb();
void drawTimer();
void timerTick();
void detectTouch();

TS_StateTypeDef TS_State;
volatile static int bombTime = 120;

int main()
{ 
    BSP_LCD_Init();
    BSP_LCD_LayerDefaultInit(LTDC_ACTIVE_LAYER, LCD_FB_START_ADDRESS);
    BSP_LCD_SelectLayer(LTDC_ACTIVE_LAYER);
    
    BSP_LCD_Clear(LCD_COLOR_WHITE);
    HAL_Delay(1000);
 
    if (BSP_TS_Init(BSP_LCD_GetXSize(), BSP_LCD_GetYSize()) == TS_OK) {
        
        drawTimebomb();        
        BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
        BSP_LCD_SetTextColor(LCD_COLOR_RED);
                
        Thread timer;
        timer.start(timerTick);
        
        Thread draw;
        draw.start(drawTimer);
        
        while(bombTime >= 1)
        {   
            drawTimer();         
            detectTouch();
            HAL_Delay(UPDATE_TIME);
        }
        
        BSP_LCD_Clear(LCD_COLOR_RED);
        BSP_LCD_SetBackColor(LCD_COLOR_RED);
        BSP_LCD_SetTextColor(LCD_COLOR_BLACK);        
        BSP_LCD_DisplayStringAt(0, LINE(5), (uint8_t *)"BUM", CENTER_MODE);
        //Watchdog::get_instance().start(RESET_AFTER_TIME); //? kde na to vezmu knihovnu ?
        HAL_Delay(RESET_AFTER_TIME);
        NVIC_SystemReset();
        
    } else {
        BSP_LCD_Clear(LCD_COLOR_RED);
        BSP_LCD_SetBackColor(LCD_COLOR_RED);
        BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
        BSP_LCD_DisplayStringAt(0, LINE(5), (uint8_t *)"TOUCHSCREEN INIT FAIL", CENTER_MODE);
    }    
}

void detectTouch()
{
    BSP_TS_GetState(&TS_State);
    int x, y; 
    
    if (TS_State.touchDetected != 0)
    {
        x = TS_State.touchX[0];
        y = TS_State.touchY[0];

        if(x >= 40 && x <= 110 && y >= 10 && y <= 80)
        {
            bombTime -= SUB_TIME;
        }
        if(x >= 320 && x <= 440 && y >= 10 && y <= 80)
        {
            bombTime += ADD_TIME;
        }                
    }
}

void timerTick()
{    
    while (bombTime > 0) {
        bombTime--;
        Thread::wait(1000);    
    }
}

void drawTimebomb()
{
    BSP_LCD_Clear(LCD_COLOR_WHITE);    
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);
    
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DrawLine(50, 20, 150, 20);
    BSP_LCD_DrawLine(150, 20, 150, 70);
    BSP_LCD_DrawLine(150, 70, 50, 70);
    BSP_LCD_DrawLine(50, 70, 50, 20);    
        
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);    
    BSP_LCD_DisplayStringAt(60, 35, (uint8_t *)"MINUS", LEFT_MODE);
    
    BSP_LCD_SetTextColor(LCD_COLOR_BLUE);
    BSP_LCD_DrawLine(330, 20, 430, 20);
    BSP_LCD_DrawLine(430, 20, 430, 70);
    BSP_LCD_DrawLine(430, 70, 330, 70);
    BSP_LCD_DrawLine(330, 70, 330, 20);
    
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    BSP_LCD_DisplayStringAt(345, 35, (uint8_t *)"PLUS", LEFT_MODE);
    
    BSP_LCD_SetTextColor(LCD_COLOR_RED);
    BSP_LCD_DisplayStringAt(168, 135, (uint8_t *)"ZBYVA TI:", LEFT_MODE);
}

void drawTimer()
{   
    int min = bombTime/60, sec = bombTime%60;
    uint8_t time[5];
    sprintf((char*) time, "%.2d:%.2d", min, sec);
    BSP_LCD_DisplayStringAt(195, 155, (uint8_t *)time, LEFT_MODE);
}
