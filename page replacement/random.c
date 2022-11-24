#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>

#define N 500

struct psr {
	int data; 
	int rw;
};

int main(void){

	int i = 0;
	struct psr s[N];

	srand((unsigned)time(NULL));
	while (i<N){
		s[i].data = rand()%29+1;
		s[i].rw = rand()%2;				//0이면 read, 1이면 write
		i++;
	}

	//파일에 쓰기
	FILE* fp;
	char* fname = "entry.txt";
	if ((fp=fopen(fname, "w+"))==NULL){
		fprintf(stderr, "fopen error with %s\n", fname);
		exit(1);
	}
	
	for (i = 0; i<N; i++){
		fwrite(&s[i], sizeof(struct psr), 1, fp);
	}
	fclose(fp);

	//파일 읽기
	
	if ((fp=fopen(fname, "r+"))==NULL){
		fprintf(stderr, "fopen error with %s\n", fname);
		exit(1);
	}

	struct psr p;
	for (i = 0; i<N; i++){
		fread(&p, sizeof(struct psr), 1, fp);
		printf("%d %d\n", p.data, p.rw);
	}
	fclose(fp);
}

