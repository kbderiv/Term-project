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
volatile uint8_t dataAvail = 0;
char rx_buff[1];
//"Welcome to my game! Please enter play (p) or exit (x) \r\n"
uint8_t Welcome[] = "Welcome to Enduring Tatters! press (x) to start. \r\n"; //Data to send
uint8_t difficulty[] = "Pick which difficulty you want.\r\n(1) Easy\r\n(2) Hard \r\n"; //Data to send
uint8_t text[] = ""; //Data to send
uint8_t options[] = "";
uint8_t screen[] = "";
uint8_t line[] = "\r\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";




int getBeer(uint8_t *beer, uint8_t *difficulty) {
	asm volatile (
		    "CMP %[r1], 1\n\t"
			"BLT easy\n\t"

			"Hard:\n\t"
			"CMP %[r0], 0\n\t"
			"ITE EQ\n\t"
			"MOVEQ %[r0], 1\n\t"
			"MOVNE %[r0], 2\n\t"
			"B end\n\t"

			"easy:\n\t"
			"CMP %[r0], 0\n\t"
			"IT EQ\n\t"
			"MOVEQ %[r0], 1\n\t"

			"end:\n\t"

		    :[r0] "+r" (*beer), [r1] "=r" (*difficulty)
	);
	return 0;
}


