# 4.3-inch CrowPanel Advance HMI Product Hardware Driver Guide

| Item | Details |
|---|---|
| Document Version | V1.0 |
| Date | 2026-07-30 |
| Author | Codex (compiled from the project schematics and verified example code) |
| Applicable Hardware | ESP32 Display 4.3 inch V1.3 / ESP32-S3-N16R8 |
| Schematic Baseline | `1.3/ESP32 Display 4.3 inch V1.3.sch`, `1.3/ESP32-Display-4.3-inch-V1.3.pdf` |
| Software Baseline | Arduino-ESP32 3.3.8; LovyanGFX 1.2.25; LVGL 9.1.0 |

> **Authority rule:** This document uses the actual configurations from successfully tested examples in the repository as the driver baseline and verifies electrical connections against the Eagle V1.3 schematic netlist. If the two conflict, the code values take precedence, and the discrepancy is recorded under “Differences and Risks.” Capabilities without supporting code evidence are marked as “schematic-confirmed/requires hardware verification” and must not be considered verified.

## 1. Summary of Conclusions

- The main controller is an ESP32-S3-WROOM-1-N16R8 (16 MB Flash, 8 MB PSRAM), and the Arduino examples use ESP32 core 3.3.8.
- The display has an 800×480 resolution, RGB565 pixel format, and a 16-bit parallel RGB interface. The stable code baseline uses a 16 MHz pixel clock; the repository also contains examples using 18 MHz and 21 MHz.
- According to the code, the touch controller is a GT911 with I2C address `0x5D`, SDA/SCL on GPIO15/GPIO16, and a 400 kHz bus speed. The code uses polling rather than interrupts.
- According to the code, the onboard STC8 auxiliary MCU uses I2C address `0x30` and handles board-level control of the backlight, touch recovery, speaker, and other functions. Its firmware protocol is not included; only a few verified commands are available.
- GPIO4/5/6 and GPIO19/20 are multiplexed through CH486F devices and DIP switches. SD, I2S audio, wireless SPI, digital microphone, and UART/Zigbee cannot all be enabled simultaneously without restrictions.
- DHT20, nRF24L01, SX1262, and Zigbee are external example devices connected through expansion ports; they are not onboard devices in the V1.3 schematic.

## 2. Evidence Levels and Porting Principles

| Level | Definition | Usage Principle |
|---|---|---|
| A - Code-verified | A complete initialization and execution path exists in the repository's product examples | Preferred values for porting |
| B - Schematic-confirmed | Explicitly present in the V1.3 schematic but not directly driven by the ESP32 examples | May be used for development, but hardware testing must be added |
| C - External example | Example code has been run with the device connected externally through an expansion port | Valid only when the corresponding module is connected and the DIP switches are set to the appropriate mode |
| D - Inferred | Inferred from device functionality or net relationships, without protocol documentation or actual testing | Must not be presented as a production-ready capability |

When porting, select a GPIO multiplexing scenario before initializing peripherals. Do not simply combine all examples. The display and I2C bus may be treated as persistent infrastructure; functions associated with GPIO4/5/6 and GPIO19/20 must be enabled mutually exclusively according to the product scenario.

## 3. Peripheral Overview

