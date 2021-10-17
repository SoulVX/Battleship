build: battleship.c API.h API.c
	gcc -Wall battleship.c API.h API.c -lncurses -o battleship
run: battleship map1.txt map2.txt map3.txt map4.txt map5.txt
	./battleship map1.txt map2.txt map3.txt map4.txt map5.txt
clean: battleship
	rm battleship