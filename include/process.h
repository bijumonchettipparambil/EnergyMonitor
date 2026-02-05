#ifndef ENERGYMONITOR_PROCESS_H_
#define ENERGYMONITOR_PROCESS_H_

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/**
    
  Create a new child process using fork and return its pid.
  @returns pid of the child process.

Notes:
- Parent (caller) gets the pid of the child (new) process created.
- pid value will be set zero within the child process.

*/
pid_t create_child_process();

/**

  Checks if the current process is a child process or not.

  @param process id returned by the fork.
  @returns 1(true) if current process is child.

*/
int is_parent(const pid_t pid);

/**

  Checks if the current process is a child process or not.

  @param process id returned by the fork.
  @returns 1(true) if current process is child.

*/
int is_child(const pid_t pid);

/**

  Suspend parent process and wait for child process to complete using waitpid.
  The child process may terminate normally or abnormally.

  @param status integer pointer variable to store the status.
  @returns the pid of the child process.

*/
pid_t suspend_and_wait_for_child_process_status(const pid_t pid, int* status);

/**

  Verify whether the child process exited successfuly using WIFEXITED macro.

  @param status pointer to store the reaped status.
  @param success_status child exit status that indicates success.
  @returns 1 (true) if successful, else return 0.

*/
int is_child_process_exit_success(const int status, const int success_status);

/**

  Verify whether the child process exited wit failure using WIFEXITED macro.
  
  @param status the reaped status.
  @param success_status integer variable containing the value that indicates success.
  @returns 1 (true) if failed, else return 0.

*/
int is_child_process_exit_failed(const int status, const int success_status);

/**

  Get the child process exit status using WEXITSTATUS macro.

  @param status pointer containing the reaped status.
  @returns the child process exit status.

*/
int get_child_process_exit_status(const int status);

#endif
