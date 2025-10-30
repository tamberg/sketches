# Idea
A simple idea generator — picks a line each from n files with newline terminated lines to generate new combinations.

## Build
```bash
$ gcc -o idea idea.c
$ ./idea
usage: ./idea file-1 file-2 ... file-n
```

## Basic example
```bash
$ echo "cat\ncow\ndog" > animal.txt
$ echo "walk\nrun\ndance" > activity.txt
$ ./idea animal.txt activity.txt
cow
dance
```

## IoT example
```bash
$ ls iot
domain.txt	input.txt	output.txt	subject.txt
$ ./idea iot/subject.txt iot/input.txt iot/output.txt iot/domain.txt
```