| Category | Device/Interface | Connection and Driver | Status |
|---|---|---|---|
| Main controller | ESP32-S3-WROOM-1-N16R8 | Arduino HAL; Wi-Fi; GPIO Matrix; PSRAM | A |
| Display | 4.3-inch 800×480 IPS | 16-bit RGB565, GPIO3/7/9-14/17/18/21/38/45-48, PCLK 39, DE 42, HSYNC 40, VSYNC 41 | A |
| Touch | GT911 (identified by code) | I2C0, SDA 15, SCL 16, `0x5D`, 400 kHz, polling | A |
| Board-control MCU | STC8H1K28 | Shares I2C 15/16 with the ESP32; accessed at `0x30` by the code | A (incomplete protocol) |
| RTC | PCF8563-compatible device + CR1220 | I2C 15/16, 32.768 kHz crystal | B |
| Storage | MicroSD card socket | SPI: MOSI 6, MISO 4, SCK 5; CS not connected to the main controller | A |
| Audio output | NS4168 + right-channel connector | I2S: DOUT 4, BCLK 5, LRCLK 6; board-control command 248 enables the speaker | A |
| Audio input | LMD3526B261 digital microphone | CLK 19, DATA 20; I2S/PDM characteristics require additional testing against the device documentation | B |
| Wireless expansion | J9/J11/J12 | SPI 4/5/6; control pins 8/19/20/2; 3.3 V/optional 5 V | A/C |
| nRF24L01 | External module | SPI 4/5/6, CSN 19, CE 20 | C |
| SX1262 LoRa | External module | SPI 4/5/6, NSS 8, DIO1 20, RESET 19, BUSY 2 | C |
| Zigbee/UART | External module | UART1 RX 19, TX 20, 115200 8N1 | C |
| Temperature and humidity | External DHT20 | I2C 15/16, address `0x38` | C |
| USB-UART | CH340K + automatic download circuit | USB-C D+/D-; connected to ESP32 UART0, EN, and IO0 | B (programming path implicitly used) |
| Expansion interfaces | J2/J10/J13/J14 | UART0, STC UART, I2C, power | B |
| Power management | TP4059, RY3420, MT9201, MOSFET power gating | USB/external 5 V/Li-ion battery input; 3.3 V and backlight boost | B |
| Actuator | Passive buzzer B2 | Driven by STC P2.7 through an NPN transistor | B |
| Indicators/keys | BOOT, RESET, charging/power LEDs, K1 DIP switch | IO0, EN; status LEDs managed by the STC; K1 selects multiplexed channels | B |

## 4. Authoritative GPIO Assignment Table

| ESP32 GPIO | Actual Usage in Code | Schematic Connection | Driver Characteristics/Notes |
|---:|---|---|---|
| 0 | Virtual CS in SD examples; BOOT | BOOT button, not connected to SD CS | Bootstrapping pin; do not connect an external strong pull-down. The CS parameter used for SD does not represent a physical chip-select signal |
| 1 | Driven low for 120 ms during touch recovery, then changed to input | `TP_INT`, connected to the touch FPC INT pin | **Code-precedence discrepancy**; normally an input/polled during operation and briefly driven low in push-pull mode during recovery |
| 2 | SX1262 BUSY; wireless-port BUSY | J11 pin 5 | Input; used when a LoRa module is connected |
| 3 | RGB R3 | LCD R6 (fourth active red bit of the panel) | RGB peripheral output; related to bootstrapping, so avoid external conflicting levels |
| 4 | SD MISO / nRF24 MISO / SX1262 MISO / I2S DOUT | Multiplexed by U11 to SD, wireless, or the amplifier | Direction varies by mode; concurrent contention is strictly prohibited |
| 5 | SD/wireless SCK / I2S BCLK | Multiplexed by U11 | SPI/I2S clock output; concurrent use is strictly prohibited |
| 6 | SD/wireless MOSI / I2S LRCLK | Multiplexed by U11 | Output; concurrent use is strictly prohibited |
| 7 | RGB R0 (code comment) | LCD R3 | RGB output |
| 8 | SX1262 NSS / wireless CS | J11 pin 6 | SPI chip-select output, idle high |
| 9-14 | RGB G0-G5 (code comments) | LCD G2-G7 | RGB outputs |
| 15 | I2C SDA | Touch, RTC, STC, J11/J13 | Open-drain bidirectional; schematic shows a 4.7 kΩ pull-up to 3.3 V |
| 16 | I2C SCL | Touch, RTC, STC, J11/J13 | Open-drain output; schematic shows a 4.7 kΩ pull-up to 3.3 V |
| 17,18 | RGB R1,R2 (code comments) | LCD R4,R5 | RGB outputs |
| 19 | nRF24 CSN / SX1262 RESET / UART1 RX / MIC CLK | Multiplexed by U9; J12 pin 1 | Direction varies by mode; some code only sets it to OUTPUT without writing a level |
| 20 | nRF24 CE / SX1262 DIO1 / UART1 TX / MIC DATA | Multiplexed by U9; J12 pin 2 | Direction varies by mode; must match the DIP-switch position |
| 21 | RGB B0 (code comment) | LCD B3 | RGB output |
| 38 | RGB B4 (code comment) | LCD B7 | RGB output |
| 39 | RGB PCLK | LCD CLK, through 0 Ω | Recommended baseline: 16 MHz; repository examples also use 18/21 MHz |
| 40 | RGB HSYNC | LCD HSYNC | Active low |
| 41 | RGB VSYNC | LCD VSYNC | Active low |
| 42 | RGB DE | LCD DE | Data-enable output |
| 45,46 | RGB B3/R4 (code comments) | LCD B6/R7 | Related to bootstrapping; outputs while the display is running |
| 47,48 | RGB B1/B2 (code comments) | LCD B4/B5 | RGB outputs |
| RXD0/TXD0 | UART0 | CH340K, J2; connected to J10 through level isolation | Debug/programming UART; example baud rates are 9600 or 115200 |

