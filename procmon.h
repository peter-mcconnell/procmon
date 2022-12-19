#ifndef MEMORY_PRINTER_H
#define MEMORY_PRINTER_H

#include <linux/types.h>

// Declare the function that prints the memory for a given process ID
void print_memory_for_pid(pid_t pid);

#endif // MEMORY_PRINTER_H
