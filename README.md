28_April_2019
Got PTT line executed via intrerupts
Got encoder executed via intrerupts
Got encoder switch via intrerupts
Dallas temperature sensor is stripped to integer value (don't need decimals)
     In this way I can read it preety fast without blocking main loop
Pwm fan is in place but using dedicated PWM FAN 4 wire (Vcc,Gnd, Pwm controll ,fan speed output )
     So a Digital pin was set to directly PWM FAN from 20 % minimum to 100% in 40 - 80 Celsius range
#############################################################################