> The LovyanGFX `pin_d0...pin_d15` comments use compressed RGB565 bit ordering, while the schematic net names use the panel's native R3-R7, G2-G7, and B3-B7 bit names. They are logically consistent and do not indicate a wiring conflict.

## 5. GPIO Multiplexing Modes

U11 multiplexes GPIO4/5/6, and U9 multiplexes GPIO19/20; K1 provides `SEL1/SEL0`. The schematic explicitly specifies only that UART1 output requires `S1=0, S0=1`. Other positions must be confirmed against the silkscreen or through hardware continuity testing and cannot be selected through software alone.

| Scenario | GPIO4 | GPIO5 | GPIO6 | GPIO19 | GPIO20 | Other |
|---|---|---|---|---|---|---|
| MicroSD | MISO | SCK | MOSI | Unused/must not conflict | Unused/must not conflict | SD CS is not physically connected |
| I2S speaker | SDATA | BCLK | LRCLK | Unused | Unused | Write 248 to `0x30` first |
| nRF24L01 | MISO | SCK | MOSI | CSN | CE | 250 kbps, channel 50 |
| SX1262 | MISO | SCK | MOSI | RESET | DIO1 | NSS 8, BUSY 2 |
| Zigbee UART | Unused | Unused | Unused | UART1 RX | UART1 TX | 115200 8N1; schematic requires S1=0, S0=1 |
| Onboard digital MIC | Unused | Unused | Unused | CLK | DATA | No capture example in the repository; format remains to be confirmed |

## 6. Detailed Driver Guide

### 6.1 ESP32-S3 Main Controller and Boot/Programming

**Hardware.** U5 is an ESP32-S3-WROOM-1, with N16R8 indicated in the schematic title. EN is active high; K4 pulls EN low to reset. K3 pulls GPIO0 low to enter download mode during reset. The CH340K automatically controls EN/IO0 through DTR/RTS and the UMH3NTN, while USB-C J1 also supplies power.

**Software layer.** Arduino-ESP32 3.3.8, using the underlying ESP-IDF HAL/drivers. Display buffers use PSRAM, while the LVGL partial double buffers are explicitly allocated in internal RAM.

```cpp
Serial.begin(115200);
// BOOT: GPIO0 low during reset; RESET: EN low.
```

**Caution.** GPIO0, 3, 45, and 46 are associated with ESP32-S3 bootstrapping. Peripherals must not force incorrect levels during power-up. The Flash/PSRAM settings for the N16R8 must match the board configuration in the Arduino IDE.

### 6.2 RGB LCD

**Interface and timing.** 800×480, RGB565, 16-bit parallel, using LovyanGFX `Bus_RGB` + `Panel_RGB`. The code mapping is as follows:

```cpp
// D0..D15
21, 47, 48, 45, 38, 9, 10, 11, 12, 13, 14, 7, 17, 18, 3, 46
// DE, VSYNC, HSYNC, PCLK
42, 41, 40, 39
```

Recommended baseline parameters:

| Parameter | Value |
|---|---:|
| Resolution/memory dimensions | 800×480 |
| Pixel format | RGB565 |
| PCLK | 16 MHz (recommended conservative value) |
| HSYNC polarity/front porch/pulse width/back porch | Active low / 8 / 4 / 8 |
| VSYNC polarity/front porch/pulse width/back porch | Active low / 8 / 4 / 8 |
| PCLK idle | High |
| Framebuffer | PSRAM |

The repository's SD examples also use 18 MHz and 21 MHz. Start porting at 16 MHz; after increasing the frequency, check for display corruption, jitter, temperature-related drift, and long-duration DMA refresh stability.

