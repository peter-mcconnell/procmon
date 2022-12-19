#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <linux/mm_types.h>

#include "procmon.h"

static int pid = -1;

void print_memory_for_pid(int pid) {
  struct task_struct *task;

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
        printk(KERN_INFO "procmon: Memory region: %lx - %lx\n", start, end);
        printk(KERN_INFO "procmon: Memory protection flags: %lx\n", vma->vm_flags);
        if (vma->vm_file) {
          printk(KERN_INFO "procmon: Backed by file: %s\n", vma->vm_file->f_path.dentry->d_name.name);
        }
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

MODULE_LICENSE("MIT");
