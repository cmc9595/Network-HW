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

FILE *out;
LinkTable *LT;
int nodeNum;
int t=0, m=0, c=0; // top, msg, chg line count variable
Topology TOP[1000];
Message MSG[100]; 
Change CHG[100];

void init_LT(){
	for(int i=0;i<nodeNum;i++){
		for(int j=0;j<nodeNum;j++){
			LT[i].link[j].dest = j;
			LT[i].link[j].next = -1;
			LT[i].link[j].dist = -1;

			if(i==j){
				LT[i].link[j].dist = 0;
				LT[i].link[j].next = i;
			}
		}
	}
}

void print_LT(){
	for(int i=0;i<nodeNum;i++){
		for(int j=0;j<nodeNum;j++){
			if(LT[i].link[j].dest!=i && LT[i].link[j].next==-1 && LT[i].link[j].dist==-1) // -1 -1은 출력x
				continue;
			fprintf(out, "%d %d %d\n", LT[i].link[j].dest, LT[i].link[j].next, LT[i].link[j].dist);
		}
		fprintf(out, "\n");
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
void dijkstra(){
	//topology info apply
	for(int i=0;i<t;i++){
		int node1 = TOP[i].node1;
		int node2 = TOP[i].node2;
		int cost = TOP[i].cost;

		LT[node1].link[node2].next = node2;
		LT[node1].link[node2].dist = cost;
		LT[node2].link[node1].next = node1;
		LT[node2].link[node1].dist = cost;
	}

	for(int tnum=0;tnum<nodeNum;tnum++){
		int v[nodeNum] = {0, }; //visit
		v[tnum] = 1;
		for(int i=0;i<nodeNum-1;i++){
			int select = -1;
			//매 턴select를 고른다.
			for(int j=0;j<nodeNum;j++){
				if(v[j]==0 && LT[tnum].link[j].dist>0){
					if(select==-1){
						select = LT[tnum].link[j].dest;
					}
					else{
						if(LT[tnum].link[j].dist < LT[tnum].link[select].dist)
							select = LT[tnum].link[j].dest;
						if(LT[tnum].link[j].dist == LT[tnum].link[select].dist){ //tie-breaking rule 2
							if( j < select )
								select = j;
						}
					}
				}
			}
			//printf("select: %d\n", select);
			v[select] = 1;
			//topology에서 갈 수 있는 곳을 찾아본다.
			for(int j=0;j<t;j++){
				int node1 = TOP[j].node1;
				int node2 = TOP[j].node2;
				int costt = TOP[j].cost;

				int r=-1; // 0, 4
				if( node1==select || node2==select ){
					if(node1==select)
						r = node2;
					else
						r = node1;
				}
				//printf("r: %d\n", r);

				if(r!=-1 && v[r]==0){
					if(LT[tnum].link[r].dist==-1){
						LT[tnum].link[r].dist = LT[tnum].link[select].dist + costt;
						LT[tnum].link[r].next = LT[tnum].link[select].next;
					}
					else{
						if(LT[tnum].link[r].dist > LT[tnum].link[select].dist + costt){
							LT[tnum].link[r].dist = LT[tnum].link[select].dist + costt;
							LT[tnum].link[r].next = LT[tnum].link[select].next;
						}
						if(LT[tnum].link[r].dist == LT[tnum].link[select].dist + costt){
							if( r < select ){
								LT[tnum].link[r].dist = LT[tnum].link[select].dist + costt;
								LT[tnum].link[r].next = LT[tnum].link[select].next;
							}
						}
					}

				}
			}
		}
	}
}
void print_MSG(){
	for(int i=0;i<m;i++){
		int start = MSG[i].start;
		int end = MSG[i].end;
		char* msg = MSG[i].message;

		int current = start;
		int cost=0;
		int arr[nodeNum]={0,};
		int idx=0;

		if(LT[start].link[end].dist==-1 && LT[start].link[end].next==-1){
			fprintf(out, "from %d to %d cost infinite hops unreachable message %s\n", start, end, msg);
			return ;
		}

		while(current!=end){
			arr[idx++] = current;
			if(current==start){
				cost = LT[current].link[end].dist;
			}
			current = LT[current].link[end].next;
		}
		fprintf(out, "from %d to %d cost %d hops ", start, end, cost);
		for(int i=0;i<idx;i++)
			fprintf(out, "%d ", arr[i]);
		fprintf(out, "message %s\n", msg);
	}
}
int main(int argc, char*argv[]){

	if(argc != 4){
		printf("usage: linkstate topologyfile messagesfile changesfile\n");
		exit(1);
	}

	for(int i=1;i<4;i++){
		FILE *fp;
		if( (fp = fopen(argv[i], "r")) == NULL){
			printf("Error: open input file.");
			exit(1);
		}
		char buffer[BUFSIZE];
		memset(buffer, 0, sizeof(buffer));

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

	out = fopen("output_ls.txt", "w");
	//print_topology();
	//print_message();
	//print_change();

	init_LT();

	dijkstra();

	print_LT();
	print_MSG();
	fprintf(out, "\n");
	
	for(int i=0;i<c;i++){
		//apply change
		int start = CHG[i].start;
		int end = CHG[i].end;
		int cost = CHG[i].cost;
		
		if(cost<0){ //link 끊어짐
			LT[start].link[end].dist = -1;
			LT[start].link[end].next = -1;
			LT[end].link[start].dist = -1;
			LT[end].link[start].next = -1;

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
		dijkstra();

		print_LT();
		print_MSG();
		fprintf(out, "\n");
	}

	printf("Complete. Output file written to output_ls.txt.\n");
	fclose(out);
	free(LT);
	return 0;
}
