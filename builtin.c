#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

// exit, cd, pwd 처리
int builtin_execute(char **argv) {
    if (strcmp(argv[0], "exit") == 0) {
        exit(0); 
    }
    if (strcmp(argv[0], "cd") == 0) {
        char *path = argv[1]; //디렉토리 경로
        if (!path) {  // 인자 없으면 HOME
            path = getenv("HOME");
            if (!path) path = "."; // HOME도 없으면 현재
        }
        if (chdir(path) != 0) {
            perror("cd"); //왜 안되는지 출력
            return 1;
        }
        return 0;
    }
    if (strcmp(argv[0], "pwd") == 0) {
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd))) {
            printf("%s\n", cwd);
        } else {
            perror("pwd"); // 안되면 에러 보여줌
            return 1;
        }
        return 0;
    }
    return -1;
}
