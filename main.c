/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Simple Math Game
  ******************************************************************************
  * @attention
  *
  * This game presents the user with a math question. It is played over the serial interface.
  * The user can reset the game with the on-board reset button of the NUCLEO board.
  * Baud Rate = 115200
  *
  * Copyright (c) 2023 STMicroelectronics.
  * Copyright (c) 2023 Dr. Billy Kihei, for CPE 2200
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_DMA_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
volatile uint8_t dataAvail = 0;										// Shows that data has been used.			// Used to determine when a button has been pressed.
char rx_buff[1];						// Buffer used to get user input.
uint8_t difficulty[] = "Pick which difficulty you want.\r\n(1) Easy\r\n(2) Hard \r\n"; // String to get difficulty.
uint8_t characterName[] = "\r\nWhat name do you want to give your character? (Max 100 characters)\r\n(Press ENTER to continue)\r\n\r\n";		// Array used to ask for character name.
uint8_t askJob[] = "What job do you want have?\r\n"; // An array used to ask for the characters job.
uint8_t tip[] =	"When in the main area of each room, you can press the 'i' key to open the inventory.\r\n(Press ENTER to continue)\r\n";					// Array used to tell the user about the items.
uint8_t text[] = "";				 // An array used to send messages to the terminal.
uint8_t options[] = "";				 // An array used to send questions for formatting.
uint8_t screen[] = "";				 // An array used to print ASCII art.
uint8_t line[] = "\r\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n"; // String for formating.



int items(uint8_t kaleidoscope, uint8_t key, uint8_t beer, uint8_t food, uint8_t hammer, uint8_t rag, uint8_t rope, uint8_t locket) {	// Function to print items
	HAL_UART_Transmit(&huart2,"Items: ",sizeof("Items: "),50);
	if (kaleidoscope==1) {
		HAL_UART_Transmit(&huart2,"Kaleidoscope Piece | ",sizeof("Kaleidoscope Piece | "),50);
	}else if (kaleidoscope==2) {
		HAL_UART_Transmit(&huart2,"Kaleidoscope Piece x2 | ",sizeof("Kaleidoscope Piece x2 | "),50);
	}else if (kaleidoscope==3) {
		HAL_UART_Transmit(&huart2,"Kaleidoscope Piece x3 | ",sizeof("Kaleidoscope Piece x3 | "),50);
	}
	if (key) {
		HAL_UART_Transmit(&huart2,"Key | ",sizeof("Key | "),50);
	}
	if (beer==1) {
		HAL_UART_Transmit(&huart2,"Beer | ",sizeof("Beer | "),50);
	}else if (beer==2) {
		HAL_UART_Transmit(&huart2,"Beer x2 | ",sizeof("Beer x2 | "),50);								// Checks to see if the item has been collected then prints the corresponding amount.
	}
	if (food) {
		HAL_UART_Transmit(&huart2,"Food | ",sizeof("Food | "),50);
	}
	if (hammer) {
		HAL_UART_Transmit(&huart2,"Hammer | ",sizeof("Hammer | "),50);
	}
	if (rag) {
		HAL_UART_Transmit(&huart2,"Rag | ",sizeof("Rag | "),50);
	}
	if (rope) {
		HAL_UART_Transmit(&huart2,"Rope | ",sizeof("Rope | "),50);
	}
	if (locket) {
		HAL_UART_Transmit(&huart2,"Locket | ",sizeof("Locket | "),50);
	}
}


int getBeer(uint8_t *beer, uint8_t *difficulty) {		// Function to manage the beer item.
	asm volatile (
		    "CMP %[r1], 1\n\t"							// Checks to see the difficulty
			"BLT easy\n\t"

			"Hard:\n\t"
			"CMP %[r0], 0\n\t"							// Checks to see if beer is 0.
			"ITE EQ\n\t"
			"MOVEQ %[r0], 1\n\t"						// If it is beer = 1
			"MOVNE %[r0], 2\n\t"						// For hard mode, beer will go up to 2
			"B end\n\t"

			"easy:\n\t"
			"CMP %[r0], 0\n\t"
			"IT EQ\n\t"
			"MOVEQ %[r0], 1\n\t"						// For easy mode, the beer will stay at 1

			"end:\n\t"

		    :[r0] "+r" (*beer), [r1] "=r" (*difficulty)
	);
	return 0;
}


