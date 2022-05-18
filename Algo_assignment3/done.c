#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define PEASANT 0x08
#define WOLF	0x04
#define GOAT	0x02
#define CABBAGE	0x01

/*
프로그램의 결과는 pwgc.net을 생성하고, 초기상태로부터 목적상태로의 모든 가능한 경로에 대한 탐색 과정을 화면에 출력해야 한다.
  - 깊이우선(depth-first) 탐색을 사용
  - 현재 상태에서 다음 상태로의 전이는 다음의 순서로 적용해야 함 (농부의 이동, 농부와 늑대의 이동, 농부와 염소의 이동, 농부와 양배추의 이동)
    위 순서대로 해야 같은 결과를 얻을 수 있음.
  - pwgc.net은 Pajek 프로그램에서 사용하는 그래프(네트워크) 파일 형식임
  - Pajek 프로그램 페이지 (http://mrvar.fdv.uni-lj.si/pajek/)
*/
// 선언
static void print_statename( FILE *fp, int state);
static void get_pwgc( int state, int *p, int *w, int *g, int *c);
static int is_dead_end( int state);
static int is_possible_transition( int state1,	int state2);
static int changeP( int state);
static int changePW( int state);
static int changePG( int state);
static int changePC( int state);
static int is_visited( int visited[], int depth, int state);
static void print_path( int visited[], int depth);
static void dfs_main( int initial_state, int goal_state, int depth, int visited[]);
void make_adjacency_matrix( int graph[][16]);
void print_graph( int graph[][16], int num);
void save_graph( char *filename, int graph[][16], int num);
// end
// 데드엔드, 상태가 전이 가능한지, 이미 방문했던곳인지
// 주어진 상태 state의 이름(마지막 4비트)을 화면에 출력
// 예) state가 7(0111)일 때, "<0111>"을 출력
static void print_statename( FILE *fp, int state){
	int a[4] = {0, };
	int idx = 0;
	for (int i=8; i>0; i/=2){
		if (state >= i){
			a[idx] = 1;
			state -= i;
		}
		idx++;
	}
	printf("<%d%d%d%d>", a[0], a[1], a[2], a[3]);
}

// 주어진 상태 state에서 농부, 늑대, 염소, 양배추의 상태를 각각 추출하여 p, w, g, c에 저장
// 예) state가 7(0111)일 때, p = 0, w = 1, g = 1, c = 1
static void get_pwgc( int state, int *p, int *w, int *g, int *c){
	int a[4] = {0, };
	int idx = 0;
	for (int i=8; i>0; i/=2){
		if (state >= i){
			a[idx] = 1;
			state -= i;
		}
		idx++;
	}

	*p = a[0];
	*w = a[1];
	*g = a[2];
	*c = a[3];
}

// 허용되지 않는 상태인지 검사
// 예) 농부없이 늑대와 염소가 같이 있는 경우 / 농부없이 염소와 양배추가 같이 있는 경우
//    0110 / 0011 / 0111
// return value: 1 허용되지 않는 상태인 경우, 0 허용되는 상태인 경우
static int is_dead_end( int state){
	int a[4] = {0, };
	int idx = 0;
	for (int i=8; i>0; i/=2){
		if (state >= i){
			a[idx] = 1;
			state -= i;
		}
		idx++;
	}

	if ( (a[0]==0 && a[1]==1 && a[2]==1) || (a[0]==0 && a[2]==1 && a[3]==1) ){ //건너가는쪽 확인
		return 1;
	}
	for (int i=0; i<4; i++) a[i] = 1-a[i];
	if ( (a[0]==0 && a[1]==1 && a[2]==1) || (a[0]==0 && a[2]==1 && a[3]==1) ){ //반대편 확인
		return 1;
	}
	else{
		return 0;
	}
}

