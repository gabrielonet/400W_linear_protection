TODO 
something is wrong !!
Coz arduino is not multithreading , in main loop I cannot simple insert Dallas DS18B20 temperature sensor since it takes about
700 miliseconds to read it. During this time if I transmit and high SWR occurs ,then until Dallas breaks the loop transistors
are allready destroyed since no SWR protection .
We got some options here :
 1. PTT is on intrerupt pin so it is main prio
    if so then stop reading dallas temp sensor
 What if i forgot transmit for a long time ?
  We should count PTT start millis and if  read milis  - last  PTT millis is greater than 30 seconds (continuous transmit ) then 
  Arduino shoud stop  QRO amplifier and show some error on display
  main draw back is that during this time no temperature is read , so high temperature  protection 
      is not to be concidered on real time
  
