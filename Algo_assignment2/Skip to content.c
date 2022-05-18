#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdbool.h>

struct point
{
	float x;
	float y;
};typedef struct point Point;

struct pointset{
	Point** points; 
	int numPoints;
};typedef struct pointset PointSet;




/**
 * Constructor for a point set
 * */
PointSet* newPointSet( void );



/**
 * Deconstructor for a point 
 * set
 * */
void freePointSet( void * data);


/**Creates a deep copy 
	*of a point. Recommend creating 
	*with void* as argument and return type 
	*In order to use the function pointer in datastructures
	**/
void* copyPoint(void * data);


/**
 * Adds a point to the PointSet struct
 * @pre: points and p are not NULL
 * @post: p is added to points. Number 
 * of points increased by 1
 * */
void addPoint(PointSet* points, Point* p);
/**
 * Creates a subset set points 
 * from the variable points such that 
 * each point in the subset 
 * is on the side of the line 
 * pq equal to the variable 'side' 
 * @pre: points = set of points, p and q are two 
 * points from the set if points, side is assigned 
 * to -1 or 1 where -1 rpresents 'left of pq' 
 * and 1 represents 'right of pq'
 * @post: subset of points is returned 
 * such that each point in the subset is to the left of
 *  or right of pq. If no such points exist, then subset 
 * returned is empty. 
 * 
 * */ 
PointSet* pointSubset(PointSet* points,  Point* p, Point* q, int side);


Point* getFurthestPoint(PointSet* set, Point* p, Point* q, int side);
/**
 * Calculates the distance r is from the line 
 * formed by pq
 * */
float lineDistance(Point p, Point q, Point r);


/**
 * Finds the side of r in relation to the line 
 * formed by pq. r is either leftof or rightof pq
 * 
 * */
int findSide(Point p, Point q, Point r);




/**
 * Adds all points in the set
 * that belong in the Hull. 
 * 
 * */
void findHull(PointSet* hull, PointSet* set, Point* p, Point* q, int side);
/**
 * Prints a set of points
 * */
void printSet(PointSet* set);
/**
 * Reads a file.
 * Returns a set of points
 * */
PointSet* readFile(char* fileName);
/***
 * Compares two points
 * */
int comparePoints(const void* a, const void* b);


int comparePoints(const void* a, const void* b)
{
	Point* p = *(Point**)a;
	Point* q = *(Point**)b;
	return round(p->x - q->x);
}

PointSet* newPointSet( void )
{
	PointSet* p = calloc(1, sizeof(PointSet));
	p->points = NULL;
	p->numPoints = 0;
	return p;
}

void freePointSet( void * data)
{
	if(data == NULL)return;

	PointSet* p = data;
	for(int i = 0; i < p->numPoints; i++)
	{
		free(p->points[i]);
	}
	free(p->points);
	free(p);
}


void* copyPoint(void * data)
{
	if(!data)return NULL;

	Point* og = data;

	Point* cpy = calloc(1, sizeof(Point));
	
	cpy->x = og->x;
	cpy->y = og->y;
	//typecase the cpy
	//as a void*. Basically tricks the compiler 
	//so that you can use this function as a function pointer 
	//in a data structure and still get a copy of a Point*
	return (void*)cpy;
}

PointSet* readFile(char* fileName)
{
	//return if filename is NULL
	if(!fileName)return NULL;
	//open the file, if NULL retunr NULL
	FILE* fp = fopen(fileName, "r");
	if(fp == NULL)return NULL;

	//for reallocing memory

	//create a pointset.
	PointSet* set = newPointSet();

	//create a char* to read in data
	char* pt = calloc(50, sizeof(char));

	//loop through file line 
	//by line with fgets
	while(fgets(pt, 50, fp) != NULL)
	{
		//remove the '\n' b/c fgets reads in new 
		//line character
		pt[strlen(pt)-1] = '\0';

		//create a Point*
		Point *p = calloc(1, sizeof(Point));
		//tokenize the line based on space
		char* token = strtok(pt, " ");
		//set the x
		p->x = atof(token);
		//set the y
		token = strtok(NULL, " ");
		p->y = atof(token);

		//add point p to the set
		addPoint(set, p);
		//free the original, because a deep copy is added to set
		free(p);
	}
	//free memory
	free(pt);
	fclose(fp);

	//return the set of points
	return set;

}



/**
 * @pre: Adds point p to PontSet set
 * */
void addPoint(PointSet* set, Point* p)
{
	//if the first point to be added. Use calloc on set->points
	if(set->numPoints == 0)set->points = calloc(1, sizeof(Point*));
	//else, use realloc on set->points
	else set->points = realloc(set->points, (set->numPoints+1)*sizeof(Point*));

	//Copy point* p and assing the cpy to the set
	Point* cpy = copyPoint(p);
	set->points[set->numPoints] = cpy;
	//increase set->numPoints by 1
	set->numPoints++;
}	

//finds which side of the line formed by pq that r lies on.
int findSide(Point p, Point q, Point r)
{
	float val = (r.y - p.y) * (q.x - p.x) - (q.y - p.y) * (r.x - p.x);

	if(val > 0)return 1;
	else if(val < 0)return -1;
	else return 0;	
}

