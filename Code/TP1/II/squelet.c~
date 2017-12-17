#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>
#include <rtai_proc_fs.h>
#include <comedilib.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Squelette de programme RTAI et carte ni-6221");
MODULE_AUTHOR("RAKOTOMALALA FARHI TOCAVEN");


/*
 *	command line parameters
 */


#define ms  1000000

#define microsec 1000

#define CARRE_HIGH 1.22
#define CARRE_LOW 0

#define CAN 0
#define CNA 1
#define DIO 2
#define CHAN_0 0
#define CHAN_1 1
#define CAN_RANGE 1 // [-5, +5]
#define PERIOD 100 // miliseconde 
#define SIZETAB 5000
#define deltat 0		// FIFO

static RT_TASK Tache1_Ptr; // Pointeur pour la tache 1
static RT_TASK IT_handler_Ptr; // Pointeur pour la tache de reprise de main


comedi_t *cf; // la carte
RTIME curTime; // un timer pour mesurer les dif de temps

/******************* tache1 ***************************/
void Tache1 (long int x)
{
	int voie = x,
	    composant=DIO ,
	    i=0;/*position dans deltat*/

	int delta_i = 0;
	  while (1)
	  {  
	  	curTime = rt_get_time();
	  	delta_i=(int)curTime;
	  	
		comedi_dio_write(cf,composant,voie,CARRE_HIGH);
		rt_task_wait_period();
		comedi_dio_write(cf,composant,voie,CARRE_LOW);
		rt_task_wait_period();

		curTime = rt_get_time();
		delta_i = (int)curTime - delta_i;
	  	
	  	i=(i+1)%SIZETAB;
	  	if( i > SIZETAB)
	  	{	
		  	if(rtf_put(deltat,&delta_i,SIZETAB)<0)
		  	{
				printk("probleme ecrite fifo : /dev/rtf0\n");

			}
	  	}
		
	  }
}


/******************* init ***************************/

int init_module(void)
{	
  RTIME now ;
 	 	// Initialisation de la carte d'E/S
  cf = comedi_open("/dev/comedi0");	
  if(cf == NULL)
  {
    comedi_perror("Comedi fails to open");
    return -1;
  }

	// init FIFO pour mesures
	
	// Configurer le device DIGITAL_INPUT pour recevoir les donnees signaux
	// et DIGITAL_OUTPUT pour envoyer les donnees signaux
	comedi_dio_config(cf,DIO,0,COMEDI_OUTPUT);
   	comedi_dio_config(cf,DIO,1,COMEDI_OUTPUT);
   	
	rt_set_oneshot_mode();
	// Lancement du timer

	now =  start_rt_timer(0); // création timer de période  50ms
	curTime = start_rt_timer( nano2count(0));
 	now = rt_get_time();
  	// Lancement des taches
 	rt_task_init(&Tache1_Ptr, 	/* Le prt de tache */
	 	      Tache1,	   	/* Nom de la tache */
	 	      1, 		/* valeur du paramètre X */
	 	      2000,		/* Taille de la pile necessaire pour la tache (memoire tempo utilisée) */
	 	      0,		/* Priorité */
	 	      0,  		/* Pas de calcul flottant */
	 	      0); 		/* choix d'un signal ou non  */
	 	      

	rt_task_make_periodic(	&Tache1_Ptr, 	/* Pointeur vers la tâche */
				now,		/* Instant de départ */
				nano2count(PERIOD*microsec/2));		/* Periode */
				
				
	rtf_create(deltat, SIZETAB);
  return 0;
}

/******************* cleanup ***************************/
void cleanup_module(void)
{
  stop_rt_timer();
rt_task_delete(&Tache1_Ptr);
  
 }