**Initialization.** After board-control I2C is ready and the backlight command has been written, call `gfx.init()` and `gfx.initDMA()`. The LVGL 9.1.0 example uses 40-line RGB565 double buffers: each buffer is `800×40×2 = 64,000` bytes and is allocated in internal RAM.

**Backlight.** The LED boost circuit is controlled by the MT9201 and STC8 rather than direct ESP32 PWM. The code writes `0` to `0x30` for maximum brightness and `245` to turn the backlight off, suggesting that `0..245` is an inverted brightness scale. No complete protocol or fade timing was found.

### 6.3 GT911 Capacitive Touch

**Code baseline.** LovyanGFX `Touch_GT911`, I2C0: SDA GPIO15, SCL GPIO16, 400 kHz, address `0x5D`, ranges 0..800/0..480, and `offset_rotation=0`. The code sets `pin_int=-1` and `pin_rst=-1` and uses polling.

```cpp
cfg.pin_sda = GPIO_NUM_15;
cfg.pin_scl = GPIO_NUM_16;
cfg.freq = 400000;
cfg.i2c_addr = 0x5D;
```

**Recovery sequence.** If `0x30` or `0x5D` does not respond, the code first writes 250 to `0x30`, then configures GPIO1 as a push-pull output and drives it low for 120 ms. It then restores GPIO1 to input mode, waits 100 ms, and retries.

**Discrepancy.** In the schematic, the GPIO1 net is named `TP_INT`, while touch reset `TP_RST` is connected to STC8 P1.7 rather than the ESP32. However, the code uses a low pulse on GPIO1 to recover touch operation. The code behavior must be preserved during maintenance; do not configure GPIO1 permanently as an interrupt-only input based solely on the net name. The board-level electrical mechanism behind this recovery procedure requires oscilloscope verification.

### 6.4 STC8 Board-Control MCU (I2C Address 0x30)

**Connection.** U13, an STC8H1K28, shares the GPIO15/16 I2C bus with the ESP32; the bus also connects to the touch controller, RTC, and expansion ports. The STC directly controls LCD backlight power/enable, the buzzer, charging-status indicators, touch reset, audio-amplifier power, and its own UART on J14.

**Verified commands.** The ESP32 acts as the I2C controller and writes one byte to the 7-bit address `0x30`:

| Command | Meaning in Code Comments | Evidence |
|---:|---|---|
| 0 | Maximum backlight brightness | Multiple LCD/SD/RF24 examples |
| 245 | Backlight off | Code comment; no independent call found |
| 248 | Turn on speaker | Online audio example |
| 250 | Activate/recover touch | All board-level startup retry paths |

```cpp
Wire.beginTransmission(0x30);
Wire.write(command);
Wire.endTransmission();
```

**Risk.** The repository does not contain the STC firmware or a complete command table. Unknown commands, returned data, intermediate backlight levels, and the command to turn off the speaker must not be guessed. Some examples refer to `0x30` as an “extended IC”; the actual device in the schematic is an STC8 MCU.

### 6.5 Shared I2C Bus, RTC, and External DHT20

**Bus.** `Wire.begin(15, 16)`; the schematic shows 4.7 kΩ pull-ups from both SDA and SCL to 3.3 V. The touch code requires 400 kHz. When Arduino `Wire.begin()` does not explicitly set the frequency, it depends on the core's default value. If the entire bus is standardized at 400 kHz, regression testing must cover the STC8, RTC, and DHT20.

| Address | Device | Status |
|---:|---|---|
| `0x30` | STC8 board-control MCU | A |
| `0x38` | External DHT20 | C; library default address |
| `0x51` (common PCF8563 address) | Onboard RTC | B/D; device confirmed by schematic, not accessed by repository code, and address requires confirmation by scanning |
| `0x5D` | GT911 touch | A; the code also notes `0x14` as a possible address, but this board uses `0x5D` |

RTC U4 uses a 32.768 kHz crystal and CR1220 backup battery; INT/CLK_OUT is not connected to the ESP32. When porting the RTC, only I2C polling can be used, and tests must be added for initial power-up, retention during power loss, and the oscillator-stop flag.

