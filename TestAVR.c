#include <avr/io.h>
#include <stdlib.h>
#include <math.h>
#include <util/delay.h>
#include <avr/eeprom.h>

#define SetRed(val) OCR2 = ceil(val);
#define SetGreen(val) OCR1B = ceil(val);
#define SetBlue(val) OCR1A = ceil(val);

#define normTo255(val) if(val<0)val=0;if(val>255)val=255;

#define DEBUG_LED PORTB=0b01000000;_delay_ms(100);PORTB=0b00000000;_delay_ms(100);

struct Color {
	float red;
	float green;
	float blue;
};

struct HSVColor {
	float hue;
	float sat;
	float val;
};


struct HSVColor now;

struct Color hsv2rgb(struct HSVColor hsv)
{
	struct Color res;

    int ii;
    float fr,c1, c2, c3;
    if (!hsv.sat)
      res.red = res.green = res.blue = hsv.val;
    else
    {
	//now.hue=0;now.sat=0xFF;now.val=128;
		fr=hsv.hue /60.0;//0
        ii = (int)(fr);//0
        fr = (fr - ii);//0
        c1 = (hsv.val * (1 - hsv.sat/255.0));//0
        c2 = (hsv.val * (1 - hsv.sat * fr/255.0));//0
        c3 = (hsv.val * (1 - hsv.sat * (1 - fr)/255.0));//0
        switch (ii)
          {
            case 0: res.red = hsv.val; res.green = c3;      res.blue = c1; break;
            case 1: res.red = c2;      res.green = hsv.val; res.blue = c1; break;
            case 2: res.red = c1;      res.green = hsv.val; res.blue = c3; break;
            case 3: res.red = c1;      res.green = c2;      res.blue = hsv.val; break;
            case 4: res.red = c3;      res.green = c1;      res.blue = hsv.val; break;
            case 5: res.red = hsv.val; res.green = c1;      res.blue = c2; break;
          }
    }

	normTo255(res.red);
	normTo255(res.green);
	normTo255(res.blue);
	return res;
}
/*
void rgbCopyToLED()
{
	SetRed(now.red);
	SetGreen(now.green);
	SetBlue(now.blue);
}
*/
void hsvCopyToLED()
{
	while (now.hue < 0)
		now.hue += 360;
	while (now.hue >= 360)
		now.hue -= 360;

	struct Color nowRGB=hsv2rgb(now);
	SetRed(nowRGB.red);
	SetGreen(nowRGB.green);
	SetBlue(nowRGB.blue);
}

void hsvMorphTo(struct HSVColor to,
			unsigned int delay)
{
	float hueDiff=to.hue-now.hue;
	if(hueDiff>180)
		to.hue=to.hue-360;
	else if((-hueDiff)>180)
		to.hue=to.hue+360;

	float deltaHue=(to.hue-now.hue)/delay;
	float deltaSat=(to.sat-now.sat)/delay;
	float deltaVal=(to.val-now.val)/delay;

	for(unsigned int i=0;i<delay;i++)
	{
		now.hue=now.hue+deltaHue;
		now.sat=now.sat+deltaSat;
		now.val=now.val+deltaVal;

		if(now.hue<0)now.hue=0;
		if(now.hue>360)now.hue=360;

		normTo255(now.sat);
		normTo255(now.val);

		hsvCopyToLED();
		_delay_ms(1);
	}

}


/*
void rgbMorphTo(struct Color to,
			unsigned int delay)
{
	float deltaRed=(to.red-now.red)/delay;
	float deltaGreen=(to.green-now.green)/delay;
	float deltaBlue=(to.blue-now.blue)/delay;

	for(unsigned int i=0;i<delay;i++)
	{
		now.red=now.red+deltaRed;
		now.green=now.green+deltaGreen;
		now.blue=now.blue+deltaBlue;

		if(now.red<0)now.red=0;
		if(now.red>255)now.red=255;

		if(now.green<0)now.green=0;
		if(now.green>255)now.green=255;

		if(now.blue<0)now.blue=0;
		if(now.blue>255)now.blue=255;

		rgbCopyToLED();
		_delay_ms(1);
	}
}*/

