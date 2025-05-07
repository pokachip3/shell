#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 한줄을 띄어쓰기 & 연산자별로 쪼개기
// (반환: tokens 배열, *ntokens에 개수. 마지막은 NULL.)
char **split_tokens(const char *line, int *ntokens) {
    int capacity = 10;
    char **tokens = malloc(capacity * sizeof(char*));
    *ntokens = 0;
    int i = 0, len = strlen(line);

    while (i < len) {
        while (i < len && isspace((unsigned char)line[i])) i++;  // 공백 스킵
        if (i >= len) break;

        // 연산자 토큰 처리
        if (line[i] == '|' && i+1 < len && line[i+1] == '|') {
            tokens[(*ntokens)++] = strdup("||"); i += 2;
        }
        else if (line[i] == '|') {
            tokens[(*ntokens)++] = strdup("|"); i++;
        }
        else if (line[i] == '&' && i+1 < len && line[i+1] == '&') {
            tokens[(*ntokens)++] = strdup("&&"); i += 2;
        }
        else if (line[i] == '&') {
            tokens[(*ntokens)++] = strdup("&"); i++;
        }
        else if (line[i] == ';') {
            tokens[(*ntokens)++] = strdup(";"); i++;
        }
        else {  // 일반 단어
            int j = i;
            while (j < len &&
                   !isspace((unsigned char)line[j]) &&
                   line[j] != '|' &&
                   line[j] != '&' &&
                   line[j] != ';') {
                j++;
            }
            int toklen = j - i;
            char *tok = malloc(toklen + 1);
            memcpy(tok, &line[i], toklen);
            tok[toklen] = '\0';
            tokens[(*ntokens)++] = tok;
            i = j;
        }

        if (*ntokens >= capacity) {  // 용량 확장
            capacity *= 2;
            tokens = realloc(tokens, capacity * sizeof(char*));
        }
    }

    tokens[*ntokens] = NULL;
    return tokens;
}

void free_tokens(char **tokens) {  // split_tokens 로 만들어진 메모리 해제
    for (int i = 0; tokens[i]; i++) {
        free(tokens[i]);
    }
    free(tokens);
}

void trim_newline(char *line) { // getline 으로 읽은 문자열 끝 개행 제거
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }
}