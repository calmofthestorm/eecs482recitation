default: clean
	spin -a mutual_exclusion.pml
	gcc -O3 -DSAFETY pan.c -o pan
	./pan
	spin -p -t mutual_exclusion.pml

clean:
	rm -f pan.b  pan.c  pan.h  pan.m  pan.p  pan.t pan mutual_exclusion.pml.trail
