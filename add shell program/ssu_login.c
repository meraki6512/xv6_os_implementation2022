//적절한 list.txt가 존재한다고 생각하고 구현.
#include "types.h"
#include "stat.h"
#include "user.h"
#include "fcntl.h"

char userID[16][32];
char pwdID[16][32];

//list.txt의 정보를 userID와 pwdID에 불러옴.
void get_user_list(){
	
	int fd;
	int i;
	int n;
	char buf[1024]; 
	//char buf[2]; 
	_Bool flag = 1;
	int a = 0, b = 0;

	if ((fd = open("list.txt", O_RDONLY))<0){
		printf(1, "ssu_login: open \"list.txt\" failed.\n");
		exit();
	}

	for (i=0; i<30; i++){
		if ((n = read(fd, buf, sizeof(buf)))<0){
			printf(1, "ssu_login: get_user_list() failed.\n");
			exit();
		}
		else if (n==0)
			break;

		for (int j=0; buf[j]!='\0'; j++){

			if (buf[j] == ' '){ //flag=0
				//userID[a][b] = '\0';
				flag = 0;
				b = 0;
				continue;
			}
			else if (buf[j] == '\n'){ //flag=1
				//pwdID[a][b] = '\0';
				flag = 1;
				a++;
				b = 0;
				continue;
			}

			if (flag){ //ID 저장
				userID[a][b++] = buf[j];
			}
			else { //pwd 저장
				pwdID[a][b++] = buf[j];
			}

			printf(1, "");
		}
	}
}

//개행문자제거
void func(char buf[], char* idpwd){
	int i = 0;
	while ((buf[i]!='\n')&&(buf[i]!='\0')){
		idpwd[i] = buf[i];
		i++;
	}
	idpwd[i] = '\0';
}

//id, pw 비교.
int check_idpw(){

	int i;
	char buf[32];
	char ID[32];
	char pwd[32];

	printf(1, "Username: ");
	gets(buf, 32);
	func(buf, ID);
	printf(1, "Password: ");
	gets(buf, 32);
	func(buf, pwd);

	for (i=0; userID[i][0]!='\0'; i++){
		if ((!strcmp(userID[i], ID))&&(!strcmp(pwdID[i], pwd)))
			return 1;
	}

	printf(1, "wrong id/password\n");
	return 0;

}

void print_user_list(){
	for (int i=0; userID[i][0]!='\0'; i++){
		printf(1, "\n[%d]\n", i);
		printf(1, "userID: %s\n", userID[i]);
		printf(1, "pwdID: %s\n", pwdID[i]);
	}
}


int main(int argc, char* argv[]){

	int pid, wpid;

	get_user_list();
	//print_user_list();

	while (check_idpw()){
		for(;;){
			printf(1, "init: starting sh\n");
			pid = fork();
			if(pid < 0){
				printf(1, "init: fork failed\n");
				exit();
			}
			if(pid == 0){
				exec("sh", argv);
				printf(1, "init: exec sh failed\n");
				exit();
			}
			while((wpid=wait()) >= 0 && wpid != pid)
				printf(1, "zombie!\n");
		}
	}

	return 0;
}
