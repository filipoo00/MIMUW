/**
 * This file is for implementation of MIMPI library.
 * */

#include "channel.h"
#include "mimpi.h"
#include "mimpi_common.h"
#include <sys/select.h>
#include <errno.h>
#include <stdint.h>

static int world_size;
static int my_rank;
static int pipe_fds[16][2];
static bool finished_processes[16];
static int buffers_size[16];
static bool deadlock_detection;
static bool process_waiting_for_me[16];
static bool deadlock[16];
static bool sended_barrier[16];

typedef struct message {
    void *data;
    int count;
    int tag;
    struct message *next;
} message_t;


typedef struct {
    message_t *head;
    message_t *tail;
} message_list;

static message_list buffers[16];
static pthread_mutex_t lock[16];
static pthread_cond_t cond[16];
static pthread_t receiver_thread[16];
static bool receiver_thread_active[16];

void receive_message(fd_set read_fds, int fd, int i) {
    if (select(fd + 1, &read_fds, NULL, NULL, NULL) > 0) {
        if (FD_ISSET(pipe_fds[my_rank][1], &read_fds)) {
            return;
        }
        pthread_mutex_lock(&lock[i]);
        message_t *msg = (message_t*)calloc(1, sizeof(message_t));
        ASSERT_SYS_NULL(msg);
        ssize_t bytes_read = chrecv(pipe_fds[i][1], msg, sizeof(message_t));
        if (bytes_read != sizeof(message_t)) {
            pthread_mutex_unlock(&lock[i]);
            free(msg);
            return;
        }
        ASSERT_SYS_OK(bytes_read);

        if (deadlock_detection == true) {
            if (msg->tag == -5) {
                deadlock[i] = true;
                pthread_mutex_unlock(&lock[i]);
                pthread_cond_signal(&cond[i]);
                free(msg);
                return;
            }
            if (msg->tag == -6) {
                process_waiting_for_me[i] = true;
                pthread_mutex_unlock(&lock[i]);
                pthread_cond_signal(&cond[i]);
                free(msg);
                return;
            }
            if (msg->tag == -7) {
                process_waiting_for_me[i] = false;
                pthread_mutex_unlock(&lock[i]);
                free(msg);
                return;
            }
        }
        if (msg->tag == -1) {
            finished_processes[i] = true;
            receiver_thread_active[i] = false;
            pthread_mutex_unlock(&lock[i]);
            pthread_cond_signal(&cond[i]);
            free(msg);
            return;
        }
        if (msg->tag == -2) {
            sended_barrier[i] = true;
            pthread_mutex_unlock(&lock[i]);
            pthread_cond_signal(&cond[i]);
            free(msg);
            return;
        }


        long long total_size = sizeof(message_t) + msg->count;
        char *msg_data = calloc(total_size, 1);
        ASSERT_SYS_NULL(msg_data);
        memcpy(msg_data, msg, sizeof(message_t));
        free(msg);

        long long total_bytes_read = bytes_read;
        while (total_bytes_read < total_size) {
            int chunk_size = (total_size - total_bytes_read > MAX_CHUNK_SIZE) ? MAX_CHUNK_SIZE : total_size - total_bytes_read;
            bytes_read = chrecv(pipe_fds[i][1], msg_data + total_bytes_read, chunk_size);
            ASSERT_SYS_OK(bytes_read);

            total_bytes_read += bytes_read;
        }
        if (total_bytes_read == total_size) {
            buffers_size[i]++;
            message_t *new_message = calloc(1, sizeof(message_t));
            ASSERT_SYS_NULL(new_message);
            new_message->count = ((message_t*)msg_data)->count;
            new_message->tag = ((message_t*)msg_data)->tag;
            new_message->data = calloc(new_message->count, 1);
            ASSERT_SYS_NULL(new_message->data);
            memcpy(new_message->data, msg_data + sizeof(message_t), new_message->count);
            if (buffers[i].head == NULL) {
                buffers[i].head = new_message;
                buffers[i].tail = new_message;
            }
            else {
                buffers[i].tail->next = new_message;
                buffers[i].tail = new_message;
            }
        }
        pthread_mutex_unlock(&lock[i]);
        pthread_cond_signal(&cond[i]);
        free(msg_data);
    }
}

