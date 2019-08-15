//Aaron Almeida
//April 5th 2019


/*Include*/
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include "SCI.h"
/*Prototypes*/
void delay1ms(unsigned int numTimes); //delay fuction
void OutCRLF(void);        
void ECLK6(void); // set bus speed
void setADC(void);
void linearLED(void);
void getAngle(void);
void BCDLED(void);
void findHigh(void);
void findLow(void);

//---------------------OutCRLF---------------------
// Output a CR,LF to SCI to go to a new line
// Input: none
// Output: none
// toggle PJ0 each time through the loop

void OutCRLF(void){   //print newline character
  SCI_OutChar(CR);
  SCI_OutChar(LF);
}

 /* DECLARE ALL YOUR GLOBAL VARIABLES BELOW*/
  /////////////////////////////////////////////////////

	unsigned int mode = 0;       //initialize variable
	unsigned int count = 0;
	unsigned int val;
	unsigned short prev = 1;
  float volt;
  int high;
  int low;
  int finalHigh;
  
  int highTemp1 = 0;
  int highTemp2= 0;
  int lowTemp = 0;    

  unsigned short degree;

 


void main(void) {		

    mode = 0;            //start mode at 0
    ECLK6();
    setADC();
     
    DDRJ = 0xFF;
    PTJ = 0xFF;
    delay1ms(100);
    
   
  
    PTJ = 0x00;

         
  TSCR1 = 0x90;    //Timer System Control Register 1
                    // TSCR1[7] = TEN:  Timer Enable (0-disable, 1-enable)
                    // TSCR1[6] = TSWAI:  Timer runs during WAI (0-enable, 1-disable)
                    // TSCR1[5] = TSFRZ:  Timer runs during WAI (0-enable, 1-disable)
                    // TSCR1[4] = TFFCA:  Timer Fast Flag Clear All (0-normal 1-read/write clears interrupt flags)
                    // TSCR1[3] = PRT:  Precision Timer (0-legacy, 1-precision)
                    // TSCR1[2:0] not used

  TSCR2 = 0x01;    //Timer System Control Register 2
                    // TSCR2[7] = TOI: Timer Overflow Interrupt Enable (0-inhibited, 1-hardware irq when TOF=1)
                    // TSCR2[6:3] not used
                    // TSCR2[2:0] = Timer Prescaler Select: See Table22-12 of MC9S12G Family Reference Manual r1.25 (set for bus/1)
  
                    
  TIOS = 0xFE;     //Timer Input Capture or Output capture
                    //set TIC[0] and input (similar to DDR)
  PERT = 0x01;     //Enable Pull-Up resistor on TIC[0]

  TCTL3 = 0x00;    //TCTL3 & TCTL4 configure which edge(s) to capture
  TCTL4 = 0x02;    //Configured for falling edge on TIC[0]

          
   
  TIE = 0x01;      //Timer Interrupt Enable
  

 
	SCI_Init(19200); //baud rate is 19200
	SCI_OutChar(CR);//newline character
	
	DDR1AD = 0b01111111;    //set ports are output
	DDR0AD= 0b11111111;

	
  
	EnableInterrupts; //CodeWarrior's method of enabling interrupts

  for(;;){
        

        // if mode is 0, stop collecting data
        if(mode == 0){
          SCI_OutUDec(0);
          OutCRLF();
          delay1ms(50); 
        }
        //if mode is 1, use BCD to display number
         else if(mode == 1){
          getAngle();
          BCDLED();
          
          
        }
        
        //if mode is 2 use linear to display number 
        else if(mode ==2){
          getAngle();
          linearLED();
        }
          
       
     
  }  
}

  /* INTERRUPT SERIVE ROUTINES (ISRs) GO BELOW*/
  /////////////////////////////////////////////////////  

/*
 * This is the Interrupt Service Routine for TIC channel 0 (Code Warrior has predefined the name for you as "Vtimch0"                                                    
 */           
interrupt  VectorNumber_Vtimch0 void ISR_Vtimch0(void)
{
  
  unsigned int temp; //DON'T EDIT THIS


//blink a certian number of times to show what mode we are in 
	if(mode == 2) {
		PTJ = 0xFF;
	  delay1ms(100);
	  PTJ = 0x00;
	}
	if (mode == 0) {
		PTJ = 0xFF;
	  delay1ms(100);
	  PTJ = 0x00;
	  delay1ms(100);
	  PTJ = 0xFF;
	  delay1ms(100);
	  PTJ = 0x00;
	  
	}
	
	if (mode == 1) {
		PTJ = 0xFF;
	  delay1ms(100);
	  PTJ = 0x00;
	  delay1ms(100);
	  PTJ = 0xFF;
	  delay1ms(100);
	  PTJ = 0x00;
	  delay1ms(100);
	 	PTJ = 0xFF;
	  delay1ms(100);
	  PTJ = 0x00; 
	}

	
	mode++;
	delay1ms(100);

  //reset the mode if it reached the max	
	if(mode==3) {
		mode=0;
	}
	
    
  temp = TC0;       //Refer back to TFFCA, we enabled FastFlagClear, thus by reading the Timer Capture input we automatically clear the flag, allowing another TIC interrupt
  }
  




void delay1ms(unsigned int numTimes){
  unsigned int i;
  unsigned int j;
  
  for(j = 0; j<numTimes; j++){
    for(i = 0; i<77; i++){
      // Delay
      PTJ = PTJ;
      PTJ = PTJ;
      PTJ = PTJ;
      PTJ = PTJ;
      PTJ = PTJ;
    }
  }   
}


