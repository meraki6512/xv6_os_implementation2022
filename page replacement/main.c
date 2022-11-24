#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define MAX 10000

/* prototypes */

int end(void);
int get_data_entry();
int simulate(void);
int choose_option(void);
int tokenize(char *input, char *argv[]);
void error(char* s);
int esc();
int get_high_priority_idx(int head, int* rbit, int*wbit);
int sc();
int lru();
int lru2();
int lfu();
int find_farthest_idx(int start_idx, int* page_frame);
int lifo();
int fifo();
int optimal();
int find_farthest_aft_idx(int start_idx, int* page_frame);
int print_page_frame(int* page_frame, char* hit);
int check_hit(int key, int* page_frame, int occupied);
int print_msg(char* msg);
int print_page_fault(int page_fault);

/* glob var */

_Bool save = 1;
int algorithm[3] = {0,};
int pf_num;
_Bool rd = 1;
int ref_num;
FILE* fp = NULL;

struct psr{
	int data;
	int rw;
};
struct psr references[MAX];


/* implementation */

int main(void){
	choose_option();
	get_data_entry();
	simulate();
	end();
}

int end(void){
	if (fp!=NULL) fclose(fp);
	return 0;
}

/*데이터를 (랜덤으로 또는 파일로부터) 얻어 변수에 저장하는 함수*/
int get_data_entry(){

	if (rd){													//랜덤으로 데이터 엔트리 불러옴
		print_msg("\n[reference string]\n");
		srand((unsigned)time(NULL));
		for (int i=0; i<ref_num; i++){
			references[i].data = rand()%29+1;
			references[i].rw = rand()%2;						//0이면 read, 1이면 write
			char tmp1[50];
			sprintf(tmp1, "%d\t%d\n", references[i].data, references[i].rw);
			printf("%s", tmp1);
			if (save) {fprintf(fp, "%s", tmp1);}
		}
		printf("\n");
		if (save) {fprintf(fp, "\n");}
	}
	else{														//파일에서 데이터 엔트리 읽어옴
		FILE* fp_r;
		char fname[100];
		printf("Enter data entry file name.extension: ");
		scanf("%s", fname);
		if ((fp_r=fopen(fname, "r+"))==NULL){
			fprintf(stderr, "fopen error with %s\n", fname);
			exit(1);
		}
		int i = 0;
		struct psr p;
		while(!feof(fp_r)){
			if ((fread(&p, sizeof(struct psr), 1, fp_r))<0) {
				fprintf(stderr, "fread error");
				exit(1);
			}
			references[i].data = p.data;
			references[i].rw = p.rw;
			i++;	
		}
		fclose(fp_r);
		if (i<500) error("Data entry number는 500 이상의 정수입니다.");	
		ref_num = i-1;
	}

}

//알고리즘 시뮬레이션 시작하는 함수
int simulate(void){

	//start simulation
	if (algorithm[0] == 8){
		optimal();
		fifo();
		lifo();
		lru();
		lfu();
		sc();
		esc();
	}
	else {
		for (int i=0; algorithm[i]!=0; i++){
			switch(algorithm[i]){
				case 1:
					optimal(); break;
				case 2:
					fifo(); break;
				case 3:
					lifo(); break;
				case 4:
					lru(); break;
				case 5:
					lfu(); break;
				case 6:
					sc(); break;
				case 7:
					esc(); break;
			}
		}
	}

}

