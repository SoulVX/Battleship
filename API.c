#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <ncurses.h>
#include <string.h>
#include <unistd.h>

int generare_rand (int max) {
	return rand() % 3721 % (max+1); 
}

int orientare_barca(int y,int x, int **map) {
	//e verticala?
	if(y==0) {
		if(map[y+1][x])
			return 2;
	}
	else if(y==9) {
			if(map[y-1][x])
				return 2; 
	}
	else if ( map[y+1][x] || map[y-1][x] )
		return 2;
	//e orizontala?
	if(x==0) {
		if(map[y][x+1])
			return 1;
	}
	else if(x==9) {
		if(map[y][x-1])
			return 1;
	}
	else if ( map[y][x+1] || map[y][x-1] )
		return 1; 
	//e barca de dim 1
	return 0;
}

void scriere_barca_in_memorie(int y,int x,int **map,int **masca,int *tip_barca,int *structura_barca,int *index_tip_barca, int *index_structura_barca) {
	int orientare = orientare_barca(y,x,map); int lungime=0;
	if(orientare==0) {
		map[y][x]=0;
		masca[y][x]=1;
		tip_barca[*index_tip_barca]=1;
		structura_barca[*index_structura_barca]=1;
		*index_structura_barca=*index_structura_barca+1;
		*index_tip_barca=*index_tip_barca+1;
		return;
	}
	if(orientare==1) {
		while( map[y][x+lungime]==1) {
			map[y][x+lungime]=0; //se sterge de pe mapa patratul barca
			structura_barca[*index_structura_barca]=masca[y][x+lungime]; //se trece in vect cum era barca
			masca[y][x+lungime]=1; //se sterge masca
			*index_structura_barca=*index_structura_barca+1; //se incrementeaza indexul
			lungime++;
			if(x+lungime==10)
				break;
		}
	}
	if(orientare==2) {
		while ( map[y+lungime][x] == 1) {
			map[y+lungime][x]=0;
			structura_barca[*index_structura_barca]=masca[y+lungime][x]; //se trece in vect cum era barca
			masca[y+lungime][x]=1;
			*index_structura_barca=*index_structura_barca+1; //se incrementeaza indexul
			lungime++;
			if(y+lungime==10)
				break;
		}
	}
	tip_barca[*index_tip_barca]=lungime;
	*index_tip_barca=*index_tip_barca+1;
}


//marcaj hit = 1 => harta PC
//marcaj hit = 0 => harta om
int doborare_barca(int y, int x, int **map,int **masca,int marcaj_hit) {
	if(orientare_barca(y,x,map)==2) {
		//mvprintw(4,0,"2");
		int ym=y, yM=y;
		while(map[ym][x]==1&&ym>0)
			ym--;
		while(map[yM][x]==1&&yM<9)
			yM++;
		if(ym > 0 || map[0][x] == 0 )
			ym++;
		if(yM < 9 || map[9][x] == 0 )
			yM--;
		//mvprintw(5,0,"%d %d",ym,yM);
		for(int i=ym;i<=yM;i++)
			if(masca[i][x]!=marcaj_hit)
				return 0;
		return yM-ym+1;
	}
	if(orientare_barca(y,x,map)==1) {
		//mvprintw(4,0,"1");
		int xm=x,xM=x;
		while(map[y][xm]==1&&xm>0)
			xm--;
		while(map[y][xM]==1&&xM<9)
			xM++;
		if(xm > 0 || map[y][0] == 0 )
			xm++;
		if(xM < 9 || map[y][9] == 0 )
			xM--;
		//mvprintw(5,0,"%d %d",xm,xM);
		for(int i=xm;i<=xM;i++)
			if(masca[y][i]!=marcaj_hit)
				return 0;
		return xM-xm+1;
	}

	if(orientare_barca(y,x,map)==0)
		if(masca[y][x]!=marcaj_hit)
			return 0;
		else
			return 1;
}



