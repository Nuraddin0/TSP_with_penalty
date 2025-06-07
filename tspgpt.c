#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

#define MAX_NODES 50000
#define K_NEAREST 10

#ifdef _WIN32
#include <windows.h>
#include <psapi.h>
#pragma comment(lib, "psapi.lib")
#endif

typedef struct { int x, y, id; } Node;
typedef struct { int node, cost; } Edge;

int skipped = 0, penalty, N;
Node nodes[MAX_NODES];
int tour[MAX_NODES], position[MAX_NODES];
Edge candidate[MAX_NODES][K_NEAREST];
int cost_accum[MAX_NODES];

int distance_nodes(int i, int j) {
    int dx = nodes[i].x - nodes[j].x;
    int dy = nodes[i].y - nodes[j].y;
    return (int)(sqrt(dx*dx + dy*dy) + 0.5);
}

void read_input(const char *fn) {
    FILE *fp = fopen(fn, "r"); if (!fp) exit(1);
    fscanf(fp, "%d", &penalty);
    for (N=0; fscanf(fp, "%d %d %d", &nodes[N].id, &nodes[N].x, &nodes[N].y)==3; N++) {
        tour[N]=position[N]=N;
    }
    fclose(fp);
}

int cmp_edge(const void *a, const void *b) {
    return ((Edge*)a)->cost - ((Edge*)b)->cost;
}

void build_candidate_set() {
    for (int i=0;i<N;i++){
        Edge *tmp=malloc(sizeof(Edge)*N);
        for(int j=0;j<N;j++){ tmp[j].node=j; tmp[j].cost=distance_nodes(i,j);}        
        qsort(tmp,N,sizeof(Edge),cmp_edge);
        for(int k=0;k<K_NEAREST;k++) candidate[i][k]=tmp[k+1];
        free(tmp);
    }
}

void reverse_segment(int lo, int hi) {
    while(lo!=hi && (lo+N-1)%N!=hi) {
        int t=tour[lo]; tour[lo]=tour[hi]; tour[hi]=t;
        position[tour[lo]]=lo; position[tour[hi]]=hi;
        lo=(lo+1)%N; hi=(hi-1+N)%N;
    }
}

void apply_2opt() {
    int improved=1;
    while(improved){ improved=0;
        for(int i=0;i<N-1 && !improved;i++){
            int a=tour[i], b=tour[(i+1)%N];
            for(int k=0;k<K_NEAREST;k++){
                int c=candidate[a][k].node, ci=position[c];
                if(abs(i-ci)<2||abs(i-ci)>N-2) continue;
                int d=tour[(ci+1)%N];
                if(distance_nodes(a,c)+distance_nodes(b,d) < distance_nodes(a,b)+distance_nodes(c,d)){
                    reverse_segment((i+1)%N, ci);
                    improved=1; break;
                }
            }
        }
    }
}

void apply_3opt() {
    int improved=1;
    while(improved){ improved=0;
        for(int i=0;i<N && !improved;i++){
            int A=tour[i], B=tour[(i+1)%N];
            for(int k1=0;k1<K_NEAREST && !improved;k1++){
                int Cidx=candidate[A][k1].node, j=position[Cidx];
                if(j==i||(j==(i+1)%N)) continue;
                int C=tour[j], D=tour[(j+1)%N];
                for(int k2=0;k2<K_NEAREST && !improved;k2++){
                    int Eidx=candidate[B][k2].node, l=position[Eidx];
                    if(l==i||l==j||(l==(j+1)%N)) continue;
                    int E=tour[l], F=tour[(l+1)%N];
                    int oldc=distance_nodes(A,B)+distance_nodes(C,D)+distance_nodes(E,F);
                    int newc;
                    // Case 1: A->C, B->E, D->F
                    newc=distance_nodes(A,C)+distance_nodes(B,E)+distance_nodes(D,F);
                    if(newc<oldc){ reverse_segment((i+1)%N,j); reverse_segment((j+1)%N,l); improved=1; break; }
                    // Case 2: A->C, B->D, E->F
                    newc=distance_nodes(A,C)+distance_nodes(B,D)+distance_nodes(E,F);
                    if(newc<oldc){ reverse_segment((i+1)%N,j); improved=1; break; }
                    // Case 3: A->B, C->E, D->F
                    newc=distance_nodes(A,B)+distance_nodes(C,E)+distance_nodes(D,F);
                    if(newc<oldc){ reverse_segment((j+1)%N,l); improved=1; break; }
                    // Case 4: A->D, E->B, C->F
                    newc=distance_nodes(A,D)+distance_nodes(E,B)+distance_nodes(C,F);
                    if(newc<oldc){
                        reverse_segment((i+1)%N,j);
                        reverse_segment((i+1)%N,l);
                        improved=1; break; }
                    // Case 5: A->E, D->B, C->F
                    newc=distance_nodes(A,E)+distance_nodes(D,B)+distance_nodes(C,F);
                    if(newc<oldc){
                        reverse_segment((j+1)%N,l);
                        reverse_segment((i+1)%N,l);
                        improved=1; break; }
                    // Case 6: A->D, B->F, C->E
                    newc=distance_nodes(A,D)+distance_nodes(B,F)+distance_nodes(C,E);
                    if(newc<oldc){
                        reverse_segment((j+1)%N,l);
                        reverse_segment((i+1)%N,j);
                        improved=1; break; }
                    // Case 7: A->E, D->F, C->B
                    newc=distance_nodes(A,E)+distance_nodes(D,F)+distance_nodes(C,B);
                    if(newc<oldc){
                        reverse_segment((i+1)%N,j);
                        reverse_segment((j+1)%N,l);
                        reverse_segment((i+1)%N,l);
                        improved=1; break; }
                }
            }
        }
    }
}

int tour_length(int *costs) {
    int t=0; costs[0]=0;
    for(int i=1;i<N;i++){ t+=distance_nodes(tour[i-1],tour[i]); costs[i]=t; }
    return t + skipped*penalty;
}

void compute_penalized_cost() {
    int dp[MAX_NODES], prev[MAX_NODES], newtour[MAX_NODES], used[MAX_NODES]={0};
    dp[0]=0; prev[0]=-1;
    for(int i=1;i<N;i++){
        dp[i]=INT_MAX;
        for(int j=1;j<=3&&i-j>=0;j++){
            int d=distance_nodes(tour[i-j],tour[i]);
            int c=dp[i-j]+d+(j-1)*penalty;
            if(c<dp[i]){dp[i]=c; prev[i]=i-j;}
        }
    }
    int idx=N-1,len=0;
    while(idx>=0){ newtour[len++]=tour[idx]; used[tour[idx]]=1; idx=prev[idx]; }
    for(int i=0;i<len;i++){ tour[i]=newtour[len-1-i]; position[tour[i]]=i; }
    skipped=0; for(int i=0;i<N;i++) if(!used[i]) skipped++;
    N=len;
}

int main() {
    read_input("example-input-3.txt");
    build_candidate_set();
    printf("Init: %d\n", tour_length(cost_accum));
    apply_2opt(); printf("2-opt: %d\n", tour_length(cost_accum));
    apply_3opt(); printf("3-opt: %d\n", tour_length(cost_accum));
    compute_penalized_cost(); printf("Penalty: %d\n", tour_length(cost_accum));
    FILE *out=fopen("tour_output.txt","w");
    for(int i=0;i<N;i++) fprintf(out,"%d\n",nodes[tour[i]].id);
    fclose(out); print_memory_usage();
    return 0;
}

