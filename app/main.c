#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>

#include "energy_monitor.h"
#include "log.h"
#include "json.h"

pid_t create_child_process();
void create_and_print_json_stub(char*, size_t);
void write_to_buffer(char*, size_t, usage_snapshot);
usage_snapshot initialise_snapshot_stub(uint64_t, double, double, double, double, uint8_t);

int main()
{

    const char* json_str = "{\"timestamp\": 1717379654, \"electric_usage\": 3.1234500000, \"electric_cost\": 0.0013230000, \"gas_cost\": 1.4335660000, \"gas_usage\": 0.0014424000, \"status_flags\": \"15\", \"switch\" : true, \"is_cancelled\" : false, \"updatedtimestamp\" : null}";

    parse_json(json_str, strlen(json_str));

//    LOG(INFO, "Testing variables: Integer=%d, Float=%.2f\n", 999, 3.141514);
    set_log_level(DEBUG);   
//    LOG(DEBUG, "%s", "Test debug message...\n");
    int status;
    pid_t child_pid = create_child_process();

    if(child_pid > 0)
    {
        waitpid(child_pid, &status, 0);
        if (WIFEXITED(status))
        {
            //LOG(INFO, "Parent: Child with %d exited with status %d.\n", child_pid, WEXITSTATUS(status));
        }
    }
        
    return 0;
}

pid_t create_child_process()
{
    pid_t pid = fork();
    if (0 == pid)
    {
        //LOG(INFO, "Child: In child process...\n");
        size_t BUFFER_SIZE = 256;
        char buffer[BUFFER_SIZE];
        create_and_print_json_stub(buffer, BUFFER_SIZE);
        //LOG(INFO, "Child: Exiting child process...\n");
        exit(EXIT_SUCCESS);
    }

    return pid;

}

void create_and_print_json_stub(char* buffer, size_t length)
{
    if (NULL == buffer)
    {
        LOG(ERROR, "Invalid buffer supplied.\n");
        exit(EXIT_FAILURE);
    }

    size_t BUFFER_SIZE = 256;
    if (length < BUFFER_SIZE)
         LOG(ERROR, "Requires a buffer of at least %zu, available %zu.\n", BUFFER_SIZE, length);
     
    usage_snapshot stub = initialise_snapshot_stub((uint64_t) 1717379654, (double) 3.12345,(double) 0.001323, 
            (double) 1.433566, (double) 0.0014424, (uint8_t) 15);

    write_to_buffer(buffer, length, stub);
    //LOG(INFO, "JSON: %s.\n", buffer);
    
}

void write_to_buffer(char* buffer, size_t length, usage_snapshot snapshot)
{
    size_t response = snprintf(buffer, length, "{\"timestamp\": %lu, \"electric_usage\": %.10lf, \"electric_cost\""
            ": %0.10lf, \"gas_cost\": %0.10lf, \"gas_usage\": %0.10lf, \"status_flags\": %d}",
            snapshot.timestamp, snapshot.electric_usage, snapshot.electric_cost, snapshot.gas_usage, 
            snapshot.gas_cost, snapshot.status);

    if (length <= response)
        LOG(ERROR, "JSON payload truncated, required buffer size %ld, available %ld.\n", response, length);

}

usage_snapshot initialise_snapshot_stub(uint64_t timestamp, double electric_consumption, 
        double electric_cost, double gas_consumption, double gas_cost, uint8_t status_bits)
{
    usage_snapshot stub = {timestamp, electric_consumption, electric_cost, gas_consumption, gas_cost, status_bits};
    return stub;
}
