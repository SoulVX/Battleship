#include <ncurses.h>
#include <stdlib.h>
#include <string.h>
#include "API.h"

int main(int argc,char *argv[]) {

char **optiuni,*nume_eroare; int ***maps,**harta_aleasa, **harta_pc,**masca_harta=NULL, **masca_hit_pc,resume=0,nave_lovite_player[5]={0,0,0,0,0},nave_lovite_pc[5]={0,0,0,0,0};

if(argc==1) {
	printf("[Eroare]: Nu s-au dat argumente de comanda.\n");
	return 1;
}
int err = verificare_harti(argc,argv);
if(err>0) {
	printf("[Eroare]: Fisierul %s nu poate fi deschis.\n",argv[err]);
	return 1;
}
	
initializare_ecran(&optiuni);

while(1) {
	int alegere_optiune_menu = setup_menu(optiuni,resume);
	switch(alegere_optiune_menu) {
		case 0:
			;
			int alegere_optiune_harta = setup_alegere_harta(argc,argv,&maps);
	 		int rez = incepere_joc(alegere_optiune_harta,harta_aleasa,&harta_pc,maps,&masca_harta,&masca_hit_pc,0,nave_lovite_player,nave_lovite_pc);
	 		if(rez!=2) {
		 		int c = getch();
		 		while (c == KEY_MOUSE) {
		 			c=getch();
		 		}
		 		resume=0;
	 		} else {
	 			resume=1;
	 		}
		break;

		case 1:
			;
			rez = incepere_joc(alegere_optiune_harta,harta_aleasa,&harta_pc,maps,&masca_harta,&masca_hit_pc,resume,nave_lovite_player,nave_lovite_pc);
		break;

		case 2:
			free(optiuni);
			clear();
			endwin();
		return 1;
	}
}
getch();
endwin();
return 1;
}