The DHT20 example calls `dht20.begin()` and reads temperature and humidity once per second. The library sends `AC 33 00`, polls every 10 ms up to 10 times, and uses the default address `0x38`. The DHT20 is a peripheral connected to the J13 I2C expansion port, not an onboard sensor.

### 6.6 MicroSD

**Connection.** GPIO6 MOSI, GPIO4 MISO, GPIO5 SCK; 3.3 V supply. The `SDCS` net in the schematic connects only to the card socket and a pull-up/test network; it is not connected to an ESP32 GPIO.

**Code baseline.** The ESP32-S3 uses `SPIClass(FSPI)`; the code passes `SD_CS=0`, and the stable example uses 40 MHz:

```cpp
SPIClass SD_SPI(FSPI);
SD_SPI.begin(5, 4, 6, 0);
SD.begin(0, SD_SPI, 40000000);
```

Another similar example in the root directory uses 80 MHz, but 40 MHz is a more appropriate porting baseline. The card socket is effectively always selected in hardware and cannot share transactions with other devices on the same physical SPI branch. GPIO0 is only an API placeholder parameter, not the SD chip-select line; also avoid changing the BOOT level as a consequence.

### 6.7 I2S Audio Output and NS4168

**Connection.** GPIO5 BCLK, GPIO6 LRCLK, and GPIO4 SDATA are routed through U11 to the NS4168. The amplifier provides differential right-channel outputs `ROUT+/-` to J15. Do not connect either output directly to ground or use them as a single-ended headphone output.

**Software.** ESP32-audioI2S 3.4.7, Wi-Fi network streaming, volume range 0..21, with the example set to 20. First initialize the shared I2C bus and write 248 to the board-control MCU, then configure the audio pins and continuously call `audio.loop()`.

```cpp
sendI2CCommand(248);
audio.setPinout(5, 6, 4); // BCLK, LRCLK, DOUT
audio.setVolume(20);
```

The NS4168 CTRL/power supply is also controlled by the STC8. Audio shares GPIO4/5/6 with SD/wireless SPI; before switching, stop and deinitialize the previous bus driver.

### 6.8 Onboard Digital Microphone

MIC1 is an LMD3526B261-OFA01: powered by 3.3 V, with GPIO19 connected to CLK and GPIO20 connected to DATA; L/R is fixed to a channel through a resistor. The signals are multiplexed through U9. The repository contains no recording or I2S/PDM initialization example, so only the electrical connections are confirmed. The sampling format, clock frequency, bit width, and channel polarity must all be determined from the device documentation and verified through actual testing.

### 6.9 External nRF24L01 Module

**Connection and configuration.** SPI: SCK 5, MISO 4, MOSI 6; CSN 19, CE 20; 3.3 V supply. RF24 1.6.1, address `"00001"`, maximum PA level, 250 kbps, RF channel 50. The receiver opens pipe 0 and calls `startListening()`; the transmitter calls `openWritingPipe()` and `stopListening()`.

```cpp
SPIClass hspi(HSPI);
hspi.begin(5, 4, 6, 19);
RF24 radio(20, 19); // CE, CSN
radio.setDataRate(RF24_250KBPS);
radio.setChannel(50);
```

GPIO19 is used both as the SPI SS parameter and as the RF24 CSN, which matches the actual code behavior. When connecting an external high-power nRF24 module, add sufficient 3.3 V decoupling close to the module and verify the available capacity of the onboard regulator.

### 6.10 External SX1262 LoRa/LoRaWAN Module

**Connection.** NSS 8, DIO1 20, RESET 19, BUSY 2, SCK 5, MISO 4, MOSI 6. Uses RadioLib 7.7.1:

```cpp
SPI.begin(5, 4, 6, 8);
SX1262 radio = new Module(8, 20, 19, 2, SPI);
radio.begin();
radio.setCurrentLimit(140.0);
radio.setTCXO(3.3);
```

The LoRaWAN example supports EU868/US915, with the default node constructed using EU868 and subBand=1. It supports ABP/OTAA, ADR, data rate, transmit power, RX2 data rate, and duty-cycle control. `setTCXO(3.3)` applies only to modules with a 3.3 V TCXO and must not be copied to modules using a standard crystal. Frequency band, antenna, and regulatory parameters must be reconfigured for the sales region.

### 6.11 External Zigbee/UART Module

