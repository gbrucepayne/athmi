# AT Command HMI for Arduino framework

Provides a simple user interface for basic testing of AT command interface
to a modem over a UART.

1. Connect to the Microcontroller unit (MCU) via USB cable from a PC.
1. Using platformio interface **Upload and Monitor**
1. In the terminal window, type AT commands and the response will be displayed.

Response reading is simplified based on reading until the linefeed character,
so a response like `<cr><lf>OK<cr><lf>` will display as 2 separate responses.
