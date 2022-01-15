// licensed under https://tamberg.mit-license.org/
// for https://twitter.com/HikingHack/status/1481353359294701570

// $ gcc -o sort sort.c
// $ ./sort n_entries m_entries max_value
// $ ./sort 3 3 2 # to see how it works
// $ time ./sort 1000000 1000000 150

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

struct row {
    int sheet; // 0 or 1
    int id; // row no
    int a;
    int b;
};

struct table {
    int n_rows;
    struct row **rows;
};

void init(struct table *t, int n, int m, int max_value) {
    t->n_rows = n + m;
    t->rows = malloc(t->n_rows * sizeof(struct row *));
    for (int i = 0; i < t->n_rows; i++) {
        t->rows[i] = malloc(sizeof(struct row));
        t->rows[i]->sheet = i < n ? 0 : 1;
        t->rows[i]->id = i < n ? i : i - n;
        t->rows[i]->a = rand() % max_value;
        t->rows[i]->b = rand() % max_value;
    }
}

void print_row(struct row *r) {
    printf("%d.%d: %d, %d\n", r->sheet, r->id, r->a, r->b);
}

void print_table(struct table *t) {
    for (int i = 0; i < t->n_rows; i++) {
        print_row(t->rows[i]);
    }
    printf("\n");
}

int compare_rows(const struct row *rx, const struct row *ry) {
    int a0 = rx->a;
    int b0 = rx->b;
    int a1 = ry->a;
    int b1 = ry->b;
    int result;
    if (a0 < a1) {
        result = -1;
    } else if (a0 > a1) {
        result = 1;
    } else {
        assert(a0 == a1);
        if (b0 < b1) {
            result = -1;
        } else if (b0 > b1) {
            result = 1;
        } else {
            assert(b0 == b1);
            result = 0;
        }
    }
    return result;
}

int compare(const void *x, const void *y) {
    const struct row *rx = *((struct row **) x);
    const struct row *ry = *((struct row **) y);
    return compare_rows(rx, ry);
}

void print_filtered(struct table *t) {
    int i = 0;
    int j = i + 1;
    while (i < t->n_rows && j < t->n_rows) {
        int first = 1;
        while (j < t->n_rows && compare_rows(t->rows[i], t->rows[j]) == 0) {
            if (first) {
                printf("\n");
                print_row(t->rows[i]);
                first = 0;
            }
            print_row(t->rows[j]);
            j++;
        }
        i = j;
        j = i + 1;
    }
}

int main(int argc, char *argv[]) {
    int n;
    int m;
    int max_value;
    if (argc == 4) {
        n = atoi(argv[1]);
        m = atoi(argv[2]);
        max_value = atoi(argv[3]);
    } else {
        printf("usage: %s sheet0_n_rows sheet1_n_rows max_value\n", argv[0]);
        exit(-1);
    }
    struct table *t0 = malloc(sizeof(struct table));
    init(t0, n, m, max_value); // TODO: parse csv files instead
    //printf("initialised:\n\n");
    //print_table(t0);
    qsort(t0->rows, t0->n_rows, sizeof(struct row *), compare);
    //printf("sorted:\n\n");
    //print_table(t0);
    printf("filtered:\n");
    print_filtered(t0);
}