void makebox(int y_stg_sus,int x_stg_sus) {
	int y_dr_jos=y_stg_sus+11;
	int x_dr_jos=x_stg_sus+21;
	int sw=1,start=65;
	mvaddch(y_stg_sus,x_stg_sus,ACS_ULCORNER);
	mvaddch(y_stg_sus,x_dr_jos,ACS_URCORNER);
	mvaddch(y_dr_jos,x_stg_sus,ACS_LLCORNER);
	mvaddch(y_dr_jos,x_dr_jos,ACS_LRCORNER);
	for(int i=x_stg_sus+1;i<x_dr_jos;i++) {
		mvaddch(y_stg_sus,i,ACS_HLINE);
		if(sw==0) {
			mvaddch(y_stg_sus-1,i,start++);
			sw=1;
		} else {
			sw=0;
		}
		mvaddch(y_dr_jos,i,ACS_HLINE);
	}
	start=49;
	for(int i=y_stg_sus+1;i<y_dr_jos;i++) {
		mvaddch(i,x_stg_sus,ACS_VLINE);
		if(i<y_dr_jos-1)
			mvaddch(i,x_stg_sus-1,start++);
		else {
			mvprintw(i,x_stg_sus-2,"10");
		}
		mvaddch(i,x_dr_jos,ACS_VLINE);
	}
}

// map = harta de afisat
// poz = pozitia pe un rand daca e cazul
// opt = afiseaza optiune selectie harta
// y x = coord stanga sus
// high y x = coord highlight
// mask = matrice masca 
void afisare_harta(int **map,int poz,int opt,int y,int x,int high_y,int high_x,int **mask) {
	if(mask==NULL) {	
		for(int j=0;j<10;j++) {
			for(int k=0;k<10;k++) {
				if(map[j][k]==0) {
					if(y+j+1==high_y && x+poz*26+2*k == high_x) {
						attron(A_REVERSE);
					}
					attron(COLOR_PAIR(10));
					mvaddch(y+j+1,x+poz*26+2*k,'~');
					mvaddch(y+j+1,x+poz*26+2*k+1,'~');
					attroff(COLOR_PAIR(10));
					attroff(A_REVERSE);
				}
				else {
					attron(COLOR_PAIR(1));
					mvaddch(y+j+1,x+poz*26+2*k,ACS_CKBOARD);
					mvaddch(y+j+1,x+poz*26+2*k+1,ACS_CKBOARD);
					attroff(COLOR_PAIR(1));
				}
				if(j==0&&k==4&&opt) {
					attron(COLOR_PAIR(2));
					mvprintw(11+j+1,poz*26+2*k+1,"[]");
					attroff(COLOR_PAIR(2));
				}
			}
			printw("\n");
		}
	}
	else {
		for(int j=0;j<10;j++) {
			for(int k=0;k<10;k++) {
				if(map[j][k]==0 && mask[j][k]==1) {
					attron(COLOR_PAIR(10));
					mvaddch(y+j+1,x+poz*26+2*k,'~');
					mvaddch(y+j+1,x+poz*26+2*k+1,'~');
					attroff(COLOR_PAIR(10));
				}
				else if(map[j][k]==1 && mask[j][k]==1) {
					attron(COLOR_PAIR(1));
					mvaddch(y+j+1,x+poz*26+2*k,ACS_CKBOARD);
					mvaddch(y+j+1,x+poz*26+2*k+1,ACS_CKBOARD);
					attroff(COLOR_PAIR(1));
				} else if(mask[j][k]==0) {
					if(y+j+1==high_y && x+poz*26+2*k == high_x) {
						attron(A_REVERSE);
					}
					attron(COLOR_PAIR(1));
					mvaddch(y+j+1,x+poz*26+2*k,' ');
					mvaddch(y+j+1,x+poz*26+2*k+1,' ');
					attroff(COLOR_PAIR(1));
					attroff(A_REVERSE);
				}
				if(j==0&&k==4&&opt) {
					attron(COLOR_PAIR(2));
					mvprintw(11+j+1,poz*26+2*k+1,"[]");
					attroff(COLOR_PAIR(2));
				}
			}
		}
	}
	if(!opt)
	makebox(y,x-1);
}

