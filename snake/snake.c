// snake - https://x.com/itsfoss2/status/1796244297336914261

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#define ROWS 4
#define COLS 6

//#define ARROW_UP 'w'
//#define ARROW_DOWN 66 's'
//#define ARROW_LEFT 68 'a'
//#define ARROW_RIGHT 67 'd'

#define ARROW_UP 65
#define ARROW_DOWN 66
#define ARROW_LEFT 68
#define ARROW_RIGHT 67

char display[COLS][ROWS];

struct point {
    int x;
    int y;
    struct point *next;
};

struct point *create_point(int x, int y) {
    struct point *p = malloc(sizeof(struct point));
    p->x = x;
    p->y = y;
    p->next = NULL;
    return p;
}

struct point *random_point() {
    return create_point(
        random() % COLS, 
        random() % ROWS);
}

int has_collision(struct point *p, struct point *list) {
    assert(p != NULL);
    int res = 0;
    while (!res && list != NULL) {
        res = 
            (p->x == list->x) && 
            (p->y == list->y);
        list = list->next;
    }
    return res;
}

// https://stackoverflow.com/a/19288271
int mod(int a, int b) {
    int res = a % b;
    if (res < 0) {
        res += b;
    }
    return res;
}

struct point *move_snake_head(struct point *head, char dir) {
    assert(head != NULL);
    struct point *p = create_point(head->x, head->y);
    if (dir == ARROW_LEFT) {
        p->x = mod(p->x - 1, COLS);
    } else if (dir == ARROW_RIGHT) {
        p->x = mod(p->x + 1, COLS);
    } else if (dir == ARROW_UP) {
        p->y = mod(p->y - 1, ROWS);
    } else if (dir == ARROW_DOWN) {
        p->y = mod(p->y + 1, ROWS);
    } else {
        assert(0); // invalid direction
    }
    p->next = head;
    return p;
}

void drag_snake_tail(struct point *head) {
    struct point *prev = NULL;
    while (head != NULL && head->next != NULL) {
        prev = head;
        head = head->next;
    }
    if (prev != NULL) {
        free(prev->next);
        prev->next = NULL;
    }
}

struct point *move_thing(struct point *thing, struct point *snake) {
    struct point *p = NULL;
    do {
        free(p);
        p = random_point();
    } while (has_collision(p, thing) || has_collision(p, snake)); // TODO: detect win
    free(thing);
    return p;
}

void clear_display() {
    for (int x = 0; x < COLS; x++) {
        for (int y = 0; y < ROWS; y++) {
            display[x][y] = '.';
        }
    }
}

void draw_snake(struct point *head) {
    while (head != NULL) {
        display[head->x][head->y] = '*';
        head = head->next;
    }
}

void draw_thing(struct point *thing) {
    assert(thing != NULL);
    display[thing->x][thing->y] = '#';
}

void show_display() {
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            printf("%c", display[x][y]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(void) {
    struct point *snake = random_point();
    struct point *thing = move_thing(random_point(), snake);
    int done = 0;
    int score = 0;
    while (!done) {
        clear_display();
        draw_thing(thing);
        draw_snake(snake);
        show_display();

        int ch;
        do {
            ch = getchar();
            //printf("%d\n", ch);
        } while ((ch != ARROW_UP) && (ch != ARROW_DOWN) && 
            (ch != ARROW_LEFT) && (ch != ARROW_RIGHT));

        snake = move_snake_head(snake, ch);
        done = has_collision(snake, snake->next);
        int scored = has_collision(snake, thing);
        if (!done) {
            if (!scored) {
                drag_snake_tail(snake);
            } else {
                score++;
                thing = move_thing(thing, snake);
            }
        }
    }
    printf("game over, score = %d\n", score);
    return 0;
}
