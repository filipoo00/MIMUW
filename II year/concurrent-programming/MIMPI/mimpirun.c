/**
 * This file is for implementation of mimpirun program.
 * */

#include "mimpi_common.h"
#include "channel.h"

int main(int argc, char *argv[]) {

    int n = atoi(argv[1]);
    char *path = argv[2];
    char **child_args = &argv[2];


    int pipe_fds[n][n][2];

    int max_dsc_number = 0;
    int how_many_dsc = 0;

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ASSERT_SYS_OK(channel(pipe_fds[i][j]));
            how_many_dsc += 2;
            if (i == n-1 && j == n-1) {
                max_dsc_number = pipe_fds[i][j][1];
            }
        }
    }

    int free_dsc = 20;
    if (max_dsc_number < 20) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                ASSERT_SYS_OK(dup2(pipe_fds[i][j][0], free_dsc));
                ASSERT_SYS_OK(close(pipe_fds[i][j][0]));
                pipe_fds[i][j][0] = free_dsc;
                free_dsc++;
                ASSERT_SYS_OK(dup2(pipe_fds[i][j][1], free_dsc));
                ASSERT_SYS_OK(close(pipe_fds[i][j][1]));
                pipe_fds[i][j][1] = free_dsc;
                free_dsc++;
            }
        }
    }
    else {
        int how_many_higher_than_nineteen = max_dsc_number - 20 + 1;
        int how_many_lower_than_twenty = how_many_dsc - how_many_higher_than_nineteen;
        int count = 0;
        free_dsc = max_dsc_number + 1;
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (how_many_lower_than_twenty+1 == count) {
                    break;
                }
                ASSERT_SYS_OK(dup2(pipe_fds[i][j][0], free_dsc));
                ASSERT_SYS_OK(close(pipe_fds[i][j][0]));
                pipe_fds[i][j][0] = free_dsc;
                free_dsc++;
                count++;
                if (how_many_lower_than_twenty+1 == count) {
                    break;
                }
                ASSERT_SYS_OK(dup2(pipe_fds[i][j][1], free_dsc));
                ASSERT_SYS_OK(close(pipe_fds[i][j][1]));
                pipe_fds[i][j][1] = free_dsc;
                free_dsc++;
                count++;
            }
            if (how_many_lower_than_twenty+1 == count) {
                break;
            }
        }
    }

    char size_str[11];
    sprintf(size_str, "%d", n);
    setenv("MIMPI_SIZE", size_str, 1);

    for (int i = 0; i < n; i++) {
        pid_t pid;
        ASSERT_SYS_OK(pid = fork());
        if (pid == 0) { 
            char rank_str[11];
            sprintf(rank_str, "%d", i);
            setenv("MIMPI_RANK", rank_str, 1);
            
            for (int j = 0; j < n; j++) {
                char pipe_read_str[11], pipe_write_str[11];
                sprintf(pipe_read_str, "%d", pipe_fds[j][i][0]);
                sprintf(pipe_write_str, "%d", pipe_fds[i][j][1]);

                char env_var_name[64];
                sprintf(env_var_name, "MIMPI_PIPE_READ_%d", j);
                setenv(env_var_name, pipe_read_str, 1);

                sprintf(env_var_name, "MIMPI_PIPE_WRITE_%d", j);
                setenv(env_var_name, pipe_write_str, 1);

            }

            for (int k = 0; k < n; k++) {
                if (k != i) {
                    for (int j = 0; j < n; j++) {
                        ASSERT_SYS_OK(close(pipe_fds[j][k][0]));
                        ASSERT_SYS_OK(close(pipe_fds[k][j][1]));
                    }
                }
            }
            
            ASSERT_SYS_OK(execvp(path, child_args));
        }
    }

    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            ASSERT_SYS_OK(close(pipe_fds[i][j][0]));
            ASSERT_SYS_OK(close(pipe_fds[i][j][1]));
        }
    }

    for (int i = 0; i < n; i++)
        ASSERT_SYS_OK(wait(NULL));

    return 0;
}