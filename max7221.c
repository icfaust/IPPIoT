#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"

#include "max7221.h"

void ICACHE_FLASH_ATTR bit_bang_SPI(int val){
  /* this takes a one byte unsigned char containing two hex values
  // and writes it out to GPIO 
  */
  //unsigned char val = 0xA9;
  
  int test = 0x80;
  int i=0;
  for(;i<8;i++) {
    
    if (val & test)
    {    
    gpio_output_set(BIT0, 0, BIT0, 0); //DIN HIGH (ON)
    }  
    else
    {  
    gpio_output_set(0, BIT0, BIT0, 0); //DIN LOW (OFF)
    }
    os_delay_us(10);


    gpio_output_set(0, BIT1, BIT1, 0); //CLK LOW (OFF)
    os_delay_us(10);    
    gpio_output_set(BIT1, 0, BIT1, 0); //CLK HIGH (ON)
    os_delay_us(10);
    
    val = val << 1;
  }
}


void ICACHE_FLASH_ATTR printf_max(int word1, int word2){
  gpio_output_set(0, BIT2, BIT2, 0); //CS LOW (OFF)
  bit_bang_SPI(word1);
  bit_bang_SPI(word2);
  gpio_output_set(BIT2, 0, BIT2, 0); //CS HIGH (ON) LATCH DATA
  os_delay_us(10);
}


void ICACHE_FLASH_ATTR max_init(void *args){

  os_delay_us(5000);
  printf_max(MAX_DISP, 0xFE); //display normal
  printf_max(MAX_SHUT, 0x01); //shutdown normal
  printf_max(MAX_DECO, 0x00); // put everything in individual control
  printf_max(MAX_SCAN, 0x0F); //scan limit all digits
  //intensity(0x0F); // set high intensity
  printf_max(MAX_INTS, 0x0F);
  clear_max();
  os_delay_us(5000); 
}


void ICACHE_FLASH_ATTR clear_max()
{
  int i=1;
  
  for(;i<9;i++)
    {
      printf_max(i, 0x00);
    }
}


/*void ICACHE_FLASH_ATTR intensity(int val)
{
  printf_max(MAX_INTS, val);
}
*/

void ICACHE_FLASH_ATTR disp_shot_text()
{
  int j=4;
    
  for(;j;j--)
    {
      printf_max(j, 0x00);
    }
  
  printf_max(0x08,MAX_S);
  printf_max(0x07,MAX_H);
  printf_max(0x06,MAX_O);
  printf_max(0x05,MAX_T);
}


void ICACHE_FLASH_ATTR printf_max_int(int val)
{
  // display number
  int max = 10000000;
  int i = 8;
  int j = 1;
  int flag=0;

  // find number of digits to display
  if(val == 0)
    {
      i = 1;
      j = 0;
    }
  else
    {
    if(val < 0)
      {
	flag = 1; // do something with flag later
	val = -1*val;
      }

    while(j)
      {
	if(val/max)
	  {
	    j=0;
	  }
	else
	  {
	    i--;
	    max = max/10;
	  }
      }
    }

  //display digits
  for(;j<i;j++)
    {
      printf_max(j+1, maxnums[val % 10]);
      val = val/10;      
    }
  for(;j<8;j++)
    {
      printf_max(j+1, 0x00);
    }
}

