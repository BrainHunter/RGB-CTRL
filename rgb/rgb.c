#include "rgb.h"
#include "stm32f10x_conf.h"
#include <math.h>

int Tim2init(void);
int GPIOinit(void);

void RGB_SetState(int State);

// save vars for the actual rgb settings
float internal_r = 0;
float internal_g = 0;
float internal_b = 0;

float internal_h = 0;       // hue
float internal_s = 0;       // satuation
float internal_v = 0;       // value

extern volatile unsigned int systime;


int RGB_init(void)
{
    GPIOinit();
    Tim2init();
    return 0;
}


int Tim2init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBase_InitStructure;
	TIM_OCInitTypeDef TIM_OC_InitStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* PA0 Pin  T2_CH1*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA1 Pin  T2_CH2*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA2 Pin  T2_CH3*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	TIM_TimeBase_InitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBase_InitStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBase_InitStructure.TIM_Period = 0xFFFF; // 9999;
	TIM_TimeBase_InitStructure.TIM_Prescaler = 0;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBase_InitStructure);

	TIM_OC_InitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OC_InitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OC_InitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Set;
	TIM_OC_InitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC_InitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OC_InitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OC_InitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	TIM_OC_InitStructure.TIM_Pulse = 0;
	TIM_OC1Init(TIM2, &TIM_OC_InitStructure);
	TIM_OC2Init(TIM2, &TIM_OC_InitStructure);
	TIM_OC3Init(TIM2, &TIM_OC_InitStructure);
	TIM_Cmd(TIM2, ENABLE);

	return 0;
}

int GPIOinit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef GPIO_InitStructure;

	/* PA0 Pin  T2_CH1*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA1 Pin  T2_CH2*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA2 Pin  T2_CH3*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* PA3 Pin  T2_CH4*/ 	// Ext Sw
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	return 0;
}

unsigned int timediff(unsigned int t1, unsigned int t2)
{
    int d = (int)t1 - (int)t2;
    if(d < 0) d = -d;
    return (unsigned int) d;
}



#define OFF     0
#define ON      1
#define Fading  2
int rgbState = 0;
unsigned int rgbTime=0;

void RGB_SetState(int State)
{
    rgbState = State;
    rgbTime = systime;
    return;
}

void RGB_loop(void)
{
    // check if we can switch to OFf
    if(internal_r == 0 && internal_g == 0 && internal_b == 0 && rgbState == ON)   // light is off
    {
        RGB_SetState(OFF);
    }

    switch(rgbState)
    {
        case OFF:
            if(timediff(systime,rgbTime) > 5000) // > 5sec
            {
                GPIO_WriteBit(GPIOA, GPIO_Pin_3, Bit_RESET);	// Ext SW
            }
            break;
        case ON:
            break;
        case Fading:
            {
                static unsigned int fadetime = 0;
                if(timediff(systime,fadetime) >= 100)
                {
                    float r,g,b;
                    internal_h += 1;
                    hsv_to_rgb(&r, &g, &b, internal_h, internal_s, internal_v);
                    RGB_set(r,g,b);
                    if(internal_h >= 360) internal_h = 0;
                }
                break;
            }
        default:
            break;
    }

}


void RGB_setChannel(int channel, float value)
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_3, Bit_SET);
    switch(channel)
    {
        case RED:
                internal_r = value;
                RGB_SetState(ON);
                break;
        case GREEN:
                internal_g = value;
                RGB_SetState(ON);
                break;
        case BLUE:
                internal_b = value;
                RGB_SetState(ON);
                break;
        case HUE:
                internal_h = value*360;
                if(internal_h >= 360) internal_h = 0;
                hsv_to_rgb(&internal_r, &internal_g, &internal_b, internal_h, internal_s, internal_v);
                break;
        case SATUATION:
                internal_s = value;
                hsv_to_rgb(&internal_r, &internal_g, &internal_b, internal_h, internal_s, internal_v);
                break;
        case VALUE:
                internal_v = value;
                if(rgbState==OFF)RGB_SetState(ON);  // leave state @ Fading
                hsv_to_rgb(&internal_r, &internal_g, &internal_b, internal_h, internal_s, internal_v);
                break;
        default:
            return;
    }

    RGB_set(internal_r,internal_g, internal_b);
    return;
}

float RGB_getChannel(int channel)
{
    if (channel == RED)     return internal_r;
    if (channel == GREEN)   return internal_g;
    if (channel == BLUE)    return internal_b;
    if (channel == HUE)     return internal_h/360;
    if (channel == SATUATION)   return internal_s;
    if (channel == VALUE)    return internal_v;
    return 0;
}

void RGB_set(float r, float g, float b)
{
    internal_r = r;
    internal_g = g;
    internal_b = b;

    TIM_SetCompare1(TIM2, val2pwm((uint16_t)(b*255.0)) );		// blue
    TIM_SetCompare2(TIM2, val2pwm((uint16_t)(r*255.0)) );		// red
    TIM_SetCompare3(TIM2, val2pwm((uint16_t)(g*255.0)) );		// green
}

uint16_t val2pwm(int val)
{
	// http://www.mikrocontroller.net/articles/Diskussion:LED-Fading#Diskussion_wissenschaftl.-technischer_Hintergrund
	/*
	float n = 255.0;
	float z = 65536.0;
	float y = 1/2.2;
	uint16_t R = floor((z - 1) * (i / (n - 1)) ^ (1/y) + 0.5) ;
	*/

	uint16_t R = floor(65535.0 * pow( val / 255.0 , 2.2) + 0.5);
	return R;
}

void hsv_to_rgb(float* r, float* g, float* b, float h, float s, float v)	// http://www.mikrocontroller.net/topic/54561
{

	int i;
	float f, p, q, t;

	if( s == 0 )
	{
		*r = *g = *b = v;
		return;
	}

	h /= 60;				// sector 0 to 5
	i = floor( h );
	f = h - i;
	p = v * ( 1 - s );
	q = v * ( 1 - s * f );
	t = v * ( 1 - s * ( 1 - f ) );

    switch( i )
    {
		case 0:
			*r = v; *g = t; *b = p; break;

		case 1:
			*r = q; *g = v; *b = p; break;

		case 2:
			*r = p; *g = v; *b = t; break;

		case 3:
			*r = p; *g = q; *b = v; break;

		case 4:
			*r = t; *g = p; *b = v; break;

		default:
			*r = v; *g = p; *b = q; break;				// case 5:
    }

}


