#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#include <pigpio.h>

/*
# ir_receive.c
# 2019-3-3
# Public Domain

gcc -Wall -pthread -o ir_receive ir_receive.c -lpigpio -lrt

sudo ./ir_receive       # Usage
sudo ./ir_receive port  # get ir receive data from port
*/

uint32_t tick[80],last_tick=0;
int  tick_index=0, end_capture=0;

void aFunction(int gpio, int level, uint32_t tick_)
{
   if(tick_index>0 && (tick_-last_tick) > 10000){
      end_capture=1;
   }
   tick[tick_index]=tick_;
   last_tick=tick_;
   tick_index += 1;
}

int main(int argc, char *argv[])
{
   int port;
   int bits[32];
   int address, address_c, command, command_c, button, starting;
   int secs, mics, secs2, mics2;

   if (argc != 2)
   {
       printf("Usage: sudo ./ir_receive port\n");
       printf("port: BCM GPIO port number\n");
       printf("Example: sudo ./ir_receive 18\n");

       return 0;
   }
   else
   {
      port = atoi(argv[1]);
   }

   if (gpioInitialise() < 0) return -1;

   gpioSetMode(port, PI_INPUT); // port for input

   // call aFunction whenever GPIO 24 changes state
   gpioSetAlertFunc(port,aFunction);
   printf("Please press a key on the remote controller in 3 seconds...\n");

   gpioTime(PI_TIME_RELATIVE, &secs, &mics);
   while(1){
       gpioTime(PI_TIME_RELATIVE, &secs2, &mics2);
       if((secs2-secs)*1000000+(mics2-mics)>3000000) break; // within 3 seconds to press button
       if(end_capture==1) break; // end to capture button of remote controller
   }
   gpioTerminate();


   // raw tick
   //printf("tick amount =%d\n", tick_index);
   //for(int i=0;i<tick_index;i++) printf("%d %d\n",tick[i], tick[i+1]-tick[i]);
   //printf("\n");

   for(int i=0; i<tick_index-1;i++) tick[i]=tick[i+1]-tick[i]; // convert into intervals
   //for(int i=0;i<tick_index-1;i++) printf("%d ",tick[i]);
   //printf("\n");

   starting=0;
   for(int i=0;i<tick_index-1;i++)
      if(abs(tick[i]-9000)<100 && abs(tick[i+1]-4500)<100) starting=i; //get the starting of NEC IR protocol

   for(int i=0;i<32;i++) bits[i] = (abs(tick[i*2+3+starting]-tick[i*2+2+starting])<500?0:1); // convert into bits
   //for(int i=0;i<32;i++) printf("%d ",bits[i]);
   //printf("\n");

   address=0;
   for(int i=0;i<8;i++){
       address <<= 1;
       address += bits[7-i];
   }

   address_c=0;
   for(int i=0;i<8;i++){
       address_c <<= 1;
       address_c += bits[15-i];
   }
   command=0;
   for(int i=0;i<8;i++){
       command <<= 1;
       command += bits[23-i];
   }

   command_c=0;
   for(int i=0;i<8;i++){
       command_c <<= 1;
       command_c += bits[31-i];
   }

   //printf("%02X %02X %02X %02X\n",address,address_c,command,command_c);

   if(((address^address_c)==0xff) && ((command^command_c)==0xff)){
       button = (address << 8) + command;
       printf("Button = %04X (NEC IR protocol)\n",button);
       return 0;
   } else {
       printf("Wrong check sum!\n");
       return -1;
   }
}