//returns the distance r is from pq
float lineDistance(Point p, Point q, Point r){

	//calculate the slope of the line formed by 
	//p and q
	float m = (q.y - p.y)/(q.x - p.x);
	//printf("m is %.4f\n", m);

	//calculate the 'b' value 
	//for the formula, y = mx+b 
	//with the line formed by p,q
	float b = p.y - (m*p.x);
	//printf("b is %.4f\n", b);
	//use the value 
	//of r's x coordinate 
	//to find the y value of the line formed by pq 
	//at that coordinate
	float fOfR = m*(r.x) + b;
	//printf("fOfR is %.4f\n", fOfR);

	//to get the distance from 'r' and the line formed 
	//by pq, we know that fOfR was calucltaed using 
	//r's x value. Therefore, take the absolute value 
	//of the distance between r's y value and fOfR

	return fabs(fOfR - r.y);
}





/**
 * Creates a subset of points from the PointSet 
 * called set. The subset contains all the points 
 * from set that are on the left or right side of the line 
 * formed by pq. 
 * If side = 1 then left side
 * If side = -1 then right side
 * 
 * @pre: set contains set of points
 * 			p and q are points
 * 			side is -1 or 1
 * @post: 
 * 		A subset of the points from set is returned such that 
 * 		each point in subset is on the side of pq
 * 
 * */

PointSet* pointSubset(PointSet* set, Point *p, Point *q, int side)
{
	PointSet* subset = newPointSet();



	for(int i = 0; i <set->numPoints; i++)
	{
		Point* r = set->points[i];
		if(findSide(*p, *q, *r)== side){
			addPoint(subset, r);		
		}//end if
		
	}//end for
	return subset;
}


Point* getFurthestPoint(PointSet* set, Point* p, Point* q, int side)
{
	/**
	 * Find the point in set with the furthest distance 
	 * from pq and on the desired side of pq
	 * Add point r to hull if it meets the conditions
	 * */
	float maxDistance = 0.0;
	int idx = -1;

	Point* r = NULL;

	/**
	 * Iterate through set to find the point R such that 
	 * R is furthest from pq and on the side of pq
	 * */

	for(int i = 0; i < set->numPoints; i++)
	{
		r = set->points[i];
		int s = findSide(*p,*q,*r);
		if(s==side && lineDistance(*p, *q, *r)>maxDistance)
		{
			idx = i;
			maxDistance = lineDistance(*p,*q,*r);
		}
	}//end for

	//if found then return
	if(idx != -1)r = set->points[idx];
	
	return r;

}

/*Quickhull algorithm
 	@pre: set: A set of points sorted by x coordinates
			hull: empty set of points to be added to
			N: Number of points in set
	@post:
		hull contains all the points from set such that 
		the points are in the convex hull
*/
void quickHull(PointSet* hull, PointSet* set, int N)
{
	Point* p = set->points[0];
	Point* q = set->points[N-1];
	//assume the set is sorted by x coordinate. Add leftmost and rightmost point to hull
	addPoint(hull, p);
	addPoint(hull, q);

	//create the subset of points to the left of line 
	
	
	findHull(hull, set, p, q, 1);
	//printf("Entering right subset\n");
	findHull(hull, set, p, q, -1);

}

/***
 * Main algoritm for quick hull
 * @pre: hull is initially empty, set contains the original set of points, p and q are extreme points 
 * on the x axis, side is -1 or 1
 * 
 * @post: hull has all points in convex hull
 * 
 * 
 * */

void findHull(PointSet* hull, PointSet* set, Point* p, Point* q, int side)
{
	/**
	 * If no more points in the set  then return
	 * */
	if(set->numPoints == 0)return;

	/**
	 * Find the point in set with the furthest distance 
	 * from pq and on the desired side of pq
	 * Add point r to hull if it meets the conditions
	 * */
	Point* r = getFurthestPoint(set, p, q, side);

	//if no point was found then return
	if(r == NULL)return;

	//r is added to the hull
	
	addPoint(hull, r); 


	
	//find the points in  the left hull
	PointSet * leftSet = pointSubset(set, p, r, side);
	findHull(hull, leftSet, p, r, side);
	freePointSet(leftSet);

	PointSet* rightSet = pointSubset(set, r, q, side);
	//find the points in the right hull
	findHull(hull, rightSet, r, q, side);
	freePointSet(rightSet);

}

/**
 * Prints a set of points by x and y value
 * */
void printSet(PointSet* set)
{
	printf("\nPrinting Set\n");
	for(int i = 0; i < set->numPoints; i++)
	{
		printf("x: %.2f  y: %.2f\n", set->points[i]->x, set->points[i]->y);
	}
	printf("--NUMBER OF POINTS IN SET: %d\n", set->numPoints);
}

int main(int argc, char* argv[])
{

	char* fileName = NULL;
	bool print = false;
	//if the user enters "1" to print
	if(argc == 2)
	{
		fileName = argv[1];
	}
	else if(argc == 3) 
	{
		fileName = argv[2];
		print = true;
	}
	else{
		printf("You need to enter a file\n");
		exit(0);
	}

	

	PointSet* points = readFile(fileName);
	if(points == NULL)
	{
		printf("Failed to open the file\n");
		exit(0);
	}
	
	
	int N = points->numPoints;
	qsort(points->points, points->numPoints, sizeof(Point**), &comparePoints);
	PointSet* hull = newPointSet();

	qsort(points->points, points->numPoints, sizeof(Point*), &comparePoints);
	quickHull(hull, points, N);
	printf("%d\n", hull->numPoints);
	if(print == true)
	{
		printSet(hull);
	}
	freePointSet(points);
	freePointSet(hull);

 	return 0;
}
