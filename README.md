# ESP32 Smart Medicine Reminder

An ESP32-based smart medicine reminder system that helps patients take medicines on time and alerts a guardian through Telegram when a scheduled medicine is missed.

## Features

- Real-time scheduling using DS3231 RTC
- OLED display for time and medicine information
- 3 medicine compartments
- Individual LEDs for each medicine
- Push buttons to confirm medicine intake
- Buzzer alert at scheduled medicine times
- Missed medicine detection
- Telegram notification to guardian for missed doses

## Hardware Used

- ESP32
- DS3231 RTC Module
- 0.96" OLED Display (SSD1306)
- 3 LEDs
- 3 Push Buttons
- Buzzer
- Connecting wires

## Pin Configuration

| **Component **         | **ESP32 Pin **|
| Buzzer                 | GPIO 4    |
| LED 1 – Paracetamol    | GPIO 2    |
| LED 2 – Dolo 65        | GPIO 15   |
| LED 3 – B-Hold         | GPIO 5    |
| Button 1 – Paracetamol | GPIO 18   |
| Button 2 – Dolo 65     | GPIO 19   |
| Button 3 – B-Hold      | GPIO 23   |
| OLED Display           | I²C       |
| DS3231 RTC             | I²C       |

## Software Used

- Arduino IDE
- Embedded C/C++
- Telegram Bot API

## Working

The DS3231 RTC provides the current time to the ESP32. At the scheduled medicine time, the corresponding LED and buzzer are activated. The user can press the corresponding button after taking the medicine.

If the medicine is not taken within the defined time, the ESP32 detects the missed dosage and sends a notification to the guardian through Telegram.

## Project Setup

![Project Setup](Images/Project_Setup.jpeg)

## Telegram Notification

![Telegram Notification](Images/Telegram_Notification.jpeg)

## Future Improvements

- Medicine history and dosage logging
- Daily medicine compliance statistics
- Raspberry Pi / Embedded Linux implementation
- Web-based monitoring dashboard
