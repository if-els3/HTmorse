# HTmorse

# Current Status 
The first checkpoint has been successfully reached. The project currently implements the following core functionality:
- USB Host on an STM32F411CEU6.
- Reads input from a standard USB keyboard.
- Displays all typed characters in real-time on an SSD1306 OLED screen.


# 🛠️ Project Setup and Replication Guide
This is a complete guide to set up the project from scratch using STM32CubeIDE and STM32CubeMX to set config the board

## Step 1: Project Configuration in STM32CubeMX (.ioc)
- Create a New Project for the STM32F411CEU6.
- Configure System Core:
1. RCC: Set High Speed Clock (HSE) and Low Speed Clock to Crystal/Ceramic Resonator.
2. SYS: Set Timebase Source to TIM1.
   
- Configure Connectivity:
1. I2C1: Enable I2C mode.
2. USART1: Enable Asynchronous mode.
3. USB_OTG_FS: Set the mode to Host_Only and set the speed to Host Full Speed
   
- Configure Middleware:
1. USB_HOST: Check Enabled. For Class for FS IP, select Human Interface Host Class (HID).
2. FREERTOS: Change the Interface to CMSIS_V1.
   
- Configure Clock:
1. Switch to the Clock Configuration tab.
2. Ensure the PLL Source Mux is set to HSE and Enable CSS
3. Adjust the PLL values so that the System Clock (HCLK) is 96 MHz.
4. Crucially, ensure the clock for 48MHz clocks is exactly 48 MHz.
<img width="770" height="340" alt="clockstm32" src="https://github.com/user-attachments/assets/f9694e82-ac32-45ce-a685-9283aa0d88af" />

- Generate Code:
Save the .ioc file (Ctrl+S). When STM32CubeIDE asks to generate code, click "Yes".

## Step 2: Code Modification
After the base code is generated, a few files need to be manually modified to add the application logic and fix common library issues.
Copy `main.c`: Replace the entire content of the newly generated `Core/Src/main.c` with the `main.c` file from this repository. This file contains all the logic for handling USB states, reading the keyboard, and displaying the output on the OLED.
Copy `usb_host.h`: Replace the entire content of `USB_HOST/App/usb_host.h` with the `usb_host.h` file from this repository. This file includes fixes for the global variable declarations (hUsbHostFS and Appli_state) that are required by main.c.

## Step 3: Build the Schematics
The pin should connected each other like this:
<img width="1000" height="900" alt="schematic" src="https://github.com/user-attachments/assets/5dc2b952-d9de-4ae8-b1a4-e90240d0b26d" />


## Step 4: Build and Flash
Clean and Build: In STM32CubeIDE, first clean the project (Project -> Clean...) and then rebuild it (Project -> Build All or Ctrl+B). This will compile all files, including your modifications. And then flash to the STM32 Board by connecting the ST-LINK to your STM32 board and computer then click the Run button (▶️ icon) on the main toolbar to upload the program to the microcontroller.
