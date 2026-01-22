#ifndef REPL_INPUT_H
#define REPL_INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

void repl_init(void);
char *repl_readline(const char *prompt);
void repl_add_history(const char *line);
void repl_free(void);

#ifdef __cplusplus
}
#endif

#endif // REPL_INPUT_H