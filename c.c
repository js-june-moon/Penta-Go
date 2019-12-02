#include <arpa/inet.h>
#include <netdb.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#define PORTNUM 5050

int count;

int is_end;					// 게임이 끝난것을 확인하는 변수,
										// 0이면 게임이 끝나지 않은 상태,
										// 1이면 흑돌 win, 2이면, 백돌 win 
char quadrant;			// 회전시킬 사분면을 저장하는 변수
char x, y, c;				// x,y는 보드의 좌표축, c는 회전시킬 사분면의 회전 방향,
										// c 는 clock wise 의 줄임말로, y or Y 가 입력되면 시계방향 회전
void get_board(int sd); // 현재 보드의 상태를 출력해주는 함수
int send_fix_board(int sd, char dol); // 현재 보드의 원하는 위치에 돌을 놓는 함수
void rotate_board(int quad, char is_clock_wise); // 현재 보드에 원하는 사분면에 원하는 방향으로 회전시키는 함수
int check_pentago(); // 게임이 끝났는지 확인하는 함수

int main(void) {
	int sd;
	char buf[1024];
	struct sockaddr_in sin, cli;
	
	if((sd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	memset((char*)&sin, '\0', sizeof(int));
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORTNUM);
	sin.sin_addr.s_addr = inet_addr("127.0.0.1");

	if(connect(sd, (struct sockaddr *)&sin, sizeof(sin))) {
		perror("connect");
		exit(1);
	}


	while (is_end == 0) {
		get_board(sd);

		while (send_fix_board(sd, '0') != 0);
		get_board(sd);
	}
	close(sd);

	return 0;
}

void get_board(int sd) {
	char buf[366];

	if(send(sd, "1", strlen("1")+1, 0) == -1) {
		perror("send");
		exit(1);
	}
	
	system("clear");

	if(recv(sd, buf, sizeof(buf), 0) == -1) {
		perror("recv");
		exit(1);
	}

	for(int i = 0; i < 14; i++) {
		for(int j = 0; j < 26; j++) {
			printf("%c", buf[26*i + j]);
		}
		printf("\n");
	}

}


// 보드에 돌을 놓는 함수
// row, col 에다가 dol을 놓는다.
int send_fix_board(int sd, char dol) {
	char x, y;
	char buf[1024];
	char str[4];
	char rcv[2];
	if(send(sd, "2", strlen("2")+1, 0) == -1) {
		perror("send");
		exit(1);
	}
	printf("fnum send\n");
	
	if(recv(sd, buf, sizeof(buf), 0) == -1) {
		perror("recv");
		exit(1);
	}
	printf("좌표 receive\n");
	printf("%s\n", buf);

	while(1) {
		scanf(" %c %c", &x, &y); // 플레이어가 원하는 좌표를 입력받는다.
		if (( (x >= 'A' && x <= 'F') || (x >= 'a' && x <= 'f')) && y >= '1' && y <= '6') {
			if (x >= 'a' && x <= 'f') x -= 32; // 'a' - 'A' = 32 소문자를 대문자로
			break;
			printf("잘못 입력하셨습니다. 다시 입력하세요 :");
		} else {
			printf("잘못 입력하셨습니다. 다시 입력하세요 :");
		}
	}
	str[0] = x;
	str[1] = y;
	str[2] = 'O';
	str[3] = '\0';

	if(send(sd, str, strlen(str)+1, 0) == -1) {
		perror("send");
		exit(1);
	}
	printf("좌표 send\n");
	if(recv(sd, rcv, sizeof(rcv), 0) == -1) {
		perror("recv");
		exit(1);
	}
	if (strcmp(rcv, "0") == 0)	return 0;
	else return -1;
}