int checkAns(char attempt[], char answer[], uint8_t *endCode) {		// Function to check attempted lock code with the correct lock code.
	for (int j=0;j<5;j++) {
		int asciians = (int)answer[j];				// Converts the answer to ASCII.
		int asciiatt = (int)attempt[j];				// Converts the attempt to ASCII.

		asm volatile (
				"CMP %[r1], %[r2]\n\t"				// Checks to see of each number is the same.
				"BEQ final\n\t"

				"MOV %[r0], #0\n\t"					// If it isn't prevents the game from ending.

				"final:\n\t"
				:[r0] "+r" (*endCode), [r1] "+r" (asciiatt), [r2] "+r" (asciians)
		);
	}
	return 0;
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_DMA_Init();
  /* USER CODE BEGIN 2 */
 HAL_UART_Receive_IT(&huart2, rx_buff, 1);
  /* USER CODE END 2 */

  /* Game engine runs in infinite loop until finished. */
  /* Game engine BEGIN WHILE */



  uint8_t start = 0;
  uint8_t init = 0;
  uint8_t begin = 0;						// Logic for the start up of the game.


  uint8_t choice = 0;						// Logic to ask the user questions.
  uint8_t roomEnter = 1; 					// Logic for dialogue when you enter a room..
  uint8_t nextRoom = 0;						// Logic for dialogue when are about to change rooms

  uint8_t endCode = 1;						// Logic for when the game will end.
  char room[] = "None";						// String to change rooms.
  char job[] = "None";						// String to hold character job.
  // Objects
  uint8_t kaleidoscope = 0;						// Item for the map.
  uint8_t key = 0;								// Item for locked door.
  uint8_t beer = 0;								// Item for the gunner.
  uint8_t food = 0;								// Item for the looker
  uint8_t hammer = 0;							// Item for the broken box.
  uint8_t rag = 0;								// Item for the worried crew mate.
  uint8_t rope = 0;								// Item for the chief mate.
  uint8_t locket = 0;							// Item for the chef.

  // Characters
  uint8_t captain = 0;							// Logic for dialogue.
  uint8_t worriedCrewmember = 0;							// Logic for dialogue.
  uint8_t looker = 0;							// Logic for dialogue.
  uint8_t chef = 0;							// Logic for dialogue.
  uint8_t gunner = 0;							// Logic for dialogue.
  uint8_t chiefMate = 0;							// Logic for dialogue.

  // Conditional dialogue
  uint8_t capKal = 0;							// Logic for getting the kaleidoscope.
  uint8_t worRag = 0;							// Logic for using the rag.
  uint8_t gunbeer = 0;							// Logic for using the beer.
  uint8_t chefBox = 0;							// Logic for fixing the box.
  uint8_t chefLoc = 0;							// logic for finding the locket.
  uint8_t lookFood = 0;							// Logic for using the food.
  uint8_t chiRope = 0;							// Logic for using the rope.
  uint8_t chiHam = 0;							// Logic for using the hammer.
  uint8_t dialogue = 0;							// Logic for more dialogue.
  uint8_t storageBox = 0;						// Logic for the armory box.
  uint8_t chest = 0;							// Logic for chest.
  uint8_t shine = 0;							// Logic for locket.
  uint8_t broken = 1;							// Logic for the broken box.

  char attempt[5];								// The string for user attempts at the lock.
  char answer[5] = "0498";						// The ending code for win the game.

  uint8_t diff = 0;							// Logic for difficultly.
  uint8_t i; 									// Initializing an iterator for later.
  uint8_t name[101];							// Array used to hold the character name.

  while(1)
  {
	  if (!start) {
		  if (dialogue==0) {
			  uint8_t screen[] = "                                        \r\n"			// ASCII art of the title screen
									"                                 ,~,    \r\n"
									"     ,~-_~--  -~___~~--_-_-_  _-~| |    \r\n"
									"     |~     \\/              |/   | |    \r\n"
									"     |                           | |    \r\n"
									"     |~                 ^        \\ |    \r\n"
									"     |  o~_            / \\       / |    \r\n"
									"     |~    \\ ENDURING TATTERS    | |    \r\n"
									"     |      \\         /    ~^~-_ , |    \r\n"
									"     |~      -   __~~-          \\| |    \r\n"
									"     |        \\_/               /| |    \r\n"
									"     |~          PRESS X      _/ / |    \r\n"
									"     |                  \\_   /   | |    \r\n"
									"     |~                   \\_/    | |    \r\n"
									"     |     /\\                    '~|    \r\n"
									"     |~__-~  ~-_~~--~_-~-_---~__-~_)    \r\n"
									"     | |                                \r\n"
									"     '~'                                \r\n"
					  	  	  	  	"                                        \r\n";
			  HAL_UART_Transmit_IT(&huart2,screen,sizeof(screen));					// Prints the title screen
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]=='x')) {						// Waits for X.
					  dataAvail = 0;										// Shows that data has been used.
					  dialogue += 1;										// Goes to new dialogue.
					  break;
				  }
			  }
		  }else if (dialogue==1) {
			  HAL_UART_Transmit(&huart2,difficulty,sizeof(difficulty),50);			// Prints the difficulty text.
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]=='1')) {								// Waits for 1.
					  uint8_t text[] = "\r\nTaking the easy way out. \r\n";
					  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
					  dataAvail = 0;										// Shows that data has been used.
					  HAL_Delay(50);											// Wait for everything to be set.
					  break;
				  }else if (dataAvail && (rx_buff[0]=='2')) {							// Waits for 2.
					  uint8_t text[] = "Enjoy the challenge. \r\n";
					  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal./ Shows that data has been used.
					  dataAvail = 0;										// Shows that data has been used.
					  diff = 1;													// Sets the logic to hard.
					  HAL_Delay(50);											// Wait for everything to be set.
					  break;
				  }
			  }
			  dialogue += 1;										// Goes to new dialogue.
		  }else if  (dialogue==2) {
			  HAL_UART_Transmit(&huart2,characterName,sizeof(characterName),10);		// Asks the user what name they want.
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0] == 127)){								// If statement for when BACKSPACE is pressed.
					  HAL_UART_Transmit(&huart2, rx_buff, sizeof(rx_buff),50);		// Shows the user input on the terminal.
					  if (i==0) {													// If statement for when the data index is 0.
						  i;												// Does not change index so there wont be unwanted characters in rx_data.
						  dataAvail = 0;										// Shows that data has been used.
					  }else{
						  i--;												// Goes back in index to counter the DEL button press.
						  dataAvail = 0;										// Shows that data has been used.
					  }
				  }else if (dataAvail && (rx_buff[0] == 13)) {									// If statement for when ENTER is pressed.
					name[i] = '\0';												// Placing null character at the end of string.
					dataAvail = 0;										// Shows that data has been used.
					HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
					break;
				  }else if (dataAvail && (i < 101)) {							// Else if statement to not exceed the max length.
					  name[i++] = rx_buff[0];											// Will only take the first 100 characters minus null.
					  HAL_UART_Transmit(&huart2, rx_buff, sizeof(rx_buff), 50);		// Shows the user input on the terminal.
					  dataAvail = 0;										// Shows that data has been used.
				  }
				  HAL_Delay(50);											// Wait for everything to be set.
			  }
			  i = 0;											// Reseting the iterator.
			  dialogue += 1;										// Goes to new dialogue.
			  HAL_Delay(50);											// Wait for everything to be set.
			  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
		  }else if  (dialogue==3) {
			  HAL_UART_Transmit(&huart2,askJob,sizeof(askJob),50);				// Gives the user a tip about the game.
			  uint8_t options[] = "(1) Navigator\r\n(2) Carpenter\r\n(3) Cooper\r\n";
			  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]=='1')) {								// Waits for 1.
					  strcpy(job,"Navigator");
					  dataAvail = 0;										// Shows that data has been used.

					  break;
				  }else if (dataAvail && (rx_buff[0]=='2')) {							// Waits for 2.
					  strcpy(job,"Carpenter");
					  dataAvail = 0;										// Shows that data has been used.

					  break;
				  }else if (dataAvail && (rx_buff[0]=='3')) {							// Waits for 2.
					  strcpy(job,"Cooper");
					  dataAvail = 0;										// Shows that data has been used.
					  break;
				  }
			  }
			  dialogue += 1;										// Goes to new dialogue.
			  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
		  }else if  (dialogue==4) {
			  HAL_UART_Transmit(&huart2,tip,sizeof(tip),50);				// Gives the user a tip about the game.
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]==13)) {						// Waits for ENTER.
					  dataAvail = 0;										// Shows that data has been used.
					  dialogue = 0;									// Reset the dialogue counter for later use.
					  start = 1;
					  break;
				  }
			  }
			  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
		  }else{ __NOP();}												// Nothing will happen.
	  }



	  else if (start && !begin) {
		  if (dialogue==0) {
			  uint8_t text[] = "\r\nYou sit inside of your workshop staring into the ceiling. Business has been slow lately it is becoming harder to maintain your shop. You almost doze off to sleep until you hear the front door bell.\r\n(Press ENTER to continue)\r\n";
			  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]==13)) {						// Waits for ENTER.
					  dataAvail = 0;										// Shows that data has been used.
					  break;
				  }
			  }
			  dialogue += 1;										// Goes to new dialogue.
		  }else if (dialogue==1) {
			  uint8_t text[] = "\r\nYou quickly leave the backroom and see a tall, bulky man standing at the front counter.\r\n(Press ENTER to continue)\r\n";
			  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]==13)) {						// Waits for ENTER.
					  dataAvail = 0;										// Shows that data has been used.
					  break;
				  }
			  }
			  dialogue += 1;										// Goes to new dialogue.
		  }else if (dialogue==2) {
			  uint8_t text[] = "\r\nDo you happen to be ";
			  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
			  HAL_UART_Transmit(&huart2,name,strlen(name),100);					// Prints the name of player character.
			  HAL_UART_Transmit_IT(&huart2,"?\r\n",sizeof("?\r\n"));			// Prints text to the terminal.

			  HAL_Delay(50);											// Wait for everything to be set.
			  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
			  uint8_t options[] = "(1) Yes";
			  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
			  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.


			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]=='1')) {						// Waits for ENTER.
					  dataAvail = 0;										// Shows that data has been used.
					  break;
				  }
			  }
			  dialogue += 1;										// Goes to new dialogue.
		  }else if (dialogue==3) {
			  if ((strcmp(job,"Navigator")==0)) {
				  uint8_t text[] = "I saw you had a posting that you needed some work. We could use a navigator. Having me be the only one steering the ship gets very demanding.\r\n(Press ENTER to continue)\r\n";
				  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
			  }else if ((strcmp(job,"Carpenter")==0)) {
				  uint8_t text[] = "I saw you had a posting that you needed some work. We could use a carpenter. My main sail has been look very questionable after a storm that hit us a week ago.\r\n(Press ENTER to continue)\r\n";
				  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
			  }else if ((strcmp(job,"Cooper")==0)) {
				  uint8_t text[] = "I saw you had a posting that you needed some work. We could use a cooper. There are a couple of barrels on my ship that have been damaged and need fixing.\r\n(Press ENTER to continue)\r\n";
				  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
			  }
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]==13)) {						// Waits for ENTER.
					  dataAvail = 0;										// Shows that data has been used.
					  break;
				  }
			  }
			  dialogue += 1;										// Goes to new dialogue.
		  }else if (dialogue==4) {
			  uint8_t text[] = "\r\nWe need more hands to be able to open this treasure to found.\r\n(Press ENTER to continue)\r\n";
			  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]==13)) {						// Waits for ENTER.
					  dataAvail = 0;										// Shows that data has been used.
					  break;
				  }
			  }
			  dialogue += 1;										// Goes to new dialogue.
		  }else if (dialogue==5) {
			  uint8_t text[] = "\r\nIf you join us, i'll give you a portion of the treasure how does that sound?\r\n";
			  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

			  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
			  uint8_t options[] = "(1) Yes\r\n(2) No";
			  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
			  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]=='1')) {
					  dataAvail = 0;										// Shows that data has been used.
					  dialogue += 1;										// Goes to new dialogue.
					  break;


				  }else if (dataAvail && (rx_buff[0]=='2')) {
					  dataAvail = 0;										// Shows that data has been used.
					  dialogue = 9;											// Goes to fake ending branch.
					  break;
				  }
			  }
		  }else if (dialogue==6) {
			  uint8_t text[] = "Great! Come down to the port tomorrow and we'll set sail at midday.\r\n(Press ENTER to continue)\r\n";
			  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]==13)) {						// Waits for ENTER.
					  dataAvail = 0;										// Shows that data has been used.
					  break;
				  }
			  }
			  dialogue += 1;										// Goes to new dialogue.
		  }else if (dialogue==7) {
			  uint8_t text[] = "\r\nYou watch as he leaves the store. You wonder if this could finally be the opportunity to turn this store around.\r\n(Press ENTER to continue)\r\n";
			  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]==13)) {						// Waits for ENTER.
					  dataAvail = 0;										// Shows that data has been used.
					  dialogue = 0;									// Reset the dialogue counter for later use.
					  begin = 1;											// Begins the game.
					  strcpy(room,"Hull");									// Changes the room to the hull.
					  break;
				  }
			  }
			  HAL_Delay(50);											// Wait for everything to be set.
		  }else if (dialogue==9) {
			  uint8_t text[] = "\r\nYou watch as he leaves the store. You wonder if you made the right choice not taking that big of a risk.\r\n(Press ENTER to continue)\r\n";
			  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
			  while (1)
			  {
				  if (dataAvail && (rx_buff[0]==13)) {						// Waits for ENTER.

					  uint8_t screen[] = "                                                                                                   \r\n"
										"      ##### /    ##                               ##### /    ##   ###                    #######   \r\n"
										"   ######  /  #####                            ######  /  #####    ###     #            /      ### \r\n"
										"  /#   /  /     #####                         /#   /  /     #####   ###   ###          /        ## \r\n"
										" /    /  ##     # ##                         /    /  ##     # ##      ##   #           ##       #  \r\n"
										"     /  ###     #                                /  ###     #         ##                       /   \r\n"
										"    ##   ##     #    /###   ##   ####           ##   ##     #         ## ###   ###  /###      /    \r\n"
										"    ##   ##     #   / ###  / ##    ###  /       ##   ##     #         ##  ###   ###/ #### /  /     \r\n"
										"    ##   ##     #  /   ###/  ##     ###/        ##   ##     #         ##   ##    ##   ###/  /      \r\n"
										"    ##   ##     # ##    ##   ##      ##         ##   ##     #         ##   ##    ##    ##  /       \r\n"
										"    ##   ##     # ##    ##   ##      ##         ##   ##     #         ##   ##    ##    ##  #       \r\n"
										"     ##  ##     # ##    ##   ##      ##          ##  ##     #         ##   ##    ##    ##   ##     \r\n"
										"      ## #      # ##    ##   ##      ##           ## #      #         /    ##    ##    ##          \r\n"
										"       ###      # ##    ##   ##      /#            ###      /##      /     ##    ##    ##   #      \r\n"
										"        #########  ######     ######/ ##            #######/ #######/      ### / ###   ### ###     \r\n"
										"          #### ###  ####       #####   ##             ####     ####         ##/   ###   ### #      \r\n"
										"                ###                                                                                \r\n"
										"    ########     ###                                                                               \r\n"
										"  /############  /#                                                                                \r\n"
										" /           ###/                                                                                  \r\n";
					    HAL_UART_Transmit_IT(&huart2,screen,sizeof(screen)); // ASCII art for fake end.
					  dataAvail = 0;										// Shows that data has been used.
				  }
			  }
		  }
	  }

	  while(begin) // The main game loop
	  {
		  if (strcmp(room,"Captain's Cabin")==0) {		  // Room: ---Captain's Cabin---
			  if (roomEnter) {
				uint8_t text[] = "\r\nYou walk into the Captain's Cabin and see he tall man from before. Along with a map and a chest.\r\n";
				HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
			    uint8_t screen[] =	"                                              \r\n"		// ASCII art of the location of the player.
									"                        CN                    \r\n"
									"                        ==      P=player      \r\n"
									"                        ||>                   \r\n"
									"                        ||                    \r\n"
									"                      | || |                  \r\n"
									"               ==     ======           ==     \r\n"
									"               ||>     /||             ||>    \r\n"
									"               ==     / ||             ==     \r\n"
									"              /||     ( ||            /||     \r\n"
									"             / ||     \\ ||           / ||     \r\n"
									"            /  ||      \\||           ( ||     \r\n"
									"           /(  ||      /||           \\ ||  |>\r\n"
									"          / (  ||     / ||            \\||  |  \r\n"
									"         /  (  ||     ( ||          /-------  \r\n"
									"        /   (  ||     \\ ||         /||||||||  \r\n"
									"       /---_(  ||      \\||        /|--------  \r\n"
									"      /      \\ ||   H   ||       /|-|  CC  `  \r\n"
									"  ___/        \\||       ||      /|- |      `  \r\n"
									"     \\_        ||       ||  ()  |-  :  P - |   \r\n"
									"       \\___________________________________|  \r\n"
									"        \\:|: |   ||: |     |_|:|:|    | :|:|  \r\n"
									"         \\---| K |---|   A_- |---| CH |----|  \r\n"
									"          \\|o|   |o|o|     |_|:o||    | ||:|  \r\n"
									"           \\-| /)|---|()  _- |---|    | |-/   \r\n"
									"            \\-----|: --------- :|--------/    \r\n"
									"             \\--------------------------/     \r\n"
									"~~~~~~~~~~~~~~\\--:|:--:|:--:|:--:|:--:|/~~~~~~\r\n"
									"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";
				HAL_UART_Transmit(&huart2,screen,sizeof(screen),1500);					// Prints the map
				roomEnter = 0;					// Proceeds to the next part of the code.
			  }else if (!choice){
				   HAL_Delay(50);											// Wait for everything to be set.
				   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				   uint8_t options[] = "Where do you want to go?\r\n(1) Captain\r\n(2) Map\r\n(3) Chest\r\n(4) Somewhere Else";
				   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
				   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				   choice = 1;					// Goes into a loop for user input.

			   }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {
					   while (1)
					   {
						   if (!captain) {
							   uint8_t text[] = "You see the captain staring at the chest trying to find any clues on how to open the chest.\r\n";
							   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   uint8_t options[] = "What do you want to do?\r\n(1) Talk\r\n(2) Go Back";
							   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   dataAvail = 0;										// Shows that data has been used.
							   captain = 1;												// Starts the dialogue loop for the captain.
						   }else if (captain) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   dataAvail = 0;										// Shows that data has been used.
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   dataAvail = 0;										// Shows that data has been used.
										   dialogue += 1;										// Goes to new dialogue.
									   }else if (dataAvail && (rx_buff[0]=='2')) {
											  dataAvail = 0;										// Shows that data has been used.
											  captain = 0;										// Ends the the dialogue loop for the captain.
											  dialogue = 0;									// Reset the dialogue counter for later use.
											  break;
									   }
									   if (dialogue==0) {
										   uint8_t text[] = "Ah it seems like there is some sort of message on the chest, but I can't seem to understand what it is supposed to be.\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "What do you want to do?\r\n(1) Ask About The Map\r\n(2) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   dialogue += 1;										// Goes to new dialogue.
									   }else if (dialogue==2) {
										   uint8_t text[] = "We were able to find the treasure pretty easy, but I don't quite understand what the four arrows are supposed to mean.\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "What do you want to do?\r\n(1) Ask About The Kaleidoscope\r\n(2) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   dialogue += 1;										// Goes to new dialogue.
									   }else if ((dialogue==4) && (!capKal)) {
										   uint8_t text[] = "We found these pieces around the cave where we found the chest, but it just looks like a piece of junk with no value. Here you can have it maybe you can find a use for it. [He hands you a piece of the kaleidoscope]\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "What do you want to do?\r\n(1) Ask About The Chest\r\n(2) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   capKal = 1;										// Shows that the kaleidoscope has been obtained from the captain.
										   kaleidoscope += 1;									// 1 piece of the kaleidoscope has been found.
										   dialogue += 1;										// Goes to new dialogue.
										   HAL_Delay(50);											// Wait for everything to be set.
									   }else if ((dialogue==4) && (capKal)) {
										   uint8_t text[] = "We found it in the cave where we found the chest, but it just looks like a piece of junk with no value. Though it does make some colorful light when I shine a candle on it.\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "What do you want to do?\r\n(1) Ask About The Chest\r\n(2) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   dialogue += 1;										// Goes to new dialogue.
									   }else if (dialogue>4) {
										   dialogue = -1;										// Resets the dialogue options for the captain.
									   }
								   }

							   }else if (dataAvail && (rx_buff[0]=='2')) {
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  break;
							   }
						   }
					   }
				   }else if (dataAvail && (rx_buff[0]=='2')) {
					   strcpy(room,"Map");									// Changes the room to the map.
					   roomEnter = 1; 										// Sets the logic for the room enter dialogue.
					   dataAvail = 0;										// Shows that data has been used.
					   choice = 0;											// Reprints the main options for the rooms.
				   }else if (dataAvail && (rx_buff[0]=='3')) {
					   strcpy(room,"Chest");								// Changes the room to the chest.
					   roomEnter = 1; 										// Sets the logic for the room enter dialogue.
					   dataAvail = 0;										// Shows that data has been used.
					   choice = 0;											// Reprints the main options for the rooms.
				   }else if (dataAvail && (rx_buff[0]=='4')) {
					   while (1)
					   {
						   if (!nextRoom) {
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   uint8_t options[] = "Where do you want to go?\r\n(1) Hull\r\n(2) Go Back";
							   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   nextRoom = 1;											// Goes into new room loop.
							   dataAvail = 0;										// Shows that data has been used.
						   }else if (nextRoom) {
							   if (dataAvail && (rx_buff[0]=='1')) {
							 	   strcpy(room,"Hull");									// Changes the room to the hull.
							 	   nextRoom = 0;										// Resets the logic to be used later.
							 	   roomEnter = 1; 										// Sets the logic for the room enter dialogue.
							 	   dataAvail = 0;										// Shows that data has been used.
							 	   choice = 0;											// Reprints the main options for the rooms.
								   break;
							   }else if (dataAvail && (rx_buff[0]=='2')) {
								   nextRoom = 0;										// Resets the logic to be used later.
								   dataAvail = 0;										// Shows that data has been used.
								   choice = 0;											// Reprints the main options for the rooms.
								   break;
							   }
						  }
					  }
				   }else if (dataAvail && (rx_buff[0]=='i')) {
						  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
						  items( kaleidoscope,  key,  beer,  food,  hammer,  rag,  rope,  locket);		// Function to print all the items the player has.
						  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
						  dataAvail = 0;										// Shows that data has been used.
						  choice = 0;											// Reprints the main options for the rooms.
					 }
			   }

		  }else if(strcmp(room,"Map")==0) {		  // Room: ---Map---
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou stand over an old, worn-down map with several symbols around the borders.\r\n";
				  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
				  roomEnter = 0;					// Proceeds to the next part of the code.
			  }else if (!choice) {
				  HAL_Delay(50);											// Wait for everything to be set.
				   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				  uint8_t options[] = "What do you want to do?\r\n(1) Circle Pattern\r\n(2) Arrow Writing\r\n(3) Go Back";
				  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
				   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				  choice = 1;					// Goes into a loop for user input.
			  }else if (choice) {
				  if (dataAvail && (rx_buff[0]=='1') && (kaleidoscope<3)) {
					  uint8_t text[] = "You see a large circle with three smaller circles and two arrows near the bottom. One going to the right and another down.\r\n";
					  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  uint8_t options[] = "What do you want to do?\r\n(1) Go Back";
					  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  dataAvail = 0;										// Shows that data has been used.
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')) {
							  dataAvail = 0;										// Shows that data has been used.
							  choice = 0;											// Reprints the main options for the rooms.
							  break;
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='1') && (kaleidoscope==3)) {
					  uint8_t text[] = "You see a large circle with three smaller circles and two arrows near the bottom. One going to the right and followed by another on going down.\r\n";
					  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  uint8_t options[] = "What do you want to do?\r\n(1) Shine The Kaleidoscope\r\n(2) Go Back";
					  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  dataAvail = 0;										// Shows that data has been used.
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')) {
							  uint8_t text[] = "You shine the kaleidoscope on the circle and line it the light with the three small circles. It revles the number '48' that lines up with the arrows on the bottom of the large circle.\r\n";
							  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  uint8_t options[] = "What do you want to do?\r\n(1) Go Back";
							  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  dataAvail = 0;										// Shows that data has been used.
							  while (1)
							  {
								  if (dataAvail && (rx_buff[0]=='1')) {
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  break;
								  }
							  }
							  break;
						  }else if (dataAvail && (rx_buff[0]=='2')) {
							  dataAvail = 0;										// Shows that data has been used.
							  choice = 0;											// Reprints the main options for the rooms.
							  break;
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='2')) {
					  uint8_t text[] = "The arrows go in a sequence of up, right, left, down.\r\n";
					  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  uint8_t options[] = "What do you want to do?\r\n(1) Go Back";
					  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  dataAvail = 0;										// Shows that data has been used.
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')) {
							  dataAvail = 0;										// Shows that data has been used.
							  choice = 0;											// Reprints the main options for the rooms.
							  break;
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='3')) {
					  strcpy(room, "Captain's Cabin");						// Changes the room to the capatin's cabin.
					  dataAvail = 0;										// Shows that data has been used.
					  choice = 0;											// Reprints the main options for the rooms.
				  }else if (dataAvail && (rx_buff[0]=='i')) {
					  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
					  items( kaleidoscope,  key,  beer,  food,  hammer,  rag,  rope,  locket);		// Function to print all the items the player has.
					  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
					  dataAvail = 0;										// Shows that data has been used.
					  choice = 0;											// Reprints the main options for the rooms.
				  }
			  }


		  }else if(strcmp(room,"Chest")==0) {		  // Room: ---Chest---
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou look at the aged chest covered in markings.\r\n";
				  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
				  roomEnter = 0;					// Proceeds to the next part of the code.

			  }else if (!choice) {
				  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				  uint8_t options[] = "What do you want to do?\r\n(1) Strange Markings\r\n(2) Lock\r\n(3) Go Back";
				  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
				  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				  HAL_Delay(50);											// Wait for everything to be set.
				  choice = 1;					// Goes into a loop for user input.
			  }else if (choice) {
				  if (dataAvail && (rx_buff[0]=='1') && !diff) {
					  uint8_t text[] = "The markings are withered due to age but reads 'the smallest and largest' along with two arrows one pointing up followed by another pointing to the left.\r\n";
					  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  uint8_t options[] = "What do you want to do\r\n(1) Go back";
					  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  dataAvail = 0;										// Shows that data has been used.
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')) {
							  dataAvail = 0;										// Shows that data has been used.
							  choice = 0;											// Reprints the main options for the rooms.
							  break;
						  }
					  }
				  }else	if (dataAvail && (rx_buff[0]=='1') & diff) {
					  uint8_t text[] = "The markings are withered due to age but reads 'het llestsma dan gestlar' along with two arrows one pointing up followed by another pointing to the left.\r\n";
					  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  uint8_t options[] = "What do you want to do\r\n(1) Go back";
					  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  dataAvail = 0;										// Shows that data has been used.
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')) {
							  dataAvail = 0;										// Shows that data has been used.
							  choice = 0;											// Reprints the main options for the rooms.
							  break;
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='2')) {
					  while (1)
					  {
						  if (!chest) {
							  uint8_t text[] = "You look at the rusty lock it shows a 4 number combination all ranging from 0-9. There is also an 'S' and 'L' on the sides of the lock.\r\n";
							  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   uint8_t options[] = "What do you want to do?\r\n(1) Try To Unlock It\r\n(2) Go Back";
							   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  dataAvail = 0;										// Shows that data has been used.
							  chest = 1;											// Starts the dialogue loop for the chest.
						  }else if (chest) {
							  // The end combination for the chest

							  if (dataAvail && (rx_buff[0]=='1')) {

								  uint8_t text[] = "Enter the combination.\r\n";
								  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
								  dataAvail = 0;										// Shows that data has been used.
								  while (1)
								  {
										if (dataAvail && i<4) {
											attempt[i++] = rx_buff[0];							// Stores each input into a variable for later.
										    HAL_UART_Transmit_IT(&huart2,rx_buff,sizeof(rx_buff));			// Shows the user what they are typing.
											dataAvail = 0;										// Shows that data has been used.
										}
										if (i==4) {
											attempt[4] = '\0';									// Ends the string with a NULL.
											HAL_Delay(50);											// Wait for everything to be set.
											checkAns(attempt, answer, &endCode);					// Call a function to check for the right answer.
											HAL_Delay(50);											// Wait for everything to be set.
										    HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										    if (endCode) {
											    uint8_t text[] = "You hear the lock click. Your eyes widen in anticipation as you open the chest. You see the \r\nglitter from the gold and jewels and sigh is relief as your gamble has payed off.\r\n";
											    HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							    				HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								 			    uint8_t options[] = "(1) Next";
											    HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
											    HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.

											    while (1)
											    {
											    	if (dataAvail && (rx_buff[0]=='1')) {
														  uint8_t screen[] = "      ##### /    ##                               ##### /    ##   ###                       \r\n"			// ASCII art for the win screen.
																				"   ######  /  #####                            ######  /  #####    ###     #                \r\n"
																				"  /#   /  /     #####                         /#   /  /     #####   ###   ###               \r\n"
																				" /    /  ##     # ##                         /    /  ##     # ##      ##   #                \r\n"
																				"     /  ###     #                                /  ###     #         ##                    \r\n"
																				"    ##   ##     #    /###   ##   ####           ##   ##     #         ## ###   ###  /###    \r\n"
																				"    ##   ##     #   / ###  / ##    ###  /       ##   ##     #         ##  ###   ###/ #### / \r\n"
																				"    ##   ##     #  /   ###/  ##     ###/        ##   ##     #         ##   ##    ##   ###/  \r\n"
																				"    ##   ##     # ##    ##   ##      ##         ##   ##     #         ##   ##    ##    ##   \r\n"
																				"    ##   ##     # ##    ##   ##      ##         ##   ##     #         ##   ##    ##    ##   \r\n"
																				"     ##  ##     # ##    ##   ##      ##          ##  ##     #         ##   ##    ##    ##   \r\n"
																				"      ## #      # ##    ##   ##      ##           ## #      #         /    ##    ##    ##   \r\n"
																				"       ###      # ##    ##   ##      /#            ###      /##      /     ##    ##    ##   \r\n"
																				"        #########  ######     ######/ ##            #######/ #######/      ### / ###   ###  \r\n"
																				"         #### ###  ####       #####   ##             ####     ####         ##/   ###   ### \r\n"
																				"                ###                                                                         \r\n"
																				"    ########     ###                                                                        \r\n"
																				"  /############  /#                                                                         \r\n"
																				" /           ###/                                                                           \r\n";
														    HAL_UART_Transmit_IT(&huart2,screen,sizeof(screen));
														    dataAvail = 0;										// Shows that data has been used.
														    while (1)
														    {
														    }
											    	}

											    }
										    }else if (!endCode) {
											    uint8_t text[] = "The lock does not open. It seems that combination was wrong.\r\n";
												HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints the different options.

							    				HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								 			    uint8_t options[] = "(1) Back";
											    HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
											    HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.



											    while (1)
											    {
											    	if (dataAvail && (rx_buff[0]=='1')) {
											    		dataAvail = 0;										// Shows that data has been used.
											    		i = 0;											// Reseting the iterator.
											    		dialogue = 0;									// Reset the dialogue counter for later use.
											    		chest = 0;										// Ends the dialogue loop for the chest.
											    		endCode = 1;									// Resets the end game logic.
											    		break;
											    	}
											    }break;
										    }
										}
								  	  }
							  }else if (dataAvail && (rx_buff[0]=='2')) {
								  dataAvail = 0;										// Shows that data has been used.
								  choice = 0;											// Reprints the main options for the rooms.
								  break;
							  }
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='3')) {
					  strcpy(room, "Captain's Cabin");						// Changes the room to the capatin's cabin.
					  dataAvail = 0;										// Shows that data has been used.
					  choice = 0;											// Reprints the main options for the rooms.
				  }else if (dataAvail && (rx_buff[0]=='i')) {
					  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
					  items( kaleidoscope,  key,  beer,  food,  hammer,  rag,  rope,  locket);		// Function to print all the items the player has.
					  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
					  dataAvail = 0;										// Shows that data has been used.
					  choice = 0;											// Reprints the main options for the rooms.
				  }
			  }

		  }else if (strcmp(room,"Hull")==0) {		  // Room: ---Hull---
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou make your way to the hull of the ship and see the bright blue sky along with the sound \r\nof the gentle waves crashing against the ship.\r\n";
					HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
				    uint8_t screen[] =	"                                              \r\n"		// ASCII art of the location of the player.
										"                        CN                    \r\n"
										"                        ==      P=player      \r\n"
										"                        ||>                   \r\n"
										"                        ||                    \r\n"
										"                      | || |                  \r\n"
										"               ==     ======           ==     \r\n"
										"               ||>     /||             ||>    \r\n"
										"               ==     / ||             ==     \r\n"
										"              /||     ( ||            /||     \r\n"
										"             / ||     \\ ||           / ||     \r\n"
										"            /  ||      \\||           ( ||     \r\n"
										"           /(  ||      /||           \\ ||  |>\r\n"
										"          / (  ||     / ||            \\||  |  \r\n"
										"         /  (  ||     ( ||          /-------  \r\n"
										"        /   (  ||     \\ ||         /||||||||  \r\n"
										"       /---_(  ||      \\||        /|--------  \r\n"
										"      /      \\ ||   H   ||       /|-|  CC  `  \r\n"
										"  ___/        \\||       ||      /|- |      `  \r\n"
										"     \\_        ||   P   ||  ()  |-  :    - |   \r\n"
										"       \\___________________________________|  \r\n"
										"        \\:|: |   ||: |     |_|:|:|    | :|:|  \r\n"
										"         \\---| K |---|   A_- |---| CH |----|  \r\n"
										"          \\|o|   |o|o|     |_|:o||    | ||:|  \r\n"
										"           \\-| /)|---|()  _- |---|    | |-/   \r\n"
										"            \\-----|: --------- :|--------/    \r\n"
										"             \\--------------------------/     \r\n"
										"~~~~~~~~~~~~~~\\--:|:--:|:--:|:--:|:--:|/~~~~~~\r\n"
										"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";
					HAL_UART_Transmit(&huart2,screen,sizeof(screen),1500);					// Prints the map
				  roomEnter = 0;					// Proceeds to the next part of the code.

			  }else if (!choice) {
				  HAL_Delay(50);											// Wait for everything to be set.
				  if (shine) { // start

					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  uint8_t options[] = "Where do you want to go?\r\n(1) Worried Crew Member\r\n(2) Sea\r\n(3) Barrel\r\n(4) Somewhere Else";
					  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				  }else{
					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  uint8_t options[] = "Where do you want to go?\r\n(1) Worried Crew Member\r\n(2) Sea\r\n(3) Somewhere Else";
					  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				  }
				  choice = 1;					// Goes into a loop for user input.

			  }else if (choice) {
				  if (dataAvail && (rx_buff[0]=='1')) {			// Worried Crew Member Choice
					  while(1)
					  {
						  if (!worriedCrewmember) {
							  uint8_t text[] = "The crew member has a scared look on his face.\r\n";
							  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  uint8_t options[] = "What do you want to do?\r\n(1) Talk\r\n(2) Go Back";
							  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  worriedCrewmember = 1;							// Starts the dialogue loop for the worried crew member.
							  dataAvail = 0;										// Shows that data has been used.
						  }else if (worriedCrewmember) {
							  if (dataAvail && (rx_buff[0]=='1') && !worRag) {
								  if (rag) {
									  uint8_t text[] = "I don't know what I am going to do. Chef will be so mad at me if he realizes I broke another mop. Do you happen to have a rag on you?\r\n";
									  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

					    				HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
						 			    uint8_t options[] = "(1) Yes\r\n(2) Go Back";
									    HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
									    HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								  }else{
									  uint8_t text[] = "I don't know what I am going to do. Chef will be so mad at me if he realizes I broke another mop. Do you happen to have a rag on you?\r\n";
									  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

					    				HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
						 			    uint8_t options[] = "(1) No\r\n(2) Go Back";
									    HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
									    HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								  }
								  dataAvail = 0;										// Shows that data has been used.
								  while (1)
								  {
									  if (dataAvail && (rx_buff[0]=='1') && dialogue) {
										  dataAvail = 0;										// Shows that data has been used.
										  worriedCrewmember = 0;							// Ends the dialogue loop for the worried crew member.
										  dialogue = 0;									// Reset the dialogue counter for later use.
										  break;
									  }else if (dataAvail && (rx_buff[0]=='1') && rag) {
										  uint8_t text[] = "You do! Here I can give you this for you saving me. [He hands you a beer]\r\n";
										  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							 			  uint8_t options[] = "(1) Go Back";
										  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										  dataAvail = 0;										// Shows that data has been used.
										  rag = 0;											// Player used rag
										  worRag = 1;										// Logic for special dialogue for worried crew member.
										  dialogue = 1;										// Goes to new dialogue.
										  getBeer(&beer, &diff);								// Goes to a function to manage the amount of beer.
										  HAL_Delay(50);											// Wait for everything to be set.
									  }else if (dataAvail && (rx_buff[0]=='1') && !rag) {
										  uint8_t text[] = "Its fine I did this to myself. Maybe I can find some somewhere on this ship.\r\n";
										  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							 			  uint8_t options[] = "(1) Go Back";
										  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										  dataAvail = 0;										// Shows that data has been used.
										  dialogue = 1;										// Goes to new dialogue.
									  }else if (dataAvail && (rx_buff[0]=='2') && !dialogue) {
										  dataAvail = 0;										// Shows that data has been used.
										  worriedCrewmember = 0;							// Ends the dialogue loop for the worried crew member.
										  break;
									  }
									  HAL_Delay(50);											// Wait for everything to be set.
								  }
							  }else if (dataAvail && (rx_buff[0]=='1') && worRag) {
								  uint8_t text[] = "Thank you for the rags! I don't know what I would have done if I couldn't fix the mop.\r\n";
								  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					 			  uint8_t options[] = "(1) Go Back";
								  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								  dataAvail = 0;										// Shows that data has been used.
								  while (1)
								  {
									  if (dataAvail && (rx_buff[0]=='1')) {
										  dataAvail = 0;										// Shows that data has been used.
										  worriedCrewmember = 0;							// Ends the dialogue loop for the worried crew member.
										  break;
									  }
								  }
							  }else if (dataAvail && (rx_buff[0]=='2')) {
								  dataAvail = 0;										// Shows that data has been used.
								  choice = 0;											// Reprints the main options for the rooms.
								  break;
							  }
						  }else{ __NOP();}
						  HAL_UART_Receive_IT(&huart2, rx_buff, 1);
					  }
				  }else if (dataAvail && (rx_buff[0]=='2')) {		// Sea Choice
					  while (1)
					  {
						  if (!dialogue) {
							  uint8_t text[] = "You look out into the vast open sea wondering if you would really get anything out of taking this job.\r\n";
							  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				 			  uint8_t options[] = "(1) Go Back";
							  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  dataAvail = 0;										// Shows that data has been used.
							  dialogue = 1;										// Goes to new dialogue.
						  }else if(dialogue) {
							  if (dataAvail && (rx_buff[0]=='1')) {
								  dataAvail = 0;										// Shows that data has been used.
								  choice = 0;											// Reprints the main options for the rooms.
								  dialogue = 0;									// Reset the dialogue counter for later use.
								  break;
							  }
						  }
					  }
				  }else if ((dataAvail && (rx_buff[0]=='3') && shine)) {		// Barrel Choice
					  while (1)
					  {
						  if (!dialogue && !chefLoc) {
							  uint8_t text[] = "You go over to the barrel near where you saw that shining light.\r\n";
							  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				 			  uint8_t options[] = "(1) Grab The Locket\r\n(2) Go Back";
							  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  dataAvail = 0;										// Shows that data has been used.
							  dialogue = 1;										// Goes to new dialogue.
							  while (1)
							  {
								  if (dataAvail && (rx_buff[0]=='1') && chefLoc) {
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  dialogue = 0;									// Reset the dialogue counter for later use.
									  break;
								  }else if (dataAvail && (rx_buff[0]=='1')) {
									  uint8_t text[] = "You pick up the locket and see that there is a picture of a pretty woman on the inside of the locket. Did someone lose this?\r\n";
									  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

									  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
						 			  uint8_t options[] = "(1) Go Back";
									  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
									  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
									  dataAvail = 0;										// Shows that data has been used.
									  locket = 1;											// Player get locket.
									  chefLoc = 1;											// logic for special dialogue for the chef.
								  }else if (dataAvail && (rx_buff[0]=='2')) {
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  dialogue = 0;									// Reset the dialogue counter for later use.
									  break;
								  }
								  HAL_Delay(50);											// Wait for everything to be set.
							  }
							  break;
						  }else if(!dialogue && chefLoc) {
							  uint8_t text[] = "You go over to the barrel near where you found the locket. Maybe this belongs to someone of the ship.\r\n";
							  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				 			  uint8_t options[] = "(1) Go Back";
							  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  dataAvail = 0;										// Shows that data has been used.
							  dialogue = 1;										// Goes to new dialogue.
							  while (1)
							  {
								  if (dataAvail && (rx_buff[0]=='1') && chefLoc) {
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  dialogue = 0;									// Reset the dialogue counter for later use.
									  break;
								  }
							  }
							  break;
						  }
					  }
				  }else if ((dataAvail && (rx_buff[0]=='3') && !shine) || (dataAvail && (rx_buff[0]=='4') && shine)) {			// Change location Choice
					  while (1)
					  {
						  if (!nextRoom) {
							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  uint8_t options[] = "Where to next?\r\n(1) Crow's Nest\r\n(2) Armory\r\n(3) Captain's Cabin\r\n(4) Go Back";
							  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  nextRoom = 1;											// Goes into new room loop.
							  dataAvail = 0;										// Shows that data has been used.
						  }else if (nextRoom) {
							  if (dataAvail && (rx_buff[0]=='1')) {
								  strcpy(room, "Crow's Nest");							// Changes the room to the crow's nest.
								  nextRoom = 0;										// Resets the logic to be used later.
								  roomEnter = 1; 										// Sets the logic for the room enter dialogue.
								  dataAvail = 0;										// Shows that data has been used.
								  choice = 0;											// Reprints the main options for the rooms.
								  break;

							  }else if (dataAvail && (rx_buff[0]=='2')) {
								  strcpy(room, "Armory");
								  nextRoom = 0;										// Resets the logic to be used later.
								  roomEnter = 1; 										// Sets the logic for the room enter dialogue.
								  dataAvail = 0;										// Shows that data has been used.
								  choice = 0;											// Reprints the main options for the rooms.
								  break;

							  }else if (dataAvail && (rx_buff[0]=='3')) {
								  strcpy(room, "Captain's Cabin");						// Changes the room to the capatin's cabin.
								  nextRoom = 0;										// Resets the logic to be used later.
								  roomEnter = 1; 										// Sets the logic for the room enter dialogue.
								  dataAvail = 0;										// Shows that data has been used.
								  choice = 0;											// Reprints the main options for the rooms.
								  break;
							  }
							  else if (dataAvail && (rx_buff[0]=='4')) {
								  nextRoom = 0;										// Resets the logic to be used later.
								  dataAvail = 0;										// Shows that data has been used.
								  choice = 0;											// Reprints the main options for the rooms.
								  break;
							  }
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='i')) {
					  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
					  items( kaleidoscope,  key,  beer,  food,  hammer,  rag,  rope,  locket);		// Function to print all the items the player has.
					  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
					  dataAvail = 0;										// Shows that data has been used.
					  choice = 0;											// Reprints the main options for the rooms.
				  }
			  }

		  }else if (strcmp(room,"Kitchen")==0) {		  // Room: ---Kitchen---
			   if (roomEnter) {
				   uint8_t text[] = "\r\nEven before you enter the kitchen, you feel the heat cover your entire body with the smell of burning charcoal.\r\n";
				   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
				   uint8_t screen[] =	"                                              \r\n"		// ASCII art of the location of the player.
										"                        CN                    \r\n"
										"                        ==      P=player      \r\n"
										"                        ||>                   \r\n"
										"                        ||                    \r\n"
										"                      | || |                  \r\n"
										"               ==     ======           ==     \r\n"
										"               ||>     /||             ||>    \r\n"
										"               ==     / ||             ==     \r\n"
										"              /||     ( ||            /||     \r\n"
										"             / ||     \\ ||           / ||     \r\n"
										"            /  ||      \\||           ( ||     \r\n"
										"           /(  ||      /||           \\ ||  |>\r\n"
										"          / (  ||     / ||            \\||  |  \r\n"
										"         /  (  ||     ( ||          /-------  \r\n"
										"        /   (  ||     \\ ||         /||||||||  \r\n"
										"       /---_(  ||      \\||        /|--------  \r\n"
										"      /      \\ ||   H   ||       /|-|  CC  `  \r\n"
										"  ___/        \\||       ||      /|- |      `  \r\n"
										"     \\_        ||       ||  ()  |-  :    - |   \r\n"
										"       \\___________________________________|  \r\n"
										"        \\:|: |   ||: |     |_|:|:|    | :|:|  \r\n"
										"         \\---| K |---|   A_- |---| CH |----|  \r\n"
										"          \\|o|   |o|o|     |_|:o||    | ||:|  \r\n"
										"           \\-|P/)|---|()  _- |---|    | |-/   \r\n"
										"            \\-----|: --------- :|--------/    \r\n"
										"             \\--------------------------/     \r\n"
										"~~~~~~~~~~~~~~\\--:|:--:|:--:|:--:|:--:|/~~~~~~\r\n"
										"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";
				   HAL_UART_Transmit(&huart2,screen,sizeof(screen),1500);					// Prints the map
				   roomEnter = 0;					// Proceeds to the next part of the code.
			   }else if (!choice){
				   HAL_Delay(50);											// Wait for everything to be set.
				   if (broken) {
					   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					   uint8_t options[] = "Where do you want to go?\r\n(1) Chef\r\n(2) Broken Storage Box\r\n(3) Somewhere Else";
					   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
					   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				   }else{
					   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					   uint8_t options[] = "Where do you want to go?\r\n(1) Chef\r\n(2) Fixed Storage Box\r\n(3) Somewhere Else";
					   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
					   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				   }
				   choice = 1;					// Goes into a loop for user input.

			   }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {				   // Chef Choice
					   while (1)
					   {
						   if (!chef) {
							   uint8_t text[] = "\r\nThe chef quickly moves around the kitchen. He is covered in sweat rummaging \r\naround the small kitchen desperately trying to find something.\r\n";
							   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   uint8_t options[] = "What do you want to do?\r\n(1) Talk\r\n(2) Go Back";
							   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   dataAvail = 0;										// Shows that data has been used.
							   chef = 1;												// Starts the dialogue loop for the chef.
						   }else if (chef) {
							   if (dataAvail && (rx_buff[0]=='1') && chefBox) {
								   uint8_t text[] = "I saw that you fixed that storage box. I have been waiting for someone to finally do it. Here I heard you have been looking for these keep it as a reward. [He hands you a piece of the kaleidoscope]\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "(1) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   dataAvail = 0;										// Shows that data has been used.
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   dataAvail = 0;										// Shows that data has been used.
										   kaleidoscope += 1;									// 1 piece of the kaleidoscope has been found.
										   chef = 0;											// Ends the dialogue loop for the chef.
										   chefBox = 0;											// Ends the special dialogue for the chef.
										   break;
									   }
								   }
							   }else if (dataAvail && (rx_buff[0]=='1') && locket) {
								   uint8_t text[] = "Sorry I don't have a lot of time to talk right now I have to finish making the crews rations for tomorrow.\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "(1) Give Locket\r\n(2) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   dataAvail = 0;										// Shows that data has been used.
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1') && !locket) {
										   dataAvail = 0;										// Shows that data has been used.
										   chef = 0;											// Ends the dialogue loop for the chef.
										   break;
									   }
									   if (dataAvail && (rx_buff[0]=='1')) {
										   uint8_t text[] = "Wow! I can't believe you found it I have been looking everywhere for it ever since this morning. Here let me give you something special. [He hands you a beer]\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "(1) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   getBeer(&beer, &diff);								// Goes to a function to manage the amount of beer.
										   locket = 0;											// Uses the locket.
									   }else if (dataAvail && (rx_buff[0]=='2') && locket) {
										   dataAvail = 0;										// Shows that data has been used.
										   chef = 0;											// Ends the dialogue loop for the chef.
										   break;
									   }
									   HAL_Delay(50);											// Wait for everything to be set.
								   }
							   }else if (dataAvail && (rx_buff[0]=='1')) {
								   uint8_t text[] = "Sorry I don't have a lot of time to talk right now I have to finish making the crews rations for tomorrow.\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "(1) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   dataAvail = 0;										// Shows that data has been used.
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   dataAvail = 0;										// Shows that data has been used.
										   chef = 0;											// Ends the dialogue loop for the chef.
										   break;
									   }
								   }
							   }else if (dataAvail && (rx_buff[0]=='2')) {
								   dataAvail = 0;										// Shows that data has been used.
								   choice = 0;											// Reprints the main options for the rooms.
								   break;
							   }
						   }else if (dataAvail && (rx_buff[0]=='2')) {
							   dataAvail = 0;										// Shows that data has been used.
							   choice = 0;											// Reprints the main options for the rooms.
							   break;
						   }
					   }

				   }else if (dataAvail && (rx_buff[0]=='2') && broken) {				   // Broken Storage Box Choice
					   while (1)
					   {
						   if (!storageBox) {
							   if (hammer) {
								   uint8_t text[] = "You see a wooden storage box with its top completely off from it hinges. Next to the box you see an old stained note that reads 'This will be fix by next week'. By the looks of the note it has probably been here longer than 1 week.\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "(1) Fix Storage Box\r\n(2) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   }else{
								   uint8_t text[] = "You see a wooden storage box with its top completely off from it hinges. Next to the box you see an old stained note that reads 'This will be fix by next week'. By the looks of the note it has probably been here longer than 1 week.\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "(1) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   }
							   dataAvail = 0;										// Shows that data has been used.
							   storageBox = 1;
						   }else if (storageBox) {
							   if (dataAvail && (rx_buff[0]=='1') && hammer) {
								   uint8_t text[] = "You use the hammer and nails to properly fix the lid to the rest of the box.\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "(1) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   dataAvail = 0;										// Shows that data has been used.
								   broken = 0;											// Fixes the box.
								   hammer = 0;											// Uses the hammer.
								   chefBox = 1;											// Logic for special dialogue.
							   }else if ((dataAvail && (rx_buff[0]=='1') && !hammer) || (dataAvail && (rx_buff[0]=='2') && hammer)) {
								   dataAvail = 0;										// Shows that data has been used.
								   choice = 0;											// Reprints the main options for the rooms.
								   storageBox = 0;										// Resets dialogue for the storage box.
								   break;
							   }
						   }
						   HAL_Delay(50);											// Wait for everything to be set.
					   }

				   }else if (dataAvail && (rx_buff[0]=='2') && !broken) {				   // Fixed Storage Box Choice
					   while (1)
					   {
						   if (!dialogue) {
							   uint8_t text[] = "You look at the restored storage box. Next to the box you see an old stained note that reads 'This will be fix by next week'. By the looks of the note it has probably been here longer than 1 week.\r\n";
							   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   uint8_t options[] = "(1) Go Back";
							   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   dataAvail = 0;										// Shows that data has been used.
							   dialogue = 1;										// Goes to new dialogue.
						   }else if (dialogue) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   dataAvail = 0;										// Shows that data has been used.
								   choice = 0;											// Reprints the main options for the rooms.
								   dialogue = 0;									// Reset the dialogue counter for later use.
								   break;
							   }
						   }
					   }

				   }else if (dataAvail && (rx_buff[0]=='3')) {				   // Change Location Choice
					   while (1)
					   {
						   if (!nextRoom) {
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   uint8_t options[] = "Where do you want to go?\r\n(1) Armory\r\n(2) Go Back";
							   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   nextRoom = 1;											// Goes into new room loop.
							   dataAvail = 0;										// Shows that data has been used.
						   }else if (nextRoom) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   strcpy(room, "Armory");								// Changes the room to the armory.
								   nextRoom = 0;										// Resets the logic to be used later.
								   roomEnter = 1; 										// Sets the logic for the room enter dialogue.
								   dataAvail = 0;										// Shows that data has been used.
								   choice = 0;											// Reprints the main options for the rooms.
								   break;
							   }else if (dataAvail && (rx_buff[0]=='2')) {
								  nextRoom = 0;										// Resets the logic to be used later.
								  dataAvail = 0;										// Shows that data has been used.
								  choice = 0;											// Reprints the main options for the rooms.
								  break;
							  }
						  }
					   }
				   }else if (dataAvail && (rx_buff[0]=='i')) {
						  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
						  items( kaleidoscope,  key,  beer,  food,  hammer,  rag,  rope,  locket);		// Function to print all the items the player has.
						  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
						  dataAvail = 0;										// Shows that data has been used.
						  choice = 0;											// Reprints the main options for the rooms.
					  }
			   }

		  }else if (strcmp(room,"Crow's Nest")==0) {		  // Room: ---Crow's Nest---
			  if (roomEnter) {
				  uint8_t text[] = "\r\nOnce you reach the top of the crow's nest, you feel a small wind hit your face as you can see over the entire ship.\r\n";
				   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
				   uint8_t screen[] =	"                                              \r\n"		// ASCII art of the location of the player.
										"                        CN                    \r\n"
										"                        ==      P=player      \r\n"
										"                        ||>                   \r\n"
										"                        ||                    \r\n"
										"                      |P|| |                  \r\n"
										"               ==     ======           ==     \r\n"
										"               ||>     /||             ||>    \r\n"
										"               ==     / ||             ==     \r\n"
										"              /||     ( ||            /||     \r\n"
										"             / ||     \\ ||           / ||     \r\n"
										"            /  ||      \\||           ( ||     \r\n"
										"           /(  ||      /||           \\ ||  |>\r\n"
										"          / (  ||     / ||            \\||  |  \r\n"
										"         /  (  ||     ( ||          /-------  \r\n"
										"        /   (  ||     \\ ||         /||||||||  \r\n"
										"       /---_(  ||      \\||        /|--------  \r\n"
										"      /      \\ ||   H   ||       /|-|  CC  `  \r\n"
										"  ___/        \\||       ||      /|- |      `  \r\n"
										"     \\_        ||       ||  ()  |-  :    - |   \r\n"
										"       \\___________________________________|  \r\n"
										"        \\:|: |   ||: |     |_|:|:|    | :|:|  \r\n"
										"         \\---| K |---|   A_- |---| CH |----|  \r\n"
										"          \\|o|   |o|o|     |_|:o||    | ||:|  \r\n"
										"           \\-| /)|---|()  _- |---|    | |-/   \r\n"
										"            \\-----|: --------- :|--------/    \r\n"
										"             \\--------------------------/     \r\n"
										"~~~~~~~~~~~~~~\\--:|:--:|:--:|:--:|:--:|/~~~~~~\r\n"
										"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";
				   HAL_UART_Transmit(&huart2,screen,sizeof(screen),1500);					// Prints the map
				  roomEnter = 0;					// Proceeds to the next part of the code.
			  }else if (!choice){
				  HAL_Delay(50);											// Wait for everything to be set.
				  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				  if (diff) {
					  uint8_t options[] = "Where do you want to go?\r\n(1) Looker\r\n(2) Look At The Ship\r\n(3) Somewhere Else";
					  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
				  }else{
					  uint8_t options[] = "Where do you want to go?\r\n(1) Looker\r\n(2) Somewhere Else";
					  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
				  }
				  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				  choice = 1;					// Goes into a loop for user input.
			  }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {
					   while(1)
					   {
						   if (!looker) {
							  uint8_t text[] = "She looks off into the distance intently watching the ocean.\r\n";
							  HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   uint8_t options[] = "What do you want to do?\r\n(1) Talk\r\n(2) Go Back";
							   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  looker = 1;
							  dataAvail = 0;										// Shows that data has been used.
						   }else if (looker) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   if (lookFood) {
									   uint8_t text[] = "I can't thank you enough for the crackers. I almost felt like I was going to pass out.\r\n";
									   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

									   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
									   uint8_t options[] = "(1) Go Back";
									   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
									   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   }else if (food) {
									   HAL_UART_Transmit(&huart2,"Hey! Are you ",sizeof("Hey! Are you "),50);			// Prints text to the terminal.
									   HAL_UART_Transmit(&huart2,name,strlen(name),100);					// Prints the name of player character.
									   uint8_t text[] = "? The chief mate mentioned that someone new would be on the ship. I have to be more alert in case of some ships show up. I never had enough time to eat breakfast. You wouldn't happen to have any food on you?\r\n";
									   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

									   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
									   uint8_t options[] = "(1) Give Her Food\r\n(2) Go Back";
									   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
									   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   }else{
									   HAL_UART_Transmit(&huart2,"Hey! Are you ",sizeof("Hey! Are you "),50);			// Prints text to the terminal.
									   HAL_UART_Transmit(&huart2,name,strlen(name),100);					// Prints the name of player character.
									   uint8_t text[] = "? The chief mate mentioned that someone new would be on the ship. I have to be more alert in case of some ships show up. I never had enough time to eat breakfast. You wouldn't happen to have any food on you?\r\n";
									   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

									   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
									   uint8_t options[] = "(1) Go Back";
									   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
									   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   }
								   dataAvail = 0;										// Shows that data has been used.
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1') && (lookFood || !food)) {
										   dataAvail = 0;										// Shows that data has been used.
										   looker = 0;											// Ends the dialogue loop for the looker.
										   break;
									   }
									   if (dataAvail && (rx_buff[0]=='1') && food) {
										   uint8_t text[] = "You do! Here I can give you this for you saving me. [She hands you a key]\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "(1) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   key = 1;												// Player gets the key
										   food = 0;											// Player uses the food.
										   lookFood = 1;										// logic for special dialogue for the looker.
									   }else if (dataAvail && (rx_buff[0]=='2') && !lookFood && food) {
										   dataAvail = 0;										// Shows that data has been used.
										   looker = 0;											// Ends the dialogue loop for the looker.
										   break;
									  }
								   }
							   }else if (dataAvail && (rx_buff[0]=='2')) {
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  break;
							   }
						   }else{ __NOP();}
					   }

				   }else if (dataAvail && (rx_buff[0]=='2') && diff) {
					   while (1)
					   {
						   if (!dialogue) {
							   if (chefLoc) {
								   uint8_t text[] = "You look out into Hull of the ship seeing just how big the ship really is.\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "(1) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   }else{
								   uint8_t text[] = "You look out into Hull of the ship seeing just how big the ship really is. Out of the corner of you eye you see and shine different from the rest of the ship. Could there be something down there?\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "(1) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   }
							   dataAvail = 0;										// Shows that data has been used.
							   shine = 1;											// Logic for special option.
							   dialogue = 1;										// Goes to new dialogue.
						   }else if (dialogue) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   dataAvail = 0;										// Shows that data has been used.
								   choice = 0;											// Reprints the main options for the rooms.
								   dialogue = 0;									// Reset the dialogue counter for later use.
								   break;
							   }
						   }
					   }
				   }

				   else if ((dataAvail && (rx_buff[0]=='2') && !diff) || (dataAvail && (rx_buff[0]=='3') && diff)) {
						  while (1) {
							  if (!nextRoom) {
								  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								  uint8_t options[] = "Where do you want to go?\r\n(1) Hull\r\n(2) Go Back";
								  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								  nextRoom = 1;											// Goes into new room loop.
								  dataAvail = 0;										// Shows that data has been used.
							  }else if (nextRoom) {
								  if (dataAvail && (rx_buff[0]=='1')) {
									   strcpy(room,"Hull");									// Changes the room to the hull.
									   nextRoom = 0;										// Resets the logic to be used later.
									   roomEnter = 1; 										// Sets the logic for the room enter dialogue.
									   dataAvail = 0;										// Shows that data has been used.
									   choice = 0;											// Reprints the main options for the rooms.
									   break;

								  }else if (dataAvail && (rx_buff[0]=='2')) {
									  nextRoom = 0;										// Resets the logic to be used later.
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  break;
								  }
							  }
						  }
					  }else if (dataAvail && (rx_buff[0]=='i')) {
						  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
						  items( kaleidoscope,  key,  beer,  food,  hammer,  rag,  rope,  locket);		// Function to print all the items the player has.
						  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
						  dataAvail = 0;										// Shows that data has been used.
						  choice = 0;											// Reprints the main options for the rooms.
					  }
			   }

		  }else if (strcmp(room,"Armory")==0) {			 // Room: ---Armory---
			   if (roomEnter) {
				 uint8_t text[] = "\r\nAs you enter the armory, you are hit with the strong smell of gunpowder. You see multiple barrels and someone polishing a gun.\r\n";
				   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
				   uint8_t screen[] =	"                                              \r\n"		// ASCII art of the location of the player.
										"                        CN                    \r\n"
										"                        ==      P=player      \r\n"
										"                        ||>                   \r\n"
										"                        ||                    \r\n"
										"                      | || |                  \r\n"
										"               ==     ======           ==     \r\n"
										"               ||>     /||             ||>    \r\n"
										"               ==     / ||             ==     \r\n"
										"              /||     ( ||            /||     \r\n"
										"             / ||     \\ ||           / ||     \r\n"
										"            /  ||      \\||           ( ||     \r\n"
										"           /(  ||      /||           \\ ||  |>\r\n"
										"          / (  ||     / ||            \\||  |  \r\n"
										"         /  (  ||     ( ||          /-------  \r\n"
										"        /   (  ||     \\ ||         /||||||||  \r\n"
										"       /---_(  ||      \\||        /|--------  \r\n"
										"      /      \\ ||   H   ||       /|-|  CC  `  \r\n"
										"  ___/        \\||       ||      /|- |      `  \r\n"
										"     \\_        ||       ||  ()  |-  :    - |   \r\n"
										"       \\___________________________________|  \r\n"
										"        \\:|: |   ||: |     |_|:|:|    | :|:|  \r\n"
										"         \\---| K |---|   A_- |---| CH |----|  \r\n"
										"          \\|o|   |o|o|     |_|:o||    | ||:|  \r\n"
										"           \\-| /)|---|()P _- |---|    | |-/   \r\n"
										"            \\-----|: --------- :|--------/    \r\n"
										"             \\--------------------------/     \r\n"
										"~~~~~~~~~~~~~~\\--:|:--:|:--:|:--:|:--:|/~~~~~~\r\n"
										"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";
				   HAL_UART_Transmit(&huart2,screen,sizeof(screen),1500);					// Prints the map

				 roomEnter = 0;					// Proceeds to the next part of the code.

			   }else if (!choice){
				   HAL_Delay(50);											// Wait for everything to be set.
				   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				   uint8_t options[] = "Where do you want to go?\r\n(1) Gunner\r\n(2) Storage Box\r\n(3) Somewhere Else";
				   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
				   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				   choice = 1;					// Goes into a loop for user input.

			   }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {					// Gunner Choice
					   while (1)
					   {
						   if (!gunner) {
							   uint8_t text[] = "You watch him as a stares intensely at a grip of the flintlock before he polishes the gun and stares at it again.\r\n";
							   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   uint8_t options[] = "What do you want to do?\r\n(1) Talk\r\n(2) Go Back";
							   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   gunner = 1;											// Starts the dialogue loop for the gunner.
							   dataAvail = 0;										// Shows that data has been used.
						   }else if (gunner) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   uint8_t text[] = "What do you want? Can't you see I'm in the middle of something.\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "(1) Mention The Kaleidoscope\r\n(2) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   dataAvail = 0;										// Shows that data has been used.
								   while (1)
								   {
									   if (!diff) {
										   if (dataAvail && (rx_buff[0]=='1') && gunbeer) {
											   uint8_t text[] = "I appreciate the gift. Hope fully you can find a use for that piece of junk.\r\n";
											   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

											   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
											   uint8_t options[] = "(1) Go Back\r\n";
											   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
											   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
											   dataAvail = 0;										// Shows that data has been used.
											   while (1)
											   {
												   if (dataAvail && (rx_buff[0]=='1')) {
													   dataAvail = 0;										// Shows that data has been used.
													   gunner = 0;											// Ends the dialogue loop for the gunner.
													   break;
												   }
											   }
											   break;
										   }else if (dataAvail && (rx_buff[0]=='1') && !beer) {
											   uint8_t text[] = "Oh the captin's got you looking for that. I may or may not have something like that depends if you have something to give\r\n";
											   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

											   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
											   uint8_t options[] = "(1) Give Beer\r\n(2) Go Back";
											   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
											   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.

											   dataAvail = 0;										// Shows that data has been used.
											   while (1)
											   {
												   if (dataAvail && (rx_buff[0]=='1') && dialogue) {
													   dataAvail = 0;										// Shows that data has been used.
													   gunner = 0;											// Ends the dialogue loop for the gunner.
													   dialogue = 0;									// Reset the dialogue counter for later use.
													   break;
												   }else if (dataAvail && (rx_buff[0]=='1')) {
													   uint8_t text[] = "Sorry if you don't have anything then there is nothing to give.\r\n";
													   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

													   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
													   uint8_t options[] = "(1) Go Back";
													   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
													   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
													   dataAvail = 0;										// Shows that data has been used.
													   dialogue = 1;										// Goes to new dialogue.
												   }else if (dataAvail && (rx_buff[0]=='2') && !dialogue) {
													   dataAvail = 0;										// Shows that data has been used.
													   gunner = 0;											// Ends the dialogue loop for the gunner.
													   break;
												   }
												   HAL_Delay(50);											// Wait for everything to be set.
											   }
											   break;
										   }else if (dataAvail && (rx_buff[0]=='1') && (beer==1)) {
											   uint8_t text[] = "Oh the captin's got you looking for that. I may or may not have something like that depends if you have something to give\r\n";
											   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

											   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
											   uint8_t options[] = "(1) Give Beer\r\n(2) Go Back";
											   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
											   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
											   dataAvail = 0;										// Shows that data has been used.
											   while (1)
											   {
												   if (dataAvail && (rx_buff[0]=='1') && dialogue) {
													   dataAvail = 0;										// Shows that data has been used.
													   gunner = 0;											// Ends the dialogue loop for the gunner.
													   dialogue = 0;									// Reset the dialogue counter for later use.
													   break;
												   }else if (dataAvail && (rx_buff[0]=='1')) {
													   uint8_t text[] = "Now you've got my attention. Here you can keep it. [He throws you the kaleidoscope]\r\n";
													   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

													   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
													   uint8_t options[] = "(1) Go Back";
													   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
													   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
													   dataAvail = 0;										// Shows that data has been used.
													   dialogue = 1;										// Goes to new dialogue.
													   kaleidoscope += 1;									// 1 piece of the kaleidoscope has been found.
													   beer = 0;											// Player uses beer.
													   gunbeer = 1;											// Logic for special dialogue for gunner.
												   }else if (dataAvail && (rx_buff[0]=='2') && !dialogue) {
													   dataAvail = 0;										// Shows that data has been used.
													   gunner = 0;											// Ends the dialogue loop for the gunner.
													   break;
												   }
												   HAL_Delay(50);											// Wait for everything to be set.
											   }
											   break;
										   }else if (dataAvail && (rx_buff[0]=='2')) {
											   dataAvail = 0;										// Shows that data has been used.
											   gunner = 0;											// Ends the dialogue loop for the gunner.
											   break;
										   }
									   }else if (diff) {
										   if (dataAvail && (rx_buff[0]=='1') && !beer) {
											   uint8_t text[] = "I may or may not have something like that depends if you have something to give\r\n";
											   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

											   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
											   uint8_t options[] = "(1) Give Beer\r\n(2) Go Back";
											   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
											   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
											   dataAvail = 0;										// Shows that data has been used.
											   while (1)
											   {
												   if (dataAvail && (rx_buff[0]=='1') && dialogue) {
													   dataAvail = 0;										// Shows that data has been used.
													   gunner = 0;											// Ends the dialogue loop for the gunner.
													   dialogue = 0;									// Reset the dialogue counter for later use.
													   break;
												   }else if (dataAvail && (rx_buff[0]=='1')) {
													   uint8_t text[] = "Sorry if you don't have anything then there is nothing to give.\r\n";
													   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

													   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
													   uint8_t options[] = "(1) Go Back";
													   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
													   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
													   dataAvail = 0;										// Shows that data has been used.
													   dialogue = 1;										// Goes to new dialogue.
												   }else if (dataAvail && (rx_buff[0]=='2') && !dialogue) {
													   dataAvail = 0;										// Shows that data has been used.
													   gunner = 0;											// Ends the dialogue loop for the gunner.
													   break;
												   }
												   HAL_Delay(50);											// Wait for everything to be set.
											   }
											   break;
										   }else if (dataAvail && (rx_buff[0]=='1') && (beer==1)) {
											   uint8_t text[] = "I may or may not have something like that depends if you have something to give\r\n";
											   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

											   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
											   uint8_t options[] = "(1) Give Beer\r\n(2) Go Back";
											   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
											   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
											   dataAvail = 0;										// Shows that data has been used.
											   while (1)
											   {
												   if (dataAvail && (rx_buff[0]=='1') && dialogue) {
													   dataAvail = 0;										// Shows that data has been used.
													   gunner = 0;											// Ends the dialogue loop for the gunner.
													   dialogue = 0;									// Reset the dialogue counter for later use.
													   break;
												   }else if (dataAvail && (rx_buff[0]=='1')) {
													   uint8_t text[] = "Close you got the right idea, but I need a little bit more. I did go through a lot of trouble to get this thing.\r\n";
													   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

													   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
													   uint8_t options[] = "(1) Go Back";
													   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
													   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
													   dataAvail = 0;										// Shows that data has been used.
													   dialogue = 1;										// Goes to new dialogue.
												   }else if (dataAvail && (rx_buff[0]=='2') && !dialogue) {
													   dataAvail = 0;										// Shows that data has been used.
													   gunner = 0;											// Ends the dialogue loop for the gunner.
													   break;
												   }
												   HAL_Delay(50);											// Wait for everything to be set.
											   }
											   break;
										   }else if (dataAvail && (rx_buff[0]=='1') && (beer==2)) {
											   uint8_t text[] = "I may or may not have something like that depends if you have something to give\r\n";
											   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

											   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
											   uint8_t options[] = "(1) Give Beer\r\n(2) Go Back";
											   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
											   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
											   dataAvail = 0;										// Shows that data has been used.
											   while (1)
											   {
												   if (dataAvail && (rx_buff[0]=='1') && dialogue) {
													   dataAvail = 0;										// Shows that data has been used.
													   gunner = 0;											// Ends the dialogue loop for the gunner.
													   dialogue = 0;									// Reset the dialogue counter for later use.
													   break;
												   }else if (dataAvail && (rx_buff[0]=='1')) {
													   uint8_t text[] = "Now you've got my attention. Here you can keep it. [He throws you the kaleidoscope]\r\n(1) Go Back\r\n";
													   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

													   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
													   uint8_t options[] = "(1) Go Back";
													   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
													   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
													   dataAvail = 0;										// Shows that data has been used.
													   dialogue = 1;										// Goes to new dialogue.
													   kaleidoscope += 1;									// 1 piece of the kaleidoscope has been found.
													   beer = 0;											// Player uses beer.
													   gunbeer = 1;											// Logic special dialogue for the gunner.
												   }else if (dataAvail && (rx_buff[0]=='2') && !dialogue) {
													   dataAvail = 0;										// Shows that data has been used.
													   gunner = 0;											// Ends the dialogue loop for the gunner.
													   break;
												   }
												   HAL_Delay(50);											// Wait for everything to be set.
											   }
											   break;
										   }else if (dataAvail && (rx_buff[0]=='2')) {
											   dataAvail = 0;										// Shows that data has been used.
											   gunner = 0;											// Ends the dialogue loop for the gunner.
											   break;
										   }
									   }
									   HAL_Delay(50);											// Wait for everything to be set.
								   }
							   }else if (dataAvail && (rx_buff[0]=='2')) {
								   dataAvail = 0;										// Shows that data has been used.
								   choice = 0;											// Reprints the main options for the rooms.
								   break;
							   }
						   }
					   }

				   }else if (dataAvail && (rx_buff[0]=='2')) {				// Storage Box choice
					   while (1)
					   {
						   if (!storageBox) {
							   uint8_t text[] = "You see and old worn down barrel with the lid placed leaning beside the wall.\r\n";
							   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   uint8_t options[] = "What do you want to do?\r\n(1) Look Inside\r\n(2) Go Back";
							   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   storageBox = 1;										// Goes into storage box loop.
							   dataAvail = 0;										// Shows that data has been used.
						   }else if (storageBox) {
							   if (dataAvail && (rx_buff[0]=='1') && (!rope ^ chiRope) && (!rag ^ worRag)) {
								   uint8_t text[] = "You look in side the barrel and see a coil of rope that doesn't look like it has been used before and a couple of clean rags thrown in. \r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "What do you want to do?\r\n(1) Get The Rope\r\n(2) Get The Clean Rags\r\n(3) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   dataAvail = 0;										// Shows that data has been used.
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   uint8_t text[] = "You grab take the rope from the barrel and hold it around your shoulder.\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "What do you want to do?\r\n(1) Get The Clean Rags\r\n(2) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   rope = 1;											// User gets rope.
										   while (1)
										   {
											   if (dataAvail && (rx_buff[0]=='1') && rag) {
												   dataAvail = 0;										// Shows that data has been used.
												   choice = 0;											// Reprints the main options for the rooms.
												   storageBox = 0;										// Ends the storage box loop.
												   break;
											   }else if (dataAvail && (rx_buff[0]=='1')) {
												   uint8_t text[] = "You take clean rags and stuff them in your pocket.\r\n";
												   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

												   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
												   uint8_t options[] = "What do you want to do?\r\n(1) Go Back";
												   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
												   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
												   dataAvail = 0;										// Shows that data has been used.
												   rag = 1;												// User gets rags.
												   HAL_Delay(50);											// Wait for everything to be set.
											   }else if (dataAvail && (rx_buff[0]=='2') && !rag) {
												   dataAvail = 0;										// Shows that data has been used.
												   choice = 0;											// Reprints the main options for the rooms.
												   storageBox = 0;										// Ends the storage box loop.
												   break;
											   }
										   }
										   break;
									   }else if (dataAvail && (rx_buff[0]=='2')) {
										   uint8_t text[] = "You take clean rags and stuff them in your pocket.\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "What do you want to do?\r\n(1) Get The Rope\r\n(2) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   rag = 1;												// User gets rags.
										   while (1)
										   {
											   if (dataAvail && (rx_buff[0]=='1') && rope) {
												   dataAvail = 0;										// Shows that data has been used.
												   choice = 0;											// Reprints the main options for the rooms.
												   storageBox = 0;										// Ends the storage box loop.
												   break;
											   }else if (dataAvail && (rx_buff[0]=='1')) {
												   uint8_t text[] = "You grab take the rope from the barrel and hold it around your shoulder.\r\n";
												   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

												   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
												   uint8_t options[] = "What do you want to do?\r\n(1) Go Back";
												   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
												   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
												   dataAvail = 0;										// Shows that data has been used.
												   rope = 1;											// User gets rope.
											   }else if (dataAvail && (rx_buff[0]=='2') && !rope) {
												   dataAvail = 0;										// Shows that data has been used.
												   choice = 0;											// Reprints the main options for the rooms.
												   storageBox = 0;										// Ends the storage box loop.
												   break;
											   }
										   }
										   break;
									   }else if (dataAvail && (rx_buff[0]=='3')) {
										   dataAvail = 0;										// Shows that data has been used.
										   choice = 0;											// Reprints the main options for the rooms.
										   storageBox = 0;										// Ends the storage box loop.
										   break;
									   }
								   }
								   break;
							   }else if (dataAvail && (rx_buff[0]=='1') && (!rag ^ worRag) && (rope ^ chiRope) ) {
								   uint8_t text[] = "You look inside the barrel and see a couple of clean rags thrown in.\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "What do you want to do?\r\n(1) Get The Clean Rags\r\n(2) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   dataAvail = 0;										// Shows that data has been used.
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   uint8_t text[] = "You take clean rags and stuff them in you pocket.\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "What do you want to do?\r\n(1) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   rag = 1;												// User gets rags.
										   while (1)
										   {
											   if (dataAvail && (rx_buff[0]=='1')) {
												   dataAvail = 0;										// Shows that data has been used.
												   choice = 0;											// Reprints the main options for the rooms.
												   storageBox = 0;										// Ends the storage box loop.
												   break;
											   }
										   }
										   break;
									   }else if (dataAvail && (rx_buff[0]=='2')) {
										   dataAvail = 0;										// Shows that data has been used.
										   choice = 0;											// Reprints the main options for the rooms.
										   storageBox = 0;										// Ends the storage box loop.
										   break;
									   }
								   }
								   break;
							   }else if (dataAvail && (rx_buff[0]=='1') && (!rope ^ chiRope) && (rag ^ worRag)) {
								   uint8_t text[] = "You look in side the barrel and see a coil of rope that doesn't look like it has been used before.\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "What do you want to do?\r\n(1) Get The Rope\r\n(2) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   dataAvail = 0;										// Shows that data has been used.
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   uint8_t text[] = "You grab take the rope from the barrel and hold it around your shoulder.\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "What do you want to do?\r\n(1) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   rope = 1;											// User gets rope.
										   while (1)
										   {
											   if (dataAvail && (rx_buff[0]=='1')) {
												   dataAvail = 0;										// Shows that data has been used.
												   choice = 0;											// Reprints the main options for the rooms.
												   storageBox = 0;										// Ends the storage box loop.
												   break;
											   }
										   }
										   break;
									   }else if (dataAvail && (rx_buff[0]=='2')) {
										   dataAvail = 0;										// Shows that data has been used.
										   choice = 0;											// Reprints the main options for the rooms.
										   storageBox = 0;										// Ends the storage box loop.
										   break;
									   }
								   }
								   break;
							   }else if (dataAvail && (rx_buff[0]=='1') && (rope ^ chiRope) && (rag ^ worRag) ) {
								   uint8_t text[] = "You look inside the barrel and see the nothing.\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "What do you want to do?\r\n(1) Go Back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   dataAvail = 0;										// Shows that data has been used.
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   dataAvail = 0;										// Shows that data has been used.
										   choice = 0;											// Reprints the main options for the rooms.
										   storageBox = 0;										// Ends the storage box loop.
										   break;
									   }
								   }
								   break;
							   }else if (dataAvail && (rx_buff[0]=='2')) {
								   dataAvail = 0;										// Shows that data has been used.
								   choice = 0;											// Reprints the main options for the rooms.
								   storageBox = 0;										// Ends the storage box loop.
								   break;
							   }

						   }

					   }

				   }else if (dataAvail && (rx_buff[0]=='3')) {				// Change Room choice
						  while (1) {
							  if (!nextRoom) {
								  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								  uint8_t options[] = "Where do you want to go?\r\n(1) Hull\r\n(2) Kitchen\r\n(3) Cargo Hold\r\n(4) Go Back";
								  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								  nextRoom = 1;											// Goes into new room loop.
								  dataAvail = 0;										// Shows that data has been used.
							  }else if (nextRoom) {
								  if (dataAvail && (rx_buff[0]=='1')) {
									  strcpy(room,"Hull");									// Changes the room to the hull.
									  nextRoom = 0;										// Resets the logic to be used later.
									  roomEnter = 1; 										// Sets the logic for the room enter dialogue.
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  break;
								  }else if (dataAvail && (rx_buff[0]=='2')) {
									  strcpy(room, "Kitchen");								// Changes the room to the kitchen.
									  nextRoom = 0;										// Resets the logic to be used later.
									  roomEnter = 1; 										// Sets the logic for the room enter dialogue.
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  break;
								  }else if (dataAvail && (rx_buff[0]=='3')) {
									  strcpy(room, "Cargo Hold");							// Changes the room to the cargo hold.
									  nextRoom = 0;										// Resets the logic to be used later.
									  roomEnter = 1; 										// Sets the logic for the room enter dialogue.
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  break;
								  }else if (dataAvail && (rx_buff[0]=='4')) {
									  nextRoom = 0;										// Resets the logic to be used later.
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  break;
								  }
							  }
						  }
					  }else if (dataAvail && (rx_buff[0]=='i')) {
						  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
						  items( kaleidoscope,  key,  beer,  food,  hammer,  rag,  rope,  locket);		// Function to print all the items the player has.
						  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
						  dataAvail = 0;										// Shows that data has been used.
						  choice = 0;											// Reprints the main options for the rooms.
					  }
			   }

		  }else if (strcmp(room,"Cargo Hold")==0) {		  // Room: ---Cargo Hold---
			   if (roomEnter) {
				 uint8_t text[] = "\r\nYou enter the dimly lit room cargo hold that has storage boxes and barrels scattered throughout the floor, with a faint smell of dust.\r\n";
				   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
				   uint8_t screen[] =	"                                              \r\n"		// ASCII art of the location of the player.
										"                        CN                    \r\n"
										"                        ==      P=player      \r\n"
										"                        ||>                   \r\n"
										"                        ||                    \r\n"
										"                      | || |                  \r\n"
										"               ==     ======           ==     \r\n"
										"               ||>     /||             ||>    \r\n"
										"               ==     / ||             ==     \r\n"
										"              /||     ( ||            /||     \r\n"
										"             / ||     \\ ||           / ||     \r\n"
										"            /  ||      \\||           ( ||     \r\n"
										"           /(  ||      /||           \\ ||  |>\r\n"
										"          / (  ||     / ||            \\||  |  \r\n"
										"         /  (  ||     ( ||          /-------  \r\n"
										"        /   (  ||     \\ ||         /||||||||  \r\n"
										"       /---_(  ||      \\||        /|--------  \r\n"
										"      /      \\ ||   H   ||       /|-|  CC  `  \r\n"
										"  ___/        \\||       ||      /|- |      `  \r\n"
										"     \\_        ||       ||  ()  |-  :    - |   \r\n"
										"       \\___________________________________|  \r\n"
										"        \\:|: |   ||: |     |_|:|:|    | :|:|  \r\n"
										"         \\---| K |---|   A_- |---| CH |----|  \r\n"
										"          \\|o|   |o|o|     |_|:o||    | ||:|  \r\n"
										"           \\-| /)|---|()  _- |---| P  | |-/   \r\n"
										"            \\-----|: --------- :|--------/    \r\n"
										"             \\--------------------------/     \r\n"
										"~~~~~~~~~~~~~~\\--:|:--:|:--:|:--:|:--:|/~~~~~~\r\n"
										"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";
				   HAL_UART_Transmit(&huart2,screen,sizeof(screen),1500);					// Prints the map
				 roomEnter = 0;					// Proceeds to the next part of the code.
			   }else if (!choice){
				   HAL_Delay(50);											// Wait for everything to be set.
				   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				   uint8_t options[] = "Where do you want to go?\r\n(1) Chief Mate\r\n(2) Locked Door\r\n(3) Somewhere Else";
				   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
				   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				   choice = 1;					// Goes into a loop for user input.
			   }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {				   // Chief Mate Choice
					   while (1)
					   {
						   if (!chiefMate) {
							   uint8_t text[] = "He silently stares at a piece of paper before writing something down. Then he goes through a storage box.\r\n";
							   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   uint8_t options[] = "What do you want to do?\r\n(1) Talk\r\n(2) Go Back";
							   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							   chiefMate = 1;										// Starts the dialogue loop for the chief mate.
							   dataAvail = 0;										// Shows that data has been used.
						   }else if (chiefMate) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   if (rope) {
									   HAL_UART_Transmit(&huart2,"mhm? Oh ",sizeof("mhm? oh "),50);
									   HAL_UART_Transmit(&huart2,name,strlen(name),100);					// Prints the name of player character.
									   uint8_t text[] = " right? I'm just checking the all that we have on the ship.\r\n";
									   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

									   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
									   uint8_t options[] = "(1) Talk\r\n(2) Give Rope\r\n(3) Go Back";
									   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
									   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   }else{
									   HAL_UART_Transmit(&huart2,"mhm? Oh ",sizeof("mhm? oh "),50);
									   HAL_UART_Transmit(&huart2,name,strlen(name),100);					// Prints the name of player character.
									   uint8_t text[] = " right? I'm just checking the all that we have on the ship.\r\n";
									   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

									   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
									   uint8_t options[] = "(1) Talk\r\n(2) Go Back";
									   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
									   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   }
								   dataAvail = 0;										// Shows that data has been used.
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   dataAvail = 0;										// Shows that data has been used.
										   dialogue += 1;										// Goes to new dialogue.
									   }else if (dataAvail && (rx_buff[0]=='2') && rope && (dialogue<1)){
										   uint8_t text[] = "Good job finding that rope. It makes this giant mess a lot easier to manage. It must have took a lot to find that have some of these. [He hands you a half eaten pack of crackers]\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "(1) Mention The Kaleidoscope\r\n(2) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   food = 1;											// Player get food.
										   rope = 0;											// Player uses the rope.
										   chiRope = 1;											// Logic for special dialogue.
										   dialogue = 2;										// Goes to new dialogue.
										   HAL_Delay(50);											// Wait for everything to be set.
									   }else if ((dataAvail && (rx_buff[0]=='2') && (!chiRope || chiRope)) || (dataAvail && (rx_buff[0]=='3') && rope)){
											  dataAvail = 0;										// Shows that data has been used.
											  chiefMate = 0;									// Ends the dialogue loop for the chief mate.
											  dialogue = 0;									// Reset the dialogue counter for later use.
											  break;
									   }
									   if ((dialogue==1) && !chiRope) {
										   uint8_t text[] = "There are so many things unaccounted for. How are we low on rope of all things.\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "(1) Mention The Kaleidoscope\r\n(2) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   dialogue += 1;										// Goes to new dialogue.
									   }else if ((dialogue==1) & chiRope) {
										   uint8_t text[] = "Good job finding that rope. It makes this giant mess a lot easier to manage.\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "(1) Mention The Kaleidoscope\r\n(2) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   dialogue += 1;										// Goes to new dialogue.
									   }else if (dialogue==3){
										   uint8_t text[] = "I'm not quite sure what it is. It does make some colorful light when I shine a candle on it. Although there is as weird circular shadow from the lens.\r\n";
										   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   uint8_t options[] = "(1) Talk More\r\n(2) Go Back";
										   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
										   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
										   dataAvail = 0;										// Shows that data has been used.
										   dialogue += 1;										// Goes to new dialogue.
									   }else if (dialogue>3) {
										   dialogue = 0;									// Reset the dialogue counter for later use.
									   }
								   }
							   }else if (dataAvail && (rx_buff[0]=='2')) {
								   dataAvail = 0;										// Shows that data has been used.
								   choice = 0;											// Reprints the main options for the rooms.
								   break;
							   }
						   }
					   }
				   }else if (dataAvail && (rx_buff[0]=='2')) {				   // Storage Room Choice
					   strcpy(room, "Locked Door");
					   roomEnter = 1; 										// Sets the logic for the room enter dialogue.
					   dataAvail = 0;										// Shows that data has been used.
					   choice = 0;											// Reprints the main options for the rooms.

				   }else if (dataAvail && (rx_buff[0]=='3')) {				   // Change location Choice
					   while (1)
					   {
						  if (!nextRoom) {
							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  uint8_t options[] = "Where do you want to go?\r\n(1) Armory\r\n(2) Go Back";
							  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  nextRoom = 1;											// Goes into new room loop.
							  dataAvail = 0;										// Shows that data has been used.
						  }else if (nextRoom) {
							  if (dataAvail && (rx_buff[0]=='1')) {
								  strcpy(room, "Armory");								// Changes the room to the armory.
								  nextRoom = 0;										// Resets the logic to be used later.
								  roomEnter = 1; 										// Sets the logic for the room enter dialogue.
								  dataAvail = 0;										// Shows that data has been used.
								  choice = 0;											// Reprints the main options for the rooms.
								  break;
							  }else if (dataAvail && (rx_buff[0]=='2')) {
								  nextRoom = 0;										// Resets the logic to be used later.
								  dataAvail = 0;										// Shows that data has been used.
								  choice = 0;											// Reprints the main options for the rooms.
								  break;
							  }
						  }
					   }
				   }else if (dataAvail && (rx_buff[0]=='i')) {
						  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
						  items( kaleidoscope,  key,  beer,  food,  hammer,  rag,  rope,  locket);		// Function to print all the items the player has.
						  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
						  dataAvail = 0;										// Shows that data has been used.
						  choice = 0;											// Reprints the main options for the rooms.
					  }
			   }

		  }else if (strcmp(room,"Locked Door")==0) {		  // Room: ---Locked door---
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou stand infront of a metal cage door with boxes behind the door.\r\n";
				   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.
				   uint8_t screen[] =	"                                              \r\n"		// ASCII art of the location of the player.
										"                        CN                    \r\n"
										"                        ==      P=player      \r\n"
										"                        ||>                   \r\n"
										"                        ||                    \r\n"
										"                      | || |                  \r\n"
										"               ==     ======           ==     \r\n"
										"               ||>     /||             ||>    \r\n"
										"               ==     / ||             ==     \r\n"
										"              /||     ( ||            /||     \r\n"
										"             / ||     \\ ||           / ||     \r\n"
										"            /  ||      \\||           ( ||     \r\n"
										"           /(  ||      /||           \\ ||  |>\r\n"
										"          / (  ||     / ||            \\||  |  \r\n"
										"         /  (  ||     ( ||          /-------  \r\n"
										"        /   (  ||     \\ ||         /||||||||  \r\n"
										"       /---_(  ||      \\||        /|--------  \r\n"
										"      /      \\ ||   H   ||       /|-|  CC  `  \r\n"
										"  ___/        \\||       ||      /|- |      `  \r\n"
										"     \\_        ||       ||  ()  |-  :    - |   \r\n"
										"       \\___________________________________|  \r\n"
										"        \\:|: |   ||: |     |_|:|:|    | :|:|  \r\n"
										"         \\---| K |---|   A_- |---| CH |----|  \r\n"
										"          \\|o|   |o|o|     |_|:o||    | ||:|  \r\n"
										"           \\-| /)|---|()  _- |---|    |P|-/   \r\n"
										"            \\-----|: --------- :|--------/    \r\n"
										"             \\--------------------------/     \r\n"
										"~~~~~~~~~~~~~~\\--:|:--:|:--:|:--:|:--:|/~~~~~~\r\n"
										"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";
				   HAL_UART_Transmit(&huart2,screen,sizeof(screen),1500);					// Prints the map
				  roomEnter = 0;					// Proceeds to the next part of the code.

			  }else if (!choice){
				  HAL_Delay(50);											// Wait for everything to be set.
				  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				  uint8_t options[] = "Where do you want to go?\r\n(1) Enter The Storage Room\r\n(2) Go back";
				  HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
				  HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
				  dataAvail = 0;										// Shows that data has been used.
				  choice = 1;					// Goes into a loop for user input.

			  }else if (choice) {
				  if (dataAvail && (rx_buff[0]=='1') && !key) {
					  uint8_t text[] = "You are unable to get through the door it seems to be locked.\r\n";
					   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

					   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					   uint8_t options[] = "(1) Go back";
					   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
					   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  dataAvail = 0;										// Shows that data has been used.
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')) {
							  dataAvail = 0;										// Shows that data has been used.
							  choice = 0;											// Reprints the main options for the rooms.
							  break;
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='1') && key) {
					  uint8_t text[] = "You use the key to unlock the door. You see a bunch of wooden boxes all round the room.\r\n";
					   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

					   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					   uint8_t options[] = "(1) Look Through The wooden Boxes\r\n(2) Go back";
					   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
					   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
					  dataAvail = 0;										// Shows that data has been used.
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')){
							  if (chiHam) {
								  uint8_t text[] = "After sorting through the boxes, you can't see to find anything useful.\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "(1) Go back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  }else{
								  uint8_t text[] = "After sorting through the boxes, you find a lightly used hammer along with some nails.\r\n";
								   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
								   uint8_t options[] = "(1) Grab The Hammer And Nails\r\n(2) Go back";
								   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
								   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
							  }
							  dataAvail = 0;										// Shows that data has been used.
							  while (1)
							  {
								  if (dataAvail && (rx_buff[0]=='1') && chiHam) {
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  break;
								  }else if (dataAvail && (rx_buff[0]=='1')) {
									   uint8_t text[] = "You put the hammer between your belt and carefully place the nail into you pocket.\r\n";
									   HAL_UART_Transmit(&huart2,text,sizeof(text),50);			// Prints text to the terminal.

									   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
									   uint8_t options[] = "(1) Go back";
									   HAL_UART_Transmit(&huart2,options,sizeof(options),50);			// Prints the different options.
									   HAL_UART_Transmit(&huart2,line,sizeof(line),50);			// Creates spacing for better visibility.
									  dataAvail = 0;										// Shows that data has been used.
									  hammer = 1;											// Player gets the hammer.
									  chiHam = 1;											// Logic for special dialogue.
								  }else if (dataAvail && (rx_buff[0]=='2') && !hammer) {
									  dataAvail = 0;										// Shows that data has been used.
									  choice = 0;											// Reprints the main options for the rooms.
									  break;
								  }
								  HAL_Delay(50);											// Wait for everything to be set.
							  }
							  break;
						  }else if (dataAvail && (rx_buff[0]=='2')) {
							  dataAvail = 0;										// Shows that data has been used.
							  choice = 0;											// Reprints the main options for the rooms.
							  break;
						  }
					  }
				  }else	if (dataAvail && (rx_buff[0]=='2')) {
					  strcpy(room,"Cargo Hold");
					  roomEnter = 1; 										// Sets the logic for the room enter dialogue.
					  dataAvail = 0;										// Shows that data has been used.
					  choice = 0;											// Reprints the main options for the rooms.
				  }else if (dataAvail && (rx_buff[0]=='i')) {
					  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
					  items( kaleidoscope,  key,  beer,  food,  hammer,  rag,  rope,  locket);		// Function to print all the items the player has.
					  HAL_UART_Transmit(&huart2,"\r\n",sizeof("\r\n"), 50);			// Creates a new line for better visibility.
					  dataAvail = 0;										// Shows that data has been used.
					  choice = 0;											// Reprints the main options for the rooms.
				  }
			  }
		  }else{ __NOP();}								// Nothing happens.
	  }
	  HAL_UART_Receive_IT(&huart2, rx_buff, 1);
  } /* Game engine END WHILE */

  while(1);
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 10;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel6_IRQn);
  /* DMA1_Channel7_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel7_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel7_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
/* If UART2 has received a byte from the user terminal this function
 * will be called.
 * dataAvail is used to indicate that the rx_buff has received new data and can be checked.
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  HAL_UART_Receive_IT(&huart2, rx_buff, 1); //You need to toggle a breakpoint on this line!
  dataAvail=1;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
