PROG= moncarrelinux
all :  $(PROG) 


$(PROG) : clean $(PROG).c
	gcc -o $(PROG) $(PROG).c -lrt -Wall -lm -lcomedi  
	cp $(PROG) /tmp

clean :
	rm -f  $(PROG)  delta.png delta.res core

gnuplot-view : delta.res
	gnuplot delta.plt; gpicview delta.png

	
