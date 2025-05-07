#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// 파이프 여러 개 있을 때 아니면 백그라운드인 명령 실행
int exec_pipeline(char ***cmds, int n_cmds, int background) {
    int in_fd = STDIN_FILENO;
    int status = 0;
    pid_t *pids = malloc(n_cmds * sizeof(pid_t));

    for (int i = 0; i < n_cmds; i++) {
        int pipe_fd[2];
        if (i < n_cmds - 1) {
            pipe(pipe_fd);  // 파이프 생성
        }
        pid_t pid = fork();
        if (pid == 0) {
            if (in_fd != STDIN_FILENO) {
                dup2(in_fd, STDIN_FILENO);
                close(in_fd);
            }
            if (i < n_cmds - 1) {
                dup2(pipe_fd[1], STDOUT_FILENO);
                close(pipe_fd[0]);
                close(pipe_fd[1]);
            }
            execvp(cmds[i][0], cmds[i]);
            perror("execvp");
            exit(EXIT_FAILURE);
        }
        pids[i] = pid;
        if (in_fd != STDIN_FILENO)
            close(in_fd);
        if (i < n_cmds - 1) {
            close(pipe_fd[1]);
            in_fd = pipe_fd[0];
        }
    }

    if (!background) {
        for (int i = 0; i < n_cmds; i++)
            waitpid(pids[i], &status, 0);
        free(pids);
        if (WIFEXITED(status))
            return WEXITSTATUS(status);
        return status;
    }
    free(pids);
    return 0;
}