static void *receiver_thread_function(void *arg) {
    int *my_arg = (int*)arg;
    fd_set read_fds;
    FD_ZERO(&read_fds);

    int fd = 0;
    FD_SET(pipe_fds[*my_arg][1], &read_fds);
    fd = pipe_fds[*my_arg][1];
    FD_SET(pipe_fds[my_rank][1], &read_fds);
    if (fd < pipe_fds[my_rank][1]) {
        fd = pipe_fds[my_rank][1];
    }

    while (receiver_thread_active[*my_arg]) {
        receive_message(read_fds, fd, *my_arg);
    }

    free(my_arg);

    return NULL;
}


void MIMPI_Init(bool enable_deadlock_detection) {
    channels_init();

    world_size = get_world_size();
    my_rank = get_my_rank();
    deadlock_detection = enable_deadlock_detection;
    for (int i = 0; i < world_size; i++) {
        process_waiting_for_me[i] = false;
        deadlock[i] = false;
        finished_processes[i] = false;
        buffers_size[i] = 0;
        sended_barrier[i] = false;

    }
    char env_var_name[64];
    int pipe_write_fd, pipe_read_fd;
    char *pipe_write_str, *pipe_read_str;

    for (int i = 0; i < world_size; i++) {
        sprintf(env_var_name, "MIMPI_PIPE_WRITE_%d", i);
        pipe_write_str = getenv(env_var_name);
        pipe_write_fd = atoi(pipe_write_str);
        pipe_fds[i][0] = pipe_write_fd;
        sprintf(env_var_name, "MIMPI_PIPE_READ_%d", i);
        pipe_read_str = getenv(env_var_name);
        pipe_read_fd = atoi(pipe_read_str);
        pipe_fds[i][1] = pipe_read_fd;
    }

    for (int i = 0; i < world_size; i++) {
        buffers[i].head = NULL;
        buffers[i].tail = NULL;
        ASSERT_ZERO(pthread_mutex_init(&lock[i], NULL));
        ASSERT_ZERO(pthread_cond_init(&cond[i], NULL));
        receiver_thread_active[i] = true;
    }

    for (int i = 0; i < world_size; i++) {
        int *arg = malloc(sizeof(int));
        ASSERT_SYS_NULL(arg);
        *arg = i;
        ASSERT_ZERO(pthread_create(&receiver_thread[i], NULL, receiver_thread_function, (void*)arg));
    }
}

void MIMPI_Finalize() {

    for (int i = 0; i < world_size; i++) {
        receiver_thread_active[i] = false;
    }

    for (int i = 0; i < world_size; i++) {
        if (finished_processes[i] == false) {
            MIMPI_Send(NULL, 0, i, -1);
        }
    }

    for (int i = 0; i < world_size; i++) {
        pthread_join(receiver_thread[i], NULL);
    }

    for (int i = 0; i < world_size; i++) {
        message_t *current = buffers[i].head;
        while (current != NULL) {
            message_t *next = current->next;
            free(current->data);
            free(current);

            current = next;
        }
    }

    for (int i = 0; i < world_size; i++) {
        ASSERT_SYS_OK(close(pipe_fds[i][0]));
        ASSERT_SYS_OK(close(pipe_fds[i][1]));
        ASSERT_ZERO(pthread_cond_destroy(&cond[i]));
        ASSERT_ZERO(pthread_mutex_destroy(&lock[i]));
    }

    channels_finalize();
}

int MIMPI_World_size() {
    return world_size;
}

int MIMPI_World_rank() {
    return my_rank;
}

MIMPI_Retcode MIMPI_Send(
    void const *data,
    int count,
    int destination,
    int tag
) {
    if (my_rank == destination && tag != -1) {
        return MIMPI_ERROR_ATTEMPTED_SELF_OP;
    }

    if (destination < 0 || destination >= world_size) {
        return MIMPI_ERROR_NO_SUCH_RANK;
    }

    if (finished_processes[destination] == true) {
        return MIMPI_ERROR_REMOTE_FINISHED;
    }

    message_t *msg = calloc(1, sizeof(message_t) + count);
    ASSERT_SYS_NULL(msg);

    msg->count = count;
    msg->tag = tag;
    if (data == NULL) {
        msg->data = NULL;
    }
    else {
        msg->data = (char*)msg + sizeof(message_t);
        memcpy(msg->data, data, count);
    }
    msg->next = NULL;


    long long total_bytes_written = 0;
    long long bytes_to_write = sizeof(message_t) + count;
    char *msg_ptr = (char*)msg;


    while (total_bytes_written < bytes_to_write) {
        int chunk_size = (bytes_to_write - total_bytes_written > MAX_CHUNK_SIZE) ? MAX_CHUNK_SIZE : bytes_to_write - total_bytes_written;

        ssize_t bytes_written = chsend(pipe_fds[destination][0], msg_ptr + total_bytes_written, chunk_size);
        if (bytes_written == -1) {
            free(msg);
            if (errno == EPIPE) {
                return MIMPI_ERROR_REMOTE_FINISHED;
            }
            ASSERT_SYS_OK(bytes_written);
        }

        total_bytes_written += bytes_written;
    }
    
    free(msg);


    return MIMPI_SUCCESS;
}

