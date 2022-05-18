#include <stdlib.h> // atoi, rand, malloc, realloc
#include <stdio.h>
#include <time.h> //time

#define RANGE 10000

// R 그림 만들기: ./convex.R 만약 안되면 chmod +x convex.R
typedef struct
{
	int x;
	int y;
} t_point;

typedef struct
{
	t_point from;
	t_point to;
} t_line;

////////////////////////////////////////////////////////////////////////////////
void print_header( char *filename)
{
	printf( "#! /usr/bin/env Rscript\n");
	printf( "png(\"%s\", width=700, height=700)\n", filename);
	
	printf( "plot(1:%d, 1:%d, type=\"n\")\n", RANGE, RANGE);
}
////////////////////////////////////////////////////////////////////////////////
void print_footer( void)
{
	printf( "dev.off()\n");
}

////////////////////////////////////////////////////////////////////////////////
/*
#points
points(2247,7459)
points(616,2904)
points(5976,6539)
points(1246,8191)
*/
void print_points( t_point *points, int num_point){
	printf("\n#points\n");
	for (int i=0; i<num_point; i++){
		printf("points(%d,%d)\n", points[i].x, points[i].y);
	}
	printf("\n");
}

/*
#line segments
segments(7107,2909,7107,2909)
segments(43,8,5,38)
segments(43,8,329,2)
segments(5047,8014,5047,8014)
*/
void print_line_segments( t_line *lines, int num_line){
	printf("#line segments\n");
	for (int i=0; i<num_line; i++){
		printf("segments(%d,%d,%d,%d)\n", lines[i].from.x, lines[i].from.y, lines[i].to.x, lines[i].to.y);
	}
}

// [input] points : set of points
// [input] num_point : number of points
// [output] num_line : number of line segments that forms the convex hull
// return value : set of line segments that forms the convex hull
t_line *convex_hull( t_point *points, int num_point, int *num_line){
	int leftCnt; // 한 loop에서 ax+by>c일때 증가
	int rightCnt; // 한 loop에서 ax+by<c일때 증가
	int temp, a, b, c;
	int idx = 0; 
	t_line *result;
	result = (t_line*)malloc(sizeof(t_line)*10);
	for (int i=0; i<num_point; i++){
		for (int j=i+1; j<num_point; j++){
			//printf("%d %d %d %d\n", points[i].x, points[i].y, points[j].x, points[j].y);
			// ax + by = c, where a=y2-y1, b=x1-x2, c=x1y2-y1x2
			a = points[j].y - points[i].y;
			b = points[i].x - points[j].x;
			c = points[i].x*points[j].y - points[i].y*points[j].x;
			leftCnt = 0;
			rightCnt = 0;
			//printf("a:%d b:%d c:%d\n", a,b,c);
			for (int dot=0; dot<num_point; dot++){
				if ((dot==i) || (dot==j)) continue;
				temp = a*points[dot].x + b*points[dot].y;
				if (temp>c){
					leftCnt++;
					//printf("left UP! %d(%d,%d) %d(%d,%d)\n", i, points[i].x, points[i].y, j,points[j].x, points[j].y);
				}
				else if (temp<c){
					rightCnt++;
					//printf("right UP! %d(%d,%d) %d(%d,%d)\n", i, points[i].x, points[i].y, j,points[j].x, points[j].y);
				}
			}
			//printf("\n");
			if ( ((leftCnt == 0)||(rightCnt==0)) ){
				result[idx].from.x = points[i].x;
				result[idx].from.y = points[i].y;
				result[idx].to.x = points[j].x;
				result[idx++].to.y = points[j].y;
				if (idx!=0 && idx%10==0) {
					fprintf(stderr, "realloc! (%d)\n", (((idx/10)+1)*10));
					result = realloc(result, sizeof(t_line)*( ((idx/10)+1)*10));
				}
			}
			
		}
	}
	*num_line = idx;
	return result;
}

////////////////////////////////////////////////////////////////////////////////
int main( int argc, char **argv)
{
	int x, y;
	int num_point; // number of points
	int num_line; // number of lines
	
	if (argc != 2)
	{
		printf( "%s number_of_points\n", argv[0]);
		return 0;
	}

	num_point = atoi( argv[1]);
	if (num_point <= 0)
	{
		printf( "The number of points should be a positive integer!\n");
		return 0;
	}

	t_point *points = (t_point *) malloc( num_point * sizeof( t_point));
		
	t_line *lines;

	// making n points
	srand( time(NULL));
	
	for (int i = 0; i < num_point; i++)
	{
		x = rand() % RANGE + 1; // 1 ~ RANGE random number
		y = rand() % RANGE + 1;
		
		points[i].x = x;
		points[i].y = y;
 	}
	
	/*
	points[0].x = 1000;
	points[0].y = 1000;
	points[1].x = 1000;
	points[1].y = 3000;
	points[2].x = 3000;
	points[2].y = 1000;
	points[3].x = 3000;
	points[3].y = 3000;
	points[4].x = 2000;
	points[4].y = 2000;
	*/

	fprintf( stderr, "%d points created!\n", num_point);

	print_header( "convex.png");
	
	print_points( points, num_point);
	
	lines = convex_hull( points, num_point, &num_line);

	fprintf( stderr, "%d lines created!\n", num_line);

	print_line_segments( lines, num_line);
	
	print_footer();
	
	free( points);
	free( lines);
	
	return 0;
}

