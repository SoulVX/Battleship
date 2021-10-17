int generare_rand (int max);
int incercare_construire_barca(int dim, int** map, int x, int y);
void generare_nr_barci_dim(int dim, int nr, int **map);
void finalizare_map(int **map);
void initializare_map(int ***map);
void generare_harta(int ***map);
void citire_harti(int argc, char *argv[] , int ****maps);

void init_options(char ***optiuni);
void init_colors();
void print_logo();
int setup_menu(char **optiuni,int resume);
int setup_alegere_harta(int argc,char *argv[], int ****maps);
int verificare_harti(int argc, char *argv[]);
void makebox(int y_stg_sus,int x_stg_sus);
void afisare_harta(int **map,int poz,int opt,int y,int x,int high_y,int high_x,int **mask);
int incepere_joc(int alegere_optiune_harta, int **harta_aleasa, int ***harta_pc,int ***maps,int ***masca_harta, int ***masca_hit_pc,int resume,int *nave_lovite_player,int *nave_lovite_pc);
void initializare_ecran(char ***optiuni);
int orientare_barca(int y,int x, int **map);
int doborare_barca(int y, int x,int **map,int **masca,int marcaj_hit);
void scriere_barca_in_memorie(int y,int x,int **map,int **masca,int *tip_barca,int *structura_barca,int *index_tip_barca, int *index_structura_barca);