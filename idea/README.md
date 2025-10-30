# Idea
A simple idea generator — picks a line each from n files with newline terminated lines to generate new combinations.

## Build
```bash
$ gcc -o idea idea.c
```

## Basic example
```bash
$ echo "cat\ncow\ndog" > animal
$ echo "walk\nrun\ndance" > activity
$ ./idea animal activity
cow
dance
```

## IoT example
```bash
$ ls iot
domain.txt	input.txt	output.txt	subject.txt
$ ./idea iot/subject.txt iot/input.txt iot/output.txt iot/domain.txt
```

