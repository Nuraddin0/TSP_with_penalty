// Traveling Salesman Problem with Penalty in C
// Pipeline: Nearest Neighbour -> 2-opt -> 3-opt -> Prune -> Reinsertion -> Iterated Local Search

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAX_CITIES 50000
#define CANDIDATE_K 10

typedef struct {
    int id;
    int x, y;
    int visited;
} City;

int penalty;
int city_count = 0;
City cities[MAX_CITIES];
int candidate_set[MAX_CITIES][CANDIDATE_K];
int used[MAX_CITIES];

int distance(City a, City b) {
    int dx = a.x - b.x;
    int dy = a.y - b.y;
    return (int)(sqrt(dx * dx + dy * dy) + 0.5);
}

void read_input(const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) { perror("Error opening file"); exit(1); }
    fscanf(fp, "%d", &penalty);
    while (fscanf(fp, "%d %d %d", &cities[city_count].id, &cities[city_count].x, &cities[city_count].y) == 3) {
        cities[city_count].visited = 0;
        used[city_count] = 0;
        city_count++;
        if (city_count >= MAX_CITIES) { printf("Too many cities\n"); exit(1); }
    }
    fclose(fp);
}

void build_candidate_set() {
    for (int i = 0; i < city_count; i++) {
        double dists[MAX_CITIES]; int ids[MAX_CITIES];
        for (int j = 0; j < city_count; j++) {
            dists[j] = (i == j) ? 1e9 : distance(cities[i], cities[j]);
            ids[j] = j;
        }
        for (int k = 0; k < CANDIDATE_K; k++) {
            int min_idx = k;
            for (int j = k + 1; j < city_count; j++) {
                if (dists[j] < dists[min_idx]) min_idx = j;
            }
            double temp_d = dists[k]; dists[k] = dists[min_idx]; dists[min_idx] = temp_d;
            int temp_id = ids[k]; ids[k] = ids[min_idx]; ids[min_idx] = temp_id;
            candidate_set[i][k] = ids[k];
        }
    }
}

void nearest_neighbour(int* tour) {
    for (int i = 0; i < city_count; i++) used[i] = 0;
    int current = 0;
    cities[current].visited = 1;
    tour[0] = current;
    used[current] = 1;
    for (int i = 1; i < city_count; i++) {
        int next = -1, min_dist = 1e9;
        for (int j = 0; j < city_count; j++) {
            if (!cities[j].visited) {
                int d = distance(cities[current], cities[j]);
                if (d < min_dist) { min_dist = d; next = j; }
            }
        }
        cities[next].visited = 1;
        tour[i] = next;
        used[next] = 1;
        current = next;
    }
}

int compute_tour_cost(int* tour, int n) {
    int total = 0;
    for (int i = 0; i < n - 1; i++) total += distance(cities[tour[i]], cities[tour[i+1]]);
    total += distance(cities[tour[n-1]], cities[tour[0]]);
    return total;
}

void reverse_segment(int* tour, int i, int k) {
    while (i < k) {
        int tmp = tour[i];
        tour[i] = tour[k];
        tour[k] = tmp;
        i++; k--;
    }
}

void two_opt_with_candidates(int* tour, int n) {
    int improvement = 1;
    while (improvement) {
        improvement = 0;
        for (int i = 1; i < n - 1; i++) {
            int a = tour[i - 1], b = tour[i];
            for (int k_idx = 0; k_idx < CANDIDATE_K; k_idx++) {
                int c = candidate_set[b][k_idx], c_index = -1;
                for (int x = 0; x < n; x++) if (tour[x] == c) { c_index = x; break; }
                if (c_index <= i + 1) continue;
                int d = tour[(c_index + 1) % n];
                int currentDist = distance(cities[a], cities[b]) + distance(cities[c], cities[d]);
                int newDist = distance(cities[a], cities[c]) + distance(cities[b], cities[d]);
                if (newDist < currentDist) { reverse_segment(tour, i, c_index); improvement = 1; goto restart; }
            }
        }
    restart: ;
    }
}

void three_opt_with_candidates(int* tour, int n) {
    int improvement = 1;
    while (improvement) {
        improvement = 0;
        for (int i = 0; i < n - 5; i++) {
            int A = tour[i], B = tour[i + 1];
            for (int k1 = 0; k1 < CANDIDATE_K; k1++) {
                int C = candidate_set[B][k1], j = -1;
                for (int x = 0; x < n; x++) if (tour[x] == C) { j = x; break; }
                if (j <= i + 2 || j >= n - 2) continue;
                int D = tour[j + 1];
                for (int k2 = 0; k2 < CANDIDATE_K; k2++) {
                    int E = candidate_set[D][k2], k = -1;
                    for (int x = 0; x < n; x++) if (tour[x] == E) { k = x; break; }
                    if (k <= j + 2 || k >= n - 1) continue;
                    int F = tour[(k + 1) % n];
                    int d0 = distance(cities[A], cities[B]) + distance(cities[C], cities[D]) + distance(cities[E], cities[F]);
                    int d1 = distance(cities[A], cities[C]) + distance(cities[B], cities[E]) + distance(cities[D], cities[F]);
                    if (d1 < d0) { reverse_segment(tour, i + 1, j); reverse_segment(tour, j + 1, k); improvement = 1; goto restart; }
                }
            }
        }
    restart: ;
    }
}