//시작 전 옵션 처리하는 함수: 잘못된 입력은 에러를 발생시키고 종료함.
int choose_option(void){

	int cnt = 0;
	printf("[choose options]\n");

	//A
	printf("A. Choose Page Replacemennt algorithm simulator (up to 3)\n");
	printf("\t(1) Optimal\n\t(2) FIFO \n\t(3) LIFO \n\t(4) LRU \n\t(5) LFU \n\t(6) SC \n\t(7) ESC \n\t(8) ALL\n");
	printf("Enter number: ");
	
	char input[10];
	char *argv[4];
	int argc = 0;
	fgets(input, sizeof(input), stdin);
	input[strlen(input)-1] = '\0';
	argc = tokenize(input, argv);
	argv[argc] = (char*) 0;
	if (argc == 0) exit(0);

	char* tmp = "알고리즘 시뮬레이터는 all을 제외하고 최대 3개 선택할 수 있습니다.";
	
	if (argc>3 || argc<1) error(tmp);
	for (int i=0;i<argc; i++){
		if (sscanf(argv[i], "%d", &algorithm[i])<1){
			fprintf(stderr, "sscanf error\n");
			exit(1);
		}
		if (algorithm[i]<1 || algorithm[i]>8) error("알고리즘 시뮬레이터 번호는 1~8 사이의 정수입니다.");
	}
	if (argc == 2 && (algorithm[0]==8 || algorithm[1]==8)) error(tmp);
	if (argc == 3 && (algorithm[0]==8 || algorithm[1]==8 || algorithm[2]==8)) error(tmp);


	//B
	printf("B. Enter Page Frame number (3 to 10): ");
	scanf("%d", &pf_num);
	if (pf_num <3 || pf_num >10) error("페이지 프레임 개수는 3이상 10이하의 정수입니다.");
	
	//C
	int i;
	cnt = 0;
	printf("c. Choose Data entry generating method (1 or 2)\n");
	printf("\t(1) random\n\t(2) user file open\n");
	do {
		if (cnt++>0) printf("Illegal number. Enter number 1 or 2.\n");
		printf("Enter number: ");
		scanf("%d", &i);
	} while (!(i==1 || i==2));
	if (i==2) rd = 0; 
	else {
		rd = 1;
		printf("Enter random data entry number: ");
		scanf("%d", &ref_num);
		if (ref_num < 500) error("Data entry number는 500 이상의 정수입니다.");
	}

	//Save: open output file to result
	char c = 'y';
	cnt = 0;
	do{
		if (cnt++>0) printf("Illegal character. Enter Y(y) or N(n)\n");
		printf("Do you want to save result as output.txt? (Y(y)|N(n)): ");
		scanf(" %c", &c);
	} while (!(c=='Y' ||c=='y' ||c=='N' ||c=='n'));
	if (c == 'y' || c=='Y'){
		save = 1;
		if ((fp = fopen("output.txt", "w"))==NULL){
			fprintf(stderr, "fopen error with output.txt\n");
			exit(1);
		}
	}
	else{save = 0;}

	//Terminate
	cnt = 0;
	printf("The program would be terminated after simulation.");
	do{
		if (cnt++>0) printf("Illegal character. Enter Y(y) or N(n)\n");
		printf("Do you want to exit now? (Y(y)|N(n)): ");
		scanf(" %c", &c);
	} while (!(c=='Y' ||c=='y' ||c=='N' ||c=='n'));
	if (c == 'y' || c=='Y'){
		end();
		exit(0);
	}

}

// 주어진 input을 띄어쓰기 문자를 기준으로 argv로 split하고 그 개수를 리턴하는 함수.
int tokenize(char *input, char *argv[])
{
	char *ptr = NULL;
	int argc = 0;
	ptr = strtok(input, " ");

	while (ptr != NULL){
		argv[argc++] = ptr;
		ptr = strtok(NULL, " ");
	}
	return argc;
}

void error(char* s){
	printf("ERROR: %s\n", s);
	end();
	exit(0);
}


/*page replacement algorithm implementation*/

