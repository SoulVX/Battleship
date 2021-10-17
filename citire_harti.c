#include <stdio.h>
#include <stdlib.h>
#include "API.h"

int main(int argc, char const *argv[])  {
	int ***maps,i,j,k;
	citire_harti(argc,argv,&maps);
	for(i=0;i<argc-1;i++) {
		for(j=0;j<=9;j++) {
			for(k=0;k<=9;k++) {
				printf("%d ", maps[i][j][k] );
			}
			printf("\n");
		}
		printf("=====================\n");
	}

	for(i=0;i<argc-1;i++) {
		for(j=0;j<10;j++) {
			free(maps[i][j]);
		}
		free(maps[i]);
	}
	free(maps);
	return 0;
}