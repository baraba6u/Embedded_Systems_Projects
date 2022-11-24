#include <xmc_gpio.h>
#include <xmc_common.h>
#include <math.h>

#define GPIO_LED2     XMC_GPIO_PORT1,  0
#define GPIO_LED1     XMC_GPIO_PORT1,  1
#define GPIO_BUTTON1  XMC_GPIO_PORT1, 14
#define GPIO_BUTTON2  XMC_GPIO_PORT1, 15

#define TICK_DIT 100
#define TICK_DAH 300
#define TICK_DD_SPACE 100
#define TICK_LETTER_SPACE 300
#define TICK_WORD_SPACE 700

const XMC_GPIO_CONFIG_t out_config = \
      {.mode=XMC_GPIO_MODE_OUTPUT_PUSH_PULL,\
       .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
       .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};
const XMC_GPIO_CONFIG_t in_config = \
      {.mode=XMC_GPIO_MODE_INPUT_TRISTATE,\
       .output_level=XMC_GPIO_OUTPUT_LEVEL_LOW,\
       .output_strength=XMC_GPIO_OUTPUT_STRENGTH_STRONG_SHARP_EDGE};


// interval1 for saving time between button1 presses
uint32_t tick, interval1, interval2 = 0;

// button1_once = check if button1 is pressed for the first time
char button1_on, button1_once = 0;

void SysTick_Handler();

// sleep timer
void sleep(uint32_t amount);

// 1 character -> symbols
void character_encode(char symbol);

// 1 sentence -> symbols
// design optimizated for "I can morse" string
void sentence_encode(const char* str, int len );

// 1 number -> symbols
void number_encode(uint32_t amount);

int main(void) {
  
  XMC_GPIO_Init(GPIO_LED1,     &out_config);
  XMC_GPIO_Init(GPIO_BUTTON1,  &in_config);
  XMC_GPIO_Init(GPIO_BUTTON2,  &in_config);

  // 1 tick = 1ms
  SysTick_Config(SystemCoreClock / 1000);

  const char str[] = "i can morse";

  while(1){

    if(XMC_GPIO_GetInput(GPIO_BUTTON1) == 0){
      button1_once = 1;
      button1_on = 1;
      sentence_encode(str,11);
    }

    if(XMC_GPIO_GetInput(GPIO_BUTTON2) == 0){
      if (!button1_once) number_encode(0);
      else number_encode(interval1);
    }

  }

  return 0;
}


void SysTick_Handler(){
  if (tick>0) tick--;

  if (!button1_on){
    interval2++;
  }
  else{
    button1_on=0;
    interval1=interval2;
    interval2=0;
  }


}

void sleep(uint32_t amount){
  tick = amount;
  while(tick>0);
}

void character_encode(char symbol){
  if (symbol == 0){ // dot
    XMC_GPIO_SetOutputHigh(GPIO_LED1);
    sleep(TICK_DIT);
    XMC_GPIO_SetOutputLow(GPIO_LED1);
    sleep(TICK_DD_SPACE);
  }
  else{             // line
    XMC_GPIO_SetOutputHigh(GPIO_LED1);
    sleep(TICK_DAH);
    XMC_GPIO_SetOutputLow(GPIO_LED1);
    sleep(TICK_DD_SPACE);
  }
}

void sentence_encode(const char* str, int len ){

  for (int i=0; i<len; i++){
    switch(str[i]){
      case 'i': // ..
        character_encode(0);
        character_encode(0);
        break;
      case 'c': // -.-.
        character_encode(1);
        character_encode(0);
        character_encode(1);
        character_encode(0);
        break;
      case 'a': //.-
        character_encode(0);
        character_encode(1);
        break;
      case 'n': // -.
        character_encode(1);
        character_encode(0);
        break;
      case 'm': // --
        character_encode(1);
        character_encode(1);
        break;
      case 'o': // ---
        character_encode(1);
        character_encode(1);
        character_encode(1);
        break;
      case 'r': // .-.
        character_encode(0);
        character_encode(1);
        character_encode(0);
        break;
      case 's': // ...
        character_encode(0);
        character_encode(0);
        character_encode(0);
        break;
      case 'e': // .
        XMC_GPIO_SetOutputHigh(GPIO_LED1);
        sleep(TICK_DIT);
        XMC_GPIO_SetOutputLow(GPIO_LED1);
        break;
      case ' ': // 200
        sleep(200);
        break;
      default :
        break;
    }
    if (i!=len-1) sleep(200);
  }
}

void number_encode(uint32_t amount){
  int len =0;
  if (amount != 0){
    len = log10(amount);
  }
  for (int i=len;i>=0;i--){
    int n = (int) (amount/pow(10,i));
    switch(n){
      case 0: // -----
        character_encode(1);
        character_encode(1);
        character_encode(1);
        character_encode(1);
        character_encode(1);
        break;
      case 1: // .----
        character_encode(0);
        character_encode(1);
        character_encode(1);
        character_encode(1);
        character_encode(1);
        break;
      case 2: // ..---
        character_encode(0);
        character_encode(0);
        character_encode(1);
        character_encode(1);
        character_encode(1);
        break;
      case 3: // ...--
        character_encode(0);
        character_encode(0);
        character_encode(0);
        character_encode(1);
        character_encode(1);
        break;
      case 4: // ....-
        character_encode(0);
        character_encode(0);
        character_encode(0);
        character_encode(0);
        character_encode(1);
        break;
      case 5: // .....
        character_encode(0);
        character_encode(0);
        character_encode(0);
        character_encode(0);
        character_encode(0);
        break;
      case 6: // -....
        character_encode(1);
        character_encode(0);
        character_encode(0);
        character_encode(0);
        character_encode(0);
        break;
      case 7: // --...
        character_encode(1);
        character_encode(1);
        character_encode(0);
        character_encode(0);
        character_encode(0);
        break;
      case 8: // ---..
        character_encode(1);
        character_encode(1);
        character_encode(1);
        character_encode(0);
        character_encode(0);
        break;
      case 9: // ----.
        character_encode(1);
        character_encode(1);
        character_encode(1);
        character_encode(1);
        character_encode(0);
        break;
      default:
        break;
    }
    amount-=n*pow(10,i);
    if (len) sleep(200);
  }
}