MIMPI_Retcode MIMPI_Recv(
    void *data,
    int count,
    int source,
    int tag
) {
    if (my_rank == source) {
        return MIMPI_ERROR_ATTEMPTED_SELF_OP;
    }

    if (source < 0 || source >= world_size) {
        return MIMPI_ERROR_NO_SUCH_RANK;
    }

    pthread_mutex_lock(&lock[source]);
    if (deadlock_detection == true && my_rank < source) {
        MIMPI_Send(NULL, 0, source, -6);
    }
    while (1) {
        if (sended_barrier[source]) {
            sended_barrier[source] = false;
            break;
        }
        message_t *current = buffers[source].head;
        message_t *prev = NULL;
        bool found = false;
        while (current != NULL) {
            if ((current->count == count && current->tag == tag) ||
                    (current->count == count && tag == 0 && current->tag >= 0)) {
                found = true;
                if (deadlock_detection == true && my_rank < source) {
                    MIMPI_Send(NULL, 0, source, -7);
                }
                break;
            }
            prev = current;
            current = current->next;
        }
        if (found) {
            memcpy(data, current->data, count);
            if (prev == NULL) {
                buffers[source].head = current->next;
                if (current->next == NULL) {
                    buffers[source].tail = NULL;
                }
            }
            else {
                prev->next = current->next;
                if (current->next == NULL) {
                    buffers[source].tail = prev;
                }
            }
            buffers_size[source]--;
            free(current->data);
            free(current);
            break;
        }
        if (deadlock_detection == true) {
            if (process_waiting_for_me[source] == true && my_rank > source) {
                process_waiting_for_me[source] = false;
                pthread_mutex_unlock(&lock[source]);
                MIMPI_Send(NULL, 0, source, -5);
                return MIMPI_ERROR_DEADLOCK_DETECTED;
            }
            if (deadlock[source] == true && my_rank < source) {
                deadlock[source] = false;
                pthread_mutex_unlock(&lock[source]);
                return MIMPI_ERROR_DEADLOCK_DETECTED;
            }
        }
        if (finished_processes[source] == true && buffers_size[source] == 0) {
            pthread_mutex_unlock(&lock[source]);
            return MIMPI_ERROR_REMOTE_FINISHED;
        }
        pthread_cond_wait(&cond[source], &lock[source]);
    }
    pthread_mutex_unlock(&lock[source]);

    return MIMPI_SUCCESS;
}

MIMPI_Retcode MIMPI_Barrier() {
    int left_child = 2*my_rank + 1;
    int right_child = 2*my_rank + 2;
    int parent = (my_rank - 1) / 2;

    if (left_child < world_size) {
        MIMPI_Retcode mimpi_code = MIMPI_Recv(NULL, 0, left_child, -2);
        if (mimpi_code == MIMPI_ERROR_REMOTE_FINISHED) {
            return mimpi_code;
        }
    }
    if (right_child < world_size) {
        MIMPI_Retcode mimpi_code = MIMPI_Recv(NULL, 0, right_child, -2);
        if (mimpi_code == MIMPI_ERROR_REMOTE_FINISHED) {
            return mimpi_code;
        }
    }
    if (my_rank != 0) {
        MIMPI_Send(NULL, 0, parent, -2);
    }

    if (my_rank == 0) {
        if (left_child < world_size) {
            MIMPI_Send(NULL, 0, left_child, -2);
        }
        if (right_child < world_size) {
            MIMPI_Send(NULL, 0, right_child, -2);
        }
    }
    else {
        MIMPI_Retcode mimpi_code = MIMPI_Recv(NULL, 0, parent, -2);
        if (mimpi_code == MIMPI_ERROR_REMOTE_FINISHED) {
            return mimpi_code;
        }
        if (left_child < world_size) {
            MIMPI_Send(NULL, 0, left_child, -2);
        }
        if (right_child < world_size) {
            MIMPI_Send(NULL, 0, right_child, -2);
        }
    }

    return MIMPI_SUCCESS;
}

