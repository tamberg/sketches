// licensed under https://tamberg.mit-license.org/
// for https://twitter.com/HikingHack/status/1481353359294701570
// assumes csv files with 1 header line, 2+ values per line, line ending "\r\n"

// $ gcc -o match match.c
// $ ./match file0.csv file1.csv

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

// parse

// csv-file = line { '\r' '\n' line } 
// line = value { ',' value } 
// value = digit { digit }
// digit = 0..9

struct value {
    int num;
    struct value *next;
};

struct line {
    struct value *values; // linked list
    struct line *next;
};

struct csv {
    struct line *lines; // linked list
};

void add_value(struct line *l, struct value *v) {
    struct value *p = l->values;
    if (p == NULL) {
        l->values = v;
    } else {
        struct value *q = NULL;
        while (p != NULL) {
            q = p;
            p = p->next;
        }
        q->next = v;
    }
}

void add_line(struct csv *c, struct line *l) {
    struct line *p = c->lines;
    if (p == NULL) {
        c->lines = l;
    } else {
        struct line *q = NULL;
        while (p != NULL) {
            q = p;
            p = p->next;
        }
        q->next = l;
    }
}

void print_csv_lines(struct csv *c) {
    struct line *l = c->lines;
    while (l != NULL) {
        struct value *v = l->values;        
        while (v != NULL) {
            printf("%d\t", v->num);
            v = v->next;
        }
        printf("\n");
        l = l->next;
    }
}

int count_csv_lines(struct csv *c) {
    int n = 0;
    struct line *l = c->lines;
    while (l != NULL) {
        n++;
        l = l->next;
    }
    return n;
}

void free_csv_line(struct line *l) {
    struct value *v = l->values;
    while (v != NULL) {
        struct value *x = v;
        v = v->next;
        free(x);
    }
    free(l);
}

void free_csv(struct csv *c) {
    struct line *l = c->lines;
    while (l != NULL) {
        struct line *x = l;
        l = l->next;
        free_csv_line(x);
    }
    free(c);
}

int read_char(int fd) {
    int result;
    char buf[1];
    int r = read(fd, buf, 1);
    if (r == 1) {
        result = buf[0];
        //printf("read ch = %d (%c)\n", result, (char) result);
    } else if (r == 0) {
        result = 0;
    } else {
        perror("read");
        exit(-1);
    }
    return result;
}

int read_value(int fd, struct line *l) {
    int ch = read_char(fd);
    if ('0' <= ch && ch <= '9') {
        struct value *v = malloc(sizeof(struct value));
        int num = 0;
        while ('0' <= ch && ch <= '9') {
            int d = ch - '0'; // ASCII
            num = (num * 10) + d;
            ch = read_char(fd);
        }
        v->num = num;
        add_value(l, v);
    }
    return ch;
}

int read_line(int fd, struct csv *c) {
    struct line *l = malloc(sizeof(struct line));
    l->values = NULL;
    int ch = read_value(fd, l);
    while (ch == ',') {
        ch = read_value(fd, l);
    }
    if (l->values != NULL) {
        add_line(c, l);
    } else {
        free(l);
    }
    return ch;
}

void skip_header_line(int fd) {
    int ch = read_char(fd);
    while (ch != '\r') {
        ch = read_char(fd);
    }
    ch = read_char(fd);
    assert(ch == '\n');
}

struct csv *read_csv_file(char *f) {
    int fd = open(f, O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(-1);
    }
    skip_header_line(fd);
    struct csv *c = malloc(sizeof(struct csv));
    c->lines = NULL;
    int ch = read_line(fd, c);
    while (ch == '\r') {
        ch = read_char(fd);
        assert(ch == '\n');
        ch = read_line(fd, c);
    }
    return c;
}

// sort

struct row {
    int csv; // original csv 0 or 1
    int id; // original csv line number
    int a; // original csv left column value
    int b; // original csv right column value
};

struct table {
    int n_rows;
    struct row **rows; // array of rows
};

void print_row(struct row *r) {
    printf("%d.%d: %d, %d\n", r->csv, r->id, r->a, r->b);
}

void print_table(struct table *t) {
    for (int i = 0; i < t->n_rows; i++) {
        print_row(t->rows[i]);
    }
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

// match

void print_filtered(struct table *t) {
    // TODO: remove sets of matches that include a single original csv file only
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

struct table *create_table(struct csv *c0, struct csv *c1) {
    int n = count_csv_lines(c0);
    int m = count_csv_lines(c1);
    struct table *t = malloc(sizeof(struct table));
    t->n_rows = n + m;
    t->rows = malloc(t->n_rows * sizeof(struct row *));
    struct line *l0 = c0->lines;
    for (int i = 0; i < n; i++) {
        struct value *v = l0->values;
        t->rows[i] = malloc(sizeof(struct row));
        t->rows[i]->csv = 0;
        t->rows[i]->id = i + 1; // skip header
        // TODO: adapt these to pick other columns in csv 0
        t->rows[i]->a = v->num;
        t->rows[i]->b = v->next->num;
        l0 = l0->next;
    }
    struct line *l1 = c1->lines;
    for (int i = n; i < n + m; i++) {
        struct value *v = l1->values;
        t->rows[i] = malloc(sizeof(struct row));
        t->rows[i]->csv = 1;
        t->rows[i]->id = i - n + 1; // skip header
        // TODO: adapt these to pick other columns in csv 1
        t->rows[i]->a = v->num;
        t->rows[i]->b = v->next->num;
        l1 = l1->next;
    }
    return t;
}

void match_csv_files(char *f0, char *f1) {
    struct csv *c0 = read_csv_file(f0);
    printf("parsed %s:\n\n", f0);
    print_csv_lines(c0);
    struct csv *c1 = read_csv_file(f1);
    printf("\nparsed %s:\n\n", f1);
    print_csv_lines(c1);
    struct table *t = create_table(c0, c1);
    free_csv(c0);
    free_csv(c1);
    printf("\ncreated table:\n\n");
    print_table(t);
    qsort(t->rows, t->n_rows, sizeof(struct row *), compare);
    printf("\nsorted table:\n\n");
    print_table(t);
    printf("\nfiltered table:\n");
    print_filtered(t);
    // TODO: free table t, now done implicitly on exit()
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("usage: %s file0.csv file1.csv\n", argv[0]);
        exit(0);
    }
    match_csv_files(argv[1], argv[2]);
    return 0;
}
