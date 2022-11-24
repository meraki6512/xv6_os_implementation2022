#include "types.h"
#include "stat.h"
#include "user.h"

#define PNUM 5
#define PRINT_CYCLE 10000000
#define TOTAL_COUNT 500000000

int main(void){

	int n, pid;

	printf(1, "start sdebug command.\n");

	//fork : fork하면서 내부적으로 weightset() 호출
	for (n = 0; n < PNUM; n++){

		if ((pid=fork())<0){
			break;
		}
		else if (pid == 0){

			int first = 1;
			int print_counter = PRINT_CYCLE;
			int start = uptime();
	
			for (int counter = 0; counter < TOTAL_COUNT; counter++){
		
				if (print_counter-- == 0){

					if (first){
						int end = uptime();
						int between = (end - start) * 10;
						printf(1, "PID: %d, ", getpid());
						printf(1, "WEIGHT: %d, ", getpid()); /*수정 fix*/
						printf(1, "TIMES: %d ms\n", between);
						first = 0;
					}

					print_counter = PRINT_CYCLE;

				}

			}

			printf(1, "PID %d terminated.\n");
			exit();

		}
	}

	//wait (til child processes all terminated)
	for (; n>0; n--){
		if (wait() < 0){
			printf(1, "wait stopped early\n");
			exit();
		}
	}

	if (wait() != -1){
		printf(1, "wait got too many\n");
		exit();
	}

	printf(1, "end of sdebug command.\n");
	exit();
	
}


/*
 * TOTAL _COUNTER는 프로세스 종료할 때 counter 값 (500000000)
 * counter
- 변수타입: 크기고려
- fork()로 생성된 프로세스가 생성 이후 스케줄되어 수행되는 시간
- counter는 프로세스가 수행될 때, 매 clock_tick마다 1씩 증가한다.
- counter == TOTAL_COUNTER가 되면, 해당 프로세스 종료.

* PRINT_CYCLE은 프로세스 정보를 출력할때 counter값 (10000000가 되면 출력)
- 프로세스당 1번만 출력
- PID, WEIGHT, TIMES
- TIMES = (프로세스 정보출력시간 - 프로세스시작시간) *10 //uptime 이용
*/
