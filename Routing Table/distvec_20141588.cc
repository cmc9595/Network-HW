#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1000

typedef struct link {
	int dest;
	int next;
	int dist;
} Link;

typedef struct linktable{
	Link *link;
} LinkTable;

typedef struct topology {
	int node1;
	int node2;
	int cost;
} Topology;

typedef struct message {
	int start;
	int end;
	char message[1000];
} Message;

typedef struct change {
	int start;
	int end;
	int cost;
} Change;

FILE *output;
LinkTable *LT;
int nodeNum;
int t=0, m=0, c=0; // top, msg, chg line count variable
Topology TOP[1000];
Message MSG[100]; 
Change CHG[100];

void init_LT(){ // topoloy table로 Link Table 초기화.
	for(int i=0;i<nodeNum;i++){
		for(int j=0;j<nodeNum;j++){
			LT[i].link[j].dest = j;
			LT[i].link[j].next = 0;
			LT[i].link[j].dist = 0;
		}
	}
	for(int i=0;i<t;i++){
		int node1 = TOP[i].node1;
		int node2 = TOP[i].node2;
		int cost = TOP[i].cost;

		LT[node1].link[node2].next = node2;
		LT[node1].link[node2].dist = cost;
		LT[node2].link[node1].next = node1;
		LT[node2].link[node1].dist = cost;
	}
}
void print_LT(){
	for(int i=0;i<nodeNum;i++){
		for(int j=0;j<nodeNum;j++){
			if(LT[i].link[j].dest!=i && LT[i].link[j].next==0 && LT[i].link[j].dist==0) //0 0인 것은 출력 안함(자기자신은 출력함)
				continue;
			fprintf(output, "%d %d %d\n", LT[i].link[j].dest, LT[i].link[j].next, LT[i].link[j].dist);
		}
		fprintf(output, "\n");
	}
}
int update_neighbor(){
	int update_count = 0;

	for(int i=0;i<t;i++){
		int node1 = TOP[i].node1; // D
		int node2 = TOP[i].node2; // E
		int cost = TOP[i].cost;

		//node2 update
		for(int j=0;j<nodeNum;j++){
			if(LT[node2].link[j].dest==node2){ // 목적지가 나
				LT[node2].link[j].next = node2;
			}
			else if(LT[node1].link[j].next==0&&LT[node1].link[j].dist==0){ // 아직 node1쪽에서 계산안됨
				continue;
			}
			else{
				if(LT[node2].link[j].dist==0)
				{
					LT[node2].link[j].next = node1;
					LT[node2].link[j].dist = cost + LT[node1].link[j].dist;
					update_count++;
					//printf("update, %d->%d, j=%d\n", node1, node2, j);
				}
				else
				{
					if(cost + LT[node1].link[j].dist < LT[node2].link[j].dist){
						LT[node2].link[j].next = node1;
						LT[node2].link[j].dist = cost + LT[node1].link[j].dist;
						update_count++;
						//printf("update, %d->%d, j=%d\n", node1, node2, j);
					}
					if(cost + LT[node1].link[j].dist == LT[node2].link[j].dist){ // Tie-breaking #1
						if( node1 < LT[node2].link[j].next ){ // node1 선택
							LT[node2].link[j].next = node1;
							LT[node2].link[j].dist = cost + LT[node1].link[j].dist;
							update_count++;
						}
					}
				}
			}
		}
		//node1 update
		for(int j=0;j<nodeNum;j++){
			if(LT[node1].link[j].dest==node1){
				LT[node1].link[j].next = node1;
			}
			else if(LT[node2].link[j].next==0&&LT[node2].link[j].dist==0){
				continue;
			}
			else{
				if(LT[node1].link[j].dist==0)
				{
					LT[node1].link[j].next = node2;
					LT[node1].link[j].dist = cost + LT[node2].link[j].dist;
					update_count++;
					//printf("update, %d->%d, j=%d\n", node2, node1, j);
				}
				else
				{
					if(cost + LT[node2].link[j].dist < LT[node1].link[j].dist){
						LT[node1].link[j].next = node2;
						LT[node1].link[j].dist = cost + LT[node2].link[j].dist;
						update_count++;
						//printf("update, %d->%d, j=%d\n", node2, node1, j);
					}
					if(cost + LT[node2].link[j].dist == LT[node1].link[j].dist){
						if( node2 < LT[node1].link[j].next ){
							LT[node1].link[j].next = node2;
							LT[node1].link[j].dist = cost + LT[node2].link[j].dist;
							update_count++;
						}
					}
				}
			}
		}
	}
	//printf("update: %d\n", update_count);
	return update_count;
}
void find_path(int start, int end, char* message){
	int current = start;
	int cost=0;
	int arr[nodeNum]={0,};
	int idx=0;

	if(LT[start].link[end].dist==0 && LT[start].link[end].next==0){ // 도달 불가
		fprintf(output, "from %d to %d cost infinite hops unreachable message %s\n", start, end, message);
		return ;
	}

	while(current!=end){

		arr[idx++] = current;
		if(current==start){
			cost = LT[current].link[end].dist;
		}
		current = LT[current].link[end].next;
	}
	fprintf(output, "from %d to %d cost %d hops ", start, end, cost);
	for(int i=0;i<idx;i++)
		fprintf(output, "%d ", arr[i]);
	fprintf(output, "message %s\n", message);
}
void print_MSG(){
	for(int i=0;i<m;i++){
		int start = MSG[i].start;
		int end = MSG[i].end;
		char* msg = MSG[i].message;
		find_path(start, end, msg);
	}
}
void print_topology(){
	printf("*****<TOPOLOGY>*****\n");
	printf("%d\n", nodeNum);
	for(int i=0;i<t;i++){
		printf("%d %d %d\n", TOP[i].node1, TOP[i].node2, TOP[i].cost);
	}
}
void print_message(){
	printf("*****<MESSAGE>******\n");
	for(int i=0;i<m;i++)
		printf("%d %d %s\n", MSG[i].start, MSG[i].end, MSG[i].message);
}
void print_change(){
	printf("*****<CHANGE>*******\n");
	for(int i=0;i<c;i++)
		printf("%d %d %d\n", CHG[i].start, CHG[i].end, CHG[i].cost);
}
int main(int argc, char *argv[]){
	if(argc != 4){
		printf("usage: linkstate topologyfile messagesfile changesfile\n");
		exit(1);
	}
	// input file open
	for(int i=1;i<4;i++){
		FILE *fp;
		if( (fp = fopen(argv[i], "r")) == NULL){
			printf("Error: open input file.");
			exit(1);
		}
		char buffer[BUFSIZE];
		memset(buffer, 0, sizeof(buffer));
		//fread(buffer, sizeof(buffer), 1, fp); // file 전체 읽어서 buffer에 저장

		if(i==1){ // topologyfile
			nodeNum = atoi(fgets(buffer, sizeof(buffer), fp));

			//malloc
			LT = (LinkTable*)malloc(sizeof(LinkTable)*nodeNum);
			for(int i=0;i<nodeNum;i++){
				LT[i].link = (Link*)malloc(sizeof(Link)*nodeNum);
			}

			while( fgets(buffer, sizeof(buffer), fp) != NULL ){
				char *ptr = strtok(buffer, " ");
				int start, end, cost;
				start = atoi(ptr);
				ptr = strtok(NULL, " ");
				end = atoi(ptr);
				ptr = strtok(NULL, " ");
				cost = atoi(ptr);
				//printf("%d %d %d\n", start, end, cost);

				TOP[t].node1 = start;
				TOP[t].node2 = end;
				TOP[t].cost = cost;
				t++;
			}
		}
		else if(i==2){ // messagesfile
			while( fgets(buffer, sizeof(buffer), fp) != NULL ){
				char *ptr = strtok(buffer, " ");
				int start, end;
				char msg[1000];
				start = atoi(ptr);
				ptr = strtok(NULL, " ");
				end = atoi(ptr);
				ptr = strtok(NULL, "\n");
				strcpy(msg, ptr);
				//printf("%d %d %s\n", start, end, msg);

				MSG[m].start = start;
				MSG[m].end = end;
				strcpy(MSG[m].message, msg);
				m++;
			}
		}
		else{ // changesfile
			while( fgets(buffer, sizeof(buffer), fp) != NULL ){
				char *ptr = strtok(buffer, " ");
				int start, end, cost;
				start = atoi(ptr);
				ptr = strtok(NULL, " ");
				end = atoi(ptr);
				ptr = strtok(NULL, " ");
				cost = atoi(ptr);

				CHG[c].start = start;
				CHG[c].end = end;
				CHG[c].cost = cost;
				c++;
			}
		}
		fclose(fp);
	}

	//print_topology();
	//print_message();
	//print_change();

	output = fopen("output_dv.txt", "w");

	init_LT();
	while((update_neighbor()) != 0){
	}
	print_LT();
	print_MSG();
	fprintf(output, "\n");

	for(int i=0;i<c;i++){
		//apply change
		int start = CHG[i].start;
		int end = CHG[i].end;
		int cost = CHG[i].cost;
		
		if(cost<0){ //link 끊어짐
			LT[start].link[end].dist = 0;
			LT[start].link[end].next = 0;
			LT[end].link[start].dist = 0;
			LT[end].link[start].next = 0;

			//topology table 에서 삭제
			for(int j=0;j<t;j++){
				int node1 = TOP[j].node1;
				int node2 = TOP[j].node2;

				if( ((node1==start)&&(node2==end)) || ((node1==end)&&(node2==start)) ){ //찾으면 삭제
					for(int k=j;k<t-1;k++){
						TOP[k] = TOP[k+1];
					}
					t--;
				}
			}
			//LinkTable 초기화
			init_LT();
		}
		else{
			LT[start].link[end].dist = cost;
			LT[start].link[end].next = end;
			LT[end].link[start].dist = cost;
			LT[end].link[start].next = start;

			//topology table 반영
			int find = 0;
			for(int j=0;j<t;j++){
				int node1 = TOP[j].node1;
				int node2 = TOP[j].node2;
				if( ((node1==start)&&(node2==end)) || ((node1==end)&&(node2==start)) ){ //있으면 값 바꾸고
					TOP[j].cost = cost;
					find = 1;
				}
			}
			if(find==0){ // 없으면 추가
				TOP[t].node1 = start;
				TOP[t].node2 = end;
				TOP[t].cost = cost;
				t++;
			}
		}

		//print_topology();
		//distvec routing
		int k=0;
		while((update_neighbor()) != 0){
			k++;
		}
		//printf("%d 번 업데이트\n", k); 
		print_LT();

		//message 출력
		print_MSG();
		fprintf(output, "\n");
	}

	printf("Complete. Output file written to output_dv.txt.\n");
	free(LT);
	fclose(output);
	return 0;
}
