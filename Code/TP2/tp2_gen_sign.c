/******************* includes ********************/

#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <rtai.h>
#include <rtai_sched.h>
#include <rtai_fifos.h>
#include <rtai_proc_fs.h>
#include <comedilib.h>


/******************* Licences ********************/
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Squelette de programme RTAI et carte ni-6221");
MODULE_AUTHOR("RAKOTOMALALA FARHI TOCAVEN");


/******************* defines ********************/
#define ms  1000000

#define microsec 1000



#define CAN 0
#define CNA 1
#define DIO 2
#define CHAN_0 0
#define CHAN_1 1

#define PORT_s 0
#define PORT_f 1
#define PORT_p 2
#define PORT_a 3
#define PORT_n 4

#define CAN_RANGE 1 // [-5, +5]
#define SIZETAB 50
#define AREF AREF_GROUND	// indique la référence de masse a utiliser
#define RANGE 0
#define LECTURE_PERIOD 500 // milisecond

/******************* Variables globales ********************/

static RT_TASK generateur1_ptr; // Pointeur pour la tache generateur1
static RT_TASK generateur2_ptr; // Pointeur pour la tache generateur2
static RT_TASK lecture_ptr; // Pointeur pour la tache lecture

static RT_TASK IT_handler_Ptr; // Pointeur pour la tache de reprise de main

RTIME now ;
comedi_t *cf; // la carte

float signal_sin[50] = {0,0.1253332336,0.2486898872,0.3681245527,0.4817536741,0.5877852523,0.6845471059,0.7705132428,
0.8443279255,0.9048270525,0.9510565163,0.9822872507,0.9980267284,0.9980267284,0.9822872507,0.9510565163,0.9048270525,
0.8443279255,0.7705132428,0.6845471059,0.5877852523,0.4817536741,0.3681245527,0.2486898872,0.1253332336,0.0,-0.1253332336,
-0.2486898872,-0.3681245527,-0.4817536741,-0.5877852523,-0.6845471059,-0.7705132428,-0.8443279255,-0.9048270525,
-0.9510565163,-0.9822872507,-0.9980267284,-0.9980267284,-0.9822872507,-0.9510565163,-0.9048270525,-0.8443279255,
-0.7705132428,-0.6845471059,-0.5877852523,-0.4817536741,-0.3681245527,-0.2486898872,-0.1253332336};

unsigned int s  	= 1; // sensibilité ( s=1 : "s=0"      ; s=100 :"s=1" )		
// signal 1
int 	s1_i		= 0; // Indice de l'echantillon a generer.
int 	s1_phi 		= 0; // Dephasage du signal
float 	s1_alpha 	= 1; // coeficient d'amplification.
unsigned int s1_period  = 10; // periode ( s=0 :       ; s=1 : )	

// signal 2
int 	s2_i		= 0; // Indice de l'echantillon a generer.
int 	s2_phi 		= 0; // Dephasage du signal
float 	s2_alpha 	= 1; // coeficient d'amplification.
unsigned int s2_period  = 10; // periode ( s=0 :       ; s=1 : )	
/******************* tache generateur ***************************/
void generateur1 (long int x)
{
	unsigned int 	data; //[0 ;2^32 -1]

	// init CHAN
	s1_i = s1_phi;

	  while (1)//loop
	  {  
	  	data = (unsigned int)(32767.5 + 65535/20*(s1_alpha*signal_sin[s1_i])); // A*sin(2pif)

	  	comedi_data_write(cf, CNA, CHAN_0, RANGE, AREF, data);
	  	s1_i = (s1_i+1)%SIZETAB;
	  	rt_task_wait_period();
	  }
}

/******************* tache generateur ***************************/
void generateur2 (long int x)
{
	unsigned int 	data; //[0 ;2^32 -1]

	// init CHAN
	s2_i = s2_phi;
	  while (1)//loop
	  {  
	  	data = (unsigned int)(32767.5 + 65535/20*(s2_alpha*signal_sin[s2_i])); // A*sin(2pif)
	  	comedi_data_write(cf, CNA, CHAN_1, RANGE, AREF, data);
	  	s2_i = (s2_i+1)%SIZETAB;
	  	rt_task_wait_period();
	  }
}
/******************* tache lecture ***************************/

