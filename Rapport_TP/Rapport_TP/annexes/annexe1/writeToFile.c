#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#define SIZETAB 5000
int main()
{
	int fifo1;
	int i;
	int *data;//[SIZETAB];
	if(fifo1 =open("/dev/rtf0",O_RDONLY) < 0)
	{	
	        printf("probleme ouverte fifo : /dev/rtf0\n");
	        exit(1);
	}
	read(fifo1, data, SIZETAB*sizeof(int));
	
	FILE *f;
	f = fopen("erreur.res","w");
	
	for(i=0;i<SIZETAB;i++)
	{
	printf("%d\n",i);
		//read(&fifo1,i,sizeof(int));
		fprintf(f, "%d",data[i]);
		i++;
	}
	fclose(f);
	return 0;
}
