#include "ets_sys.h"
#include "osapi.h"
#include "gpio.h"
#include "os_type.h"
#include "user_interface.h"
#include "jsonparse.h"

#include "max7221.h"
#include "httpclient.h"

static volatile os_timer_t polling_timer;
static volatile os_timer_t disp_timer;

int inp = 0x01; //intensity metric
int shot = 0;
int hysteresis = 1;

struct jsonparse_state JSONdata;

void ICACHE_FLASH_ATTR dweet_callback(char * response_body, int http_status, char * response_headers, int body_size)
{
  int json_type;
  char string[128];
  int flag = 0;
  jsonparse_setup(&JSONdata, response_body, body_size);
  json_type = jsonparse_next(&JSONdata);

  while(json_type && !flag)
  {
    if(json_type == JSON_TYPE_PAIR_NAME)
      {
	jsonparse_copy_value(&JSONdata, string, 128);
	if(!strcmp("shot", string))
	  {
	    flag = 1;
	  }
      }
    json_type = jsonparse_next(&JSONdata);
  }
  
  if(flag && (json_type == JSON_TYPE_INT || json_type == JSON_TYPE_NUMBER))
    {
      shot = jsonparse_get_value_as_int(&JSONdata);
    }
  else
    {
      shot = 0;
    }
}
  
void ICACHE_FLASH_ATTR scan_intensity(void *args)
{
  printf_max(MAX_INTS,inp);
  //intensity(inp);  
  inp = inp + 1;
  if(inp > 0x0F)
    {
      inp = 0x00;
    }
}

void ICACHE_FLASH_ATTR heartbeat(void *arg)
{
  //function to minimally show that the device is on
  if(inp != 0)
    {
      printf_max(0x01,0x80);
      inp = 0;
    }
  else
    {
      printf_max(0x01,0x00);
      inp = 1;
    }
}

void ICACHE_FLASH_ATTR startup( void *arg)
{
  int i=8;

  if(inp <= 0x01 || inp > 0x40)
  {
    inp = 0x40;
  }
    //function to minimally show that the device is on
  for(;i;i--)
    {
      printf_max(i,inp);
    }
  inp = inp >> 1;
}

void ICACHE_FLASH_ATTR polling(void *arg){

  http_get("https://dweet.io/get/latest/dweet/for/AUGSIF","",dweet_callback);

  if(shot != hysteresis)
    {

      os_timer_disarm(&disp_timer);
      os_timer_disarm(&polling_timer);
      
      if(shot == 0)
	{
	  clear_max();
	  //intensity(0x0F);
	  printf_max(MAX_INTS,0x0F);
	  
	  os_timer_setfn(&disp_timer, (os_timer_func_t *)heartbeat, NULL);
	  os_timer_arm(&disp_timer, 1000, 1);
	  
	  os_timer_setfn(&polling_timer, (os_timer_func_t *)polling, NULL);
	  os_timer_arm(&polling_timer, 600000, 1); // only check every 10 minutes if data in unavailable
	  
	}
      else if(shot < 0)
	{
	  disp_shot_text();
	  //start timer
	  os_timer_setfn(&disp_timer, (os_timer_func_t *)scan_intensity, NULL);
	  os_timer_arm(&disp_timer, 100, 1);
	  
	  os_timer_setfn(&polling_timer, (os_timer_func_t *)polling, NULL);
	  os_timer_arm(&polling_timer, 5000, 1); // check every 5 seconds while in shot cycle
	  
	}
      else
	{
	  //display shot
	  //intensity(0x0F);
	  printf_max(MAX_INTS,0x0F);
	  printf_max_int(shot);
	  
	  os_timer_setfn(&polling_timer, (os_timer_func_t *)polling, NULL);
	  os_timer_arm(&polling_timer, 20000, 1); // check every 20 seconds during runday
	}

      hysteresis = shot;
    }
}

void ICACHE_FLASH_ATTR wifi_handle_event_cb(System_Event_t *evt)
{
  
  switch (evt->event) {
  case EVENT_STAMODE_CONNECTED:
    os_timer_disarm(&disp_timer);
    os_timer_disarm(&polling_timer);
    
    clear_max();
    //intensity(0x0F);
    printf_max(MAX_INTS,0x0F);

    os_timer_setfn(&disp_timer, (os_timer_func_t *)heartbeat, NULL);
    os_timer_arm(&disp_timer, 1000, 1);
    break;
    
  case EVENT_STAMODE_DISCONNECTED:
    os_timer_disarm(&disp_timer);
    os_timer_disarm(&polling_timer);
    
    os_timer_setfn(&disp_timer, (os_timer_func_t *)startup, NULL);
    os_timer_arm(&disp_timer, 100, 1);
    break;
    
  case EVENT_STAMODE_AUTHMODE_CHANGE:
    os_timer_disarm(&disp_timer);
    os_timer_disarm(&polling_timer);
    
    os_timer_setfn(&disp_timer, (os_timer_func_t *)startup, NULL);
    os_timer_arm(&disp_timer, 100, 1);
    break;
    
  case EVENT_STAMODE_GOT_IP:    
    //os_timer_disarm(&polling_timer);
    //os_timer_setfn(&polling_timer, (os_timer_func_t *)polling, NULL);
    //os_timer_arm(&polling_timer, 20000, 1);
    polling(NULL);
    break;
    
  default:
    break;
  }
}

void ICACHE_FLASH_ATTR user_init()
{  
  struct station_config wifi_config;
  // init gpio subsytem
  gpio_init();

  // configure UART TXD to be GPIO1, set as output
  PIN_FUNC_SELECT(PERIPHS_IO_MUX_U0TXD_U, FUNC_GPIO1); 
  /*gpio_output_set(0, BIT2, BIT2, 0);
  gpio_output_set(0, BIT1, BIT1, 0); //OFF
  gpio_output_set(0, BIT0, BIT0, 0);*/

  //gpio_output_set(BIT2, 0, BIT2, 0);
  //gpio_output_set(BIT1, 0, BIT1, 0); //ON
  //gpio_output_set(BIT0, 0, BIT0, 0);
  
  max_init(NULL);

  //printf_max(0x01,0xFF);
  // startup no wifi display animation
  os_timer_disarm(&disp_timer);
  os_timer_setfn(&disp_timer, (os_timer_func_t *)startup, NULL);
  os_timer_arm(&disp_timer, 100, 1);

  wifi_set_opmode(STATION_MODE);
  // set station configuration, save to flash
  os_memset(&wifi_config, 0, sizeof(wifi_config));
  os_memcpy(&wifi_config.ssid, "IPP Guests", os_strlen("IPP Guests"));
  os_memcpy(&wifi_config.password, "", os_strlen(""));
  wifi_station_set_config(&wifi_config);
  
  wifi_set_event_handler_cb(wifi_handle_event_cb);
}