// state1 상태에서 state2 상태로의 전이 가능성 점검
// 농부 또는 농부와 다른 하나의 아이템이 강 반대편으로 이동할 수 있는 상태만 허용
// 허용되지 않는 상태(dead-end)로의 전이인지 검사
// return value: 1 전이 가능한 경우, 0 전이 불이가능한 경우 
// 강 양쪽에 있는애를 함께 옮긴다 하는건 불가능하지.
static int is_possible_transition( int state1,	int state2){
	// 0: 옮기면 죽는상황, 양쪽에 있는애를 옮기려 하는. // 3개이상 옮기는건 사전에 안하겠지?..
	if (is_dead_end(state1)==1) return 0;
	if (is_dead_end(state2)==1) return 0;
	if ( (state1>>3)==(state2>>3) ) return 0; // p위치가 같으면 안되지. 이동하면 바뀌어야되니까.
	
	int xor = state1^state2;
	//fprintf(stderr,"xor:[%d]\n", xor);
	int a[5] = {0, };
	int idx=0;
	int cnt=0;
	for (int i=8; i>0; i/=2){
		if (xor >= i){
			a[idx] = 1;
			xor -= i;
		}
		idx++;
	}

	for (int i=0; i<4; i++){
		if (a[i]==1) cnt++;
	}
	if (cnt>2) return 0;
	return 1;
	/*
	int diff;
	if (state1 > state2){
		diff = state1 - state2;
	}
	else{
		diff = state2 - state1;
	}
	// diff -to-binary-> 이동한 것들만 1
	if (diff < 8) return 0; //8보다 작을 시, 사람 없이 이동한다는 의미. -> 불가능
	else return 1;
	*/
}

// 상태 변경: 농부 이동
// return value : 새로운 상태
static int changeP( int state){
	return state^8; //xor 하면 해당 비트만 반전됨.
}

// 상태 변경: 농부, 늑대 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePW( int state){
	if ( (bool)(state&(1<<3))== (bool)(state&(1<<2)) ){
		return state^12;
	}
	else return -1;
}

// 상태 변경: 농부, 염소 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1
static int changePG( int state){
	if ( (bool)(state&(1<<3))== (bool)(state&(1<<1)) ){
		return state^10;
	}
	else return -1;
}
// 상태 변경: 농부, 양배추 이동
// return value : 새로운 상태, 상태 변경이 불가능한 경우: -1 
static int changePC( int state){
	if ( (bool)(state&(1<<3))== (bool)(state&(1<<0)) ){
		return state^9;
	}
	else return -1;
}

// 주어진 state가 이미 방문한 상태인지 검사
// return value : 1 visited, 0 not visited
static int is_visited( int visited[], int depth, int state){
	for (int i=0; i<depth; i++){
		if ( visited[i]==state ) return 1;
	}
	return 0;
}

// 방문한 경로(상태들)을 차례로 화면에 출력
static void print_path( int visited[], int depth){
	for (int n=0; n<=depth; n++){
		int a[4] = {0, };
		int idx = 0;
		int tmp = visited[n];
		for (int i=8; i>0; i/=2){
			if ( tmp >= i){
				a[idx] = 1;
				tmp -= i;
			}
			idx++;
		}
		printf( "<%d%d%d%d>\n", a[0], a[1], a[2], a[3] );
	}
	printf("\n");
}

