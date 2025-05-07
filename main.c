#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

// support.c 구현 함수
char **split_tokens(const char *line, int *ntokens);
void free_tokens(char **tokens);
void trim_newline(char *line);

int builtin_execute(char **argv);  // builtin 명령 실행 함수
int exec_pipeline(char ***cmds, int n_cmds, int background);  // 파이프라인 실행 함수

int main() {
    char *line = NULL;  // 입력 받은 문자열 포인터
    size_t len = 0;  // 버퍼 크기
    int prev_op = 0;  // 0:없음, 1:&&, 2:||
    int last_status = 0;  // 이전 명령의 종료 상태 

    while (1) {
        // 현재 작업 디렉토리 출력
        char cwd[PATH_MAX];
        if (getcwd(cwd, sizeof(cwd))) {
            printf("%s$ ", cwd);
        } else {
            printf("$ ");
        }
        fflush(stdout);

        // 입력 줄 읽기 
        if (getline(&line, &len, stdin) == -1) {
            printf("\n");
            break;
        }
        trim_newline(line);  // 끝의 문자 제거
        if (line[0] == '\0') continue;  // 빈 줄이면 다시

        // 띄어쓰기, 연산자 기준으로 토큰 분리 
        int ntokens = 0;
        char **tokens = split_tokens(line, &ntokens);
        int idx = 0;
        prev_op = 0;

        // 토큰 확인하면서 쪼갠 단위로 실행
        while (idx < ntokens) {
            if ((prev_op == 1 && last_status != 0) ||  // 앞에꺼가 &&인데 실패했거나 ||인데 성공이면 다음 블록까지 스킵 
                (prev_op == 2 && last_status == 0)) {
                while (idx < ntokens && strcmp(tokens[idx], ";") != 0) idx++;
                if (idx < ntokens) idx++;  // ; 자신도 넘김
                prev_op = 0;  // 건너뛰기 후 초기화
                continue;
            }

            char ***cmds = malloc(ntokens * sizeof(char**));
            int cmd_count = 0;
            int background = 0;

            // 파이프 기준으로 argv 묶기
            while (idx < ntokens) {
                if (strcmp(tokens[idx], "|") == 0) {
                    idx++;  //파이프 기호 패스
                    continue;
                }
                if (strcmp(tokens[idx], "&") == 0 ||
                    strcmp(tokens[idx], "&&") == 0 ||
                    strcmp(tokens[idx], "||") == 0 ||
                    strcmp(tokens[idx], ";") == 0) {
                    break;
                }
                int start = idx;
                while (idx < ntokens &&
                       strcmp(tokens[idx], "|") != 0 &&
                       strcmp(tokens[idx], "&") != 0 &&
                       strcmp(tokens[idx], "&&") != 0 &&
                       strcmp(tokens[idx], "||") != 0 &&
                       strcmp(tokens[idx], ";") != 0) {
                    idx++;
                }
                int argc = idx - start;
                char **argv = malloc((argc + 1) * sizeof(char*));
                for (int i = 0; i < argc; i++) {
                    argv[i] = tokens[start + i];
                }
                argv[argc] = NULL;
                cmds[cmd_count++] = argv;
            }

            if (idx < ntokens && strcmp(tokens[idx], "&") == 0) {  // 뒤에 & 있을때 
                background = 1;
                idx++;
            }

            int next_op = 0;  // 다음 연산자 기억 (&&, ||)
            if (idx < ntokens) {
                if (strcmp(tokens[idx], "&&") == 0) next_op = 1;
                else if (strcmp(tokens[idx], "||") == 0) next_op = 2;
                else next_op = 0;
                idx++;
            }

            // 내장이면 builtin 아니면 파이프라인 실행
            if (cmd_count > 0) {
                if (cmd_count == 1 &&
                    (strcmp(cmds[0][0], "cd") == 0 ||
                     strcmp(cmds[0][0], "pwd") == 0 ||
                     strcmp(cmds[0][0], "exit") == 0)) {
                    last_status = builtin_execute(cmds[0]);
                } else {
                    last_status = exec_pipeline(cmds, cmd_count, background);
                }
            }
            prev_op = next_op;

            for (int i = 0; i < cmd_count; i++) {  // argv 메모리 해제
                free(cmds[i]);
            }
            free(cmds);
        }

        free_tokens(tokens);  // 토큰 배열 메모리 해제
    }

    free(line);
    return 0;
}
