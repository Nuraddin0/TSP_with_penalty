#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <time.h>

#define MAX_NODES 50000
int K_NEAREST;

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
Edge **candidate;
int position[MAX_NODES];


int distance(int i, int j) {
    long long dx = nodes[i].x - nodes[j].x;
    long long dy = nodes[i].y - nodes[j].y;
    return (int)(sqrt((double)(dx * dx + dy * dy)) + 0.5);
}

long long lower_triangle_index(long long i, long long j) {
    return i * (i - 1) / 2 + j;
}

void build_lower_triangle_matrix(int* distance_lower_triangle) {
    printf("Building lower triangle matrix...\n");
    for (long long i = 1; i < N; i++) {
        for (long long j = 0; j < i; j++) {
            distance_lower_triangle[lower_triangle_index(i, j)] = distance(i, j);
        }
    }
}

int get_distance(long long i, long long j, int* distance_lower_triangle) {
    if (i == j) return 0;
    return (i > j) ? distance_lower_triangle[lower_triangle_index(i, j)] : distance_lower_triangle[lower_triangle_index(j, i)];
}

void read_input(const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        printf("Unable to open file %s\n", filename);
        exit(1);
    }

    int id;
    fscanf(fp, "%d", &penalty);
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

void build_candidate_set(int* distance_lower_triangle) {
    int i, j, k;
    for (i = 0; i < N; i++) {
        Edge temp[N];
        for (j = 0; j < N; j++) {
            temp[j].node = j;
            temp[j].cost = get_distance((long long)i, (long long)j, distance_lower_triangle);
        }
        qsort(temp, N, sizeof(Edge), cmp_edge);
        for (k = 0; k < K_NEAREST; k++) {
            candidate[i][k] = temp[k+1];
        }
    }
}

void build_candidate_set_for_tour(int* distance_lower_triangle) {
    int ii, jj, k;
    for (ii = 0; ii < N; ii++) {
        int a = tour[ii];
        static Edge temp[MAX_NODES];
        for (jj = 0; jj < N; jj++) {
            int c = tour[jj];
            temp[jj].node = c;
            temp[jj].cost = get_distance(a, c, distance_lower_triangle);
        }
        qsort(temp, N, sizeof(Edge), cmp_edge);
        for (k = 0; k < K_NEAREST; k++) {
            candidate[a][k] = temp[k+1];
        }
    }
}

void apply_2opt(int* distance_lower_triangle) {
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
                c_idx = position[c];

                int diff = abs(i - c_idx);
                if (diff < 2 || diff > N - 2)
                    continue;

                d = tour[(c_idx + 1) % N];
                int old_cost = get_distance(a, b, distance_lower_triangle) + get_distance(c, d, distance_lower_triangle);
                int new_cost = get_distance(a, c, distance_lower_triangle) + get_distance(b, d, distance_lower_triangle);

                if (new_cost < old_cost - 1e-6) {
                    lo = (i + 1) % N;
                    hi = c_idx;
                    while (lo != hi && (lo + N - 1) % N != hi) {
                        tmp = tour[lo];
                        tour[lo] = tour[hi];
                        tour[hi] = tmp;

                        position[tour[lo]] = lo;
                        position[tour[hi]] = hi;

                        lo = (lo + 1) % N;
                        hi = (hi - 1 + N) % N;
                    }
                    improved = 1;
                    break;
                }
            }
            if (improved) break;
        }
    }
}

long long tour_length(int* distance_lower_triangle) {
    int i;
    long long tcost = 0, dist = 0;

    for (i = 1; i < N; i++) {
        dist = (long long)get_distance(tour[i-1], tour[i], distance_lower_triangle);
        tcost += dist;
    }
    tcost += (long long)get_distance(tour[N-1], tour[0], distance_lower_triangle);
    tcost += (long long)(skipped * penalty);
    return tcost;
}

void compute_penalized_cost(int penalty, int* distance_lower_triangle) {
    int dp[MAX_NODES];
    int prev[MAX_NODES];
    int i, j;

    dp[0] = 0;
    prev[0] = -1;

    for (i = 1; i < N; i++) {
        dp[i] = INT_MAX;
        prev[i] = -1;

        for (j = 1; j <= N && i - j >= 0; j++) {
            int d = get_distance(tour[i - j], tour[i], distance_lower_triangle);
            int total_cost = dp[i - j] + d + (j - 1) * penalty;

            if (total_cost < dp[i]) {
                dp[i] = total_cost;
                prev[i] = i - j;
            }
        }
    }

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

    for (i = 0; i < new_len; i++) {
        tour[i] = new_tour[new_len - 1 - i];
        position[tour[i]] = i;
    }

    for (i = 0; i < N; i++) {
        if (!used[i]) skipped++;
    }
    N = new_len;
}

int main() {
    FILE *out;
    int i;
    read_input("test-input-3.txt");
    int input_len = N;

    if (input_len < 350)
        K_NEAREST = 300;
    else if (input_len < 1000)
        K_NEAREST = 600;
    else if (input_len < 7500)
        K_NEAREST = 70;
    else if (input_len <= 50000)
        K_NEAREST = 40;
    else K_NEAREST = 20;

    candidate = malloc(N * sizeof(Edge *));
    for (i = 0; i < N; i++) {
        candidate[i] = malloc(K_NEAREST * sizeof(Edge));
    }

    long long matrix_size = (long long)N * (N - 1) / 2;
    size_t total_bytes = (size_t)matrix_size * sizeof(int);

    int* distance_lower_triangle = malloc(total_bytes);
    if (!distance_lower_triangle) {
        printf("Memory allocation failed!\n");
        return 1;
    }

    build_lower_triangle_matrix(distance_lower_triangle);
    printf("Matrix constructed.\n");

    build_candidate_set(distance_lower_triangle);
    printf("Initial tour length: %lld\n", tour_length(distance_lower_triangle));

    apply_2opt(distance_lower_triangle);
    printf("After 2-opt tour length: %lld\n", tour_length(distance_lower_triangle));

    compute_penalized_cost(penalty, distance_lower_triangle);
    printf("After penalty tour length: %lld\n", tour_length(distance_lower_triangle));

    build_candidate_set_for_tour(distance_lower_triangle);

    for (i = 0; i < N; i++)
        position[tour[i]] = i;

    apply_2opt(distance_lower_triangle);
    long long finalCost = tour_length(distance_lower_triangle);
    printf("After 2opt second tour length: %lld\n", finalCost);

    out = fopen("tour_output.txt", "w");
    fprintf(out, "%lld %d\n", finalCost, input_len - skipped);
    for (i = 0; i < N; i++) {
        fprintf(out, "%d\n", nodes[tour[i]].id);
    }
    fclose(out);
    free(distance_lower_triangle);

    for (i = 0; i < N; i++) {
        free(candidate[i]);
    }
    free(candidate);

    return 0;
}
