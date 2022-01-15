// licensed under https://tamberg.mit-license.org/

// $ gcc -o parse parse.c
// $ ./parse file.csv

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>

// csv-file = line { '\r\n' line } 
// line = value { ',' value } 
// value = digit { digit }
// digit = 0..9

struct value {
    int num;
    struct value *next;
};

struct line {
    struct value *values;
    struct line *next;
};

struct csv {
    struct line *lines;
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

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("usage: %s file\n", argv[0]);
        exit(0);
    }
    struct csv *c = read_csv_file(argv[1]);
    printf("parsed:\n\n");
    print_csv_lines(c);
    int n = count_csv_lines(c);
    printf("\ncounted %d lines\n", n);
    free_csv(c);
    return 0;
}
