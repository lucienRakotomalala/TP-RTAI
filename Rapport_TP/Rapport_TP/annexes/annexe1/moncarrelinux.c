/* compile using  "gcc -o swave swave.c -lrt -Wall"  */
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
#include <signal.h>
#include <sys/io.h>
#include </usr/local/src/comedilib/include/comedilib.h>

#define NSEC_PER_SEC    1000000000
#define DIO 2
#define SIZETAB 5000

comedi_t *cf;

/* array who contain all outputs periodes */
   unsigned int deltat[SIZETAB];

/* the struct timespec consists of nanoseconds
 * and seconds. if the nanoseconds are getting
 * bigger than 1000000000 (= 1 second) the
 * variable containing seconds has to be
 * incremented and the nanoseconds decremented
 * by 1000000000.
 */
static inline void tsnorm(struct timespec *ts)
{
   while (ts->tv_nsec >= NSEC_PER_SEC) {
      ts->tv_nsec -= NSEC_PER_SEC;
      ts->tv_sec++;
   }
}

/* increment counter and write to parallelport */
void out()
{
   static unsigned char state=0;
   comedi_dio_write(cf,DIO,0,state);
	state=!state;
}

/* for print the datas into a file */
void IntHandler(int sig)
{
	FILE *file;
	int i;
	file = fopen("/home/m2istr_13/Documents/TP_RTAI/M2ISTR_RTAI/tp1_mesures_TR/I/delta.res","w");
	if(file==NULL)
	{
		fprintf(stderr,"Erreur de creation du fichier\n");
	}
	for(i=0;i<SIZETAB;i++)
	{
		fprintf(file,"%d %d\n",i,deltat[i]);
	}
	fclose(file);
	exit(0);
}

/*****************************  MAIN  ********************************************/
int main()
{
   struct timespec t, /* for output signal */
   		  t1, 		  /* for get the time at the beginning of the loop */
   	      t2;         /* for get the time at the end of the loop */
  
   /* default interval = 50000ns = 50us
    * cycle duration = 100us
    */
   int interval=50000,
       i = 0; /* for moving into deltat */
	   
   /*   attatch the Ctr+C action to print file  */
   signal(SIGINT,IntHandler);
   cf=comedi_open("/dev/comedi0");
   if(cf==NULL)
   {
      comedi_perror("Comedi fails to open");
      return -1;
   }

   // Configure le device ANALOG_OUTPUT pour envoyer les donnees signaux
   comedi_dio_config(cf,DIO,0,COMEDI_OUTPUT);
   comedi_dio_config(cf,DIO,1,COMEDI_OUTPUT);

   /* get current time */
   clock_gettime(0,&t);

   /* start after one second */
   t.tv_sec++;
   
   while(1){
      /* wait untill next shot */
      clock_gettime(0,&t1);      
      clock_nanosleep(0, TIMER_ABSTIME, &t, NULL);
      /* do the stuff */
      out();
      /* calculate next shot */
      t.tv_nsec+=interval;
      tsnorm(&t);
      clock_gettime(0,&t2);
      deltat[i] = t2.tv_nsec - t1.tv_nsec + t2.tv_sec - t1.tv_sec;
      i=(i+1)%SIZETAB;
   }
   return 0;
}
