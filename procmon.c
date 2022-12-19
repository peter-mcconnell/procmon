#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/mm_types.h>
#include <linux/highmem.h>

#include "procmon.h"

static int pid = -1;

// Function to print the contents of a buffer in hexadecimal format
void print_buffer(char *buffer, unsigned long size) {
  unsigned long i;
  for (i = 0; i < size; i++) {
    printk(KERN_CONT "%02x ", buffer[i]);
    if (i % 16 == 15) {
      printk(KERN_CONT "\n");
    }
  }
}

void print_memory_for_pid(int pid) {
  struct task_struct *task;
  long copied;

  // Iterate over all tasks in the system
  for_each_process(task) {
    if (task->pid == pid) {
      // Found the task with the given pid
      struct mm_struct *mm = task->mm;
      struct vm_area_struct *vma;

      // Iterate over all memory regions (vm_area_structs) in the task's memory map
      for (vma = mm->mmap; vma; vma = vma->vm_next) {
        unsigned long start = vma->vm_start;
        unsigned long end = vma->vm_end;
        unsigned long size = end - start;

        // Allocate a buffer in kernel space to hold the contents of the memory region
        char *buffer = kmalloc(size, GFP_KERNEL);
        if (!buffer) {
          printk(KERN_ERR "procmon: Failed to allocate buffer\n");
          continue;
        }

        // Copy the contents of the memory region into the buffer
        copied = copy_from_user(buffer, (void __user *)start, size);
        if (copied != 0) {
          printk(KERN_ERR "procmon: Failed to copy memory contents: %ld bytes not copied\n", copied);
          kfree(buffer);
          continue;
        }

        // Print the contents of the buffer
        printk(KERN_INFO "procmon: Memory region: %lx - %lx\n", start, end);
        print_buffer(buffer, size);

        // Free the buffer
        kfree(buffer);
      }
    }
  }
}

static int __init procmon_module_init(void) {
  print_memory_for_pid(pid);
  return 0;
}

static void __exit procmon_module_exit(void) {
  // Do any necessary cleanup here
}

module_param(pid, int, 0644);
module_init(procmon_module_init);
module_exit(procmon_module_exit);

MODULE_LICENSE("GPL");
