#include "mbed.h"
#include "rtos.h"
#include "stm32746g_discovery_lcd.h"
#include "stm32746g_discovery_ts.h"
#include <string>

#define BUTTON_HEIGHT 50

typedef enum state 
{ 
  STATE_ON,
  STATE_OFF,
  STATE_RADIO,
  STATE_MUSIC,
  STATE_CALL  
} state;

state currState;
state prevState = STATE_OFF;

class Button {
    public:
        int posX;
        int posY;
        int width;
        string text;
        
        Button(int x, int y, int w, string t) {
            posX = x;
            posY = y;
            width = w;
            text = t;
        }
};
Button on(7, 5, 88, "ON");
Button off(100, 5, 88, "OFF");
Button radio(193, 5, 93, "RADIO");
Button music(291, 5, 88, "MP3");
Button call(384, 5, 88, "CALL");
    
bool touchDetected();
bool btnOnTouch();
bool btnOffTouch();
bool btnRadioTouch();
bool btnMusicTouch();
bool btnCallTouch();
void initScreen();
void initUI();
void threadDraw(string *str);
void changeState(state newState);

TS_StateTypeDef TS_State;
int touchX = 0, touchY = 0;
Semaphore smphr(1);
Thread threads[50];
int currThread = 0;

int main()
{ 
    initScreen();  
    initUI();
    
    changeState(STATE_ON);  
    
    while(true) { //check for screen touch every 100ms
        
        if (touchDetected()) {
            
            if (currState == STATE_OFF) {
                if (btnOnTouch()) { changeState(STATE_ON); }
            } else {
                if (btnOffTouch()) { changeState(STATE_OFF); }
                if (currState != STATE_RADIO && btnRadioTouch()) { changeState(STATE_RADIO); }
                if (currState != STATE_MUSIC && btnMusicTouch()) { changeState(STATE_MUSIC); }
                if (currState != STATE_CALL && btnCallTouch()) { changeState(STATE_CALL); }
            }
            
            while (true) { //wait for screen touch release
                if (!touchDetected()) { break; }
                Thread::wait(100);
            }
        }    
        HAL_Delay(100);
    }  
}

void threadDraw(string *str) {
    smphr.wait();    
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);   
    BSP_LCD_FillRect(0, 70, 480, 180);   
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);  
    BSP_LCD_DisplayStringAt(0, 150, (uint8_t *)str, CENTER_MODE);    
}

void changeState(state newState) {
    threads[currThread].terminate();
    threads[currThread].join();
    currThread++;   
    
    smphr.release();

    switch (newState) {
        case STATE_ON:
            if (prevState == STATE_OFF || prevState == STATE_ON) { 
                threads[currThread].start(callback(threadDraw, (string *)"Ahoj! Jsem tvoje radio"));
            } else {
                changeState(prevState);
            }
            break;
        case STATE_OFF:
            threads[currThread].start(callback(threadDraw, (string *)"Predstiram mrtvolu"));
            prevState = currState;
            break;
        case STATE_RADIO:
            threads[currThread].start(callback(threadDraw, (string *)"Poslouchas radio Raaaaadio"));
            break;
        case STATE_MUSIC:
            threads[currThread].start(callback(threadDraw, (string *)"Tvoje oblibene cedecko"));
            break;
        case STATE_CALL:
            threads[currThread].start(callback(threadDraw, (string *)"Volam na lesy"));
            break;
    }
    
    currState = newState;
}

bool touchDetected()
{
    BSP_TS_GetState(&TS_State);    
    if (TS_State.touchDetected != 0) { 
        touchX = TS_State.touchX[0];
        touchY = TS_State.touchY[0];
        return true; 
    } else { return false; }
}

bool btnOnTouch() {
    if (touchX >= on.posX && touchY >= on.posY && touchX <= on.posX + on.width && touchY <= BUTTON_HEIGHT)
    { return true; } else { return false; }            
}

bool btnOffTouch() {
    if (touchX >= off.posX && touchY >= off.posY && touchX <= off.posX + off.width && touchY <= BUTTON_HEIGHT)
    { return true; } else { return false; }            
}

bool btnRadioTouch() {
    if (touchX >= radio.posX && touchY >= radio.posY && touchX <= radio.posX + radio.width && touchY <= BUTTON_HEIGHT)
    { return true; } else { return false; }            
}

bool btnMusicTouch() {
    if (touchX >= music.posX && touchY >= music.posY && touchX <= music.posX + music.width && touchY <= BUTTON_HEIGHT)
    { return true; } else { return false; }            
}

bool btnCallTouch() {
    if (touchX >= call.posX && touchY >= call.posY && touchX <= call.posX + call.width && touchY <= BUTTON_HEIGHT)
    { return true; } else { return false; }            
}

void initScreen() {
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
}

void initUI() {
    
    BSP_LCD_Clear(LCD_COLOR_BLACK);
    BSP_LCD_SetTextColor(LCD_COLOR_WHITE);
    BSP_LCD_SetBackColor(LCD_COLOR_WHITE);    
    BSP_LCD_FillRect(0, 70, 480, 180);
    
    Button buttons[5] = {on, off, radio, music, call};
    
    BSP_LCD_SetTextColor(LCD_COLOR_CYAN);
    BSP_LCD_SetBackColor(LCD_COLOR_CYAN);
    for (int i = 0; i < 5; i++) {
        BSP_LCD_FillRect(buttons[i].posX, buttons[i].posY, buttons[i].width, BUTTON_HEIGHT);
    }
    
    BSP_LCD_SetTextColor(LCD_COLOR_BLACK);
    for (int i = 0; i < 5; i++) {        
        BSP_LCD_DisplayStringAt(buttons[i].posX + 5, buttons[i].posY  + 15, (uint8_t *)buttons[i].text.c_str(), LEFT_MODE);
    }      
}