MIMPI_Retcode MIMPI_Bcast(
    void *data,
    int count,
    int root
) {
    int parent, left_child, right_child;
    
    int relative_rank = (my_rank - root + world_size) % world_size;
    int relative_left_child = 2 * relative_rank + 1;
    int relative_right_child = 2 * relative_rank + 2;
    
    left_child = (relative_left_child >= world_size) ? -1 : (relative_left_child + root) % world_size;
    right_child = (relative_right_child >= world_size) ? -1 : (relative_right_child + root) % world_size;

    parent = relative_rank > 0 ? (relative_rank - 1) / 2 : -1;
    if (parent >= 0) {
        parent = (parent + root) % world_size;
    }

    if (my_rank == root) {
        if (left_child != -1) {
            MIMPI_Send(data, count, left_child, -3);
        }
        if (right_child != -1) {
            MIMPI_Send(data, count, right_child, -3);      
        }
    }
    else {
        MIMPI_Retcode mimpi_code = MIMPI_Recv(data, count, parent, -3);
        if (mimpi_code == MIMPI_ERROR_REMOTE_FINISHED) {
            return mimpi_code;
        }
        if (left_child != -1) {
            MIMPI_Send(data, count, left_child, -3);
        }
        if (right_child != -1) {
            MIMPI_Send(data, count, right_child, -3);
        }
    }
    
    return MIMPI_SUCCESS;
}

MIMPI_Retcode MIMPI_Reduce(
    void const *send_data,
    void *recv_data,
    int count,
    MIMPI_Op op,
    int root
) {
    int parent, left_child, right_child;
    
    int relative_rank = (my_rank - root + world_size) % world_size;
    int relative_left_child = 2 * relative_rank + 1;
    int relative_right_child = 2 * relative_rank + 2;
    
    left_child = (relative_left_child >= world_size) ? -1 : (relative_left_child + root) % world_size;
    right_child = (relative_right_child >= world_size) ? -1 : (relative_right_child + root) % world_size;

    parent = relative_rank > 0 ? (relative_rank - 1) / 2 : -1;
    if (parent >= 0) {
        parent = (parent + root) % world_size;
    }

    uint8_t *recv_buffer = malloc(count);
    ASSERT_SYS_NULL(recv_buffer);
    uint8_t *buffer = malloc(count);
    ASSERT_SYS_NULL(buffer);
    const uint8_t *typed_send_data = (const uint8_t *)send_data;
    for (int i = 0; i < count; i++) {
        buffer[i] = typed_send_data[i];
    }

    if (left_child != -1) {
        MIMPI_Retcode mimpi_code = MIMPI_Recv(recv_buffer, count, left_child, -4);
        if (mimpi_code == MIMPI_ERROR_REMOTE_FINISHED) {
            free(recv_buffer);
            if (my_rank != root) {
                free(buffer);
            }
            return mimpi_code;
        }
        for (int j = 0; j < count; j++) {
            switch (op) {
                case MIMPI_SUM:
                    buffer[j] += recv_buffer[j];
                    break;
                case MIMPI_MAX:
                    if (buffer[j] < recv_buffer[j]) {
                        buffer[j] = recv_buffer[j];
                    }
                    break;
                case MIMPI_MIN:
                    if (buffer[j] > recv_buffer[j]) {
                        buffer[j] = recv_buffer[j];  
                    }
                    break;
                case MIMPI_PROD:
                    buffer[j] *= recv_buffer[j];
                    break;
            }
        }
    }
    if (right_child != -1) {
        MIMPI_Retcode mimpi_code = MIMPI_Recv(recv_buffer, count, right_child, -4);
        if (mimpi_code == MIMPI_ERROR_REMOTE_FINISHED) {
            free(recv_buffer);
            if (my_rank != root) {
                free(buffer);
            }
            return mimpi_code;
        }
        for (int j = 0; j < count; j++) {
            switch (op) {
                case MIMPI_SUM:
                    buffer[j] += recv_buffer[j];
                    break;
                case MIMPI_MAX:
                    if (buffer[j] < recv_buffer[j]) buffer[j] = recv_buffer[j];
                    break;
                case MIMPI_MIN:
                    if (buffer[j] > recv_buffer[j]) buffer[j] = recv_buffer[j];
                    break;
                case MIMPI_PROD:
                    buffer[j] *= recv_buffer[j];
                    break;
            }
        }
    }
    if (my_rank != root) {
        MIMPI_Send(buffer, count, parent, -4);
    }
    else {
        memcpy(recv_data, buffer, count);
    }

    free(buffer);
    free(recv_buffer);

    return MIMPI_SUCCESS;    
}