void main(void)
{
// Input/Output Ports initialization
// Port B initialization
// Func7=In Func6=In Func5=In Func4=In Func3=Out Func2=Out Func1=Out Func0=In
// State7=T State6=T State5=T State4=T State3=0 State2=0 State1=0 State0=T
PORTB=0x00;
DDRB=0b01001110;

// Port C initialization
// Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
// State6=T State5=T State4=T State3=T State2=T State1=T State0=T
PORTC=0x00;
DDRC=0x00;

// Port D initialization
// Func7=In Func6=In Func5=In Func4=In Func3=In Func2=In Func1=In Func0=In
// State7=T State6=T State5=T State4=T State3=T State2=T State1=T State0=T
PORTD=0x00;
DDRD=0x00;

// Timer/Counter 0 initialization
// Clock source: System Clock
// Clock value: Timer 0 Stopped
TCCR0=0x00;
TCNT0=0x00;

// Timer/Counter 1 initialization
// Clock source: System Clock
// Clock value: 8000,000 kHz
// Mode: Fast PWM top=00FFh
// OC1A output: Non-Inv.
// OC1B output: Non-Inv.
// Noise Canceler: Off
// Input Capture on Falling Edge
// Timer 1 Overflow Interrupt: Off
// Input Capture Interrupt: Off
// Compare A Match Interrupt: Off
// Compare B Match Interrupt: Off
TCCR1A=0xA1;
TCCR1B=0x09;
TCNT1H=0x00;
TCNT1L=0x00;
ICR1H=0x00;
ICR1L=0x00;
OCR1AH=0x00;
OCR1AL=0x00;
OCR1BH=0x00;
OCR1BL=0x00;

// Timer/Counter 2 initialization
// Clock source: System Clock
// Clock value: 8000,000 kHz
// Mode: Fast PWM top=FFh
// OC2 output: Non-Inverted PWM
ASSR=0x00;
TCCR2=0x69;
TCNT2=0x00;
OCR2=0x00;

// External Interrupt(s) initialization
// INT0: Off
// INT1: Off
MCUCR=0x00;

// Timer(s)/Counter(s) Interrupt(s) initialization
TIMSK=0x00;

// Analog Comparator initialization
// Analog Comparator: Off
// Analog Comparator Input Capture by Timer/Counter 1: Off
ACSR=0x80;
SFIOR=0x00;

srand(eeprom_read_word(0));             //eieoeaeece?oai ?aiaiiaeca? yoei cia?aieai
eeprom_write_word(0,rand());

now.hue=0;
now.sat=0;
now.val=0;
hsvCopyToLED();

struct HSVColor to;
int delayToMorph=0;

to.sat=0xFF;
to.val=0xFF;
float oldHue=0;
while (1)
      {
/*
		oldHue=to.hue;
		to.hue=-1;
		while(! (to.hue>0 && to.hue<360 && fabs(oldHue-to.hue)>20) )
			to.hue=rand();

		//while(! (to.sat>0 && to.sat<0xFF) )
			to.sat=0xff;//rand();

		//while(! (to.val>128 && to.val<0xFF) )
			to.val=0xFF; //rand(); // иначе может мигать на низкой яркости

		while(! (delayToMorph>1000 && delayToMorph<5000))
			delayToMorph=rand();

		hsvMorphTo(to,delayToMorph);

		while(!(delayToMorph>3000 && delayToMorph<7000))
			delayToMorph=rand();

		_delay_ms(delayToMorph);
*/

	to.hue=now.hue+22;
	to.sat=0xFF;
	while(! (to.val>172 && to.val<0xFF) )
				to.val=rand();
	hsvMorphTo(to,200);

	/*struct Color g;
	g.red=0xFF;
	g.green=0;
	g.blue=0;
	rgbMorphTo(g,1000);

	g.red=0;
	g.green=0xFF;
	g.blue=0;
	rgbMorphTo(g,1000);

	g.red=0;
	g.green=0;
	g.blue=0xFF;
	rgbMorphTo(g,1000);*/
      }
}
