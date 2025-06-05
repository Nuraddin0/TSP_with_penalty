#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int roundNearestInt(double x){
	return x<(int)x+0.5 ? (int)x : (int)x + 1;
}

/*
// Orientation fonksiyonu
int orientation(int x1, int y1, int x2, int y2, int x3, int y3) {
    int val = (y2 - y1) * (x3 - x2) - (x2 - x1) * (y3 - y2);
    
    if (val == 0) return 0;           // kolinear
    return (val > 0) ? 1 : 2;         // 1 -> saat yönü, 2 -> saat yönü tersi
}

// Bir nokta diðer kenarýn üzerinde mi?
int onSegment(int x1, int y1, int x2, int y2, int x3, int y3) {
    if (x2 <= ((x1 > x3) ? x1 : x3) && x2 >= ((x1 < x3) ? x1 : x3) &&
        y2 <= ((y1 > y3) ? y1 : y3) && y2 >= ((y1 < y3) ? y1 : y3))
        return 1;
    return 0;
}

// Ýki doðru parçasý kesiþiyor mu?
int isIntersects(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4) {
    int o1 = orientation(x1, y1, x2, y2, x3, y3);
    int o2 = orientation(x1, y1, x2, y2, x4, y4);
    int o3 = orientation(x3, y3, x4, y4, x1, y1);
    int o4 = orientation(x3, y3, x4, y4, x2, y2);

    // Genel durum
    if (o1 != o2 && o3 != o4)
        return 1;

    // Özel durumlar (kolinear ve üst üste binme)
    if (o1 == 0 && onSegment(x1, y1, x3, y3, x2, y2)) return 1;
    if (o2 == 0 && onSegment(x1, y1, x4, y4, x2, y2)) return 1;
    if (o3 == 0 && onSegment(x3, y3, x1, y1, x4, y4)) return 1;
    if (o4 == 0 && onSegment(x3, y3, x2, y2, x4, y4)) return 1;

    return 0; // Kesilmiyor
} */

void printPath(int path[],int length, int dist[][76]){
	int *arr = (int*)(calloc(length-1,sizeof(int)));
	int i;
	for(i = 1; i < length; i++){
		arr[path[i]]++;
		if(arr[path[i]] != 1){
			printf("\nWRONG PATH!!!\n");
			return;
		}
	}
	if(path[0] != path[length -1]){
		printf("\nWRONG PATH!!!\n");
		return;
	}
	
	int cost = 0;
	for(i = 0; i < length;i++){
		if(i > 0){
			cost += dist[path[i-1]][path[i]];	
			if(i%20 == 0){
				printf("%d\n",path[i]);
				continue;
			} 
		} 
		printf("%d -> ",path[i]);
		
	}
	printf("\nTotal cost is %d\n",cost);
	
}

int main(){
	int size = 76;
	int path[77];
	FILE *input1 = fopen("input1.txt","r");
	int cities[76][2] = {0};
	int penalty;
	fscanf(input1,"%d\n",&penalty);
	int i;
	for(i = 0; i < size; i++){
		int trash;
		fscanf(input1,"%d %d %d\n",&trash,&cities[i][0], &cities[i][1]);
	}
	for(i = 0; i < size; i++){
		printf("%d %d\n",cities[i][0], cities[i][1]);
	}
	int dist[76][76];
	int j;
	double distance;
	for(i = 0; i < size; i++){
		for(j = 0; j < size; j++){
			distance = sqrt((cities[i][0]-cities[j][0])*(cities[i][0]-cities[j][0])+(cities[i][1]-cities[j][1])*(cities[i][1]-cities[j][1]));
			dist[i][j] = roundNearestInt(distance);
		}
	}
	
	printf("\n\n\n");
	
	//Basic Greedy
	int totalcost = 0;
	int startIndex = 73;		//Random
	path[0] = startIndex;
	int index = startIndex;
	printf("%d\n",startIndex);
	int minIndex;
	int used[76] = {0};
	used[index] = 1;
	for(i = 0; i < size-1; i++){
		int min = 2147483647;
		for(j = 0; j < size; j++){
			if(used[j] == 0){
				if(dist[index][j] < min){
					minIndex = j;
					min = dist[index][j];
				}
			}
		}
		totalcost += min;
		used[minIndex] = 1;
		printf("%d\n",minIndex);
		path[i+1] = minIndex;
		index = minIndex;
	}
	path[i+1] = startIndex;
	totalcost+=dist[index][startIndex];
	printf("%d\n",startIndex);
	printf("Total cost is %d\n",totalcost);
	printPath(path,77,dist);
	
	for(i = 0; i < 77; i++){
		printf("%d\n", path[i]);
	}
	printf("/////////////////////////////////\n");
	/////////////////2-opt
	int isSwapped;
	do {
    	isSwapped = 0;
    	for(i = 0; i < 75; i++){
    		for(j = i+2; j < 76; j++){
    			int before = dist[path[i]][path[i+1]] + dist[path[j]][path[j+1]];
    			int after = dist[path[i]][path[j]] + dist[path[i+1]][path[j+1]];
    			if(after < before){
    				int temp = path[i+1];
					path[i+1] = path[j];
					path[j] = temp;
					isSwapped = 1; 
				}
			}
		}
	} while(isSwapped);

	printPath(path,77,dist);
	
	//////////////////////////3-opt
	
		
	return 0;
}