//function to get angle
void getAngle(){
        val=ATDDR0;
       

         if(val > 1340){ //for less than 0
          degree = 0;    
        }
        
        if(val < 1340 && val > 1247){ //for 0-20
          degree = (-0.2103*val + 281.44);    
        }
        
        if(val <= 1247 && val > 1074){ //for 20-80
          degree = (-0.3252*val + 422.37);
        }
        
        if(val <= 1074 && val > 1062){//for 80-90
          degree = (-5*val + 5445);
        }
        
        if(val <= 1071){ //beyond 90 keep it at 90 
           degree = 90;
        }
        
       
        
}



void ECLK6(void) {  // Bus Speed = 6 MHz
  CPMUCLKS = 0x80; // choose PLL = 1
  CPMUOSC = 0x00; // source clock chosen
  CPMUPROT = 0x26; // protection of clock config is enable
  // now frequency_ref = 1MHz
  CPMUSYNR = 0x17; // VCOFRQ = 00, SYNDIN = 23 = F; 32MHz = VCOCLK = 2*1MHz*(1+SYNDIV);
  CPMUFLG = 0x00; // LOCK = 0
  // PLLCLK = 48MHz/4 = 12MHz and BUSCLK = PLLCLK/2 = 6MHz 
  //PLLCLK = VCOclk/(postdiv+1),  
}

void setADC(void) {     // ADC - AN5, 12-BIT		
	ATDCTL1 = 0x4F; // 12-bit resolutoin, 2F for 10 bits
	ATDCTL3 = 0x88;	 
	ATDCTL4 = 0x01;	// prescaler = 1; ATD clock = 6MHz / (2 * (2 + 1)) == 1MHz      
	ATDCTL5 = 0x27;		//  channel 7 
}

//sampling frequency is the speed at which data is received by the esduino = 1mhz
//conversion rate is fastest possible sampling rate

void BCDLED(void){
   high = degree/10; //integer division to find the tens place number
   low = degree - high*10;   //find number in 1's place
   
   SCI_OutUDec(degree);   //output the degree(for matlab)
   OutCRLF();
   delay1ms(100);
   
   findHigh();   //functions to find the high and low of the numbers
   findLow();   
   PT0AD = lowTemp;          //output to the pins for the lower number
   PT1AD = highTemp1|highTemp2; //output to pins for higher numbers  
   
   
   
}


void linearLED(void){
   SCI_OutUDec(degree); //output the angle (matlab)
   OutCRLF();

   delay1ms(50);
   
   
   //if statements that determine led to light up
   if(degree<5){
          PT1AD = 0b00000000;
	        PT0AD = 0b00000000;
        }
        
        else if(degree>= 5 && degree<15){
          PT1AD = 0b00001000;
	        PT0AD = 0b00000000;
        }
        
         else if(degree>= 15 && degree<25){
          PT1AD = 0b00001100;
	        PT0AD = 0b00000000;
        }
        
         else if(degree>= 25 && degree<35){
          PT1AD = 0b00001110;
	        PT0AD = 0b00000000;
        }
        
         else if(degree>= 35 && degree<45){
          PT1AD = 0b00001111;
	        PT0AD = 0b00000000;
        }
        
         else if(degree>= 45 && degree<55){
          PT1AD = 0b00011111;
	        PT0AD = 0b00000000;
        }
        
         else if(degree>= 55 && degree<65){
          PT1AD = 0b00111111;
	        PT0AD = 0b00000000;
        }
        
         else if(degree>= 65 && degree<75){
          PT1AD = 0b01111111;
	        PT0AD = 0b00000000;
        }
        
         else if(degree>= 75 && degree<85){
          PT1AD = 0b01111111;
	        PT0AD = 0b00000001;
        }
        
        else if(degree>=85){
          PT1AD = 0b01111111;
	        PT0AD = 0b00001001;
        }
        
        
}

//function to find bits to turn on

void findHigh(void){
  if(high == 0){
    highTemp1 = 0;
    lowTemp = 0;
  } 
  
  else if(high == 1){
    highTemp1 = 16;
  lowTemp = 0;
  }
  
   else if(high == 2){
    highTemp1 = 1;
   lowTemp = 0;
  }
  
   else if(high == 3){
    highTemp1 = 17;
   lowTemp = 0;
  }
  
   else if(high == 4){
    highTemp1 = 2;
   lowTemp = 0;
  }
  
   else if(high == 5){
    highTemp1 = 18;
    lowTemp = 0;
  }
  
   else if(high == 6){
    highTemp1 = 3;
   lowTemp = 0;
   
  }
  
   else if(high == 7){
    highTemp1 = 19;
   lowTemp = 0;
  }
  
   else if(high == 8){
    highTemp1 = 4;
   lowTemp = 0;
  }
  else if(high == 9){
    highTemp1 = 20;
    lowTemp = 0;
  }
  
}

//function to find bits to turn on

void findLow(void){

if(low == 0){
    highTemp2 = 0;
    lowTemp = 0;
  } 
  
  else if(low == 1){
    highTemp2 = 0;
    lowTemp = 8;
  }
  
   else if(low == 2){
   highTemp2 = 0;
    lowTemp = 1;
  }
  
   else if(low == 3){
   highTemp2 = 0;
    lowTemp = 9;
  }
  
   else if(low == 4){
    highTemp2 = 64;
   lowTemp = 0;
  }
  
   else if(low == 5){
    highTemp2 = 64;
    lowTemp = 8;
  }
  
   else if(low == 6){
    lowTemp = 0b00000001;
    highTemp2 = 0b01000000;
  }
  
   else if(low == 7){
    highTemp2 = 0b01000000;
    lowTemp = 0b00001001;
  }
  
   else if(low == 8){
    highTemp2 = 0b00100000;
   lowTemp = 0;
   
  }
  else if(low == 9){
   highTemp2 = 0b00100000;
   lowTemp = 8;
  }
}