int esc(){

	int counter[pf_num];
	int cnt = 0;
	int rbit[pf_num];												//페이지 프레임의 read 레퍼런스 비트
	int wbit[pf_num];												//페이지 프레임의 write 레퍼런스 비트
	int page_frame[pf_num];
	int page_fault = 0;
	int occupied = 0;

	print_msg("\n[Enhanced Second Chance]\n");						//출력 및 저장

	for (int i=0; i<pf_num; i++) { 									//rbit, wbit 초기화
		rbit[i] = 0; wbit[i]=0;
		page_frame[i] = -1; counter[i]=0;							//페이지 프레임 초기화 후
	}
	print_page_frame(page_frame, "");								//페이지 프레임 출력

	for (int i=0; i<ref_num; i++) {									//페이지 ref 스트링 탐색
		int tmp;
		cnt++;
		if ((tmp = check_hit(references[i].data, page_frame, occupied))>=0) {
			print_page_frame(page_frame, "(hit!)"); 
			wbit[tmp] = 1;											//참조된 페이지 wbit 업데이트
			counter[tmp] = cnt;										//히트된 페이지 cnt 업데이트
			continue;
		}
		page_fault++;
		if (occupied >= pf_num){									//페이지 프레임이 가득 찬 경우
			//제일 오래된 애 선택 (FIFO)
			int min_idx = 0;
			for (int j=0; j<pf_num; j++){
				if (counter[j] < counter[min_idx]) min_idx = j;
			}
			//걔로부터 탐색해 우선순위 높은 애 선택 (00 > 01 > 10 > 11)
			int index = get_high_priority_idx(min_idx, rbit, wbit);
			page_frame[index] = references[i].data;
			//페이지 교체
			if(references[i].rw == 0){
				rbit[index] = 1;
				wbit[index] = 0;
			}
			else{
				rbit[index] = 0;
				wbit[index] = 1;
			}
			counter[index] = cnt;									//교체한 페이지 cnt 업데이트
		}
		else {														//cold page fault
			page_frame[occupied] = references[i].data;
			if(references[i].rw == 0){
				rbit[occupied] = 1;
				wbit[occupied] = 0;
			}
			else{
				rbit[occupied] = 0;
				wbit[occupied] = 1;
			}
			counter[occupied] = cnt;
			occupied++;
		}
		print_page_frame(page_frame, "");						//페이지 교체 후 페이지 프레임 출력
	}

	print_page_fault(page_fault);
}

/*esc()에서 페이지 폴트 시 victim 선택을 위해 사용되는 함수*/
int get_high_priority_idx(int head, int* rbit, int*wbit){
	
	int tmp[pf_num];
	if (rbit[head]==0 && wbit[head]==0) return head;
	//page frame 테이블 우선순위 저장
	for (int i=0; i<pf_num; i++){
		if (rbit[i]==0 && wbit[i]==0) tmp[i] = 0;
		else if (rbit[i]==0 && wbit[i]==1) tmp[i] = 1;
		else if (rbit[i]==1 && wbit[i]==0) tmp[i] = 2;
		else if (rbit[i]==1 && wbit[i]==1) tmp[i] = 3;
	}
	//우선순위가 가장 높은(tmp값 가장 작은) index 리턴
	for (int i=0; i<pf_num; i++){
		if (tmp[head] > tmp[i]){ head = i;}
	}
	return head;
}

int sc(){

	int head = 0;													//가장 오래된 페이지 가리킬 헤드 (FIFO)
	int rbit[pf_num];												//페이지 프레임의 레퍼런스 비트
	int page_frame[pf_num];
	int page_fault = 0;
	int occupied = 0;

	print_msg("\n[Second Chance]\n");								//출력 및 저장

	for (int i=0; i<pf_num; i++) { 
		rbit[i] = 0; 												//rbit 초기화
		page_frame[i] = -1;								 			//페이지 프레임 초기화 후
	}
	print_page_frame(page_frame, "");								//페이지 프레임 출력

	for (int i=0; i<ref_num; i++) {									//페이지 ref 스트링
		int tmp;
		if ((tmp = check_hit(references[i].data, page_frame, occupied))>=0) {
			print_page_frame(page_frame, "(hit!)"); 
			rbit[tmp] = 1;											//참조된 페이지 rbit 업데이트
			head = (head+1) % pf_num;							
			continue;
		}
		page_fault++;
		if (occupied >= pf_num){									//페이지 프레임이 가득 찬 경우
			//가장 먼저 들어간 ref 선택해 rbit 체크
			while (rbit[head]) { rbit[head]=0; head = (head+1) % pf_num; }
			page_frame[head] = references[i].data; 
			rbit[head]=1;	
		}
		else {														//cold page fault
			page_frame[occupied] = references[i].data;
			rbit[occupied] = 1;										//참조된 페이지 rbit 업데이트
			occupied++;
		}
		print_page_frame(page_frame, "");							//페이지 교체 후 페이지 프레임 출력
	}

	print_page_fault(page_fault);

}