int incercare_construire_barca_custom(int dim, int** map,int **masca, int x, int y, int *structura_barca,int index_structura_barca) {
	int i,j,directie = generare_rand(3);
	//printf("%d : %d %d\n", directie,x,y);
	if(directie == 0) {
		for(i=y+1; i< y+dim; i++) {
			//printf("%d %d\n", x,i);
			if( i>=10 || map[x][i] != 0) {
				return 1;
			}
		}
		for(i=x-1;i<=x+1;i++)
			for(j=y-1;j<=y+dim;j++)
				if(i<10&&j<10&&i>-1&&j>-1)
					map[i][j]=2;
		for(i=y; i< y+dim; i++) {
			map[x][i]=1;
			masca[x][i]=structura_barca[index_structura_barca+i-y];
		}
		mvprintw(10,0,"0");
	}
	if(directie == 1) {
		for(i=x-1; i> x-dim; i--) {
			//printf("%d %d\n", i,y);
			if( i<=-1 || map[i][y] != 0) {
				return 1; 
			}
		}
		for(i=x+1;i>= x-dim;i--)
			for(j=y-1;j<=y+1;j++)
				if(i<10&&j<10&&i>-1&&j>-1)
					map[i][j]=2;
		for(i=x;i> x-dim; i--) {
			map[i][y]=1;
			masca[i][y]=structura_barca[index_structura_barca+x-i];
		}
				mvprintw(10,0,"1");

	}
	if(directie == 2) {
		for(i=y-1; i> y-dim; i--) {
			//printf("%d %d\n", x,i);
			if( i<=-1 || map[x][i] != 0) {
				return 1; 
			}
		}
		for(i=x-1;i<=x+1;i++)
			for(j=y-dim;j<=y+1;j++)
				if(i<10&&j<10&&i>-1&&j>-1)
					map[i][j]=2;
		for(i=y; i> y-dim; i--) {
			map[x][i]=1;
			masca[x][i]=structura_barca[index_structura_barca+y-i];
		}
				mvprintw(10,0,"2");
	}
	if(directie == 3) {
		for(i=x+1; i< x+dim; i++) {
			//printf("%d %d\n", i,y);
			if( i>=10 || map[i][y] != 0) {
				return 1; 
			}
		}
		for(i=x-1;i<=x+dim;i++)
			for(j=y-1;j<=y+1;j++)
				if(i<10&&j<10&&i>-1&&j>-1)
					map[i][j]=2;
		for(i=x; i< x+dim; i++) {
			map[i][y]=1;
			masca[i][y]=structura_barca[index_structura_barca+i-x];
		}
				mvprintw(10,0,"3");
	}
	mvprintw(11,0,"%d %d",x,y);
	return 0;
}

int incercare_construire_barca(int dim, int** map, int x, int y) {
	int i,j,directie = generare_rand(3);
	//printf("%d : %d %d\n", directie,x,y);
	if(directie == 0) {
		for(i=y+1; i< y+dim; i++) {
			//printf("%d %d\n", x,i);
			if( i>=10 || map[x][i] != 0) {
				return 1;
			}
		}
		for(i=x-1;i<=x+1;i++)
			for(j=y-1;j<=y+dim;j++)
				if(i<10&&j<10&&i>-1&&j>-1)
					map[i][j]=2;
		for(i=y; i< y+dim; i++)
			map[x][i]=1;
	}
	if(directie == 1) {
		for(i=x-1; i> x-dim; i--) {
			//printf("%d %d\n", i,y);
			if( i<=-1 || map[i][y] != 0) {
				return 1; 
			}
		}
		for(i=x+1;i>= x-dim;i--)
			for(j=y-1;j<=y+1;j++)
				if(i<10&&j<10&&i>-1&&j>-1)
					map[i][j]=2;
		for(i=x;i> x-dim; i--)
			map[i][y]=1;

	}
	if(directie == 2) {
		for(i=y-1; i> y-dim; i--) {
			//printf("%d %d\n", x,i);
			if( i<=-1 || map[x][i] != 0) {
				return 1; 
			}
		}
		for(i=x-1;i<=x+1;i++)
			for(j=y-dim;j<=y+1;j++)
				if(i<10&&j<10&&i>-1&&j>-1)
					map[i][j]=2;
		for(i=y; i> y-dim; i--)
			map[x][i]=1;
	}
	if(directie == 3) {
		for(i=x+1; i< x+dim; i++) {
			//printf("%d %d\n", i,y);
			if( i>=10 || map[i][y] != 0) {
				return 1; 
			}
		}
		for(i=x-1;i<=x+dim;i++)
			for(j=y-1;j<=y+1;j++)
				if(i<10&&j<10&&i>-1&&j>-1)
					map[i][j]=2;
		for(i=x; i< x+dim; i++)
			map[i][y]=1;
	}
	return 0;
}