// recursive function 순서는 사람, 사람+늑대, 사람+염소, 사람+양배추 순서로.
// depth is even: 건너가는, odd: 돌아오는. 
static void dfs_main( int initial_state, int goal_state, int depth, int visited[]){
	visited[depth] = initial_state;
	
	int a[4] = {0, };
	int idx = 0;
	int tmp = initial_state;
	for (int i=8; i>0; i/=2){
		if (tmp >= i){
			a[idx++] = 1;
			tmp -= i;
		}
	} 
	//printf("current state is <%d%d%d%d> (depth %d)\n", a[0], a[1], a[2], a[3], depth);
	printf("current state is ");
	print_statename(stdout, initial_state);
	printf(" (depth %d)\n", depth);
	if (initial_state == goal_state) {
		//printf("current state is <%d%d%d%d> (depth %d)\n", a[0], a[1], a[2], a[3], depth);
		printf("Goal-state found!\n"); 
		print_path(visited, depth);
		// back to 2번
	}
	else{
		int arr[4]={changeP(initial_state), changePW(initial_state), changePG(initial_state), changePC(initial_state)};
		//for (int i=0; i<4; i++) fprintf(stderr, "%d \n", arr[i]);
		// 건너가는애가 양쪽에 따로 있는 상황도 검출해야됨.
		for (int i=0; i<4; i++){
			if ( (arr[i] != -1)&&(is_dead_end(arr[i])==0)&&(is_visited(visited, depth, arr[i])==0) ){
				//printf("current state is <%d%d%d%d> (depth %d)\n", a[0], a[1], a[2], a[3], depth);
				fprintf(stderr, "%d ->", arr[i]);
				dfs_main(arr[i], goal_state, depth+1, visited);
				//printf("back to <%d%d%d%d> (depth %d)\n", a[0], a[1], a[2], a[3], depth);
				//printf("back to ");
				//print_statename(stdout, arr[i]);
				//printf("(depth %d)\n", depth);
				printf("back to ");
				print_statename(stdout, initial_state);
				printf(" (depth %d)\n", depth);
			}
			else if ( is_dead_end(arr[i]) ){
				printf("\tnext state ");
				print_statename(stdout, arr[i]);
				printf(" is dead-end\n");
			}
			else if ( is_visited(visited, depth, arr[i]) ){
				printf("\tnext state ");
				print_statename(stdout, arr[i]);
				printf(" has been visited\n");
			}
			
		}
		
		//printf("back to ");
		//print_statename(stdout, initial_state);
		//printf("(depth %d)\n", depth);
	}
	
}

////////////////////////////////////////////////////////////////////////////////
// 상태들의 인접 행렬을 구하여 graph에 저장
// 상태간 전이 가능성 점검
// 허용되지 않는 상태인지 점검 
void make_adjacency_matrix( int graph[][16]){
	// 양쪽에 떨어져있는거:0, dead:1, visited:2, 가능:3
	for (int row=0; row<16; row++){
		for (int col=0; col<16; col++){
			graph[row][col] = is_possible_transition(row, col);
			//fprintf(stderr, "%d ", is_possible_transition(row, col));
		}
	}
}

// 인접행렬로 표현된 graph를 화면에 출력
void print_graph( int graph[][16], int num){
	for (int row=0; row<num; row++){
		for (int col=0; col<num; col++){
			printf("%d\t", graph[row][col]);
		}
		printf("\n");
	}
}

// 주어진 그래프(graph)를 .net 파일로 저장
// pgwc.net 참조
void save_graph( char *filename, int graph[][16], int num){
	FILE *fp = fopen("pwgc2.net", "w");
	int a=0, b=0, c0=0, d=0;
	int *p, *w, *g, *c;
	p=&a, w=&b, g=&c0, c=&d;
	
	fputs("*Vertices 16\n", fp);
	for (int i=0; i<num; i++){
		get_pwgc(i, p, w, g, c);
		fprintf(fp, "%d %c<%d%d%d%d>%c\n", i+1, '"', *p, *w, *g, *c, '"');
	}
	fputs("*Edges\n", fp);
	for (int row=0; row<(num/2); row++){
		for (int col=0; col<num; col++){
			if (graph[row][col]==1){
				fprintf(fp, "  %d  %d\n", row+1, col+1);
			}
		}
	}

	fclose(fp);
}

////////////////////////////////////////////////////////////////////////////////
// 깊이 우선 탐색 (초기 상태 -> 목적 상태)
void depth_first_search( int initial_state, int goal_state)
{
	int depth = 0;
	int visited[16] = {0,}; // 방문한 정점을 저장
	
	dfs_main( initial_state, goal_state, depth, visited); 
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int graph[16][16] = {0,};

	// 인접 행렬 만들기
	make_adjacency_matrix( graph);

	// 인접 행렬 출력 (only for debugging) 제출시엔 주석처리 하기.
	//print_graph( graph, 16);
	
	// .net 파일 만들기
	save_graph( "pwgc.net", graph, 16);

	// 깊이 우선 탐색
	depth_first_search( 0, 15); // initial state, goal state
	
	return 0;
}