int lfu(){

	int counter[pf_num];											//lfu ref 찾기 위한 카운터
	int page_frame[pf_num];
	int page_fault = 0;
	int occupied = 0;

	print_msg("\n[Least Frequently Used]\n");						//출력 및 저장

	for (int i=0; i<pf_num; i++) { 
		counter[i]=0; 												//counter 초기화
		page_frame[i] = -1;								 			//페이지 프레임 초기화 후
	}
	print_page_frame(page_frame, "");								//페이지 프레임 출력

	for (int i=0; i<ref_num; i++) {									//페이지 ref 스트링
		int tmp;
		if ((tmp = check_hit(references[i].data, page_frame, occupied))>=0) {
			print_page_frame(page_frame,  "(hit!)");
			counter[tmp]++;											//히트된 페이지 cnt 업데이트
			continue;
		}
		page_fault++;
		if (occupied >= pf_num){									//페이지 프레임이 가득 찬 경우
			//가장 레퍼런싱된지 오래된, 즉 counter 값이 가장 낮은 레퍼런스 선택
			int min_idx = 0;
			for (int j=0; j<pf_num; j++){
				if (counter[j] < counter[min_idx]) min_idx = j;
			}
			page_frame[min_idx] = references[i].data;
			counter[min_idx]++;										//교체한 페이지 cnt 업데이트
		}
		else {														//cold page fault
			page_frame[occupied] = references[i].data;
			counter[occupied]++;									//교체한 페이지 cnt 업데이트
			occupied++;
		}
		print_page_frame(page_frame, "");							//페이지 교체 후 페이지 프레임 출력
	}
	print_page_fault(page_fault);
}

/*lru 또다른 방식으로 구현한 함수: 프로그램 내에서 이 함수를 사용하지는 않는다.*/
int lru2(){

	int counter[pf_num], cnt=0;										//lru ref 찾기 위한 카운터
	int page_frame[pf_num];
	int page_fault = 0;
	int occupied = 0;

	print_msg("\n[Least Recently Used]\n");							//출력 및 저장

	for (int i=0; i<pf_num; i++) { 
		counter[i]=0; 												//counter 초기화
		page_frame[i] = -1;								 			//페이지 프레임 초기화 후
	}
	print_page_frame(page_frame, "");								//페이지 프레임 출력

	for (int i=0; i<ref_num; i++) {									//페이지 ref 스트링
		cnt++;														//페이지 ref 스트링 하나 읽을 때마다 cnt 증가
		int tmp;
		if ((tmp = check_hit(references[i].data, page_frame, occupied))>=0) {
			print_page_frame(page_frame,  "(hit!)");
			counter[tmp] = cnt;										//히트된 페이지 cnt 업데이트
			continue;
		}
		page_fault++;
		if (occupied >= pf_num){									//페이지 프레임이 가득 찬 경우
			//가장 레퍼런싱된지 오래된, 즉 counter 값이 가장 낮은 레퍼런스 선택
			int min_idx = 0;
			for (int j=0; j<pf_num; j++){
				if (counter[j] < counter[min_idx]) min_idx = j;
			}
			page_frame[min_idx] = references[i].data;
			counter[min_idx] = cnt;									//교체한 페이지 cnt 업데이트
		}
		else {														//cold page fault
			page_frame[occupied] = references[i].data;
			counter[occupied] = cnt;								//교체한 페이지 cnt 업데이트
			occupied++;
		}
		print_page_frame(page_frame, "");							//페이지 교체 후 페이지 프레임 출력
	}
	print_page_fault(page_fault);
}

