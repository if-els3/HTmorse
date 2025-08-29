/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#include "cmsis_os.h"
#include "usb_host.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "usbh_hid.h"
#include "usbh_hid_keybd.h"
#include <string.h>
#include <stdio.h>
#include "fonts.h"
#include "ssd1306.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define FONT Font_7x10
#define CHAR_WIDTH 7
#define CHAR_HEIGHT 10
#define MAX_BUFFER_LINES 50
#define MAX_CHARS_PER_LINE (SSD1306_WIDTH / CHAR_WIDTH) // ~18
#define MAX_SCREEN_LINES (SSD1306_HEIGHT / CHAR_HEIGHT) // ~6

// for non alfanumeric
#define KEY_HOME 0x4A
#define KEY_ARROW_RIGHT 0x4F
#define KEY_ARROW_LEFT 0x50
#define KEY_ARROW_DOWN 0x51
#define KEY_ARROW_UP 0x52
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
UART_HandleTypeDef huart1;

osThreadId defaultTaskHandle;
/* USER CODE BEGIN PV */
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
void StartDefaultTask(void const * argument);

/* USER CODE BEGIN PFP */
void ProcessKeyboardData(uint8_t* data);
void redraw_screen(void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
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
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */

  char* start_msg = "USB Host Keyboard with FreeRTOS Ready...\r\n";
  HAL_UART_Transmit(&huart1, (uint8_t*)start_msg, strlen(start_msg), HAL_MAX_DELAY);

  //FOR LCD 1306 BIAR NYALA YEKAN
  ssd1306_Init();
  ssd1306_WriteString("HOST USB", Font_11x18, White);
  ssd1306_SetCursor(0, 20);
  ssd1306_WriteString("READY CUY!", Font_7x10, White);
  ssd1306_UpdateScreen();
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 1024);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
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
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