void generare_nr_barci_custom(int** map,int **masca, int *structura_barca, int *tip_barca,int nr) {
	int cap,i,x,y,ind=0;
	for(i=1;i<=nr;i++) {
		cap=1;
		while(cap!=0) {
			x = generare_rand(9);
			y = generare_rand(9);
			cap = map[x][y];
		}
		while(incercare_construire_barca_custom(tip_barca[nr-1],map,masca,x,y,structura_barca,ind));
		ind=ind+tip_barca[nr-1];
	}
}

void generare_nr_barci_dim(int dim, int nr, int **map) {
	int cap,i,x,y;
	for(i=1;i<=nr;i++) {
		cap=1;
		while(cap!=0) {
			x = generare_rand(9);
			y = generare_rand(9);
			cap = map[x][y];
		}
		while(incercare_construire_barca(dim, map, x, y));
	}
}

void finalizare_map(int **map) {
	for(int i=0;i<10;i++)
		for(int j=0;j<10;j++)
			if(map[i][j]==2)
				map[i][j]=0;
}

void initializare_map(int ***map) {
	*map = malloc(10*sizeof(int*));
	for(int i=0;i<10;i++)
		(*map)[i]=malloc(10*sizeof(int));
	for(int i=0;i<10;i++)
		for(int j=0;j<10;j++)
			(*map)[i][j]=0;
}

void generare_harta(int ***map) {
	initializare_map(map);
	generare_nr_barci_dim(4,1,*map);
	generare_nr_barci_dim(3,2,*map);
	generare_nr_barci_dim(2,3,*map);
	generare_nr_barci_dim(1,4,*map);
	finalizare_map(*map);
}

int verificare_harti(int argc, char *argv[]) {
	FILE *fis_mapa;
	for(int i=1; i<argc; i++) {
		fis_mapa = fopen(argv[i],"r");
		if(!fis_mapa) {
			return i;
		}
		fclose(fis_mapa);
	}
	return -1;
}

void citire_harti(int argc, char *argv[] , int ****maps) {
	FILE *fis_mapa; int i,j,k,val;
	*maps = malloc( (argc-1) * sizeof(int**) );
	for(i=1; i<argc; i++) {
		fis_mapa = fopen(argv[i],"r");
		if(!fis_mapa) {
			return;
		}
		(*maps)[i-1]= malloc(10*sizeof(int*));
		for(j=0;j<=9;j++) {
			(*maps)[i-1][j] = malloc(10*sizeof(int));
			for(k=0;k<=9;k++) {
				fscanf(fis_mapa,"%d",&val);
				(*maps)[i-1][j][k] = val;
			}
		}
		fclose(fis_mapa);
	}
}

void init_options(char ***optiuni) {
	*optiuni = malloc(3*sizeof(char*));
	(*optiuni)[0] = "NEW GAME";
	(*optiuni)[1] = "RESUME";
	(*optiuni)[2] = "QUIT";
}

void init_colors() {
	start_color();
	init_pair(1, COLOR_RED, COLOR_BLACK);
    init_pair(2, COLOR_GREEN, COLOR_BLACK);
    init_pair(3, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_BLUE, COLOR_BLACK);
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);
    init_pair(10,COLOR_CYAN, COLOR_BLUE);
}