GPIO19 is UART1 RX, GPIO20 is UART1 TX, and the configuration is `115200, SERIAL_8N1`. The schematic explicitly requires DIP-switch settings `S1=0, S0=1` when using UART1_OUT.

```cpp
Serial1.begin(115200, SERIAL_8N1, 19, 20);
```

This mode is mutually exclusive with the onboard digital microphone and the GPIO19/20 control signals for nRF24 and SX1262. Confirm that the external module uses 3.3 V logic levels; a 5 V UART must not be connected directly to the ESP32.

### 6.12 USB-UART, UART0, and Expansion Ports

- J1 USB-C: USB 2.0 D+/D- connects to the CH340K rather than the ESP32-S3 native USB data interface; VBUS also enters the power path.
- J2: ESP32 UART0 RX, TX, 3V3, GND.
- J10: UART0 RX and TX through a level-shifting/isolation network, external 5 V input, and GND.- J13: SCL (GPIO16), SDA (GPIO15), 3V3_OUT, and GND; suitable for I2C peripherals.

- J14: STC8 TX, RX, VIN, and GND; this is not the ESP32 UART.
- J9/J11/J12: Wireless/multiplexed expansion; see the GPIO multiplexing table for specific functions.

UART0 example: lesson-01 uses 9600, while other debugging code generally uses 115200. Production firmware should standardize on 115200 and avoid continuously occupying the download/logging channel with application UART traffic.

### 6.13 Power, Charging, and Backlight Power

**Power path (confirmed by schematic).** USB-C VBUS, J10 external 5 V, and the battery input are combined into VIN through Schottky diodes/MOSFETs; the TP4059 charges a single-cell lithium battery; the RY3420 generates 3.3 V; the MT9201 provides boosted power for the LCD LEDs; and multiple P-channel MOSFETs control peripheral power. J3 is the battery connector, with VBAT on the positive terminal and the remaining pads connected to ground.

**Software boundary.** The ESP32 does not directly configure the TP4059, RY3420, or MT9201. Charging `CHG/STD`, backlight enable, and power gating are monitored or controlled by the STC8. The repository does not contain a battery-level ADC, charging-status readout, or shutdown API. These functions are therefore classified as Level B and cannot be used by the main application until the STC protocol is available.

**Safety.** Use only a single-cell lithium battery system; verify battery polarity, permitted charging current, and temperature conditions. The expansion-port `3V3_OUT` is gated by Q8 and must not be assumed to remain active whenever the board is powered. J9 `IOT_5V` connects to VIN through L3, which is marked NC. Depending on production assembly, the 5 V pin may be unpowered and must be verified by measurement.

### 6.14 Buzzer, LEDs, Buttons, and DIP Switches

- Passive buzzer B2 is driven on the low side by STC8 P2.7 through Q4, an NPN transistor, and includes a flyback/protection network. There is no direct ESP32 GPIO connection, and the repository contains no buzzer command.
- K3 BOOT: Pulls GPIO0 low when pressed. K4 RESET: Pulls ESP32_EN low when pressed.
- D14 is a hardware power indicator LED. The charging/completion states of dual-color LED D1 are controlled through the STC8 and TP4059 status path.
- The two-position K1 DIP switch generates `SEL1/SEL0` and also selects the U9/U11 signal routing. Before changing functions, power off the board or first place the relevant GPIOs in a high-impedance state.

## 7. Recommended Initialization Sequence

```text
1. Start and initialize Serial(115200)
2. Wire.begin(15,16), then wait 50 ms
3. Probe 0x30 and 0x5D
4. If probing fails: command 250 -> drive GPIO1 low for 120 ms -> set GPIO1 as input -> wait 100 ms -> retry
5. Write the backlight value (typically 0) to 0x30
6. gfx.init() -> gfx.initDMA() -> clear the display
7. Initialize LVGL/touch polling
8. Based on the DIP-switch setting and product use case, initialize only one multiplexed function group
9. For audio use cases, additionally send command 248; complete address probing for external sensors
```

Production firmware should not wait indefinitely in a loop for I2C devices. It should implement timeouts, a degraded-mode UI, and error codes. This document retains the timing from the original examples as a reference for hardware recovery.

## 8. Schematic and Code Discrepancies

