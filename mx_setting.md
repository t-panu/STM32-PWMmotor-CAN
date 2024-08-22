Here is how to config STM32F767ZI to create PWM signal </br>
- Using PE9 for EN , PE11 and PF14 for direction (GPIO_OUTPUT) </br>
![image](https://github.com/user-attachments/assets/8fcf4294-1c91-4a35-9575-6321b0f76f3a)
- Clock Confuguration tab , HCLK set ot 216 MHz (MHz Max) and look at APB1 and APB2. Remember these number. </br>
![image](https://github.com/user-attachments/assets/b2e9c569-32b5-408b-b297-19f2c2c8e430)
- In this case , we're gonna use PE9 pin as timer pin , it is declared as TIM1_CH1 if you point at the pin in the right side </br>
![image](https://github.com/user-attachments/assets/936fcc1a-b3c5-448a-90c3-da6339e1853e)
- PinOut & Confuguration tab > Timers > TIM1 </br>
![image](https://github.com/user-attachments/assets/04420379-8bed-4b3c-a6e3-88c363512d18)
- <b>THIS IS THE MOST TRICKY PART!!</b> Parameter setting </br>
![image](https://github.com/user-attachments/assets/5df216ae-36b9-4d79-ace5-8a84a955bf9a)
    - We're gonna use 500Hz of PWM for L298N motor driver
    - TIMER1 is connected to APB2 (according to 20th page of [datasheet](https://www.st.com/resource/en/datasheet/stm32f765bi.pdf) which is 108MHz
    - We divide with _<b><span style="color: green">prescale</span></b>_ (108) >> 108x10^7 / 108 = 10^7  
    - Wedicide again with _<b><span style="color: green">count period</span></b>_ (2000) >> 10^7 / 2000 = 500 Hz
    - We need to minus 1 tobe 108-1 and 2000-1 somehow (T^T)