void print_logo() {
	mvprintw(3,(COLS-108)/2,"      ___         ___                          ___  ___          ___          ___                           ");
	mvprintw(4,(COLS-108)/2,"     /  /\\       /  /\\    ___      ___        /  /\\/  /\\        /  /\\        /  /\\        ___       ___     ");
	mvprintw(5,(COLS-108)/2,"    /  /::\\     /  /::\\  /__/\\    /__/\\      /  /:/  /::\\      /  /::\\      /  /:/       /__/\\     /  /\\    ");
	mvprintw(6,(COLS-108)/2,"   /  /:/\\:\\   /  /:/\\:\\ \\  \\:\\   \\  \\:\\    /  /:/  /:/\\:\\    /__/:/\\:\\    /  /:/        \\__\\:\\   /  /::\\   ");
	mvprintw(7,(COLS-108)/2,"  /  /::\\ \\:\\ /  /::\\ \\:\\ \\__\\:\\   \\__\\:\\  /  /:/  /::\\ \\:\\  _\\_ \\:\\ \\:\\  /  /::\\ ___    /  /::\\ /  /:/\\:\\  ");
	mvprintw(8,(COLS-108)/2," /__/:/\\:\\_\\:/__/:/\\:\\_\\:\\/  /::\\  /  /::\\/__/:/__/:/\\:\\ \\:\\/__/\\ \\:\\ \\:\\/__/:/\\:\\  /\\__/  /:/\\//  /::\\ \\:\\ ");
	mvprintw(9,(COLS-108)/2," \\  \\:\\ \\:\\/:|__\\/  \\:\\/:/  /:/\\:\\/  /:/\\:\\  \\:\\  \\:\\ \\:\\_\\/\\  \\:\\ \\:\\_\\/\\__\\/  \\:\\/:/__/\\/:/~~/__/:/\\:\\_\\:\\");
	mvprintw(10,(COLS-108)/2,"  \\  \\:\\_\\::/     \\__\\::/  /:/__\\/  /:/__\\/\\  \\:\\  \\:\\ \\:\\   \\  \\:\\_\\:\\       \\__\\::/\\  \\::/   \\__\\/  \\:\\/:/");
	mvprintw(11,(COLS-108)/2,"   \\  \\:\\/:/      /  /:/__/:/   /__/:/      \\  \\:\\  \\:\\_\\/    \\  \\:\\/:/       /  /:/  \\  \\:\\        \\  \\::/ ");
	mvprintw(12,(COLS-108)/2,"    \\__\\::/      /__/:/\\__\\/    \\__\\/        \\  \\:\\  \\:\\       \\  \\::/       /__/:/    \\__\\/         \\__\\/  ");
	mvprintw(13,(COLS-108)/2,"        ~~       \\__\\/                        \\__\\/\\__\\/        \\__\\/        \\__\\/                          ");
}

int setup_menu(char **optiuni,int resume) {
	int choice, highlight = 0,clr=1,i;
	clear();
	while(1) {
		attron(COLOR_PAIR(clr));
		print_logo();
		for(i=0;i<3;i++) {
			if(i==highlight)
				attron(A_REVERSE);
			mvprintw((LINES-11)/2+8+2*i,(COLS-strlen(optiuni[i]+1))/2,optiuni[i]);
			attroff(A_REVERSE);
		}
		move(14,COLS/2);
		choice = getch();
		clr++;
		if(clr==7)
			clr=1;
		switch(choice) {
			case KEY_UP:
				highlight--;
				if(!resume && highlight==1)
					highlight--;
				if(highlight == -1)
					highlight = 2;
			break;

			case KEY_DOWN:
					highlight++;
					if(!resume && highlight==1)
						highlight++;
					if(highlight==3)
						highlight=0;
			break;

			default:
			break;
		}
		if(choice == 10)
			return highlight;
	}
}

int setup_alegere_harta(int argc,char *argv[], int ****maps) {
	clear();
	citire_harti(argc,argv,maps);
	for(int i=0;i<argc-1;i++)
		afisare_harta((*maps)[i],i,1,0,0,-1,-1,NULL);
	int choice, highlight = 0,i;
	while(1) {
		attron(COLOR_PAIR(2));
		for(i=0;i<argc-1;i++) {
			if(i==highlight)
				attron(A_REVERSE);
			mvprintw(12,i*26+9,"[]");
			attroff(A_REVERSE);
		}
		choice = getch();
		switch(choice) {
			case KEY_LEFT:
				highlight--;
				if(highlight == -1)
					highlight = argc-2;
			break;

			case KEY_RIGHT:
				highlight++;
				if(highlight==argc-1)
					highlight=0;
			break;

			default:
			break;
		}
		if(choice == 10)
			return highlight;
	}
}

void update_log(char *linie1, char *linie2 , char *linie3, char *mesaj) {
	strcpy(linie3,linie2);
	strcpy(linie2,linie1);
	strcpy(linie1,mesaj);
}

