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

typedef struct {
    int x, y;
    int id;
} Node;

typedef struct {
    int node;
    int cost;
} Edge;

int skipped = 0;
int penalty;
int N;
Node nodes[MAX_NODES];
int tour[MAX_NODES];
Edge candidate[MAX_NODES][K_NEAREST];
int cost[MAX_NODES];
int position[MAX_NODES];


void print_memory_usage() {
#ifdef _WIN32
    PROCESS_MEMORY_COUNTERS_EX memInfo;
    if (GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&memInfo, sizeof(memInfo))) {
        SIZE_T rss = memInfo.WorkingSetSize;
        printf("Memory usage (RSS): %.2f MB\n", rss / (1024.0 * 1024.0));
    } else {
        printf("Unable to get memory info.\n");
    }
#else
    printf("Memory usage logging not supported on this platform.\n");
#endif
}

int distance(int i, int j) {
    int dx = nodes[i].x - nodes[j].x;
    int dy = nodes[i].y - nodes[j].y;
    return (int)(sqrt(dx * dx + dy * dy) + 0.5);
}

void read_input(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) exit(1);

    int id;
    fscanf(fp, "%d", &penalty);
    printf("girdi1");

    N = 0;
    while (fscanf(fp, "%d %d %d", &id, &nodes[N].x, &nodes[N].y) == 3) {
        nodes[N].id = id;
        tour[N] = N;
        position[N] = N;
        N++;
    }
    fclose(fp);
}

int cmp_edge(const void *p1, const void *p2) {
    const Edge *e1 = (const Edge*)p1;
    const Edge *e2 = (const Edge*)p2;
    if (e1->cost < e2->cost) return -1;
    if (e1->cost > e2->cost) return  1;
    return 0;
}

void build_candidate_set() {
    printf("girdi2");
    int i, j, k;
    for (i = 0; i < N; i++) {
        Edge temp[N];
        for (j = 0; j < N; j++) {
            temp[j].node = j;
            temp[j].cost = distance(i, j);
        }
        qsort(temp, N, sizeof(Edge), cmp_edge);

        for (k = 0; k < K_NEAREST; k++) {
            candidate[i][k] = temp[k+1];
        }
    }
}

void apply_2opt() {
    printf("girdi3");
    int improved = 1;
    int i, k, a, b, c, c_idx, d;
    int lo, hi, tmp;

    while (improved) {
        improved = 0;
        for (i = 0; i < N - 1; i++) {
            a = tour[i];
            b = tour[(i+1) % N];

            for (k = 0; k < K_NEAREST; k++) {
                c = candidate[a][k].node;
                c_idx = position[c];  // ARTIK O(1), lineer tarama yok

                // a ve c tur iï¿½inde ï¿½ok yakï¿½nsa atla:
                int diff = abs(i - c_idx);
                if (diff < 2 || diff > N - 2)
                    continue;

                d = tour[(c_idx + 1) % N];
                int old_cost = distance(a, b) + distance(c, d);
                int new_cost = distance(a, c) + distance(b, d);

                if (new_cost < old_cost - 1e-6) {
                    lo = (i + 1) % N;
                    hi = c_idx;
                    // Ters ï¿½evirme iï¿½lemi:
                    while (lo != hi && (lo + N - 1) % N != hi) {
                        // tour[lo] ile tour[hi] takas
                        tmp = tour[lo];
                        tour[lo] = tour[hi];
                        tour[hi] = tmp;

                        // Pozisyon dizisini de gï¿½ncelle
                        position[tour[lo]] = lo;
                        position[tour[hi]] = hi;

                        lo = (lo + 1) % N;
                        hi = (hi - 1 + N) % N;
                    }
                    improved = 1;
                    break;  // Bir swap yapï¿½ldï¿½, yeni turla yeniden baï¿½la
                }
            }
            if (improved) break;
        }
    }
}