| Item | Schematic/Label | Verified Code | Adopted Value and Handling |
|---|---|---|---|
| Touch GPIO1 | `TP_INT` | Driven low for 120 ms, then restored as an input | Use the recovery sequence from the code; poll during normal operation, with the root cause pending hardware confirmation |
| Touch reset | `TP_RST` connected to STC8 P1.7 | LovyanGFX `pin_rst=-1` | The ESP32 does not reset it directly; recover through board-control command 250 plus the GPIO1 sequence |
| MicroSD CS | `SDCS` not connected to the ESP32 | `SD_CS=0` | There is no physical CS; GPIO0 is an API placeholder and must not be treated as a conventional multi-device SPI chip-select |
| LCD RGB bit names | Schematic uses R3-R7/G2-G7/B3-B7 | Code comments use R0-R4/G0-G5/B0-B4 | This is a naming difference caused by compacting the active bits; use the GPIO mapping as authoritative |
| LCD PCLK | No software frequency specified in the schematic | Multiple versions use 16/18/21 MHz | Use 16 MHz as the porting baseline; higher frequencies require regression testing |
| Touch address | Hardware diagram does not explicitly specify an address | `0x5D`; code notes that `0x14` may also be possible | Use `0x5D` on this board; test `0x14` only after replacing the display or changing strap settings |
| GPIO19 initialization | Multiplexed signal in the schematic | Some display/LVGL examples set it to OUTPUT without explanation | Do not treat this as a required display step; initialize its direction according to the selected U9 mode |
| Wireless chip select | J9 pin 1 through the U9/U11 network | nRF24 uses GPIO19; SX1262 uses GPIO8 | These are two different module configurations; use the corresponding code for each and do not combine them |
| SD SPI frequency | No value specified in the schematic | 40 MHz and 80 MHz | Use 40 MHz as the conservative baseline; 80 MHz requires card-type and temperature regression testing |
| I2C frequency | Touch driver uses 400 kHz; `Wire.begin` does not explicitly set the frequency | Multiple devices share the bus | If using 400 kHz, verify the STC8, RTC, and DHT20; otherwise retain the core default |
| U13 function name | STC8H1K28 in the schematic | Referred to as “extended IC/microcontroller” in the code | Standardize the documentation on “STC8 board-control MCU”; protocol version unknown |

## 9. Risks and Precautions

1. **Multiplexing conflicts (high).** GPIO4/5/6 and GPIO19/20 may be used simultaneously by different libraries. Before combining projects, implement a single mode state machine. When switching modes, stop SPI/I2S/UART first, then change the DIP-switch/routing configuration and reinitialize the interfaces.
2. **No SD chip select (high).** The card remains connected to the bus at all times. SD mode cannot simply share the bus with wireless SPI/audio, and the card may drive MISO during power-up.
3. **3.3 V logic levels (high).** The ESP32, touch controller, SD card, and wireless/I2C expansion interfaces are all designed for 3.3 V logic. External 5 V UART/I2C/SPI devices require level shifting.
4. **Boot strapping (high).** The power-up levels on GPIO0/3/45/46 may affect boot behavior. The display and expansion modules must not strongly drive these pins during the reset sampling window.
5. **Missing board-control protocol (high).** The STC8 firmware and complete command set are not included in the repository. Before porting backlight, speaker, buzzer, power, and charging-status functions, obtain the protocol and firmware version from the hardware team/vendor.
6. **Backlight/speaker drive characteristics (medium-high).** The LCD LEDs use a boosted constant-current circuit, and the speaker uses a BTL differential output. Do not treat the backlight pin or amplifier outputs as standard GPIOs or single-ended audio.
7. **Pixel/SPI clock margin (medium).** Successful operation with the repository code does not guarantee stability across all production lots, temperatures, SD cards, and ribbon cables. Use a 16 MHz PCLK and 40 MHz SD SPI by default for production, and perform stress testing.
8. **I2C bus blocking (medium).** The board controller, touch controller, RTC, and external sensors share the same bus. Before adding devices, check for address conflicts, total pull-up resistance, trace/cable length, and clock stretching.
9. **RTC backup battery (medium).** CR1220 polarity, initial assembly voltage, and shipping requirements must be incorporated into the manufacturing process. Software must handle invalid time values and oscillator-stop conditions.
10. **Example credentials (high, software security).** The online-audio example contains plaintext Wi-Fi SSIDs/passwords. Production firmware must not reuse them; replace them and purge historically exposed credentials before delivery.
11. **LoRa regulatory compliance (high).** EU868/US915 settings, transmit power, duty cycle, and antenna must comply with regional certification requirements. The TCXO voltage must also match the module version.