int lru(){

	int page_frame[pf_num];
	int page_fault = 0;
	int occupied = 0;

	print_msg("\n[Least Recently Used]\n");							//출력 및 저장
	
	for (int i=0; i<pf_num; i++) { page_frame[i] = -1; }			//페이지 프레임 초기화 후
	print_page_frame(page_frame, "");								//페이지 프레임 출력

	for (int i=0; i<ref_num; i++) {									//페이지 ref 스트링
		if ((check_hit(references[i].data, page_frame, occupied))>=0) {
			print_page_frame(page_frame, "(hit!)");
			continue;
		}
		page_fault++;
		if (occupied >= pf_num){									//페이지 프레임이 가득 찬 경우
			//페이지 프레임 중 앞에서 가장 멀리서 사용된 레퍼런스 선택
			page_frame[find_farthest_idx(i-1, page_frame)] = references[i].data;
		}
		else {														//cold page fault
			page_frame[occupied++] = references[i].data;
		}
		print_page_frame(page_frame, "");							//페이지 교체 후 페이지 프레임 출력
	}
	print_page_fault(page_fault);
}

/*lru()에서 페이지 폴트 시 victim 선택을 위해 사용되는 함수*/
int find_farthest_idx(int start_idx, int* page_frame){

	int farthest_idx = 0;
	int distance[pf_num];

	for (int i=0; i<pf_num; i++){									//페이지 엔트리별로
		for (int j=start_idx; j>=0; j--){							//가장 멀리 있는 같은 ref 스트링 인덱스
			if (page_frame[i] == references[j].data){				//페이지 프레임에 있는 엔트리면
				distance[i] = j; 									//거리 계산 (j가 클수록 가까움)
				break;
			}
		}
	}

	//distance 중 가장 먼(값이 작은) idx 리턴
	for (int i=0; i<pf_num; i++){
		if (distance[i] < distance[farthest_idx]){
			farthest_idx = i;
		}
	}
	return farthest_idx;
}

int lifo(){

	int page_frame[pf_num];
	int page_fault = 0;
	int occupied = 0;

	print_msg("\n[LIFO]\n");										//출력 및 저장

	for (int i=0; i<pf_num; i++) { page_frame[i] = -1; }			//페이지 프레임 초기화 후
	print_page_frame(page_frame, "");								//페이지 프레임 출력

	for (int i=0; i<ref_num; i++) {									//페이지 ref 스트링
		if ((check_hit(references[i].data, page_frame, occupied))>=0) {print_page_frame(page_frame, "(hit!)"); continue;}
		page_fault++;
		if (occupied >= pf_num){									//페이지 프레임이 가득 찬 경우
			//가장 나중에 들어간 레퍼런스 선택
			page_frame[occupied-1] = references[i].data;
		}
		else {														//cold page fault
			page_frame[occupied++] = references[i].data;
		}
		print_page_frame(page_frame, "");							//페이지 교체 후 페이지 프레임 출력
	}
	
	print_page_fault(page_fault);

}

int fifo(){

	int page_frame[pf_num];
	int page_fault = 0;
	int occupied = 0;

	print_msg("\n[FIFO]\n");										//출력 및 저장

	for (int i=0; i<pf_num; i++) { page_frame[i] = -1; }			//페이지 프레임 초기화 후
	print_page_frame(page_frame, "");								//페이지 프레임 출력

	for (int i=0; i<ref_num; i++) {									//페이지 ref 스트링
		if ((check_hit(references[i].data, page_frame, occupied))>=0) {print_page_frame(page_frame, "(hit!)"); continue;}
		page_fault++;
		if (occupied >= pf_num){									//페이지 프레임이 가득 찬 경우
			//가장 먼저 들어간 레퍼런스 선택
			page_frame[(page_fault-1)%pf_num] = references[i].data;
		}
		else {														//cold page fault
			page_frame[occupied++] = references[i].data;
		}
		print_page_frame(page_frame, "");							//페이지 교체 후 페이지 프레임 출력
	}
	
	print_page_fault(page_fault);

}

