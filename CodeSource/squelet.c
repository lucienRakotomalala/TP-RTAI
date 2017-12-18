#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>

#include </usr/local/src/comedi/include/linux/comedilib.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Squelette de programme RTAI et carte ni-6221");
MODULE_AUTHOR("HERRERO	REY");


/*
 *	command line parameters
	dmesg pour debugger
	lsmod pour voir les modules installés
	lsmod | grep squelet
 */


#define ms  1000000

#define microsec 1000

#define nb_ech_sinus 50
#define CAN 0
#define CNA 1
#define DIO 2
#define pi 3.1416
#define CHAN_0 0
#define CHAN_1 1
#define CAN_RANGE 1 // [-5, +5]
#define CNA_RANGE 0
int aref = AREF_GROUND;
static RT_TASK Tache1_Ptr; // Pointeur pour la tache 1
//static RT_TASK IT_handler_Ptr; // Pointeur pour la tache de reprise de main
float sinus[50]={0,
0.127877161684506,
0.253654583909507,
0.375267004879374,
0.490717552003938,
0.598110530491216,
0.695682550603486,
0.781831482468030,
0.855142763005346,
0.914412623015812,
0.958667853036661,
0.987181783414450,
0.999486216200688,
0.995379112949198,
0.974927912181824,
0.938468422049760,
0.886599306373000,
0.820172254596956,
0.740277997075316,
0.648228395307789,
0.545534901210549,
0.433883739117558,
0.315108218023621,
0.191158628701373,
0.064070219980713,
-0.06407021998071,
-0.191158628701372,
-0.315108218023621,
-0.433883739117558,
-0.545534901210549,
-0.648228395307789,
-0.740277997075316,
-0.820172254596956,
-0.886599306373000,
-0.938468422049761,
-0.974927912181824,
-0.995379112949198,
-0.999486216200688,
-0.987181783414450,
-0.958667853036661,
-0.914412623015813,
-0.855142763005346,
-0.781831482468030,
-0.695682550603486,
-0.598110530491216,
-0.490717552003938,
-0.375267004879375,
-0.253654583909508,
-0.127877161684507,
0};



lsampl_t Potar;
comedi_t *carte;

void Tache1 (long int x)
{

	
  
	char voie, Voie_f=0;
	
	int j = 0;
	float Tension = 0;
	int Sortie = 0;
	

	voie = x;


	printk("\n\nBonjour La tache\n\n");
	while (1)
  	{     
		if(rtf_get(1,&Voie_f,sizeof(Voie_f))>0)
		{
			voie = Voie_f;
		printk("\n\nBonjour le if\n\n");
		}
		
		Tension = sinus[j];

		Sortie=(Tension+10)*(65535/20);

		
		comedi_data_write(carte, CNA, CHAN_0, CNA_RANGE,aref, Sortie);

		//comedi_data_read(carte, CAN, CHAN_0, CAN_RANGE,aref, &Potar);
		//comedi_dio_write(carte,DIO,voie,1);
		rt_task_wait_period();
	
		printk("\n\nBonjour tout court potar = %d\n\n",Potar);
		j++;
		if(j==49) j=0;
	}  
}

/*
// Tache permettant de récupérer la main sur Linux
void IT_handler(long int x)
{
  rt_printk ("  interruption déclenchée \n ");
  rt_task_suspend(&Tache1_Ptr);
  rt_task_wait_period();
}
*/ 


int init_module(void)
{
	//RTIME timer_period;
	RTIME now;
		printk("\nDébut initialisation\n\n");
  // Creation des taches
 
  	carte = comedi_open("/dev/comedi0");	// Initialisation de la carte d'E/S
	if(carte == NULL)
	{
	   comedi_perror("Comedi fails to open");
	    return -1;
	}
	printk("\n\nBonjour task_init\n\n");
	rt_task_init(&Tache1_Ptr,Tache1,0,2000,5,0,0);
	rt_set_oneshot_mode();
	now=start_rt_timer(nano2count(ms));
	
	printk("\n\nBonjour rt_task_make_periodic\n\n");
	rt_task_make_periodic(&Tache1_Ptr,now+20*nano2count(ms),(1)*nano2count(ms));
		printk("\n\nBonjour rt_task_resume\n\n");
	//rtf_create(1,500);
	rt_task_resume(&Tache1_Ptr);
		printk("\n\nBonjour comedi dio config\n\n");
	comedi_dio_config(carte,DIO,0,COMEDI_OUTPUT);
	printk("\n\nau revoir init module\n");
  // Lancement des taches

  return 0;
}


void cleanup_module(void)
{
  	stop_rt_timer();
	//rtf_destroy(1);
	rt_task_delete(&Tache1_Ptr);
  // Destruction des objets de l'application
  
 }






