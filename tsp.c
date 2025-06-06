#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

#define MAX_NODES 50000
#define K_NEAREST 10

typedef struct {
    double x, y;
    int id;
} Node;

typedef struct {
    int node;
    double cost;
} Edge;

int skipped = 0;
double penalty;
int N;
Node nodes[MAX_NODES];
int tour[MAX_NODES];
Edge candidate[MAX_NODES][K_NEAREST];
double cost[MAX_NODES];
int position[MAX_NODES];

double distance(int i, int j) {
    double dx = nodes[i].x - nodes[j].x;
    double dy = nodes[i].y - nodes[j].y;
    return sqrt(dx * dx + dy * dy);
}

void read_input(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) exit(1);

    int id;
    fscanf(fp, "%lf", &penalty);
    printf("girdi1");

    N = 0;
    while (fscanf(fp, "%d %lf %lf", &id, &nodes[N].x, &nodes[N].y) == 3) {
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

                // a ve c tur içinde çok yakýnsa atla:
                int diff = abs(i - c_idx);
                if (diff < 2 || diff > N - 2)
                    continue;

                d = tour[(c_idx + 1) % N];
                double old_cost = distance(a, b) + distance(c, d);
                double new_cost = distance(a, c) + distance(b, d);

                if (new_cost < old_cost - 1e-6) {
                    lo = (i + 1) % N;
                    hi = c_idx;
                    // Ters çevirme iþlemi:
                    while (lo != hi && (lo + N - 1) % N != hi) {
                        // tour[lo] ile tour[hi] takas
                        tmp = tour[lo];
                        tour[lo] = tour[hi];
                        tour[hi] = tmp;

                        // Pozisyon dizisini de güncelle
                        position[tour[lo]] = lo;
                        position[tour[hi]] = hi;

                        lo = (lo + 1) % N;
                        hi = (hi - 1 + N) % N;
                    }
                    improved = 1;
                    break;  // Bir swap yapýldý, yeni turla yeniden baþla
                }
            }
            if (improved) break;
        }
    }
}

double tour_length(double *cost) {
	printf("CEZA %d",skipped);
    printf("girdi4\n");
    int i;
    double tcost = 0, dist = 0;
    cost[0] = 0;

    for (i = 1; i < N; i++) {
        dist = distance(tour[i-1], tour[i]);
        cost[i] = cost[i-1] + dist;
        tcost += dist;
    }
    tcost+=skipped*penalty;
    return tcost;
}

void compute_penalized_cost(double *cumulative_cost, double penalty) {
    double dp[MAX_NODES];
    int prev[MAX_NODES];
    int i, j;

    dp[0] = 0;
    prev[0] = -1;

    for (i = 1; i < N; i++) {
        dp[i] = DBL_MAX;
        prev[i] = -1;

        for (j = 1; j <= 3 && i - j >= 0; j++) {
            double d = distance(tour[i - j], tour[i]);
            double total_cost = dp[i - j] + d + (j - 1) * penalty;

            if (total_cost < dp[i]) {
                dp[i] = total_cost;
                prev[i] = i - j;
            }
        }

    }


    // Geriye doðru en iyi yolu takip et
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

    // Yeni turu ters çevirip ana tour[]'a yaz
    for (i = 0; i < new_len; i++) {
        tour[i] = new_tour[new_len - 1 - i];
        position[tour[i]] = i;
    }

    // Ziyaret edilmeyen node'lar için ceza hesapla
    for (i = 0; i < N; i++) {
        if (!used[i]) skipped++;
    }
    printf("%d",skipped);
    // Yeni turun uzunluðu artýk bu kadar
    N = new_len;
}

int main() {
    FILE *out;
    int i;
    read_input("example-input-3.txt");

    build_candidate_set();
	printf("Initial tour length: %.2f\n", tour_length(cost));
     
	apply_2opt();
    printf("After 2-opt tour length: %.2f\n", tour_length(cost));
    
    compute_penalized_cost(cost,penalty);
 
    for (i = 0; i < N; i++) {
        printf("%d,%d\n", nodes[tour[i]].id,i);
    }
    printf("After penalyty tour length: %.2f\n", tour_length(cost));
    out = fopen("tour_output.txt", "w");
    for (i = 0; i < N; i++) {
        fprintf(out, "%d\n", nodes[tour[i]].id);
    }
    fclose(out);
    printf("Tour written to 'tour_output.txt'\n");

    return 0;
}