// 3-opt implementation: consider triple swaps using candidate sets
void apply_3opt() {
    int improved = 1;
    int A, B, C, D, E, F;
    while (improved) {
        improved = 0;
        for (int i = 0; i < N && !improved; i++) {
            A = tour[i];
            B = tour[(i+1)%N];
            for (int k1 = 0; k1 < K_NEAREST && !improved; k1++) {
                int Cidx = candidate[A][k1].node;
                int j = position[Cidx];
                if (abs(i - j) < 2 || abs(i - j) > N-2) continue;
                C = tour[j];
                D = tour[(j+1)%N];
                // second edge from B side
                for (int k2 = 0; k2 < K_NEAREST && !improved; k2++) {
                    int Eidx = candidate[B][k2].node;
                    int l = position[Eidx];
                    if (l == i || l == j) continue;
                    if (abs((j+1)%N - l) < 2) continue;
                    E = tour[l];
                    F = tour[(l+1)%N];
                    // evaluate 7 possible reconnections
                    int old2 = distance_nodes(A,B) + distance_nodes(C,D) + distance_nodes(E,F);
                    // case 1: reconnect A->C, B->E, D->F
                    int new2 = distance_nodes(A,C) + distance_nodes(B,E) + distance_nodes(D,F);
                    if (new2 + 0 < old2) {
                        // perform segments reversal accordingly
                        // reverse segment B->D
                        int start = (i+1)%N, end = j;
                        while (start != end && (start+N-1)%N != end) {
                            int tmp = tour[start]; tour[start] = tour[end]; tour[end] = tmp;
                            position[tour[start]] = start; position[tour[end]] = end;
                            start = (start+1)%N; end = (end-1+N)%N;
                        }
                        // reverse segment D->F
                        start = (j+1)%N; end = l;
                        while (start != end && (start+N-1)%N != end) {
                            int tmp = tour[start]; tour[start] = tour[end]; tour[end] = tmp;
                            position[tour[start]] = start; position[tour[end]] = end;
                            start = (start+1)%N; end = (end-1+N)%N;
                        }
                        improved = 1;
                    }
                    // other cases omitted for brevity – implement similar checks
                }
            }
        }
    }
}

int tour_length(int *cost) {
	printf("CEZA %d",skipped);
    printf("girdi4\n");
    int i;
    int tcost = 0, dist = 0;
    cost[0] = 0;

    for (i = 1; i < N; i++) {
        dist = distance(tour[i-1], tour[i]);
        cost[i] = cost[i-1] + dist;
        tcost += dist;
    }
    tcost += skipped*penalty;
    return tcost;
}

void compute_penalized_cost(int *cumulative_cost, int penalty) {
    int dp[MAX_NODES];
    int prev[MAX_NODES];
    int i, j;

    dp[0] = 0;
    prev[0] = -1;

    for (i = 1; i < N; i++) {
        dp[i] = INT_MAX;
        prev[i] = -1;

        for (j = 1; j <= 3 && i - j >= 0; j++) {
            int d = distance(tour[i - j], tour[i]);
            int total_cost = dp[i - j] + d + (j - 1) * penalty;

            if (total_cost < dp[i]) {
                dp[i] = total_cost;
                prev[i] = i - j;
            }
        }

    }


    // Geriye doï¿½ru en iyi yolu takip et
    int new_tour[MAX_NODES];
    int used[MAX_NODES];
    for (i = 0; i < N; i++) used[i] = 0;

    i = N - 1;
    int new_len = 0;

    while (i >= 0) {
        new_tour[new_len] = tour[i];
        used[tour[i]] = 1;
        new_len++;
        i = prev[i];
    }

    // Yeni turu ters ï¿½evirip ana tour[]'a yaz
    for (i = 0; i < new_len; i++) {
        tour[i] = new_tour[new_len - 1 - i];
        position[tour[i]] = i;
    }

    // Ziyaret edilmeyen node'lar iï¿½in ceza hesapla
    for (i = 0; i < N; i++) {
        if (!used[i]) skipped++;
    }
    printf("%d",skipped);
    // Yeni turun uzunluï¿½u artï¿½k bu kadar
    N = new_len;
}

int main() {
    FILE *out;
    int i;
    read_input("example-input-3.txt");

    build_candidate_set();
	printf("Initial tour length: %d\n", tour_length(cost));

	apply_2opt();
    printf("After 2-opt tour length: %d\n", tour_length(cost));

    compute_penalized_cost(cost,penalty);

    for (i = 0; i < N; i++) {
        printf("%d,%d\n", nodes[tour[i]].id,i);
    }
    printf("After penalyty tour length: %d\n", tour_length(cost));
    out = fopen("tour_output.txt", "w");
    for (i = 0; i < N; i++) {
        fprintf(out, "%d\n", nodes[tour[i]].id);
    }
    fclose(out);
    printf("Tour written to 'tour_output.txt'\n");
    print_memory_usage();
    return 0;
}
