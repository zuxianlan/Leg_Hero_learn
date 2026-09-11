/**


  */
#include "Led_Flow_Task.h"
#include "cmsis_os.h"
#include "ws2812.h"
#include "Uart_Send_Task.h"


/**
  * @brief          led RGBÈÎÎñ
  * @param[in]      pvParameters: NULL
  * @retval         none
  */
uint8_t R_ = 1;
uint8_t G = 1;
uint8_t B = 1;
uint8_t LED_Flow_mode = 1;

void LED_Flow(void)
{
    static uint8_t mod = 0;
    mod++;
    if (mod == 5)
    {
        WS2812_Ctrl(R_, G, B);
        if (R_ == 255)
        {
            LED_Flow_mode = 1;
        }
        else if (G == 255)
        {
            LED_Flow_mode = 2;
        }
        else if (B == 255)
        {
            LED_Flow_mode = 3;
        }
        switch (LED_Flow_mode)
        {
        case 1:
        {
            R_--;
            G++;
            break;
        }
        case 2:
        {
            G--;
            B++;
            break;
        }
        case 3:
        {
            B--;
            R_++;
            break;
        }
        }
        mod = 0;
    }
}

