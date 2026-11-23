#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>

#define SOCKET_PATH "/tmp/ceag_ipc.sock"
#define BUFFER_SIZE 512

static volatile sig_atomic_t keep_running = 1;
static int server_fd = -1;

void handle_signal(int sig) {
    (void)sig;
    keep_running = 0;
}

int check_binder_status(void) {
    return (access("/dev/binder", F_OK) == 0 || access("/dev/binderfs/binder", F_OK) == 0) ? 1 : 0;
}

long get_available_memory_kb(void) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp) return -1;
    char line[128];
    long mem_avail = -1;
    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "MemAvailable: %ld kB", &mem_avail) == 1) break;
    }
    fclose(fp);
    return mem_avail;
}

double get_cpu_load_1m(void) {
    FILE *fp = fopen("/proc/loadavg", "r");
    if (!fp) return -1.0;
    double load1 = -1.0;
    if (fscanf(fp, "%lf", &load1) != 1) load1 = -1.0;
    fclose(fp);
    return load1;
}

void process_command(const char *cmd, char *response, size_t resp_size) {
    if (strncmp(cmd, "GET_TELEMETRY", 13) == 0) {
        snprintf(response, resp_size,
                 "{\"status\":\"OK\",\"arch\":\"x86_64\",\"binder_active\":%d,"
                 "\"mem_available_kb\":%ld,\"cpu_load_1m\":%.2f}\n",
                 check_binder_status(), get_available_memory_kb(), get_cpu_load_1m());
    } else if (strncmp(cmd, "PING", 4) == 0) {
        snprintf(response, resp_size, "{\"status\":\"PONG\",\"gateway\":\"CEAG\"}\n");
    } else {
        snprintf(response, resp_size, "{\"status\":\"ERROR\",\"msg\":\"Comando no reconocido\"}\n");
    }
}

int main(void) {
    struct sockaddr_un addr;
    struct sigaction sa;

    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = handle_signal;
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    unlink(SOCKET_PATH);
    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (server_fd < 0) return EXIT_FAILURE;

    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, SOCKET_PATH, sizeof(addr.sun_path) - 1);

    if (bind(server_fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        close(server_fd);
        return EXIT_FAILURE;
    }

    chmod(SOCKET_PATH, 0666);
    if (listen(server_fd, 5) < 0) {
        close(server_fd);
        unlink(SOCKET_PATH);
        return EXIT_FAILURE;
    }

    printf("[CEAG-Daemon] Pasarela IPC activa en %s\n", SOCKET_PATH);

    while (keep_running) {
        int client_fd = accept(server_fd, NULL, NULL);
        if (client_fd < 0) {
            if (errno == EINTR) continue;
            break;
        }
        char buffer[BUFFER_SIZE] = {0};
        char response[BUFFER_SIZE] = {0};
        if (read(client_fd, buffer, sizeof(buffer) - 1) > 0) {
            buffer[strcspn(buffer, "\r\n")] = 0;
            process_command(buffer, response, sizeof(response));
            write(client_fd, response, strlen(response));
        }
        close(client_fd);
    }

    close(server_fd);
    unlink(SOCKET_PATH);
    return EXIT_SUCCESS;
}
