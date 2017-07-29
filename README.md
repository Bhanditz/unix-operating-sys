# Mini Shell

write a simple shell program called minish. You start the shell by typing minish at the prompt. This will give a prompt of your shell as follows:
minish>

From here onwards, you should be able to execute and control any program/command just as you would in a normal shell. For instance
minish> ls
[ OUTPUT OF ls COMMAND SHOWN HERE ]

Additionally, your shell should be able to do the following:

1. Execute commands with multiple arguments. For example:
                      minish> Command arg1 arg2 arg3

2. Execute commands in either foreground or background mode. In foreground mode, the shell just waits for the command to complete before displaying the shell prompt again (as in the above example). In background mode, a command is executed with an ampersand & suffix. The shell prompt appears immediately after typing a command name (say Command1) and shell becomes ready to accept and execute the next command (say Command2), even as Command1 continues executing in the background. For example:
                      minish> Command1 &
                      minish> Command2
                      [OUTPUT OF Command1 AND Command2 MAY INTERLEAVE HERE IN ARBITRARY ORDER]

3. Maintain multiple processes running in background mode simultaneously. For example:
                      minish> Command1 &
                      minish> Command2 &
                      minish> Command3 &
                      minish>
                      [OUTPUT OF Command1, Command2, AND Command3 MAY INTERLEAVE HERE IN ARBITRARY ORDER]
    
4. Redirect the input of a command from a file. For example:
                  minish> Command < input_file
    Redirect the output of a command to a file. For example:
                  minish> Command > output_file

5. Implement command filters, i.e., redirect the stdout of one command to stdin of another using pipes. For example:
                  minish> ls -l | wc -l
                  minish> cat somefile | grep somestring | less
    Ideally, your shell should be able to handle any number of filter components.

6. Terminate a process by pressing [Ctrl-C]. Your shell must not get killed; only the process
running inside your shell must terminate.

7. Temporarily stop a process using [Ctrl-Z]. Your shell must not stop; only the process
running inside your shell must stop. Your shell must be able to hold multiple processes in
stopped state.

8. Kill a stopped process or a process in background using the kill command.

9. Any signal should be delivered to the entire process-group of the immediate child process, not
just to the immediate child process. For example, if mysh starts process A and process A forks another process B, then any signal should be delivered to both process A and process B. How will you do this? For this you need to understand the notion of a process group. Check out the man pages for setsid() and killpg().

10. Bring a backgrounded process to foreground using the fg directive. The fg directive must be able to bring any stopped/backgrounded process to foreground using the %N argument.

11. Be able to execute any feasible combinations of the above features.

12. The exit command should terminate your shell. Take care to avoid orphan processes.

13. The cd command must actually change the directory of your shell and the pwd command must
return the current directory of your shell. Note that normal fork-exec mechanism won't work here.

# System Calls

You will deonstrate how to implement a simple system call, how to write a user-level application to test your implementation, and how to crash your kernel.

Part A: Kernel Compilation.

Part B: Writing your own System Call

1. Implement and test a system call that returns the current system time by means of a call-by- reference argument. More specifically, your system call should have the following prototype as seen by user-level applications.
      int my_xtime(struct timespec *current_time);
struct timespec is defined in include/linux/time.h as follows:
      struct timespec {
              time_t  tv_sec;         /* seconds */
              long    tv_nsec;        /* nanoseconds */
};
The system call should first verify that user memory space, pointed to by current_time, is valid and writable. If not, is should return a failure (EFAULT) to the user process. If the memory- check succeeds, then it should copy the contents of the kernel's xtime variable to user space address (pointed to by current_time argument). The system call should also print the current time in nanoseconds, to the console using the printk() function.

2. Also write a user-level application which calls your new system call my_xtime(...) and prints out the time obtained from the call-by-reference argument.

Part C: Experimenting With "Bad" Code

1. Now, change implementation of your system call so that it does something "bad" to crash/corrupt/fail your kernel. Compile it. If it compiles OK, test it. But before testing it, use "sync" to synchronize the file system in-memory structures with those on disk, since it is likely that the system will crash, and you will need to reboot.
The following are some typical "bad things" you can do. Try them one at a time. Some of these
  
may be caught at compilation or link time, some may only cause problems when you make the system call.
• division by zero
• dereferencinganullpointer
• returning no value or an incorrect value from the function that implements your system call
• Using a C library routine (e.g., malloc() or printf())inside the function that implements your system call.
• whatever else that you think might cause a problem

Repeat a few such experiments, and see if you can crash entire system.
IMPORTANT; Save copies of the "bad" code that you tested, by giving them different names. When the system crashes, you may lose your code.

BEWARE that some of the things you do may have no immediate visible effect, but may have a delayed effect that is disastrous. This is generally the case if you write garbage into random locations of kernel memory. The location you corrupt may not be referenced for a while. It will be referenced later, and then the effect will occur. Therefore, you cannot assume that the thing you did most recently is necessarily the cause of a crash.

Do not go overboard on this part of the assignment. The objective is to expose you to the various ways a kernel failure can manifest itself, in preparation for testing your own code. The objective is not to do the most damage possible to your system. In particular, it would better if you you do not trash your hard drive, since reinstalling the entire system can take a frustratingly long time.

# Character-device

Implement a kernel-level pipe for exchanging strings among user-level processes. You will learn about concurrency, synchronization, and various kernel primitives.

This is a classical producer-consumer problem. Implement a miscellaneous character device in the Linux Kernel. This device must maintain a FIFO queue (i.e. a pipe) of maximum N strings (configured as a module parameter).

1. In user space, there are several concurrent processes that are consumers and producers of strings.

2. Producers produce strings and write them to a character device (say /dev/mypipe).

3. Consumers read the strings from the character device and print it on the screen.

4. When the pipe is full, i.e. when there are N strings stored in character device, then any producer
trying to write will block.

5. When the pipe is empty, i.e. when there are 0 strings stored in character device, then any
consumer trying to read will block.

6. When a consumer reads from a (full) pipe, it wakes up all blocked producers. In this case, no
blocked consumer should be woken up.

7. When a producer writes to a (empty) pipe, it wakes up all blocked consumers. In this case, no
blocked producer should be woken up.

Notes
You might have noticed that this is not really a "character device" by strict definition, because it doesn't provide a byte-stream abstractions. Instead user processes are writing and reading whole
strings at a time. We are just using the character device interface in Linux as a convenient mechanism to implement a pipe of strings.

The reason "full" and "empty" are in brackets in the last two steps above is to let you choose whether to notify upon every single read/write or only when really necessary. Whatever option you choose, make sure no notifications are "lost" by blocked producers/consumers.

# Tracking Page Faults for a Process

Use kernel-level probing mechanisms to intercept and track kernel events.

Kprobes enables you to dynamically break into any kernel routine and collect debugging and performance information non-disruptively. You can trap at almost any kernel code address(*), specifying a handler routine to be invoked when the breakpoint is hit.
