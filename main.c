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

uint8_t text[] = "What is 1+1? \r\n"; //Data to send
//uint8_t room[] = "Hull";


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

  uint8_t begin = 0;
  uint8_t exit = 0;
  uint8_t death = 0;
  uint8_t inDeath = 0;
  uint8_t choice = 0;
  uint8_t playerAlive = 0;

  uint8_t roomEnter = 1;
  uint8_t nextRoom = 0;


  char room[] = "Hull";

  // Objects
  uint8_t kaleidoscope = 0;
  uint8_t key = 0;
  uint8_t beer = 0;
  uint8_t food = 0;
  uint8_t hammer = 0;
  uint8_t rag = 0;
  uint8_t locket = 1;
  uint8_t rope = 0;


  // Characters
  uint8_t worriedCrewmember = 0;
  uint8_t looker = 0;
  uint8_t chef = 0;


  while(1)
  {
	  // If the game has not initialized, present player with welcome screen
	  if (!begin){
		  if(!init){
			  HAL_UART_Transmit_IT(&huart2,Welcome,sizeof(Welcome));
			  init = 1;
		  }else if(init && !inGame){ // If game initialized but game not started
			  if(dataAvail && (rx_buff[0]=='x')) {
				  inGame=1; // enter game
				  dataAvail=0; // always clear dataAvail flag AFTER it is set by HAL_UART_RxCpltCallback
			  }
		  }else if (inGame && !askedGameQuestion){
			  HAL_UART_Transmit(&huart2,difficulty,sizeof(difficulty),10);
			  askedGameQuestion = 1;
		  }else if (inGame){
			  if (dataAvail && (rx_buff[0]=='1')){
				  uint8_t text[] = "Taking the easy way out \r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  playerAlive = 1;
				  inGame = 0;
				  dataAvail=0;
				  begin = 1;
				  HAL_Delay(100);

			  }else if (dataAvail && (rx_buff[0]=='2')){
				  uint8_t text[] = "Best watch your tongue. \r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  playerAlive = 1;
				  inGame = 0;
				  dataAvail=0;
				  begin = 1;
				  HAL_Delay(100);
			  }
		  }else{ __NOP();}
	  }



	  while(playerAlive) // The main game loop
	  {

		  // Room: ---Captain's Cabin---
		  if (strcmp(room,"Captain's Cabin")==0) {
			  if (roomEnter) {
				uint8_t text[] = "\r\nYou are in the Captain's Cabin\r\n";
				HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				roomEnter = 0;
			  }else if (!choice){
				   HAL_Delay(100);
				   uint8_t text[] = "Where do you want to go?\r\n(1) Hull\r\n(2) Map\r\n";
				   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				   choice = 1;

			   }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {
					   strcpy(room,"Hull");

					   roomEnter=1;
					   dataAvail=0;
					   choice = 0;
				   }else if (dataAvail && (rx_buff[0]=='2')) {
					   strcpy(room, "Map");

					   roomEnter=1;
					   dataAvail=0;
					   choice = 0;
				   }
			   }


		  // Room: ---Map---
		  }else if(strcmp(room,"Map")==0) {
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou are in the Map\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  roomEnter = 0;

			  }else if (!choice) {
				  HAL_Delay(100);
				  uint8_t text[] = "Where do you want to go?\r\n(1) Captain's Cabin\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  choice = 1;
			  }else if (choice) {
				  if (dataAvail && (rx_buff[0]=='1')) {
					  strcpy(room, "Captain's Cabin");

					  roomEnter=1;
					  dataAvail=0;
					  choice = 0;
				  }
			  }

		  // Room: ---Hull---
		  }else if (strcmp(room,"Hull")==0) {
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou are in the Hull\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  roomEnter = 0;

			  }else if (!choice) {
				  HAL_Delay(100);
				  uint8_t text[] = "Where do you want to go?\r\n(1) Worried Crew Member\r\n(2) Sea\r\n(3) Somewhere Else\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  choice = 1;

			  }else if (choice) {
				  if (dataAvail && (rx_buff[0]=='1')) {
					  while(1)
					  {
						  if (!worriedCrewmember) {
							  uint8_t text[] = "The crew member has a scared look on his face\r\nWhat do you want to do?\r\n(1) Talk\r\n(2) Go Back\r\n";
							  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							  worriedCrewmember = 1;
							  dataAvail = 0;

						  }else if (worriedCrewmember) {
							  if (dataAvail && (rx_buff[0]=='1')) {
								  uint8_t text[] = "I completely Chef will be so mad at me if he realizes I broke another mop. Do you happen to have a rag on you?\r\n(1) Yes\r\n(2) No\r\n(3) Go Back\r\n";
								  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								  dataAvail = 0;
								  while (1)
								  {
									  if (dataAvail && (rx_buff[0]=='1') && rag) {
										  uint8_t text[] = "You do! Here I can give you this for you saving me. [He hands you a beer]\r\n";
										  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										  dataAvail = 0;
										  if (beer == '0') {
											  beer = 1;
										  }else {
											  beer = 2;
										  }
									  }else if (dataAvail && (rx_buff[0]=='2') && !rag) {
										  uint8_t text[] = "Its fine I did this to myself.\r\n";
										  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										  dataAvail = 0;
									  }else if (dataAvail && (rx_buff[0]=='3')) {
										  dataAvail = 0;
										  worriedCrewmember = 0;
										  break;
									  }
									  HAL_UART_Receive_IT(&huart2, rx_buff, 1);
								  }


							  }else if (dataAvail && (rx_buff[0]=='2')) {
								  dataAvail = 0;
								  choice = 0;
								  break;
							  }
						  }else{ __NOP();}
						  HAL_UART_Receive_IT(&huart2, rx_buff, 1);
					  }
				  }else if (dataAvail && (rx_buff[0]=='3')) {
					  while (1) {
						  if (!nextRoom) {
							  uint8_t text[] = "\r\nWhere do you want to go?\r\n(1) Crow's Nest\r\n(2) Armory\r\n(3) Captain's Cabin\r\n";
							  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
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
						  }
					  }
				  }
			  }







		  // Room: ---Kitchen---
		  }else if (strcmp(room,"Kitchen")==0) {
			   if (roomEnter) {
				   uint8_t text[] = "\r\nYou are in the Kitchen\r\n";
				   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				   roomEnter = 0;

			   }else if (!choice){
				   HAL_Delay(100);
				   uint8_t text[] = "Where do you want to go?\r\n(1) Chef\r\n(2) Broken Storage Box\r\n(3) Somewhere Else\r\n";
				   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				   choice = 1;

			   }else if (choice) {
				   // Character: ---Chef---
				   if (dataAvail && (rx_buff[0]=='1')) {
					   while (1)
					   {
						   if (!chef) {
							   uint8_t text[] = "The chef quickly moves around the kitchen. What do you want to do?\r\n(1) Talk\r\n(2) Go Back\r\n";
							   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
							   dataAvail = 0;
							   chef = 1;

						   }else if (chef) {
							   if (dataAvail && (rx_buff[0]=='1') && !locket) {
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
							   }else if (dataAvail && (rx_buff[0]=='1') && locket) {
								   uint8_t text[] = "Sorry I don't have a lot of time to talk right now I have to finish making the crews rations for tomorrow.\r\n(1) Give Locket\r\n(2) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   dataAvail = 0;
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1')) {
										   uint8_t text[] = "Wow I can't believe you found it I have been looking everywhere for it ever since the raid. Here let me give you something special. [He hands you a beer]\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   dataAvail = 0;
										   HAL_Delay(100);
										   if (beer == '0') {
											   beer = 1;
										   }else {
											   beer = 2;
										   }
										   locket = 0;
										   chef = 0;
										   break;

									   }else if (dataAvail && (rx_buff[0]=='2')) {
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

				   // Change location
				   }else if (dataAvail && (rx_buff[0]=='3')) {
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



		  // Room: ---Crow's Nest---
		  }else if (strcmp(room,"Crow's Nest")==0) {
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou are in the Crow's Nest\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  roomEnter = 0;

			  }else if (!choice){
				  HAL_Delay(100);
				  uint8_t text[] = "Where do you want to go?\r\n(1) Looker\r\n(2) Look At The Ship\r\n(3) Somewhere Else\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
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
								   uint8_t text[] = "Hey are you apart of the new crew? With the raid this morning I have to be more alert in case of ship. I never has enough time to eat breakfast. What do you want to do?\r\n(1) Give Her Food\r\n(2) Go Back\r\n";
								   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
								   dataAvail = 0;
								   while (1)
								   {
									   if (dataAvail && (rx_buff[0]=='1') && food) {
										   uint8_t text[] = "You do! Here I can give you this for you saving me. [He hands you a beer]\r\n";
										   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
										   key = 1;
										   dataAvail = 0;
									   }else if (dataAvail && (rx_buff[0]=='2')) {
										   dataAvail = 0;
										   looker = 0;
										   break;
									  }
									  HAL_UART_Receive_IT(&huart2, rx_buff, 1);
								   }



							   }else if (dataAvail && (rx_buff[0]=='2')) {
									  dataAvail = 0;
									  choice = 0;
									  break;
							   }

						   }else{ __NOP();}
					   }

				   }else if (dataAvail && (rx_buff[0]=='3')) {
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



		  // Room: ---Armory---
		  }else if (strcmp(room,"Armory")==0) {
			   if (roomEnter) {
				 uint8_t text[] = "\r\nYou are in the Armory\r\n";
				 HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				 roomEnter = 0;

			   }else if (!choice){
				   HAL_Delay(100);
				   uint8_t text[] = "Where do you want to go?\r\n(1) Hull\r\n(2) Kitchen\r\n(3) Cargo Hold\r\n";
				   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				   choice = 1;

			   }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {
					   strcpy(room,"Hull");

					   roomEnter=1;
					   dataAvail=0;
					   choice = 0;
				   }else if (dataAvail && (rx_buff[0]=='2')) {
					   strcpy(room, "Kitchen");

					   roomEnter=1;
					   dataAvail=0;
					   choice = 0;
				   }else if (dataAvail && (rx_buff[0]=='3')) {
					   strcpy(room, "Cargo Hold");

					   roomEnter=1;
					   dataAvail=0;
					   choice = 0;
				   }
			   }


		  // Room: ---Cargo Hold---
		  }else if (strcmp(room,"Cargo Hold")==0) {
			   if (roomEnter) {
				 uint8_t text[] = "\r\nYou are in the Cargo Hold\r\n";
				 HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				 roomEnter = 0;

			   }else if (!choice){
				   HAL_Delay(100);
				   uint8_t text[] = "Where do you want to go?\r\n(1) Armory\r\n(2) Locked Door\r\n";
				   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				   choice = 1;

			   }else if (choice) {
				   if (dataAvail && (rx_buff[0]=='1')) {
					   strcpy(room,"Armory");

					   roomEnter=1;
					   dataAvail=0;
					   choice = 0;
				   }else if (dataAvail && (rx_buff[0]=='2') && key) {
					   strcpy(room, "Locked Door");

					   roomEnter=1;
					   dataAvail=0;
					   choice = 0;
				   }else if (dataAvail && (rx_buff[0]=='2') && !key) {
					   uint8_t text[] = "You can not open the door it seems to be locked.\r\n";
					   HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
					   dataAvail=0;
				   }
			   }


		  // Room: ---Locked door---
		  }else if (strcmp(room,"Locked Door")==0) {
			  if (roomEnter) {
				  uint8_t text[] = "\r\nYou are in the Locked Door\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  roomEnter = 0;

			  }else if (!choice){
				  HAL_Delay(100);
				  uint8_t text[] = "Where do you want to go?\r\n(1) Cargo Hold\r\n";
				  HAL_UART_Transmit_IT(&huart2,text,sizeof(text));
				  choice = 1;

			  }else if (choice) {
				  if (dataAvail && (rx_buff[0]=='1')) {
					  strcpy(room,"Cargo Hold");

					  roomEnter=1;
					  dataAvail=0;
					  choice = 0;
				  }
			  }
		  }else{ __NOP();}



		  HAL_UART_Receive_IT(&huart2, rx_buff, 1);
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

