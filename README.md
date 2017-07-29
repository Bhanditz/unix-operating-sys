# unix-operating-sys
1. Mini Shell
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

10.Bring a backgrounded process to foreground using the fg directive. The fg directive must be able to bring any stopped/backgrounded process to foreground using the %N argument.

11.Be able to execute any feasible combinations of the above features.

12.The exit command should terminate your shell. Take care to avoid orphan processes.

13.The cd command must actually change the directory of your shell and the pwd command must
return the current directory of your shell. Note that normal fork-exec mechanism won't work here.