int prune_tour(int* tour, int* tour_size) {
    int removed = 0;
    for (int i = 1; i < *tour_size - 1; i++) {
        int prev = tour[i - 1], curr = tour[i], next = tour[i + 1];
        int old_cost = distance(cities[prev], cities[curr]) + distance(cities[curr], cities[next]);
        int new_cost = distance(cities[prev], cities[next]);
        if ((old_cost - new_cost) > penalty) {
            used[curr] = 0;
            for (int j = i; j < *tour_size - 1; j++) tour[j] = tour[j + 1];
            (*tour_size)--; removed++; i--;
        }
    }
    return removed;
}

int insert_skipped_cities(int* tour, int* tour_size) {
    int inserted = 0;
    for (int c = 0; c < city_count; c++) {
        if (used[c]) continue;
        int best_pos = -1, best_gain = penalty;
        for (int i = 0; i < *tour_size; i++) {
            int a = tour[i], b = tour[(i + 1) % *tour_size];
            int old_cost = distance(cities[a], cities[b]);
            int new_cost = distance(cities[a], cities[c]) + distance(cities[c], cities[b]);
            int gain = old_cost - new_cost;
            if (gain > best_gain) { best_gain = gain; best_pos = i + 1; }
        }
        if (best_pos != -1) {
            for (int j = *tour_size; j > best_pos; j--) tour[j] = tour[j - 1];
            tour[best_pos] = c; (*tour_size)++; used[c] = 1; inserted++;
        }
    }
    return inserted;
}

void copy_tour(int* dest, int* src, int n) {
    for (int i = 0; i < n; i++) dest[i] = src[i];
}

void perturb_tour(int* tour, int n) {
    int a = rand() % (n - 5);
    int b = a + 2 + rand() % 3;
    if (b >= n) b = n - 1;
    reverse_segment(tour, a, b);
}

void iterated_local_search(int* tour, int* tour_size, int iterations) {
    int* best_tour = malloc(sizeof(int) * city_count);
    copy_tour(best_tour, tour, *tour_size);
    int best_cost = compute_tour_cost(tour, *tour_size) + penalty * (city_count - *tour_size);

    for (int iter = 0; iter < iterations; iter++) {
        perturb_tour(tour, *tour_size);
        two_opt_with_candidates(tour, *tour_size);
        three_opt_with_candidates(tour, *tour_size);
        int cost = compute_tour_cost(tour, *tour_size) + penalty * (city_count - *tour_size);
        if (cost < best_cost) {
            best_cost = cost;
            copy_tour(best_tour, tour, *tour_size);
            printf("[ILS] Improved to cost: %d at iteration %d\n", best_cost, iter + 1);
        } else {
            copy_tour(tour, best_tour, *tour_size);
        }
    }
    copy_tour(tour, best_tour, *tour_size);
    free(best_tour);
}

int main() {
    srand(time(NULL));
    read_input("example-input-1.txt");

    int* tour = malloc(sizeof(int) * city_count);
    nearest_neighbour(tour);
    build_candidate_set();
    two_opt_with_candidates(tour, city_count);

    int tour_size = city_count;
    while (prune_tour(tour, &tour_size) > 0);

    int inserted = insert_skipped_cities(tour, &tour_size);
    if (inserted > 0) two_opt_with_candidates(tour, tour_size);

    three_opt_with_candidates(tour, tour_size);
    two_opt_with_candidates(tour, tour_size);
    two_opt_with_candidates(tour, tour_size);
    iterated_local_search(tour, &tour_size, 20);

    int final_cost = compute_tour_cost(tour, tour_size) + (penalty * (city_count - tour_size));
    printf("Final tour cost: %d\n", final_cost);
    printf("Final tour length: %d cities (removed: %d)\n", tour_size, city_count - tour_size);

    FILE* out = fopen("final_tour.txt", "w");
    if (out) {
        for (int i = 0; i < tour_size; i++) {
            fprintf(out, "%d\n", cities[tour[i]].id);
        }
        fclose(out);
        printf("Tour saved to final_tour.txt\n");
    } else {
        perror("Error writing tour to file");
    }

    free(tour);
    return 0;
}