void print_log(char *linie1, char *linie2 , char *linie3) {
	mvprintw(0,0,linie1); clrtoeol();
	mvprintw(1,0,linie2); clrtoeol();
	mvprintw(2,0,linie3); clrtoeol();
	refresh();
}


//return = 2 => Q
//return = 1 => castig
//return = 0 => pierdere
int incepere_joc(int alegere_optiune_harta, int **harta_aleasa, int ***harta_pc,int ***maps, int ***masca_harta, int ***masca_hit_pc,int resume, int *nave_lovite_player,int *nave_lovite_pc) {
	if(!resume) {
		*masca_harta = malloc(10*sizeof(int*));
		for(int i=0;i<10;i++) {
			(*masca_harta)[i]=malloc(10*sizeof(int));
			for(int j=0;j<10;j++)
				(*masca_harta)[i][j]=0;
		}
		*masca_hit_pc = malloc(10*sizeof(int*));
		for(int i=0;i<10;i++) {
			(*masca_hit_pc)[i]=malloc(10*sizeof(int));
			for(int j=0;j<10;j++)
				(*masca_hit_pc)[i][j]=1;
		}
		generare_harta(harta_pc);
		for(int i=0;i<=4;i++) {
			nave_lovite_player[i]=0;
			nave_lovite_pc[i]=0;
		}
	}
	clear();
	int tura = 0;
	harta_aleasa=maps[alegere_optiune_harta];
	afisare_harta(*harta_pc,0,0,LINES/2-5,COLS/4-10,-1,-1,*masca_harta);
	afisare_harta(harta_aleasa,0,0,LINES/2-5,3*COLS/4-10,-1,-1,*masca_hit_pc);
	char linie1[200],linie2[200],linie3[200];
	strcpy(linie1,"*");
	strcpy(linie2,"*");
	strcpy(linie3,"*");
	print_log(linie1,linie2,linie3);
	refresh();
	for (;;) {
		if(tura == 0) {
		    int c = getch();
			if (c == KEY_MOUSE) {
		    	MEVENT event;
	        	if (getmouse(&event) == OK) {
		        	int y = event.y;
		        	int x = event.x;

		        	if(x>=COLS/4-10 && x<=COLS/4-10+19 && y>= LINES/2-4 && y<= LINES/2+5) {

		        		//highlight map
		        		if((x-(COLS/4-10))%2==0) {
		        			afisare_harta(*harta_pc,0,0,LINES/2-5,COLS/4-10,y,x,*masca_harta);
		        			afisare_harta(harta_aleasa,0,0,LINES/2-5,3*COLS/4-10,-1,-1,*masca_hit_pc);
		        		}
		        		else {
		        			afisare_harta(*harta_pc,0,0,LINES/2-5,COLS/4-10,y,x-1,*masca_harta);
		        			afisare_harta(harta_aleasa,0,0,LINES/2-5,3*COLS/4-10,-1,-1,*masca_hit_pc);
		        		}

		        		//click
		        		if(event.bstate==2  && (*masca_harta)[(y-(LINES/2-4))][(x-(COLS/4-10))/2] == 0) {
		       				(*masca_harta)[(y-(LINES/2-4))][(x-(COLS/4-10))/2]=1;
		       				afisare_harta(*harta_pc,0,0,LINES/2-5,COLS/4-10,y,x,*masca_harta);
		        			afisare_harta(harta_aleasa,0,0,LINES/2-5,3*COLS/4-10,-1,-1,*masca_hit_pc);
		        			refresh();
		        			if((*harta_pc)[(y-(LINES/2-4))][(x-(COLS/4-10))/2]==1) {
		        				int k = doborare_barca( (y-(LINES/2-4)) , (x-(COLS/4-10))/2 , *harta_pc , *masca_harta , 1);
		        				nave_lovite_player[k]++;
		        				//mvprintw(7,0,"[%d] = %d", k, nave_lovite_player[k]);
		        				update_log(linie1,linie2,linie3,"Ai nimerit!");
								tura=0;
		        			}
							else {
								update_log(linie1,linie2,linie3,"Ai ratat!");
								tura=1;
							}

							int sw=0;
		        			for(int i=0;i<10;i++)
		        				for(int j=0;j<10;j++)
		        					if((*harta_pc)[i][j]==1 && (*masca_harta)[i][j] == 0)
		        						sw=1;
		        			if(sw==0) {
		        				update_log(linie1,linie2,linie3,"Ai castigat! Apasa orice tasta pentru a reveni la meniu!");
		        				update_log(linie1,linie2,linie3,"Ai doborat toate cele 10 nave ale PC-ului!");
								char mesaj[200];
								sprintf(mesaj,"PC ul ti a doborat urmatoarele nave dupa formatul dimensiune x numar nave doborate : 1x%d 2x%d 3x%d 4x%d",nave_lovite_pc[1],nave_lovite_pc[2],nave_lovite_pc[3],nave_lovite_pc[4]);
								update_log(linie1,linie2,linie3,mesaj);
		        				print_log(linie1,linie2,linie3);
				       			return 1;
		        			}

							print_log(linie1,linie2,linie3);
						}
					}
				}
			}
			else if( c=='d' ) {
				int y,x;
				for(int i=1;i<=10;i++) {

					y=generare_rand(9);
					x=generare_rand(9);
					while((*masca_harta)[y][x]==1) {
						y=generare_rand(9);
						x=generare_rand(9);
					}
					(*masca_harta)[y][x]=1;
					afisare_harta(*harta_pc,0,0,LINES/2-5,COLS/4-10,y,x,*masca_harta);
		        	refresh();
		        	if((*harta_pc)[y][x]==1) {
		        		int k = doborare_barca( y , x , *harta_pc , *masca_harta , 1);
		        		nave_lovite_player[k]++;
		        		update_log(linie1,linie2,linie3,"Ai nimerit!");
						//tura=0;
		        	}
					else {
						update_log(linie1,linie2,linie3,"Ai ratat!");
						//tura=1;
					}
					print_log(linie1,linie2,linie3);
					//sleep(1);

					int sw=0;
		        	for(int i=0;i<10;i++)
		        		for(int j=0;j<10;j++)
		        			if((*harta_pc)[i][j]==1 && (*masca_harta)[i][j] == 0)
		        				sw=1;
		        	if(sw==0) {
		        		update_log(linie1,linie2,linie3,"Ai castigat!  Apasa orice tasta pentru a reveni la meniu!");
		        		update_log(linie1,linie2,linie3,"Ai doborat toate cele 10 nave ale PC-ului!");
						char mesaj[200];
						sprintf(mesaj,"PC ul ti a doborat urmatoarele nave dupa formatul dimensiune x numar nave doborate : 1x%d 2x%d 3x%d 4x%d",nave_lovite_pc[1],nave_lovite_pc[2],nave_lovite_pc[3],nave_lovite_pc[4]);
						update_log(linie1,linie2,linie3,mesaj);
		        		print_log(linie1,linie2,linie3);
				        return 1;
		        	}

		        	//sleep(1);
					y=generare_rand(9);
					x=generare_rand(9);
					while((*masca_hit_pc)[y][x]==0) {
						y=generare_rand(9);
						x=generare_rand(9);
					}
					(*masca_hit_pc)[y][x]=0;
				    afisare_harta(harta_aleasa,0,0,LINES/2-5,3*COLS/4-10,y,x,*masca_hit_pc);
				    refresh();
				    if(harta_aleasa[y][x]==1) {
				    	int k = doborare_barca( y , x , harta_aleasa , *masca_hit_pc , 0);
		        		nave_lovite_pc[k]++;
				    	update_log(linie1,linie2,linie3,"PC-ul a nimerit!");
				    }
					else {
						update_log(linie1,linie2,linie3,"PC-ul a ratat!");
						
					}
				    sw=0;
				    for(int i=0;i<10;i++)
				    	for(int j=0;j<10;j++)
				        	if((harta_aleasa)[i][j]==1 && (*masca_hit_pc)[i][j] == 1)
				        		sw=1;
				    if(sw==0) {
				        update_log(linie1,linie2,linie3,"Ai pierdut! Apasa orice tasta pentru a reveni la meniu!");
				        update_log(linie1,linie2,linie3,"PC ul a doborat toate cele 10 nave ale tale!");
				        char mesaj[200];
				        sprintf(mesaj,"Ai doborat urmatoarele nave dupa formatul dimensiune x numar nave doborate : 1x%d 2x%d 3x%d 4x%d",nave_lovite_player[1],nave_lovite_player[2],nave_lovite_player[3],nave_lovite_player[4]);
				        update_log(linie1,linie2,linie3,mesaj);
				        print_log(linie1,linie2,linie3);
				        return 0;
				    }
				    print_log(linie1,linie2,linie3);
				    sleep(1);
				}
			}
			else if(c == 'q') {
				return 2;
			}
			else if( c == 'r') {
				int sp=0; int tip_barca[10],structura_barca[20],index_tip_barca=0,index_structura_barca=0;
				for(int i=0;i<10;i++)
					for(int j=0;j<10;j++)
						if(harta_aleasa[i][j]==0&&(*masca_hit_pc)[i][j]==0)
							(*masca_hit_pc)[i][j]=1;
				for(int i=0;i<10;i++)
					for(int j=0;j<10;j++)
						if(harta_aleasa[i][j]==1 && doborare_barca( i , j , harta_aleasa , *masca_hit_pc , 0) == 0) {
							scriere_barca_in_memorie(i,j,harta_aleasa,*masca_hit_pc,tip_barca,structura_barca,&index_tip_barca,&index_structura_barca);
						}
				/*afisare_harta(harta_aleasa,0,0,LINES/2-5,3*COLS/4-10,0,0,*masca_hit_pc);
				refresh();
				getch();
				generare_nr_barci_custom(harta_aleasa,structura_barca,tip_barca,index_tip_barca);
				finalizare_map(harta_aleasa);
				afisare_harta(harta_aleasa,0,0,LINES/2-5,3*COLS/4-10,0,0,*masca_hit_pc);
				refresh();*/
				int cap=1,x,y;
				while(cap!=0) {
					x = generare_rand(9);
					y = generare_rand(9);
					cap = harta_aleasa[x][y];
				}
				while(incercare_construire_barca_custom(tip_barca[0],harta_aleasa,*masca_hit_pc,x,y,structura_barca,0));
			}
		}
		else {
			int y=generare_rand(9);
			int x=generare_rand(9);
			while((*masca_hit_pc)[y][x]==0) {
				y=generare_rand(9);
				x=generare_rand(9);
			}
			(*masca_hit_pc)[y][x]=0;
		    afisare_harta(harta_aleasa,0,0,LINES/2-5,3*COLS/4-10,y,x,*masca_hit_pc);
		    sleep(3);
		    refresh();
		    if(harta_aleasa[y][x]==1) {
		    	int k = doborare_barca( y , x , harta_aleasa , *masca_hit_pc , 0);
		        nave_lovite_pc[k]++;
		    	update_log(linie1,linie2,linie3,"PC-ul a nimerit!");
				tura=1;
		    }
			else {
				update_log(linie1,linie2,linie3,"PC-ul a ratat!");
				tura=0;
			}
		    int sw=0;
		    for(int i=0;i<10;i++)
		    	for(int j=0;j<10;j++)
		        	if((harta_aleasa)[i][j]==1 && (*masca_hit_pc)[i][j] == 1)
		        		sw=1;
		    if(sw==0) {
		        update_log(linie1,linie2,linie3,"Ai pierdut! Apasa orice tasta pentru a reveni la meniu!");
				update_log(linie1,linie2,linie3,"PC ul a doborat toate cele 10 nave ale tale!");
				char mesaj[200];
				sprintf(mesaj,"Ai doborat urmatoarele nave dupa formatul dimensiune x numar nave doborate : 1x%d 2x%d 3x%d 4x%d",nave_lovite_player[1],nave_lovite_player[2],nave_lovite_player[3],nave_lovite_player[4]);
				update_log(linie1,linie2,linie3,mesaj);
				print_log(linie1,linie2,linie3);
				return 0;
			}
		    print_log(linie1,linie2,linie3);
		}
	}
}

void initializare_ecran(char ***optiuni) {
	init_options(optiuni);
	initscr();	
	noecho();
	cbreak();
	keypad(stdscr,true);
	mouseinterval(0);
	srand(time(0));
	mousemask(ALL_MOUSE_EVENTS | REPORT_MOUSE_POSITION, NULL);
	printf("\033[?1003h\n");
	init_colors();
}