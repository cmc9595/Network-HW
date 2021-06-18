#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define MAXDATASIZE 10000 //buffer size

int main(int argc, char *argv[]){
	int sockfd;
	int numbytes;
	char buf[MAXDATASIZE];
	struct addrinfo hints, *servinfo;
	int rv;
	char s[INET_ADDRSTRLEN]; //ip address

	if( (argc != 2) or (strncmp(argv[1], "http://", 7)!=0) ){
		fprintf(stderr, "usage: http_client http://hostname[:port][/path/to/file]\n");
		exit(1);
	}

	char* http = argv[1];
	char* addr = &argv[1][7];
	char* port = strchr(addr, ':');
	char* path = strchr(addr, '/');

	char addr_str[100] = "";
	char port_str[100] = "";
	char path_str[100] = "";

	if(path!=NULL){ 
		//path o port o
		if(port!=NULL){ 
			strncpy(addr_str, addr, strlen(addr)-strlen(port));
			strncpy(port_str, port, strlen(port)-strlen(path));
			strncpy(path_str, path ,strlen(path));
		}
		//path o port x
		else{
			strncpy(addr_str, addr, strlen(addr)-strlen(path));
			strcpy(port_str, ":80");
			strncpy(path_str, path ,strlen(path));
		}
	}
	else{
		//path x port o
		if(port!=NULL){
			strncpy(addr_str, addr, strlen(addr)-strlen(port));
			strncpy(port_str, port, strlen(port));
			strcpy(path_str, "/");
		}
		//path x port x
		else{
			strncpy(addr_str, addr, strlen(addr));
			strcpy(port_str, ":80");
			strcpy(path_str, "/");
		}
	}
	char* portnum = port_str+1;
	if(path==NULL && port==NULL)
		strcpy(addr_str, addr);
	
	//printf("http: %s\naddr: %s\nport: %s\npath: %s\n", http, addr_str, portnum, path_str);
	memset(&hints, 0, sizeof hints);
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	//domain name error
	//웃긴게 https://붙으면 안됨
	if((rv = getaddrinfo(addr_str, portnum, &hints, &servinfo)) != 0){
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
		return 1;
	}
	//create socket
	if((sockfd = socket(servinfo->ai_family, servinfo->ai_socktype, servinfo->ai_protocol)) == -1){
		perror("socket");
		exit(1);
	}
	//tcp connect fail
	if(connect(sockfd, servinfo->ai_addr, servinfo->ai_addrlen) == -1){
		close(sockfd);
		perror("connect");
		exit(1);
	}
	//inet_ntop(servinfo->ai_family, &((struct sockaddr_in*)servinfo->ai_addr)->sin_addr, s, sizeof s);
	//printf("client: connecting to %s\n", s);
	freeaddrinfo(servinfo);

	char msg[1000] = "";
	if(strlen(path_str)==0)
		strcpy(path_str, "/");
	
	sprintf(msg, "GET %s HTTP/1.1\r\nHost: %s%s\r\n\r\n", path_str, addr_str, port_str);
	//printf("**sent:\n%s\n", msg);

	if(send(sockfd, msg, strlen(msg), 0) == -1){
		perror("send");
		close(sockfd);
		exit(1);
	}

	FILE *fp = fopen("20141588.out", "w");
	char candidate[3][15] = {"Content-Length", "Content-length", "content-length"};
	int firstline=0, find=0;
	int current=0, content_length=0;
	
	while( (numbytes = recv(sockfd, buf, MAXDATASIZE-1, 0)) >= 0 ){
		//printf("********numbytes = %d\n", numbytes);
		if(numbytes==-1){
			perror("recv");
			close(sockfd);
			exit(1);
		}
		buf[numbytes] = '\0';
		
		//strtok등을 사용하면 기존 buf가 변형되므로 tmp에복사해서 사용
		char tmp[MAXDATASIZE] = "";
		strcpy(tmp, buf);

		//첫 줄 읽기
		if(firstline == 0){
			char* line = strtok(tmp, "\r\n");
			printf("%s\n", line);
			strcpy(tmp, buf); //tmp계속 초기화해야 버그방지
			firstline = 1;
		}

		//Content-Length 찾기
		if(find==0){
			for(int i=0;i<sizeof(candidate)/sizeof(candidate[0]);i++){
				char *cl, *ptr;
				if( (cl=strstr(tmp, candidate[i])) != NULL ){
					printf("%s bytes written to 20141588.out\n", ptr=strtok(cl+16, "\r\n"));
					content_length = atoi(ptr);
					find = 1;
					break;
				}
			}
		}
		if(find==0){
			printf("Content-Length not specified.\n");
			exit(1);
		}
		//파일에 쓰기
		fprintf(fp, "%s", buf);
		current += numbytes;
		//printf("%d/%d done\n", current, content_length);
		//printf("server: %s", buf);
		if(current > content_length)
			break;
	}
	fclose(fp);
	close(sockfd);
	return 0;
}
