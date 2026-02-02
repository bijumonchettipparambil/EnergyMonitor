#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "energy_monitor.h"

pid_t fork_child_process();
void create_and_print_json_stub(char*, size_t);
void write_to_buffer(char*, size_t, usage_snapshot);
usage_snapshot initialise_snapshot_stub(uint64_t, double, double, double, double, uint8_t);

int main()
{
    int status;
    pid_t child_pid = fork_child_process();

    if(child_pid > 0)
    {
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status))
        {
            printf("[INFO] Parent: Child with %d exited with status %d.\n", child_pid, WEXITSTATUS(status));
        }
    }
        
    return 0;
}

pid_t fork_child_process()
{
    pid_t pid = fork();
    if (0 == pid)
    {
        fprintf(stdout, "[INFO] In child process...\n");
        size_t BUFFER_SIZE = 256;
        char buffer[BUFFER_SIZE];
        create_and_print_json_stub(buffer, BUFFER_SIZE);
        fprintf(stdout, "[INFO] Exiting child process...\n");
        exit(EXIT_SUCCESS);
    }

    return pid;

}

void create_and_print_json_stub(char* buffer, size_t length)
{
    if (NULL == buffer)
    {
        fprintf(stderr, "[ERROR] %s.%s.%d: Invalid buffer supplied.\n",
               __FILE__, __func__, __LINE__);
        exit(EXIT_FAILURE);
    }

    size_t BUFFER_SIZE = 256;
    if (length < BUFFER_SIZE)
         fprintf(stderr, "[ERROR] %s.%s.%d: Require a buffer of at least %zu, available %zu.\n",
               __FILE__, __func__, __LINE__, BUFFER_SIZE, length);
     
    usage_snapshot stub = initialise_snapshot_stub((uint64_t) 1717379654, (double) 3.12345,(double) 0.001323, 
            (double) 1.433566, (double) 0.0014424, (uint8_t) 15);

    write_to_buffer(buffer, length, stub);
    printf("JSON: %s.\n", buffer);
    
}

void write_to_buffer(char* buffer, size_t length, usage_snapshot snapshot)
{
    size_t response = snprintf(buffer, length, "{\"timestamp\": %lu, \"electric_usage\": %.10lf, \"electric_cost\""
            ": %0.10lf, \"gas_cost\": %0.10lf, \"gas_usage\": %0.10lf, \"status_flags\": %d}",
            snapshot.timestamp, snapshot.electric_usage, snapshot.electric_cost, snapshot.gas_usage, snapshot.gas_cost, snapshot.status);

    if (length <= response)
        fprintf(stderr, "[ERROR] %s.%s.%d: JSON payload truncated, required buffer size %ld, available %ld.\n",
               __FILE__, __func__, __LINE__, response, length);

}

usage_snapshot initialise_snapshot_stub(uint64_t timestamp, double electric_consumption, 
        double electric_cost, double gas_consumption, double gas_cost, uint8_t status_bits)
{
    usage_snapshot stub = {timestamp, electric_consumption, electric_cost, gas_consumption, gas_cost, status_bits};
    return stub;
}