## 10. Software Dependencies and Porting Checklist

| Function | Software Layer/Version | Key Entry Point |
|---|---|---|
| Basic HAL | Arduino-ESP32 3.3.8 | `Wire`, `SPIClass`, `Serial`, GPIO |
| LCD/touch | LovyanGFX 1.2.25 | `Bus_RGB`, `Panel_RGB`, `Touch_GT911` |
| GUI | LVGL 9.1.0 | 800×480, 40-row double buffering, partial render |
| SD | Arduino ESP32 `FS`/`SD` | FSPI, 40 MHz, no physical CS |
| Network audio | ESP32-audioI2S 3.4.7 | GPIO 5/6/4, continuous `audio.loop()` |
| nRF24 | RF24 1.6.1 | Custom HSPI, CE 20/CSN 19 |
| LoRaWAN | RadioLib 7.7.1 | SX1262 + `LoRaWANNode` |
| DHT20 | Repository `Crowbits_DHT20` | Wire, address 0x38 |

When porting to ESP-IDF, the corresponding layers are `esp_lcd_rgb_panel`, `esp_lcd_touch_gt911` (or a custom GT911 driver), I2C master, SPI master/SDSPI, I2S STD/PDM, UART, and GPIO. The board-control MCU startup commands and multiplexing constraints must be preserved.

## 11. Maintenance Acceptance Checklist

- [ ] Flashing, BOOT, RESET, and serial logging operate correctly.
- [ ] Both cold boot and warm reset can detect `0x30` and `0x5D`, and the touch recovery path works.
- [ ] LCD solid-color, grayscale, checkerboard, and 24-hour DMA refresh tests complete without display corruption.
- [ ] Touch behavior is correct at all four corners, along the edges, under rotation, and with multitouch.
- [ ] SD repeatedly mounts, reads and writes large files, and recovers from power loss using the target card type.
- [ ] The selected multiplexing position matches the software mode, with no GPIO contention in other positions.
- [ ] Audio is free of pops, the BTL output is not grounded, and temperature rise at maximum volume is acceptable.
- [ ] The 3.3 V rail does not sag when the wireless module transmits at maximum power, and EMI/RF performance meets requirements.
- [ ] After all I2C peripherals are connected, there are no address conflicts or long-term bus lockups.
- [ ] USB, external 5 V, and lithium-battery power paths and charging status have all been verified.
- [ ] Example Wi-Fi credentials have been removed, and LoRa regional parameters match certification requirements.

## 12. Code and Hardware Documentation Index

- Schematic: `1.3/ESP32 Display 4.3 inch V1.3.sch`
- Schematic PDF: `1.3/ESP32-Display-4.3-inch-V1.3.pdf`
- LCD/touch/LVGL: `Arduino/lesson-03/BigInch_LVGL/`
- SD: `Arduino/lesson-04/SD_CrowPanel_ESP32_Advance_HMI_4_3_5_0_7_0/`
- I2C DHT20 expansion example: `Arduino/lesson-05/Port_CrowPanel_ESP32_Advance_HMI_4_3_5_0_7_0/`
- nRF24: `Arduino/lesson-06/READ/`, `Arduino/lesson-06/WRITE/`
- SX1262/LoRaWAN: `Arduino/lesson-07/code/sendATcommands_7.0/`
- Zigbee/UART: `Arduino/lesson-09/zigbee_7.0/`
- I2S network audio: `Arduino/lesson-02/OnlineAudio_large/`

## 13. Current Documentation Gaps

The following materials are not included in the repository and should be prioritized for future maintenance: the STC8 board-control firmware and complete I2C protocol; exact LCD panel and touchscreen part numbers/data sheets; the MT9201 backlight current design value; the TP4059 configured charging current; the digital microphone interface format; the official function table for all four DIP-switch combinations; external pinout diagrams for each expansion port; and the production hardware validation report. Once these materials are available, this document should be versioned accordingly, and Level B/D items should be updated to Level A conclusions verified through testing.