int changeRoom(char attempt[], char answer[], uint8_t *endCode) {
	for (int j=0;j<5;j++) {
		int asciians = (int)answer[j];
		int asciiatt = (int)attempt[j];
		/*
		asm volatile (
				"CMP %[r1], %[r2]\n\t"
				"BEQ final\n\t"

				"MOV %[r0], #0\n\t"

				"final:\n\t"
				:[r0] "+r" (*endCode), [r1] "+r" (asciiatt), [r2] "+r" (asciians)
		);
*/

		if (attempt[j]!=answer[j]){
			*endCode = 0;
		}

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

  /* USER CODE END 2 */

  /* Game engine runs in infinite loop until finished. */
  /* Game engine BEGIN WHILE */
  uint8_t init = 0;
  uint8_t inGame = 0;
  uint8_t askedGameQuestion = 0;


  uint8_t diff = 0;
  uint8_t begin = 0;
  uint8_t exit = 0;
  uint8_t death = 0;
  uint8_t inDeath = 0;
  uint8_t choice = 0;
  uint8_t playerAlive = 0;

  uint8_t roomEnter = 1;
  uint8_t nextRoom = 0;

  uint8_t endCode = 1;
  char room[] = "Hull";

  // Objects
  uint8_t kaleidoscope = 0;
  uint8_t key = 0;
  uint8_t beer = 0;
  uint8_t food = 0;
  uint8_t hammer = 0;
  uint8_t rag = 0;
  uint8_t rope = 0;
  uint8_t locket = 0;



  // Characters
  uint8_t captain = 0;
  uint8_t worriedCrewmember = 0;
  uint8_t looker = 0;
  uint8_t chef = 0;
  uint8_t gunner = 0;
  uint8_t chiefMate = 0;


  // Conditional dialogue
  uint8_t capKal = 0;
  uint8_t worRag = 0;

  uint8_t gunbeer = 0;
  uint8_t chefBox = 0;
  uint8_t lookFood = 0;
  uint8_t chiRope = 0;
  uint8_t chiHam = 0;
  uint8_t dialogue = 0;
  uint8_t storageBox = 0;
  uint8_t chest = 0;
  uint8_t shine = 0;
  uint8_t broken = 1;


  char attempt[5];
  char answer[5] = "0498";

  uint8_t i = 0;


  while(1)
  {
/*
	  if (!init) {





		  uint8_t screen[] =  "                                \r\n"
		  						"                                \r\n"
		  						"                        CN                    \r\n"
		  						"                        ==      P=player      \r\n"
		  						"                        ||>                   \r\n"
		  						"                        ||                    \r\n"
		  						"                      |p|| |                  \r\n"
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
		  						"     \\_        ||   p   ||  ()  |-  :  p - |   \r\n"
		  						"       \\___________________________________|  \r\n"
		  						"        \\:|: |   ||: |     |_|:|:|    | :|:|  \r\n"
		  						"         \\---| K |---|   A_- |---| CH |----|  \r\n"
		  						"          \\|o|   |o|o|     |_|:o||    | ||:|  \r\n"
		  						"           \\-|p/)|---|()p _- |---|  p |p|-/   \r\n"
		  						"            \\-----|: --------- :|--------/    \r\n"
		  						"             \\--------------------------/     \r\n"
		  						"~~~~~~~~~~~~~~\\--:|:--:|:--:|:--:|:--:|/~~~~~~\r\n"
		  						"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\r\n";

		  	  HAL_UART_Transmit_IT(&huart2,screen,sizeof(screen));
		  	  init = 1;
	  }*/




	  // If the game has not initialized, present player with welcome screen
	  if (!begin){
		  if(!init){
				uint8_t screen[] = "                                        \r\n"
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
									"     '~'                                \r\n";
			  HAL_UART_Transmit_IT(&huart2,screen,sizeof(screen));
			  init = 1;
		  }else if(init && !inGame){ // If game initialized but game not started
			  if(dataAvail && (rx_buff[0]=='x')) {
				  inGame=1; // enter game
				  dataAvail = 0; // always clear dataAvail flag AFTER it is set by HAL_UART_RxCpltCallback
			  }
		  }else if (inGame && !askedGameQuestion){
			  HAL_UART_Transmit(&huart2,difficulty,sizeof(difficulty),10);
			  askedGameQuestion = 1;
		  }else if (inGame){
			  if (dataAvail && (rx_buff[0]=='1')){
				  uint8_t text[] = "Taking the easy way out. \r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  playerAlive = 1;
				  inGame = 0;
				  dataAvail = 0;
				  begin = 1;
				  HAL_Delay(100);

			  }else if (dataAvail && (rx_buff[0]=='2')){
				  uint8_t text[] = "Enjoy the challenge. \r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  playerAlive = 1;
				  inGame = 0;
				  dataAvail = 0;
				  begin = 1;
				  diff = 1;
				  HAL_Delay(100);
			  }
		  }else{ __NOP();}
	  }



	  while(playerAlive) // The main game loop
	  {
		  if (strcmp(room,"Captain's Cabin")==0) {		  // Room: ---Captain's Cabin---
			  if (roomEnter) {
				uint8_t text[] = "\r\nYou are in the Captain's Cabin\r\n";
				HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				roomEnter = 0;
			  }else if (!choice){
				   HAL_Delay(100);
				   uint8_t text[] = "Where do you want to go?\r\n(1) Captain\r\n(2) Map\r\n(3) Chest\r\n(4) Somewhere Else\r\n";
				   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				   choice = 1;

			   }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {
					   while (1)
					   {
						   if (!captain) {
							   uint8_t text[] = "You see the captain staring at the chest trying to find any clues on how to open the chest.\r\nWhat do you want to do?\r\n(1) Talk\r\n(2) Go Back\r\n";
							   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							   dataAvail = 0;
							   captain = 1;
						   }else if (captain) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   dataAvail = 0;
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   dataAvail = 0;
										   dialogue += 1;
									   }else if (dataAvail && (rx_buff[0]=='2')) {
											  dataAvail = 0;
											  captain = 0;
											  dialogue = 0;
											  break;
									   }
									   if (dialogue==0) {
										   uint8_t text[] = "Ah it seems like there is some sort of message on the chest, but I can't seem to understand what it is supposed to be.\r\nWhat do you want to do?\r\n(1) Ask About The Map\r\n(2) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   dialogue += 1;
									   }else if (dialogue==2) {
										   uint8_t text[] = "We were able to find the treasure pretty easy, but I don't quite understand what the four arrows are supposed to mean.\r\nWhat do you want to do?\r\n(1) Ask About The Kaleidoscope\r\n(2) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   dialogue += 1;
									   }else if ((dialogue==4) && (!capKal)) {
										   uint8_t text[] = "We found these pieces around the cave where we found the chest, but it just looks like a piece of junk with no value. Though it does make some colorful light when I shine a candle on it. Here you can have it maybe you can find a use for it. [He hands you a piece of the kaleidoscope]\r\nWhat do you want to do?\r\n(1) Ask About The Chest\r\n(2) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   capKal = 1;
										   kaleidoscope += 1;
										   dialogue += 1;
										   HAL_Delay(100);
									   }else if ((dialogue==4) && (capKal)) {
										   uint8_t text[] = "We found it in the cave where we found the chest, but it just looks like a piece of junk with no value. Though it does make some colorful light when I shine a candle on it.\r\nWhat do you want to do?\r\n(1) Ask About The Chest\r\n(2) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   dialogue += 1;
									   }else if (dialogue>4) {
										   dialogue = -1;
									   }
								   }

							   }else if (dataAvail && (rx_buff[0]=='2')) {
									  dataAvail = 0;
									  choice = 0;
									  break;
							   }
						   }
					   }
				   }else if (dataAvail && (rx_buff[0]=='2')) {
					   strcpy(room,"Map");
					   roomEnter=1;
					   dataAvail=0;
					   choice = 0;
				   }else if (dataAvail && (rx_buff[0]=='3')) {
					   strcpy(room,"Chest");
					   roomEnter=1;
					   dataAvail=0;
					   choice = 0;
				   }else if (dataAvail && (rx_buff[0]=='4')) {
					   while (1)
					   {
						   if (!nextRoom) {
							   uint8_t text[] = "\r\nWhere do you want to go?\r\n(1) Hull\r\n(2) Go Back\r\n";
							   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							   nextRoom = 1;
							   dataAvail = 0;
						   }else if (nextRoom) {
							   if (dataAvail && (rx_buff[0]=='1')) {
							 	   strcpy(room,"Hull");
							 	   nextRoom = 0;
							 	   roomEnter=1;
							 	   dataAvail=0;
							 	   choice = 0;
								   break;
							   }else if (dataAvail && (rx_buff[0]=='2')) {
								   nextRoom = 0;
								   dataAvail = 0;
								   choice = 0;
								   break;
							   }
						  }
					  }
				   }
			   }



		  }else if(strcmp(room,"Map")==0) {		  // Room: ---Map---
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou are in the Map\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  roomEnter = 0;
			  }else if (!choice) {
				  HAL_Delay(100);
				  uint8_t text[] = "What do you want to do?\r\n(1) Circle Pattern\r\n(2) Arrow Writing\r\n(3) Go Back\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  choice = 1;
			  }else if (choice) {
				  if (dataAvail && (rx_buff[0]=='1') && (kaleidoscope<3)) {
					  uint8_t text[] = "You see a large circle with three smaller circles and two arrows near the bottom. One going to the right and another down.\r\nWhat do you want to do?\r\n(1) Go Back\r\n";
					  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
					  dataAvail = 0;
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')) {
							  dataAvail = 0;
							  choice = 0;
							  break;
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='1') && (kaleidoscope==3)) {
					  uint8_t text[] = "You see a large circle with three smaller circles and two arrows near the bottom. One going to the right and followed by another on going down.\r\nWhat do you want to do?\r\n(1) Shine The Kaleidoscope\r\n(2) Go Back\r\n";
					  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
					  dataAvail = 0;
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')) {
							  uint8_t text[] = "You shine the kaleidoscope on the circle and line it the light with the three small circles. It revles the number '48' that lines up with the arrows on the bottom of the large circle.\r\nWhat do you want to do?\r\n(1) Go Back\r\n";
							  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							  dataAvail = 0;
							  while (1)
							  {
								  if (dataAvail && (rx_buff[0]=='1')) {
									  dataAvail = 0;
									  choice = 0;
									  break;
								  }
							  }
							  break;
						  }else if (dataAvail && (rx_buff[0]=='2')) {
							  dataAvail = 0;
							  choice = 0;
							  break;
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='2')) {
					  uint8_t text[] = "The arrows go in a sequence of up, right, left, down.\r\nWhat do you want to do?\r\n(1) Go Back\r\n";
					  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
					  dataAvail = 0;
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')) {
							  dataAvail = 0;
							  choice = 0;
							  break;
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='3')) {
					  strcpy(room, "Captain's Cabin");
					  dataAvail = 0;
					  choice = 0;
				  }
			  }


		  }else if(strcmp(room,"Chest")==0) {		  // Room: ---Chest---
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou are in the chest\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  roomEnter = 0;

			  }else if (!choice) {
				  HAL_Delay(100);
				  uint8_t text[] = "Where do you want to go?\r\n(1) Strange Markings\r\n(2) Lock\r\n(3) Go Back\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  choice = 1;
			  }else if (choice) {
				  if (dataAvail && (rx_buff[0]=='1')) {
					  uint8_t text[] = "The markings are widthered due to age but reads 'The smallest and largest' along with two arrows one pointing up followed by another pointing to the left.\r\nWhat do you want to do\r\n(1) Go back\r\n";
					  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
					  dataAvail = 0;
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')) {
							  dataAvail = 0;
							  choice = 0;
							  break;
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='2')) {
					  while (1)
					  {
						  if (!chest) {
							  uint8_t text[] = "You look at the rusty lock it shows a 4 number combination all ranging from 0-9. There is also an 'S' and 'L' on the sides of the lock.\r\nWhat do you want to do?\r\n(1) Try To Unlock It\r\n(2) Go Back\r\n";
							  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							  dataAvail = 0;
							  chest = 1;
						  }else if (chest) {
							  // The end combination for the chest

							  if (dataAvail && (rx_buff[0]=='1')) {

								  uint8_t text[] = "Enter the combination.\r\n";
								  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								  dataAvail = 0;
								  while (1)
								  {
										if (dataAvail && i<4) {
											attempt[i++] = rx_buff[0];
										    HAL_UART_Transmit_IT(&huart2,rx_buff,sizeof(rx_buff));
											dataAvail = 0;
										}
										if (i==4) {
											attempt[4] = '\0';
											HAL_Delay(50);
											changeRoom(attempt, answer, &endCode);
											HAL_Delay(50);
										    if (endCode) {
											    HAL_UART_Transmit(&huart2,line,sizeof(line),50);
											    uint8_t text[] = "You hear the lock click. Your eyes widen in anticipation as you open the chest. You see the \r\nglitter from the gold and jewels and sigh is relief as you have finally eared your stay \r\namong the pirates.\r\n(1) Next";
											    HAL_UART_Transmit(&huart2,text,sizeof(text),50);
											    HAL_UART_Transmit(&huart2,line,sizeof(line),50);
											    while (1)
											    {
											    	if (dataAvail && (rx_buff[0]=='1')) {
														  uint8_t screen[] = "      ##### /    ##                               ##### /    ##   ###                       \r\n"
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
														    dataAvail = 0;
											    	}

											    }
										    }else if (!endCode) {
											    uint8_t text[] = "\r\nThe lock does not open. It seems that combination was wrong.\r\n(1) Go Back\r\n";
											    HAL_UART_Transmit_IT(&huart2,text,sizeof(text));

											    while (1)
											    {
											    	if (dataAvail && (rx_buff[0]=='1')) {
											    		dataAvail = 0;
											    		i = 0;
											    		dialogue = 0;
											    		chest = 0;
											    		endCode = 1;
											    		break;
											    	}
											    }break;
										    }
										}
								  	  }
							  }else if (dataAvail && (rx_buff[0]=='2')) {
								  dataAvail = 0;
								  choice = 0;
								  break;
							  }
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='3')) {
					  strcpy(room, "Captain's Cabin");
					  dataAvail = 0;
					  choice = 0;
				  }
			  }

		  }else if (strcmp(room,"Hull")==0) {		  // Room: ---Hull---
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou make your way to the hull of the ship and see the bright blue sky along with the sound \r\nof the gentle waves crashing against the ship.\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  roomEnter = 0;

			  }else if (!choice) {
				  HAL_Delay(100);
				  if (shine) {

					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);

					  uint8_t options[] = "Where do you want to go?\r\n(1) Worried Crew Member\r\n(2) Sea\r\n(3) Barrel\r\n(4) Somewhere Else\r\n";
					  HAL_UART_Transmit(&huart2,options,sizeof(options),50);
					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);
				  }else{
					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);
					  uint8_t options[] = "Where do you want to go?\r\n(1) Worried Crew Member\r\n(2) Sea\r\n(3) Somewhere Else";
					  HAL_UART_Transmit(&huart2,options,sizeof(options),50);
					  HAL_UART_Transmit(&huart2,line,sizeof(line),50);
				  }
				  choice = 1;

			  }else if (choice) {
				  if (dataAvail && (rx_buff[0]=='1')) {			// Worried Crew Member Choice
					  while(1)
					  {
						  if (!worriedCrewmember) {
							  uint8_t text[] = "The crew member has a scared look on his face\r\nWhat do you want to do?\r\n(1) Talk\r\n(2) Go Back\r\n";
							  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							  worriedCrewmember = 1;
							  dataAvail = 0;
						  }else if (worriedCrewmember) {
							  if (dataAvail && (rx_buff[0]=='1') && !worRag) {
								  if (rag) {
									  uint8_t text[] = "I don't know what I am going to do. Chef will be so mad at me if he realizes I broke another mop. Do you happen to have a rag on you?\r\n(1) Yes\r\n(2) Go Back\r\n";
									  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								  }else{
									  uint8_t text[] = "I don't know what I am going to do. Chef will be so mad at me if he realizes I broke another mop. Do you happen to have a rag on you?\r\n(1) No\r\n(2) Go Back\r\n";
									  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								  }
								  dataAvail = 0;
								  while (1)
								  {
									  if (dataAvail && (rx_buff[0]=='1') && dialogue) {
										  dataAvail = 0;
										  worriedCrewmember = 0;
										  dialogue = 0;
										  break;
									  }else if (dataAvail && (rx_buff[0]=='1') && rag) {
										  uint8_t text[] = "You do! Here I can give you this for you saving me. [He hands you a beer]\r\n(1) Go Back\r\n";
										  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										  dataAvail = 0;
										  rag = 0;
										  worRag = 1;
										  dialogue = 1;
										  getBeer(&beer, &diff);
										  HAL_Delay(50);
									  }else if (dataAvail && (rx_buff[0]=='1') && !rag) {
										  uint8_t text[] = "Its fine I did this to myself.\r\n(1) Go Back\r\n";
										  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										  dataAvail = 0;
										  dialogue = 1;
									  }else if (dataAvail && (rx_buff[0]=='2') && !dialogue) {
										  dataAvail = 0;
										  worriedCrewmember = 0;
										  break;
									  }
									  HAL_Delay(50);
								  }
							  }else if (dataAvail && (rx_buff[0]=='1') && worRag) {
								  uint8_t text[] = "Thank you for the rags! I don't know what I would have done if I couldn't fix the mop. \r\n(1) Go Back\r\n";
								  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								  dataAvail = 0;
								  while (1)
								  {
									  if (dataAvail && (rx_buff[0]=='1')) {
										  dataAvail = 0;
										  worriedCrewmember = 0;
										  break;
									  }
								  }
							  }else if (dataAvail && (rx_buff[0]=='2')) {
								  dataAvail = 0;
								  choice = 0;
								  break;
							  }
						  }else{ __NOP();}
						  HAL_UART_Receive_IT(&huart2, rx_buff, 1);
					  }
				  }else if (dataAvail && (rx_buff[0]=='2')) {		// Sea Choice
					  while (1)
					  {
						  if (!dialogue) {
							  uint8_t text[] = "You look out into the vast open sea wondering how did you get into this situation.\r\n(1) Go Back\r\n";
							  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							  dataAvail = 0;
							  dialogue = 1;
						  }else if(dialogue) {
							  if (dataAvail && (rx_buff[0]=='1')) {
								  dataAvail = 0;
								  choice = 0;
								  dialogue = 0;
								  break;
							  }
						  }
					  }
				  }else if ((dataAvail && (rx_buff[0]=='3') && shine)) {		// Barrel Choice
					  while (1)
					  {
						  if (!dialogue && !locket) {
							  uint8_t text[] = "You go over to the barrel near where you saw that shining light.\r\n(1) Grab The Locket\r\n(2) Go Back\r\n";
							  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							  dataAvail = 0;
							  dialogue = 1;
							  while (1)
							  {
								  if (dataAvail && (rx_buff[0]=='1') && locket) {
									  dataAvail = 0;
									  choice = 0;
									  dialogue = 0;
									  break;
								  }else if (dataAvail && (rx_buff[0]=='1')) {
									  uint8_t text[] = "You pick up the locket and see that there is a picture of a pretty woman on the inside of the locket. Did someone lose this?\r\n(1) Go Back\r\n";
									  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
									  dataAvail = 0;
									  locket = 1;
								  }else if (dataAvail && (rx_buff[0]=='2')) {
									  dataAvail = 0;
									  choice = 0;
									  dialogue = 0;
									  break;
								  }
								  HAL_Delay(50);
							  }
							  break;
						  }else if(!dialogue && locket) {
							  uint8_t text[] = "You go over to the barrel near where you found the locket. Maybe this belongs to someone of the ship.\r\n(1) Go Back\r\n";
							  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							  dataAvail = 0;
							  dialogue = 1;
							  while (1)
							  {
								  if (dataAvail && (rx_buff[0]=='1') && locket) {
									  dataAvail = 0;
									  choice = 0;
									  dialogue = 0;
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
							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);
							  uint8_t text[] = "Where to next?\r\n(1) Crow's Nest\r\n(2) Armory\r\n(3) Captain's Cabin\r\n(4) Go Back";
							  HAL_UART_Transmit(&huart2,text,sizeof(text),50);
							  HAL_UART_Transmit(&huart2,line,sizeof(line),50);
							  nextRoom = 1;
							  dataAvail = 0;
						  }else if (nextRoom) {
							  if (dataAvail && (rx_buff[0]=='1')) {
								  strcpy(room, "Crow's Nest");
								  nextRoom = 0;
								  roomEnter = 1;
								  dataAvail = 0;
								  choice = 0;
								  break;

							  }else if (dataAvail && (rx_buff[0]=='2')) {
								  strcpy(room, "Armory");
								  nextRoom = 0;
								  roomEnter = 1;
								  dataAvail = 0;
								  choice = 0;
								  break;

							  }else if (dataAvail && (rx_buff[0]=='3')) {
								  strcpy(room, "Captain's Cabin");
								  nextRoom = 0;
								  roomEnter = 1;
								  dataAvail = 0;
								  choice = 0;
								  break;
							  }
							  else if (dataAvail && (rx_buff[0]=='4')) {
								  nextRoom = 0;
								  dataAvail = 0;
								  choice = 0;
								  break;
							  }
						  }
					  }
				  }
			  }






		  }else if (strcmp(room,"Kitchen")==0) {		  // Room: ---Kitchen---
			   if (roomEnter) {
				   uint8_t text[] = "\r\nYou are in the Kitchen\r\n";
				   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				   roomEnter = 0;

			   }else if (!choice){
				   HAL_Delay(50);
				   if (broken) {
					   uint8_t text[] = "Where do you want to go?\r\n(1) Chef\r\n(2) Broken Storage Box\r\n(3) Somewhere Else\r\n";
					   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				   }else{
					   uint8_t text[] = "Where do you want to go?\r\n(1) Chef\r\n(2) Fixed Storage Box\r\n(3) Somewhere Else\r\n";
					   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				   }
				   choice = 1;

			   }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {				   // Chef Choice
					   while (1)
					   {
						   if (!chef) {
							   uint8_t text[] = "\r\nThe chef quickly moves around the kitchen. He is covered in sweat rummaging \r\naround the small kitchen desperately trying to find something.\r\n";
							   HAL_UART_Transmit(&huart2,text,sizeof(text),50);

							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);
							   uint8_t options[] = "What do you want to do?\r\n(1) Talk\r\n(2) Go Back";
							   HAL_UART_Transmit(&huart2,options,sizeof(options),50);
							   HAL_UART_Transmit(&huart2,line,sizeof(line),50);

							   dataAvail = 0;
							   chef = 1;
						   }else if (chef) {
							   if (dataAvail && (rx_buff[0]=='1') && chefBox) {
								   uint8_t text[] = "I saw that you fixed that storage box. I have been waiting for someone to finally do it. Here I heard you have been looking for these keep it as a reward. [He hands you a piece of the kaleidoscope]\r\n(1) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   dataAvail = 0;
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   dataAvail = 0;
										   kaleidoscope += 1;
										   chef = 0;
										   chefBox = 0;
										   break;
									   }
								   }
							   }else if (dataAvail && (rx_buff[0]=='1') && locket) {
								   uint8_t text[] = "Sorry I don't have a lot of time to talk right now I have to finish making the crews rations for tomorrow.\r\n(1) Give Locket\r\n(2) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   dataAvail = 0;
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1') && !locket) {
										   dataAvail = 0;
										   chef = 0;
										   break;
									   }
									   if (dataAvail && (rx_buff[0]=='1')) {
										   uint8_t text[] = "Wow I can't believe you found it I have been looking everywhere for it ever since the raid. Here let me give you something special. [He hands you a beer]\r\n(1) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   getBeer(&beer, &diff);
										   locket = 0;
									   }else if (dataAvail && (rx_buff[0]=='2') && locket) {
										   dataAvail = 0;
										   chef = 0;
										   break;
									   }
									   HAL_Delay(50);
								   }
							   }else if (dataAvail && (rx_buff[0]=='1')) {
								   uint8_t text[] = "Sorry I don't have a lot of time to talk right now I have to finish making the crews rations for tomorrow.\r\n(1) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   dataAvail = 0;
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   dataAvail = 0;
										   chef = 0;
										   break;
									   }
								   }
							   }else if (dataAvail && (rx_buff[0]=='2')) {
								   dataAvail = 0;
								   choice = 0;
								   break;
							   }
						   }else if (dataAvail && (rx_buff[0]=='2')) {
							   dataAvail = 0;
							   choice = 0;
							   break;
						   }
					   }

				   }else if (dataAvail && (rx_buff[0]=='2') && broken) {				   // Broken Storage Box Choice
					   while (1)
					   {
						   if (!storageBox) {
							   if (hammer) {
								   uint8_t text[] = "You see and wooden storage box the top completely off from it hinges. \r\n(1) Fix Storage Box\r\n(2) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							   }else{
								   uint8_t text[] = "You see and wooden storage box the top completely off from it hinges. \r\n(1) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							   }
							   dataAvail = 0;
							   storageBox = 1;
						   }else if (storageBox) {
							   if (dataAvail && (rx_buff[0]=='1') && hammer) {
								   uint8_t text[] = "You use the hammer and nails to properly fix the lid to the rest of the box. \r\n(1) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   dataAvail = 0;
								   broken = 0;
								   hammer = 0;
								   chefBox = 1;
							   }else if ((dataAvail && (rx_buff[0]=='1') && !hammer) || (dataAvail && (rx_buff[0]=='2') && hammer)) {
								   dataAvail = 0;
								   choice = 0;
								   storageBox = 0;
								   break;
							   }
						   }
						   HAL_Delay(50);
					   }


				   }else if (dataAvail && (rx_buff[0]=='2') && !broken) {				   // Fixed Storage Box Choice
					   while (1)
					   {
						   if (!dialogue) {
							   uint8_t text[] = "You look at the restored storage box. You see an old stained note that reads 'This will be fix by next week'. By the looks of the note it has probably been here longer than 1 week. \r\n(1) Go Back\r\n";
							   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							   dataAvail = 0;
							   dialogue = 1;
						   }else if (dialogue) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   dataAvail = 0;
								   choice = 0;
								   dialogue = 0;
								   break;
							   }
						   }
					   }

				   }else if (dataAvail && (rx_buff[0]=='3')) {				   // Change Location Choice
					   while (1)
					   {
						   if (!nextRoom) {
							   uint8_t text[] = "\r\nWhere do you want to go?\r\n(1) Armory\r\n(2) Go Back\r\n";
							   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							   nextRoom = 1;
							   dataAvail = 0;
						   }else if (nextRoom) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   strcpy(room,"Armory");
								   nextRoom = 0;
								   roomEnter=1;
								   dataAvail=0;
								   choice = 0;
								   break;
							   }else if (dataAvail && (rx_buff[0]=='2')) {
								  nextRoom = 0;
								  dataAvail = 0;
								  choice = 0;
								  break;
							  }
						  }
					   }
				   }
			   }





		  }else if (strcmp(room,"Crow's Nest")==0) {		  // Room: ---Crow's Nest---
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou are in the Crow's Nest\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  roomEnter = 0;
			  }else if (!choice){
				  HAL_Delay(50);
				  if (diff) {
					  uint8_t text[] = "Where do you want to go?\r\n(1) Looker\r\n(2) Look At The Ship\r\n(3) Somewhere Else\r\n";
					  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  }else{
					  uint8_t text[] = "Where do you want to go?\r\n(1) Looker\r\n(2) Somewhere Else\r\n";
					  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  }
				  choice = 1;
			  }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {
					   while(1)
					   {
						   if (!looker) {
							  uint8_t text[] = "She looks off into the distance intently watching the ocean. What do you want to do?\r\n(1) Talk\r\n(2) Go Back\r\n";
							  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							  looker = 1;
							  dataAvail = 0;
						   }else if (looker) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   if (lookFood) {
									   uint8_t text[] = "I can't thank you enough for the crackers. I almost felt like I was going to pass out. \r\n(1) Go Back\r\n";
									   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   }else{
									   uint8_t text[] = "Hey are you apart of the new crew? With the raid this morning I have to be more alert in case of other ships show up. I never has enough time to eat breakfast. You wouldn't happen to have any food on you?\r\n(1) Give Her Food\r\n(2) Go Back\r\n";
									   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   }
								   dataAvail = 0;
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1') && lookFood) {
										   dataAvail = 0;
										   looker = 0;
										   break;
									   }
									   if (dataAvail && (rx_buff[0]=='1') && food) {
										   uint8_t text[] = "You do! Here I can give you this for you saving me. [She hands you a beer]\r\n(1) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   key = 1;
										   food = 0;
										   lookFood = 1;
									   }else if (dataAvail && (rx_buff[0]=='2') && !lookFood) {
										   dataAvail = 0;
										   looker = 0;
										   break;
									  }
								   }
							   }else if (dataAvail && (rx_buff[0]=='2')) {
									  dataAvail = 0;
									  choice = 0;
									  break;
							   }
						   }else{ __NOP();}
					   }

				   }else if (dataAvail && (rx_buff[0]=='2') && diff) {
					   while (1)
					   {
						   if (!dialogue) {
							   if (locket) {
								   uint8_t text[] = "You look out into Hull of the ship seeing just how big the ship really is. \r\n(1) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							   }else{
								   uint8_t text[] = "You look out into Hull of the ship seeing just how big the ship really is. Out of the corner of you eye you see and shine different from the rest of the ship. Could there be something down there? \r\n(1) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							   }
							   dataAvail = 0;
							   shine = 1;
							   dialogue = 1;
						   }else if (dialogue) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   dataAvail = 0;
								   choice = 0;
								   dialogue = 0;
								   break;
							   }
						   }
					   }
				   }

				   else if ((dataAvail && (rx_buff[0]=='2') && !diff) || (dataAvail && (rx_buff[0]=='3') && diff)) {
						  while (1) {
							  if (!nextRoom) {
								  uint8_t text[] = "\r\nWhere do you want to go?\r\n(1) Hull\r\n(2) Go Back\r\n";
								  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								  nextRoom = 1;
								  dataAvail = 0;
							  }else if (nextRoom) {
								  if (dataAvail && (rx_buff[0]=='1')) {
									   strcpy(room,"Hull");
									   nextRoom = 0;
									   roomEnter=1;
									   dataAvail=0;
									   choice = 0;
									   break;

								  }else if (dataAvail && (rx_buff[0]=='2')) {
									  nextRoom = 0;
									  dataAvail = 0;
									  choice = 0;
									  break;
								  }
							  }
						  }
					  }
			   }



		  }else if (strcmp(room,"Armory")==0) {			 // Room: ---Armory---
			   if (roomEnter) {
				 uint8_t text[] = "\r\nYou are in the Armory\r\n";
				 HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				 roomEnter = 0;

			   }else if (!choice){
				   HAL_Delay(100);
				   uint8_t text[] = "Where do you want to go?\r\n(1) Gunner\r\n(2) Storage Box\r\n(3) Somewhere Else\r\n";
				   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				   choice = 1;

			   }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {					// Gunner Choice
					   while (1)
					   {
						   if (!gunner) {
							   uint8_t text[] = "You watch him as a stares intensely at a grip of the flintlock before he polishes the gun and stares at it again. What do you want to do?\r\n(1) Talk\r\n(2) Go Back\r\n";
							   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							   gunner = 1;
							   dataAvail = 0;
						   }else if (gunner) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   uint8_t text[] = "What do you want can't you see I'm in the middle of something.\r\n(1) Mention The Kaleidoscope\r\n(2) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   dataAvail = 0;
								   while (1)
								   {
									   if (!diff) {
										   if (dataAvail && (rx_buff[0]=='1') && gunbeer) {
											   uint8_t text[] = "I appreciate the gift. Hope fully you can find a use for that piece of junk.\r\n(1) Go Back\r\n";
											   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
											   dataAvail = 0;
											   while (1)
											   {
												   if (dataAvail && (rx_buff[0]=='1')) {
													   dataAvail = 0;
													   gunner = 0;
													   break;
												   }
											   }
											   break;
										   }else if (dataAvail && (rx_buff[0]=='1') && !beer) {
											   uint8_t text[] = "Oh the captin's got you looking for that. I may or may not have something like that depends if you have something to give\r\n(1) Give Beer\r\n(2) Go Back\r\n";
											   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
											   dataAvail = 0;
											   while (1)
											   {
												   if (dataAvail && (rx_buff[0]=='1') && dialogue) {
													   dataAvail = 0;
													   gunner = 0;
													   dialogue = 0;
													   break;
												   }else if (dataAvail && (rx_buff[0]=='1')) {
													   uint8_t text[] = "Sorry if you don't have anything then there is nothing to give.\r\n(1) Go Back\r\n";
													   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
													   dataAvail = 0;
													   dialogue = 1;
												   }else if (dataAvail && (rx_buff[0]=='2') && !dialogue) {
													   dataAvail = 0;
													   gunner = 0;
													   break;
												   }
												   HAL_Delay(50);
											   }
											   break;
										   }else if (dataAvail && (rx_buff[0]=='1') && (beer==1)) {
											   uint8_t text[] = "Oh the captin's got you looking for that. I may or may not have something like that depends if you have something to give\r\n(1) Give Beer\r\n(2) Go Back\r\n";
											   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
											   dataAvail = 0;
											   while (1)
											   {
												   if (dataAvail && (rx_buff[0]=='1') && dialogue) {
													   dataAvail = 0;
													   gunner = 0;
													   dialogue = 0;
													   break;
												   }else if (dataAvail && (rx_buff[0]=='1')) {
													   uint8_t text[] = "Now you've got my attention. Here you can keep it. [He throws you the kaleidoscope]\r\n(1) Go Back\r\n";
													   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
													   dataAvail = 0;
													   dialogue = 1;
													   kaleidoscope += 1;
													   gunbeer = 1;
												   }else if (dataAvail && (rx_buff[0]=='2') && !dialogue) {
													   dataAvail = 0;
													   gunner = 0;
													   break;
												   }
												   HAL_Delay(50);
											   }
											   break;
										   }else if (dataAvail && (rx_buff[0]=='2')) {
											   dataAvail = 0;
											   gunner = 0;
											   break;
										   }
									   }else if (diff) {
										   if (dataAvail && (rx_buff[0]=='1') && !beer) {
											   uint8_t text[] = "I may or may not have something like that depends if you have something to give\r\n(1) Give Beer\r\n(2) Go Back\r\n";
											   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
											   dataAvail = 0;
											   while (1)
											   {
												   if (dataAvail && (rx_buff[0]=='1') && dialogue) {
													   dataAvail = 0;
													   gunner = 0;
													   dialogue = 0;
													   break;
												   }else if (dataAvail && (rx_buff[0]=='1')) {
													   uint8_t text[] = "Sorry if you don't have anything then there is nothing to give.\r\n(1) Go Back\r\n";
													   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
													   dataAvail = 0;
													   dialogue = 1;
												   }else if (dataAvail && (rx_buff[0]=='2') && !dialogue) {
													   dataAvail = 0;
													   gunner = 0;
													   break;
												   }
												   HAL_Delay(50);
											   }
											   break;
										   }else if (dataAvail && (rx_buff[0]=='1') && (beer==1)) {
											   uint8_t text[] = "I may or may not have something like that depends if you have something to give\r\n(1) Give Beer\r\n(2) Go Back\r\n";
											   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
											   dataAvail = 0;
											   while (1)
											   {
												   if (dataAvail && (rx_buff[0]=='1') && dialogue) {
													   dataAvail = 0;
													   gunner = 0;
													   dialogue = 0;
													   break;
												   }else if (dataAvail && (rx_buff[0]=='1')) {
													   uint8_t text[] = "Close you got the right idea, but I need a little bit more. I did go through a lot of trouble to get this thing.\r\n(1) Go Back\r\n";
													   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
													   dataAvail = 0;
													   dialogue = 1;
												   }else if (dataAvail && (rx_buff[0]=='2') && !dialogue) {
													   dataAvail = 0;
													   gunner = 0;
													   break;
												   }
												   HAL_Delay(50);
											   }
											   break;
										   }else if (dataAvail && (rx_buff[0]=='1') && (beer==2)) {
											   uint8_t text[] = "I may or may not have something like that depends if you have something to give\r\n(1) Give Beer\r\n(2) Go Back\r\n";
											   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
											   dataAvail = 0;
											   while (1)
											   {
												   if (dataAvail && (rx_buff[0]=='1') && dialogue) {
													   dataAvail = 0;
													   gunner = 0;
													   dialogue = 0;
													   break;
												   }else if (dataAvail && (rx_buff[0]=='1')) {
													   uint8_t text[] = "Now you've got my attention. Here you can keep it. [He throws you the kaleidoscope]\r\n(1) Go Back\r\n";
													   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
													   dataAvail = 0;
													   dialogue = 1;
													   kaleidoscope += 1;
													   gunbeer = 1;
												   }else if (dataAvail && (rx_buff[0]=='2') && !dialogue) {
													   dataAvail = 0;
													   gunner = 0;
													   break;
												   }
												   HAL_Delay(50);
											   }
											   break;
										   }else if (dataAvail && (rx_buff[0]=='2')) {
											   dataAvail = 0;
											   gunner = 0;
											   break;
										   }
									   }
									   HAL_Delay(50);
								   }
							   }else if (dataAvail && (rx_buff[0]=='2')) {
								   dataAvail = 0;
								   choice = 0;
								   break;
							   }
						   }
					   }

				   }else if (dataAvail && (rx_buff[0]=='2')) {				// Storage Box choice
					   while (1)
					   {
						   if (!storageBox) {
							   uint8_t text[] = "You see and old worn down barrel with the lid placed leaning beside the wall. \r\nWhat do you want to do?\r\n(1) Look Inside\r\n(2) Go Back\r\n";
							   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							   storageBox = 1;
							   dataAvail = 0;
						   }else if (storageBox) {


							   if (dataAvail && (rx_buff[0]=='1') && (!rope ^ chiRope) && (!rag ^ worRag)) {
								   uint8_t text[] = "You look in side the barrel and see a coil of rope that doesn't look like it has been used before and a couple of clean rags thrown in. \r\nWhat do you want to do?\r\n(1) Get The Rope\r\n(2) Get The Clean Rags\r\n(3) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   dataAvail = 0;
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   uint8_t text[] = "You grab take the rope from the barrel and hold it around your shoulder. \r\nWhat do you want to do?\r\n(1) Get The Clean Rags\r\n(2) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   rope = 1;
										   while (1)
										   {
											   if (dataAvail && (rx_buff[0]=='1') && rag) {
												   dataAvail = 0;
												   choice = 0;
												   storageBox = 0;
												   break;
											   }else if (dataAvail && (rx_buff[0]=='1')) {
												   uint8_t text[] = "You take clean rags and stuff them in your pocket. \r\nWhat do you want to do?\r\n(1) Go Back\r\n";
												   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
												   dataAvail = 0;
												   rag = 1;
												   HAL_Delay(50);
											   }else if (dataAvail && (rx_buff[0]=='2') && !rag) {
												   dataAvail = 0;
												   choice = 0;
												   storageBox = 0;
												   break;
											   }
										   }
										   break;
									   }else if (dataAvail && (rx_buff[0]=='2')) {
										   uint8_t text[] = "You take clean rags and stuff them in your pocket. \r\nWhat do you want to do?\r\n(1) Get The Rope\r\n(2) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   rag = 1;
										   while (1)
										   {
											   if (dataAvail && (rx_buff[0]=='1') && rope) {
												   dataAvail = 0;
												   choice = 0;
												   storageBox = 0;
												   break;
											   }else if (dataAvail && (rx_buff[0]=='1')) {
												   uint8_t text[] = "You grab take the rope from the barrel and hold it around your shoulder. \r\nWhat do you want to do?\r\n(1) Go Back\r\n";
												   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
												   dataAvail = 0;
												   rope = 1;
											   }else if (dataAvail && (rx_buff[0]=='2') && !rope) {
												   dataAvail = 0;
												   choice = 0;
												   storageBox = 0;
												   break;
											   }
										   }
										   break;
									   }else if (dataAvail && (rx_buff[0]=='3')) {
										   dataAvail = 0;
										   choice = 0;
										   storageBox = 0;
										   break;
									   }
								   }
								   break;
							   }else if (dataAvail && (rx_buff[0]=='1') && (!rag ^ worRag) && (rope ^ chiRope) ) {
								   uint8_t text[] = "You look inside the barrel and see a couple of clean rags thrown in. \r\nWhat do you want to do?\r\n(1) Get The Clean Rags\r\n(2) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   dataAvail = 0;
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   uint8_t text[] = "You take clean rags and stuff them in you pocket. \r\nWhat do you want to do?\r\n(1) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   rag = 1;
										   while (1)
										   {
											   if (dataAvail && (rx_buff[0]=='1')) {
												   dataAvail = 0;
												   choice = 0;
												   storageBox = 0;
												   break;
											   }
										   }
										   break;
									   }else if (dataAvail && (rx_buff[0]=='2')) {
										   dataAvail = 0;
										   choice = 0;
										   storageBox = 0;
										   break;
									   }
								   }
								   break;
							   }else if (dataAvail && (rx_buff[0]=='1') && (!rope ^ chiRope) && (rag ^ worRag)) {
								   uint8_t text[] = "You look in side the barrel and see a coil of rope that doesn't look like it has been used before. \r\nWhat do you want to do?\r\n(1) Get The Rope\r\n(2) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   dataAvail = 0;
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   uint8_t text[] = "You grab take the rope from the barrel and hold it around your shoulder. \r\nWhat do you want to do?\r\n(1) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   rope = 1;
										   while (1)
										   {
											   if (dataAvail && (rx_buff[0]=='1')) {
												   dataAvail = 0;
												   choice = 0;
												   storageBox = 0;
												   break;
											   }
										   }
										   break;
									   }else if (dataAvail && (rx_buff[0]=='2')) {
										   dataAvail = 0;
										   choice = 0;
										   storageBox = 0;
										   break;
									   }
								   }
								   break;
							   }else if (dataAvail && (rx_buff[0]=='1') && (rope ^ chiRope) && (rag ^ worRag) ) {
								   uint8_t text[] = "You look inside the barrel and see the nothing. \r\nWhat do you want to do?\r\n(1) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   dataAvail = 0;
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   dataAvail = 0;
										   choice = 0;
										   storageBox = 0;
										   break;
									   }
								   }
								   break;
							   }else if (dataAvail && (rx_buff[0]=='2')) {
								   dataAvail = 0;
								   choice = 0;
								   storageBox = 0;
								   break;
							   }

						   }

					   }

				   }else if (dataAvail && (rx_buff[0]=='3')) {				// Change Room choice
						  while (1) {
							  if (!nextRoom) {
								  uint8_t text[] = "\r\nWhere do you want to go?\r\n(1) Hull\r\n(2) Kitchen\r\n(3) Cargo Hold\r\n(4) Go Back\r\n";
								  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								  nextRoom = 1;
								  dataAvail = 0;
							  }else if (nextRoom) {
								  if (dataAvail && (rx_buff[0]=='1')) {
									  strcpy(room,"Hull");
									  nextRoom = 0;
									  roomEnter=1;
									  dataAvail=0;
									  choice = 0;
									  break;
								  }else if (dataAvail && (rx_buff[0]=='2')) {
									  strcpy(room, "Kitchen");
									  nextRoom = 0;
									  roomEnter=1;
									  dataAvail=0;
									  choice = 0;
									  break;
								  }else if (dataAvail && (rx_buff[0]=='3')) {
									  strcpy(room, "Cargo Hold");
									  nextRoom = 0;
									  roomEnter=1;
									  dataAvail=0;
									  choice = 0;
									  break;
								  }else if (dataAvail && (rx_buff[0]=='4')) {
									  nextRoom = 0;
									  dataAvail = 0;
									  choice = 0;
									  break;
								  }
							  }
						  }
					  }
			   }

		  }else if (strcmp(room,"Cargo Hold")==0) {		  // Room: ---Cargo Hold---
			   if (roomEnter) {
				 uint8_t text[] = "\r\nYou are in the Cargo Hold\r\n";
				 HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				 roomEnter = 0;

			   }else if (!choice){
				   HAL_Delay(100);
				   uint8_t text[] = "Where do you want to go?\r\n(1) Chief Mate\r\n(2) Locked Door\r\n(3) Somewhere Else\r\n";
				   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				   choice = 1;

			   }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {				   // Chief Mate Choice
					   while (1)
					   {
						   if (!chiefMate) {
							   uint8_t text[] = "He silently stares at a piece of paper before writing something down. Then he goes through a storage box. \r\n(1) Talk\r\n(2) Go Back\r\n";
							   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							   chiefMate = 1;
							   dataAvail = 0;
						   }else if (chiefMate) {
							   if (dataAvail && (rx_buff[0]=='1')) {
								   if (rope) {
									   uint8_t text[] = "mhm? oh your one of the new people right. I'm just checking the all that we have on the ship. \r\n(1) Talk\r\n(2) Give Rope\r\n(3) Go Back\r\n";
									   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   }else{
									   uint8_t text[] = "mhm? oh your one of the new people right. I'm just checking the all that we have on the ship. \r\n(1) Talk\r\n(2) Go Back\r\n";
									   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   }
								   dataAvail = 0;
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   dataAvail = 0;
										   dialogue += 1;
									   }else if ((dataAvail && (rx_buff[0]=='2') && !rope) || (dataAvail && (rx_buff[0]=='3') && rope)){
											  dataAvail = 0;
											  chiefMate = 0;
											  dialogue = 0;
											  break;
									   }
									   if ((dialogue==1) && !chiRope) {
										   uint8_t text[] = "There are so many things unaccounted for. How are we low on rope of all things. \r\n(1) Mention The Kaleidoscope\r\n(2) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   dialogue += 1;
									   }else if ((dialogue==1) & chiRope) {
										   uint8_t text[] = "Good job finding that rope. It makes this giant mess a lot easier to manage. \r\n(1) Mention The Kaleidoscope\r\n(2) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   dialogue += 1;
									   }else if (dialogue==3){
										   uint8_t text[] = "I'm not quite sure what it is. Though it does make some colorful light when I shine a candle on it. \r\n(1) Talk More\r\n(2) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   dialogue += 1;
									   }else if (dataAvail && (rx_buff[0]=='2') && rope){
										   uint8_t text[] = "Good job finding that rope. It makes this giant mess a lot easier to manage. It must have took a lot to find that have some of these. [He hands you a half eaten pack of crackers] \r\n(1) Mention The Kaleidoscope\r\n(2) Go Back\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   food = 1;
										   rope = 0;
										   chiRope = 1;
										   dialogue = 2;
										   HAL_Delay(50);
									   }else if (dialogue>3) {
										   dialogue = 0;
									   }
								   }
							   }else if (dataAvail && (rx_buff[0]=='2')) {
								   dataAvail = 0;
								   choice = 0;
								   break;
							   }
						   }
					   }
				   }else if (dataAvail && (rx_buff[0]=='2')) {				   // Storage Room Choice
					   strcpy(room, "Locked Door");
					   roomEnter = 1;
					   dataAvail = 0;
					   choice = 0;

				   }else if (dataAvail && (rx_buff[0]=='3')) {				   // Change location Choice
					   while (1)
					   {
						  if (!nextRoom) {
							  uint8_t text[] = "\r\nWhere do you want to go?\r\n(1) Armory\r\n(2) Go Back\r\n";
							  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							  nextRoom = 1;
							  dataAvail = 0;
						  }else if (nextRoom) {
							  if (dataAvail && (rx_buff[0]=='1')) {
								  strcpy(room,"Armory");
								  nextRoom = 0;
								  roomEnter = 1;
								  dataAvail = 0;
								  choice = 0;
								  break;
							  }else if (dataAvail && (rx_buff[0]=='2')) {
								  nextRoom = 0;
								  dataAvail = 0;
								  choice = 0;
								  break;
							  }
						  }
					   }
				   }
			   }

		  }else if (strcmp(room,"Locked Door")==0) {		  // Room: ---Locked door---
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou are in the Locked Door\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  roomEnter = 0;

			  }else if (!choice){
				  HAL_Delay(100);
				  uint8_t text[] = "Where do you want to go?\r\n(1) Enter The Storage Room\r\n(2) Go back\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  dataAvail = 0;
				  choice = 1;

			  }else if (choice) {
				  if (dataAvail && (rx_buff[0]=='1') && !key) {
					  uint8_t text[] = "You are unable to get through the door it seems to be locked. \r\n(1) Go back\r\n";
					  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
					  dataAvail = 0;
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')) {
							  dataAvail = 0;
							  choice = 0;
							  break;
						  }
					  }
				  }else if (dataAvail && (rx_buff[0]=='1') && key) {
					  uint8_t text[] = "You use the key to unlock the door. You see a bunch of wooden boxes all round the room. \r\n(1) Look Through The wooden Boxes\r\n(2) Go back\r\n";
					  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
					  dataAvail = 0;
					  while (1)
					  {
						  if (dataAvail && (rx_buff[0]=='1')){
							  if (chiHam) {
								  uint8_t text[] = "After sorting through the boxes, you can't see to find anything useful. \r\n(1) Go back\r\n";
								  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							  }else{
								  uint8_t text[] = "After sorting through the boxes, you find a lightly used hammer along with some nails. \r\n(1) Grab The Hammer And Nails\r\n(2) Go back\r\n";
								  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							  }
							  dataAvail = 0;
							  while (1)
							  {
								  if (dataAvail && (rx_buff[0]=='1') && chiHam) {
									  dataAvail = 0;
									  choice = 0;
									  break;
								  }else if (dataAvail && (rx_buff[0]=='1')) {
									  uint8_t text[] = "You put the hammer between your belt and carefully place the nail into you pocket. \r\n(1) Go back\r\n";
									  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
									  dataAvail = 0;
									  hammer = 1;
									  chiHam = 1;
								  }else if (dataAvail && (rx_buff[0]=='2') && !hammer) {
									  dataAvail = 0;
									  choice = 0;
									  break;
								  }
								  HAL_Delay(50);
							  }
							  break;
						  }else if (dataAvail && (rx_buff[0]=='2')) {
							  dataAvail = 0;
							  choice = 0;
							  break;
						  }
					  }
				  }else	if (dataAvail && (rx_buff[0]=='2')) {
					  strcpy(room,"Cargo Hold");
					  roomEnter = 1;
					  dataAvail = 0;
					  choice = 0;
				  }
			  }
		  }else{ __NOP();}
	  }

/*
	  while (death) // The replay loop
	  {
		  if (!inDeath) {
			  uint8_t text[] = "Do you want to continue? \r\n(1) Yes \r\n(2) No \r\n";
			  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
			  HAL_Delay(100);
			  inDeath = 1;
		  }else if (dataAvail && inDeath) {
			  if (dataAvail && (rx_buff[0]=='1')) {
				  playerAlive = 1;
				  dataAvail=0;
				  break;
			  }else if (dataAvail && (rx_buff[0]=='2')) {
				  uint8_t text[] = "Thank you for playing the game!";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  dataAvail=0;
				  death = 0;
				  break;
			  }
		  }else{ __NOP();}
		  HAL_UART_Receive_IT(&huart2, rx_buff, 1);
	  }
*/

	  HAL_UART_Receive_IT(&huart2, rx_buff, 1);
  } /* Game engine END WHILE */
  HAL_UART_Transmit(&huart2,"you are out of loop",sizeof("you are out of loop"),10);

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
