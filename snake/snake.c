// snake - https://x.com/itsfoss2/status/1796244297336914261

// $ gcc -o snake snake.c
// $ stty -icanon -echo; ./snake
// $ stty sane # fix terminal after game

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>

#define ROWS 4
#define COLS 6

#define ARROW_UP 'w'
#define ARROW_DOWN 's'
#define ARROW_LEFT 'a'
#define ARROW_RIGHT 'd'

// MacOS arrow keys (hack)
//#define ARROW_UP 'A'
//#define ARROW_DOWN 'B'
//#define ARROW_LEFT 'D'
//#define ARROW_RIGHT 'C'

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
    int has = 0;
    while (!has && list != NULL) {
        has = 
            (p->x == list->x) && 
            (p->y == list->y);
        list = list->next;
    }
    return has;
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
        assert(0); // unknown direction
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

struct point *move_token(struct point *token, struct point *snake) {
    time_t t0 = time(NULL);
    struct point *p = NULL;
    do {
        free(p);
        if (time(NULL) - t0 < 3) {
            p = random_point();
        } else { // timeout
            p = NULL;
        }
    } while (
        p != NULL && (
        has_collision(p, token) || 
        has_collision(p, snake)));
    free(token);
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

void draw_token(struct point *token) {
    assert(token != NULL);
    display[token->x][token->y] = '#';
}

void show_display() { // TODO: redraw in place
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            printf("%c", display[x][y]);
        }
        printf("\n");
    }
    printf("\n");
}

int main(void) {
    // TODO: enable cbreak mode
    printf("\npress '%c' = UP, '%c' = DOWN, '%c' = LEFT or '%c' = RIGHT\n\n",
        ARROW_UP, ARROW_DOWN, ARROW_LEFT, ARROW_RIGHT);
    struct point *snake = random_point();
    struct point *token = move_token(random_point(), snake);
    int done = 0;
    int score = 0;
    while (!done) {
        // show current state
        clear_display();
        draw_token(token);
        draw_snake(snake);
        show_display();

        // read user input
        int ch;
        do {
            ch = getchar(); // TODO: timeout
        } while (
            (ch != ARROW_UP) && (ch != ARROW_DOWN) && 
            (ch != ARROW_LEFT) && (ch != ARROW_RIGHT));

        // update state
        snake = move_snake_head(snake, ch);
        done = has_collision(snake, snake->next);
        int scored = has_collision(snake, token);
        if (!done) {
            if (!scored) {
                drag_snake_tail(snake);
            } else {
                score++;
                token = move_token(token, snake);
                done = token == NULL; // you win
            }
        }
    }
    printf("%s, score = %d\n", token == NULL ? "you win" : "game over", score);
    return 0;
}