int optimal(){

	int page_frame[pf_num];
	int page_fault = 0;
	int occupied = 0;

	print_msg("[OPTIMAL]\n");										//출력 및 저장

	for (int i=0; i<pf_num; i++) { page_frame[i] = -1; }			//페이지 프레임 초기화 후
	print_page_frame(page_frame, "");								//페이지 프레임 출력

	for (int i=0; i<ref_num; i++) {									//페이지 ref 스트링
		if ((check_hit(references[i].data, page_frame, occupied))>=0) {
			print_page_frame(page_frame, "(hit!)"); 
			continue;}
		page_fault++;
		if (occupied >= pf_num){									//페이지 프레임이 가득 찬 경우
			//페이지 프레임 중 나중에 가장 멀리서 사용되는 레퍼런스 선택
			int tmp = find_farthest_aft_idx(i+1, page_frame);
			page_frame[tmp] = references[i].data;
		}
		else {														//cold page fault
			page_frame[occupied++] = references[i].data;
		}
		print_page_frame(page_frame, "");							//페이지 교체 후 페이지 프레임 출력
	}
	
	print_page_fault(page_fault);
}

/*optimal()에서 페이지 폴트 시 victim 선택을 위해 사용되는 함수*/
int find_farthest_aft_idx(int start_idx, int* page_frame){

	int j;
	int max = start_idx;
	int max_idx = -1;

	for (int i=0; i<pf_num; i++){									//페이지 엔트리별로
		for (j=start_idx; j<ref_num; j++){							//가장 멀리 있는 같은 ref 스트링 인덱스
			if (page_frame[i] == references[j].data){
				if (j > max) { max = j; max_idx = i; }
				break;
			}
		}
		if (j==ref_num) return i;									//i번째 프레임이 ref 스트링에 없는 경우
	}

	return ((max_idx==-1)? 0: max_idx); 							//프레임 모두가 ref 스트링에 없는 경우
}

/*주어진 msg를 출력하고 파일에 저장하는 함수*/
int print_msg(char* msg){

	if (save) {fprintf(fp, "%s", msg);}
	printf("%s", msg);

}

/*주어진 페이지폴트 수를 출력하고 파일에 저장하는 함수*/
int print_page_fault(int page_fault){

	char tmp[100] = "";
	sprintf(tmp, "page_fault: %d개 / %d개\n", page_fault, ref_num);
	printf("%s", tmp);
	if (save ) {fprintf(fp, "%s", tmp);}

}

/* 모든 알고리즘에서 페이지 프레임 현재 상태 출력을 위한 함수*/
int print_page_frame(int* page_frame, char* hit){
	char tmp[100] = "";
	char* tmp1;
	for (int i=0; i<pf_num; i++){
		tmp1 = tmp;
		sprintf(tmp, "%s%d\t", tmp1, page_frame[i]);
	}
	tmp1 = tmp;
	sprintf(tmp, "%s%s\n", tmp1, hit);
	printf("%s", tmp);
	if (save ) {fprintf(fp, "%s", tmp);}
}

/* 모든 알고리즘에서 페이지 hit or fault 여부를 판단하기 위해 사용되는 함수*/
// hit 시 해당 페이지 인덱스를, fault 시 -1을 리턴한다.
int check_hit(int key, int* page_frame, int occupied){

	int i, ret = -1;
	if (occupied < pf_num) occupied++;

	for (i=0; i<occupied; i++){
		if (key == page_frame[i])
		{
			ret = i;
			break;
		}
	}
	return ret;
}
