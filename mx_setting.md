Here is how to config STM32F767ZI to create PWM signal </br>
- Using PE9 for EN , PE11 and PF14 for direction (GPIO_OUTPUT) </br>
![image](https://github.com/user-attachments/assets/8fcf4294-1c91-4a35-9575-6321b0f76f3a)
- Clock Confuguration tab , HCLK set ot 216 MHz (MHz Max) and look at APB1 and APB2. Remember these number. </br>
![image](https://github.com/user-attachments/assets/b2e9c569-32b5-408b-b297-19f2c2c8e430)
- In this case , we're gonna use PE9 pin as timer pin , it is declared as TIM1_CH1 if you point at the pin in the right side </br>
![image](https://github.com/user-attachments/assets/936fcc1a-b3c5-448a-90c3-da6339e1853e)
- PinOut & Confuguration tab > Timers > TIM1 </br>
![image](https://github.com/user-attachments/assets/04420379-8bed-4b3c-a6e3-88c363512d18)
- <b>THIS IS THE MOST TRICKY PART!!</b> Parameter setting
