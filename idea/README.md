# Idea
A simple idea generator.

Picks a line each from n files with newline terminated lines to generate new combinations.

## Basic example
```bash
$ gcc -o idea idea.c
$ echo "cat\ncow\ndog" > animal
$ echo "walk\nrun\ndance" > activity
$ ./idea animal activity
cow
dance
```

## IoT example
```bash
$ ./idea iot/subject.txt iot/input.txt iot/output.txt iot/domain.txt
```