void lecture (long int x)
{
unsigned int 	s0,
		f,
		p,
		a,
		n;
		
 while (1)//loop
  {
  s0=0; 
  f=0; 
  p=0;
  a=0;
  n=0;
    	
 	// lecture digit
	comedi_dio_read(cf,DIO,PORT_n,&n); // pour n
	comedi_dio_read(cf,DIO,PORT_s,&s0); // pour s	  
	comedi_dio_read(cf,DIO,PORT_p,&p); // pour p	  
	comedi_dio_read(cf,DIO,PORT_a,&a); // pour a	  
	comedi_dio_read(cf,DIO,PORT_f,&f); // pour f
	// sensibilitee
	s = (s0==0)*1 + (s0==1)*100;

	
	//rt_printk("n%d, s%d, p%d, a%d, f%d\n",n,s,p,a,f);

		if(a==1)
 		{
 			comedi_data_read(cf,CAN,0,1,AREF,&a); //range = 1 : [0 ; 5]
 			if(n==0)
 			{
 				s1_alpha=10.0/65535.0*a;
 			}
 			else
 			{
 				s2_alpha=10.0/65535.0*a;
 			}
 		}
 		else if(p==1)
 		{
 			comedi_data_read(cf,CAN,0,1,AREF,&p); //range = 1 : [0 ; 5]
 			if(n==0)
 			{
 				s1_i=((int)(s1_i+0.0007629511*p-0.0015260252))%SIZETAB;
 			}
 			else
 			{
 				s2_i=((int)(s2_i+0.0007629511*p-0.0015260252))%SIZETAB;
 			}
 		}
    
	
	
	
	rt_task_wait_period();
 }
}

/******************* init_module ***************************/

int init_module(void)
{	 
 	 	// Initialisation de la carte d'E/S
  cf = comedi_open("/dev/comedi0");	
  if(cf == NULL)
  {
    comedi_perror("Comedi fails to open");
    return -1;
  }

	// INPUTS
		// Configurer le device DIGITAL_INPUT pour recevoir les donnees signaux
		// et DIGITAL_OUTPUT pour envoyer les donnees signaux
		comedi_dio_config(cf,DIO,PORT_s,COMEDI_INPUT); // port 0 pour s
	   	comedi_dio_config(cf,DIO,PORT_f,COMEDI_INPUT); // port 1 pour f
	   	
	   	comedi_dio_config(cf,DIO,PORT_p,COMEDI_INPUT); // port 2 pour p
	   	comedi_dio_config(cf,DIO,PORT_a,COMEDI_INPUT); // port 3 pour a
		comedi_dio_config(cf,DIO,PORT_n,COMEDI_INPUT); // port 4 pour n
   	// OUTPUTS

	rt_set_oneshot_mode();
	// Lancement du timer

	now =  start_rt_timer(0); // création timer de période  50ms
 	now = rt_get_time();
  	// Lancement des taches
  		// lecture 
	 
	rt_task_make_periodic(	&lecture_ptr, 	/* Pointeur vers la tâche */
				now,		/* Instant de départ */
				nano2count(LECTURE_PERIOD*ms));		/* Periode */	      
				
				
  		// generateur 1 
 	rt_task_init( &generateur1_ptr, 	/* Le prt de tache */
	 	      generateur1,	   	/* Nom de la tache */
	 	      1, 		/* valeur du paramètre X */
	 	      2000,		/* Taille de la pile necessaire pour la tache (memoire tempo utilisée) */
	 	      0,		/* Priorité */
	 	      1,  		/* Calcul flottant */
	 	      0); 		/* choix d'un signal ou non  */
	 	      

	rt_task_make_periodic(	&generateur1_ptr, 	/* Pointeur vers la tâche */
				now,		/* Instant de départ */
				nano2count(s1_period*ms/SIZETAB));		/* Periode */
				
				
  		// generateur 2 
 	rt_task_init( &generateur2_ptr, 	/* Le prt de tache */
	 	      generateur2,	   	/* Nom de la tache */
	 	      1, 		/* valeur du paramètre X */
	 	      2000,		/* Taille de la pile necessaire pour la tache (memoire tempo utilisée) */
	 	      0,		/* Priorité */
	 	      1,  		/* Calcul flottant */
	 	      0); 		/* choix d'un signal ou non  */
	 	      
 	now = rt_get_time(); // IMPORTANT : décalage des deux horlogues pour éviter perturbations sur sorties 
 			     // (chevauchement de taches)
	rt_task_make_periodic(	&generateur2_ptr, 	/* Pointeur vers la tâche */
				now,		/* Instant de départ */
				nano2count(s2_period*ms/SIZETAB));		/* Periode */
				
				

  return 0;
}

/******************* cleanup ***************************/
void cleanup_module(void)
{
  stop_rt_timer();
rt_task_delete(&generateur1_ptr);
rt_task_delete(&generateur2_ptr);
rt_task_delete(&lecture_ptr);
 }
