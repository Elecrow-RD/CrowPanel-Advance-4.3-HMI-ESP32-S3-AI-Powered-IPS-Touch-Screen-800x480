## Menu
### 1, Product picture
### 2, Product version number
### 3, product information
### 4, Use the driver module
### 5,Quick Start
### 6,Folder structure.
### 7,Pin definition
### 8,Guidelines for downloading some of the files from the github repository


### 1, Product picture

![Crowpanel Advance 4.3 HMI display support Meshtastic](https://www.elecrow.com/media/catalog/product/cache/9e67447b006ee4d9559353b91d12add5/e/s/esp32_advance_hmi_04_dis01943a_1.jpg)

### 2, Product version number

|      | Hardware | Software | Remark |
| ---- | -------- | -------- | ------ |
| 1    | V1.0     | V1.0     | old |
| 2    | V1.2     | V1.2     | latest |

### Version update points:
① Version 1.2 updated the control of backlight on the basis of the original, the backlight of version 1.2 is controlled by STC8H1K28 microcontroller, and the backlight is lit in the program by sending the value to this microcontroller address (0x30). The values are 0-245, where 245 switches off the backlight and 0 is the maximum brightness.

② Version 1.2 has a new option for function switching, see above for details. When using the case, please select the corresponding program to test or modify according to the version number of your motherboard.

### 3, product information

| Main Chip-ESP32-S3-WROOM-1-N16R8         |                                                              |
| -------------------------------------------- | ------------------------------------------------------------ |
| CPU/SoC                                      | high-performance Xtensa 32-bit LX7 dual-core processor, with up to 240MHz |
| System Memory                                | 512KB SRAM、8M PSRAM                                         |
| Memory                                       | 16M Flash，384KB ROM                                         |
| Development Language                         | MicroPython、C/C++                                           |
| Development Environment                      | ESP-IDF、Arduino IDE、PlatformIO、Micro Python、LVGL         |
| **Screen**                                   |                                                              |
| Size                                         | 4.3 inch                                                     |
| Diver IC                                     | ST7265                                                       |
| Resolution                                   | 800*480                                                      |
| Display Panel                                | IPS Panel                                                    |
| Touch Panel                                  | Capacitive Single Touch                                      |
| Viewing Angle                                | 178°                                                         |
| Brightness                                   | 400 cd/m²(Typ.)                                              |
| Color Depth                                  | 16-bit                                                       |
| **Wireless Communication - Onboard Antenna** |                                                              |
| WiFi                                         | Support 2.4GHz, 802.11a/b/g/n                                |
| Bluetooth                                    | Support Bluetooth 5.0 and BLE                                |
| Other                                        | **LoRa**                                                     |
| **Interface/Function**                       |                                                              |
| Interface                                    | USB port, UART, I2C, SD card slot, battery socket, speaker port, microphone, etc. |
| Function                                     | RTC clock, audio amplifier, volume control, battery charge management, USB to UART, etc. |
| **Button/LED Indicator**                     |                                                              |
| Reset Button                                 | Yes, press to reset the device                               |
| Boot Button                                  | Yes, press and hold the power button to burn the program     |
| PWR                                          | Power indicator                                              |
| CHG                                          | Lithium battery charging status, completion indication       |
| **Other**                                    |                                                              |
| Installation method                          | Back hanging, fixed hole                                     |
| Operating temperature                        | -20~70 °C                                                    |
| Storage temperature                          | -30~80 °C                                                    |
| Power Input                                  | 5V/2A, USB or UART terminal                                  |
| Active Area                                  | 95.5mm*54mm                                                  |
| Dimensions                                   | 121.9*75.6*15.3mm                                            |

### 4, Use the driver module

| Name   | dependency library                      |
| ------ | --------------------------------------- |
| LVGL   | lvgl/lvgl@8.3.3                         |
| ST7789 | Adafruit GFX Library<br/>version=1.11.0 |

### 5,Quick Start
### Arduino IDE starts

#### Teaching Case Path：./example/example_code4.3

1.Download the library files used by this product to the 'libraries' folder.

C:\Users\Documents\Arduino\libraries\

![2](https://github.com/user-attachments/assets/86c568bb-3921-4a07-ae91-62d7ce752e50)



2.Open the Arduino IDE

![1](https://github.com/user-attachments/assets/17b4e9af-a863-4bfd-839e-be94f00a33ad)


3.Open the code configuration environment and burn it

![3](https://github.com/user-attachments/assets/1a58d8ff-616b-4b71-9465-c2dac03f3399)



### ESP-IDF starts

#### Teaching Case Path：./example/idf_code

1.Right-click on an empty space in the project folder and select "Open with VS Code" to open the project.
![4](https://github.com/user-attachments/assets/a842ad62-ed8b-49c0-bfda-ee39102da467)



2.In the IDF plug-in, select the port, then compile and flash
![5](https://github.com/user-attachments/assets/76b6182f-0998-4496-920d-d262a5142df3)




### 6,Folder structure.
|--3D file： Contains 3D model files (.stp) for the hardware. These files can be used for visualization, enclosure design, or integration into CAD software.

|--Datasheet: Includes datasheets for components used in the project, providing detailed specifications, electrical characteristics, and pin configurations.

|--Eagle_SCH&PCB: Contains Eagle CAD schematic (.sch) and PCB layout (.brd) files. These are used for circuit design and PCB manufacturing.

|--example: Provides example code and projects to demonstrate how to use the hardware and libraries. These examples help users get started quickly.

|--factory_firmware: Stores pre-compiled factory firmware that can be directly flashed onto the device. This ensures the device runs the default functionality.

|--factory_sourcecode: Contains the source code for the factory firmware, allowing users to modify and rebuild the firmware as needed.

|--libraries: Includes necessary libraries required for compiling and running the project. These libraries provide drivers and additional functionalities for the hardware.

|--Squareline_studio Instructions for use: Squareline_studio instructions: Demonstrate the use of squareline studio.


### 7,Pin definition

#define HSPI_MISO  4
#define HSPI_MOSI  6
#define HSPI_SCLK  5
#define HSPI_SS    19

#define SD_MOSI 6
#define SD_MISO 4
#define SD_SCK 5
#define SD_CS 0 //The chip selector pin is not connected to IO

#define I2S_DOUT 4
#define I2S_BCLK 5
#define I2S_LRC 6

​      cfg.pin_d0 = GPIO_NUM_21;    // B0
​      cfg.pin_d1 = GPIO_NUM_47;    // B1
​      cfg.pin_d2 = GPIO_NUM_48;   // B2
​      cfg.pin_d3 = GPIO_NUM_45;    // B3
​      cfg.pin_d4 = GPIO_NUM_38;    // B4
​      cfg.pin_d5 = GPIO_NUM_9;    // G0
​      cfg.pin_d6 = GPIO_NUM_10;    // G1
​      cfg.pin_d7 = GPIO_NUM_11;    // G2
​      cfg.pin_d8 = GPIO_NUM_12;   // G3
​      cfg.pin_d9 = GPIO_NUM_13;   // G4
​      cfg.pin_d10 = GPIO_NUM_14;   // G5
​      cfg.pin_d11 = GPIO_NUM_7;  // R0
​      cfg.pin_d12 = GPIO_NUM_17;  // R1
​      cfg.pin_d13 = GPIO_NUM_18;  // R2
​      cfg.pin_d14 = GPIO_NUM_3;  // R3
​      cfg.pin_d15 = GPIO_NUM_46;  // R4

      cfg.pin_henable = GPIO_NUM_42;
      cfg.pin_vsync = GPIO_NUM_41;
      cfg.pin_hsync = GPIO_NUM_40;
      cfg.pin_pclk = GPIO_NUM_39;


#define TOUCH_GT911_SCL 16
#define TOUCH_GT911_SDA 15

### 8,Guidelines for downloading some of the files from the github repository

##### 1, open the link in your browser: https://downgit.github.io/

##### 2, paste the URL of the GitHub destination folder in the input box (example format: `https://github.com/username/repository name/tree/branch name/folder path`)

![github_download1](https://github.com/user-attachments/assets/8986d646-34c3-4c71-88ff-08199a8bc14a)

##### 3, even a that way to download the file.

①Click the “Create Download Link” button to create a download link, copy the link and open it with your browser to download the contents of the corresponding folder directly.

![github_download2](https://github.com/user-attachments/assets/cf377329-8766-4f20-b1fa-82ce0e8b26a3)

②Click Download, and you can also download it directly.

![github_download3](https://github.com/user-attachments/assets/47f9f682-ce5d-4507-bc43-ca90acb1ab74)



#### Note: DownGit downloads are stored by default in **your browser's default downloads folder**, usually:

- **Windows**: `C:\Users\your username\Downloads\` (downloads folder)
- **macOS**: `~/Downloads/` (downloads folder)
- **Linux**: `~/Downloads/` (Downloads folder)

If you change the default download path in your browser, the file will be stored in the download location you set. You can find the downloaded file path in your browser's **Download History** (shortcut `Ctrl + J` or `Cmd + J`).