// For transforming enum HOST_StateTypeDef To String
void PrintHostState(HOST_StateTypeDef state) {
    char* msg;
    switch(state) {
        case HOST_IDLE: msg = "Host State -> HOST_IDLE\r\n"; break;
        case HOST_DEV_ATTACHED: msg = "Host State -> HOST_DEV_ATTACHED\r\n"; break;
        case HOST_DEV_DISCONNECTED: msg = "Host State -> HOST_DEV_DISCONNECTED\r\n"; break;
        case HOST_DETECT_DEVICE_SPEED: msg = "Host State -> HOST_DETECT_SPEED\r\n"; break;
        case HOST_ENUMERATION: msg = "Host State -> HOST_ENUMERATION\r\n"; break;
        case HOST_CLASS_REQUEST: msg = "Host State -> HOST_CLASS_REQUEST\r\n"; break;
        case HOST_INPUT: msg = "Host State -> HOST_INPUT\r\n"; break;
        case HOST_SET_CONFIGURATION: msg = "Host State -> HOST_SET_CONFIGURATION\r\n"; break;
        case HOST_CHECK_CLASS: msg = "Host State -> HOST_CHECK_CLASS\r\n"; break;
        case HOST_CLASS: msg = "Host State -> HOST_CLASS\r\n"; break;
        case HOST_SUSPENDED: msg = "Host State -> HOST_SUSPENDED\r\n"; break;
        case HOST_ABORT_STATE: msg = "Host State -> HOST_ABORT_STATE\r\n"; break;
        default: msg = "Host State -> UNKNOWN\r\n"; break;
    }
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

// For transforming enum ApplicationTypeDef to string string
void PrintAppState(ApplicationTypeDef state) {
    char* msg;
    switch(state) {
        case APPLICATION_IDLE: msg = "Appli State -> APPLICATION_IDLE\r\n"; break;
        case APPLICATION_START: msg = "Appli State -> APPLICATION_START\r\n"; break;
        case APPLICATION_READY: msg = "Appli State -> APPLICATION_READY (Keyboard siap!)\r\n"; break;
        case APPLICATION_DISCONNECT: msg = "Appli State -> APPLICATION_DISCONNECT (Keyboard terputus)\r\n"; break;
        default: msg = "Appli State -> UNKNOWN\r\n"; break;
    }
    HAL_UART_Transmit(&huart1, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
}

void USBH_UserProcess(USBH_HandleTypeDef *phost, uint8_t id)
{

  static HOST_StateTypeDef last_gState = HOST_IDLE;
  static ApplicationTypeDef last_appliState = APPLICATION_IDLE;

  if (phost->gState != last_gState) {
    last_gState = phost->gState;
    PrintHostState(last_gState);
  }
  // check state
  if (Appli_state != last_appliState) {
    last_appliState = Appli_state;
    PrintAppState(last_appliState);
  }
}

// Buffer for saving text
char text_buffer[MAX_BUFFER_LINES][MAX_CHARS_PER_LINE + 1];

int cursor_line = 0;
int cursor_col = 0;
int display_top_line = 0;
int total_lines = 1;
uint8_t cursor_visible = 1;


// IMPORTANT! CONVERT HID Keycode ke ASCII
const uint8_t keycodes[] = {
  '\0', '\0', '\0', '\0', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l',
  'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0',
  '\n', '\e', '\b', '\t', ' ', '-', '=', '[', ']', '\\',
  '\0', ';', '\'', '`', ',', '.', '/', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0', '\0'
};

const uint8_t keycodes_shifted[] = {
  '\0', '\0', '\0', '\0', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L',
  'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
  '!', '@', '#', '$', '%', '^', '&', '*', '(', ')',
  '\n', '\e', '\b', '\t', ' ', '_', '+', '{', '}', '|',
  '\0', ':', '"', '~', '<', '>', '?', '\0', '\0', '\0', '\0', '\0',
  '\0', '\0', '\0', '\0', '\0', 'k', '\0', '\0', '\0', '\0', '\0', '\0'
};

// Callback when thereis event on Keyboard
void USBH_HID_EventCallback(USBH_HandleTypeDef *phost)
{
  // this will sent  dot every time
  //HAL_UART_Transmit(&huart1, (uint8_t*)".", 1, 10);

  HID_HandleTypeDef *HID_Handle = (HID_HandleTypeDef*) phost->pActiveClass->pData;

  if(HID_Handle->length > 0 && HID_Handle->pData[2] != 0)
  {
    ProcessKeyboardData(HID_Handle->pData);
    memset(HID_Handle->pData, 0, HID_Handle->length);
  }
}

/**
  * @brief redraw OLED from text buffer.
  */
void redraw_screen(void)
{
    ssd1306_Fill(Black);

    for (int i = 0; i < MAX_SCREEN_LINES; i++)
    {
        int line_to_draw = display_top_line + i;
        if (line_to_draw < total_lines)
        {
            ssd1306_SetCursor(0, i * CHAR_HEIGHT);
            ssd1306_WriteString(text_buffer[line_to_draw], FONT, White);
        }
    }

    if (cursor_visible && cursor_line >= display_top_line && cursor_line < display_top_line + MAX_SCREEN_LINES)
    {
        int screen_y = (cursor_line - display_top_line) * CHAR_HEIGHT;
        int screen_x = cursor_col * CHAR_WIDTH;

        for (int y = 0; y < CHAR_HEIGHT; y++) {
            for (int x = 0; x < CHAR_WIDTH; x++) {
                ssd1306_DrawPixel(screen_x + x, screen_y + y, White);
            }
        }
    }

    ssd1306_UpdateScreen();
}


void ProcessKeyboardData(uint8_t* data)
{
    uint8_t modifier = data[0];
    uint8_t keycode = data[2];
    char ascii_char;

    // Handle spesial button (Non-Alfanumeric)
    if (keycode >= KEY_HOME && keycode <= KEY_ARROW_UP) {
        switch(keycode) {
            case KEY_ARROW_UP: if (cursor_line > 0) cursor_line--; break;
            case KEY_ARROW_DOWN: if (cursor_line < total_lines - 1) cursor_line++; break;
            case KEY_ARROW_LEFT:
                if (cursor_col > 0) cursor_col--;
                else if (cursor_line > 0) {
                    cursor_line--;
                    cursor_col = strlen(text_buffer[cursor_line]);
                }
                break;
            case KEY_ARROW_RIGHT:
                if (cursor_col < strlen(text_buffer[cursor_line])) cursor_col++;
                else if (cursor_line < total_lines - 1) {
                    cursor_line++;
                    cursor_col = 0;
                }
                break;
            case KEY_HOME:
            	cursor_line = 0; cursor_col = 0; display_top_line = 0;
            	break;
        }
    } else { 
        if (modifier & 0x22) { ascii_char = keycodes_shifted[keycode]; }
        else { ascii_char = keycodes[keycode]; }

        if (ascii_char != '\0') {
            if (ascii_char == '\n') { // Enter
                if (total_lines < MAX_BUFFER_LINES) {
                    total_lines++;
                    cursor_line++;
                    cursor_col = 0;
                }
            } else if (ascii_char == '\b') { // Backspace
                if (cursor_col > 0) {
                    memmove(&text_buffer[cursor_line][cursor_col - 1], &text_buffer[cursor_line][cursor_col], strlen(text_buffer[cursor_line]) - cursor_col + 1);
                    cursor_col--;
                } 
            } else if (strlen(text_buffer[cursor_line]) < MAX_CHARS_PER_LINE) { 
                memmove(&text_buffer[cursor_line][cursor_col + 1], &text_buffer[cursor_line][cursor_col], strlen(text_buffer[cursor_line]) - cursor_col + 1);
                text_buffer[cursor_line][cursor_col] = ascii_char;
                cursor_col++;
            }
        }
    }

    if (cursor_col > strlen(text_buffer[cursor_line])) {
        cursor_col = strlen(text_buffer[cursor_line]);
    }

    // Scrolling
    if (cursor_line >= display_top_line + MAX_SCREEN_LINES) {
        display_top_line = cursor_line - MAX_SCREEN_LINES + 1;
    }
    if (cursor_line < display_top_line) {
        display_top_line = cursor_line;
    }

    redraw_screen();
}
/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* init code for USB_HOST */
  MX_USB_HOST_Init();

  /* USER CODE BEGIN 5 */

  static ApplicationTypeDef last_appliState = APPLICATION_IDLE;
  uint32_t last_cursor_blink_tick = 0;
  char msg_buffer[50];

  /* Infinite loop */
  for(;;)
  {
    USBH_Process(&hUsbHostFS);

    //CHECKING and report STATE
    if (Appli_state != last_appliState)
    {
      last_appliState = Appli_state;

    if (Appli_state == APPLICATION_READY)
    {
      // for device descriptor
      ssd1306_Fill(Black);
      ssd1306_SetCursor(0, 0);
      ssd1306_WriteString("Keyboard Ready", Font_7x10, White);

      uint16_t vid = hUsbHostFS.device.DevDesc.idVendor;
      uint16_t pid = hUsbHostFS.device.DevDesc.idProduct;

      sprintf(msg_buffer, "VID: 0x%04X", vid);
      ssd1306_SetCursor(0, 12);
      ssd1306_WriteString(msg_buffer, Font_7x10, White);

      sprintf(msg_buffer, "PID: 0x%04X", pid);
      ssd1306_SetCursor(0, 24);
      ssd1306_WriteString(msg_buffer, Font_7x10, White);

      ssd1306_UpdateScreen();
      osDelay(1400);
      ssd1306_Fill(Black);
      ssd1306_UpdateScreen();

      // Reset editor
      memset(text_buffer, 0, sizeof(text_buffer));
      cursor_line = 0; cursor_col = 0;
      display_top_line = 0; total_lines = 1;
      redraw_screen();
    }
    // when keyboard disconnected
    else if (Appli_state == APPLICATION_DISCONNECT)
    {
      ssd1306_Fill(Black);
      ssd1306_SetCursor(0, 0);
      ssd1306_WriteString("EAK COPOT", Font_11x18, White);
      ssd1306_SetCursor(0, 20);
      ssd1306_WriteString("colok lagi lah we", Font_7x10, White);
      ssd1306_UpdateScreen();
    }
    // cursor
    if (HAL_GetTick() - last_cursor_blink_tick > 400)
    {
        last_cursor_blink_tick = HAL_GetTick();
        cursor_visible = !cursor_visible;
        if (Appli_state == APPLICATION_READY) {
            redraw_screen();
        }
    }
    osDelay(20);
  }
  /* USER CODE END 5 */
}
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

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
    // Anda bisa menambahkan kode di sini untuk menyalakan LED sebagai indikator error
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
