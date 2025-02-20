1. Can you think of why we use `fork/execvp` instead of just calling `execvp` directly? What value do you think the `fork` provides?

    > **Answer**:  Using fork/execvp allows the shell to make a new process for executing commands while also keeping the shell running. If execvp was called without fork the shell would be replaced by any new commands, resulting in us not returning back to the shell.

2. What happens if the fork() system call fails? How does your implementation handle this scenario?

    > **Answer**:  If the fork() system call fails, it will return -1 which indicates that no child process was created. In my implementation the retunr value of fork() is checked and depedning on its value an error message is given.

3. How does execvp() find the command to execute? What system environment variable plays a role in this process?

    > **Answer**:  Execvp() finds the command in directories given by the PATH variable, and if this command isn't absolute or relative it will iterate through the directories in PATH to find a file matching the command name.

4. What is the purpose of calling wait() in the parent process after forking? What would happen if we didn’t call it?

    > **Answer**:  The purpose of calling wait() is to allow the shell to wait until any child process finish. If we didn't call wait() the shell could go on ahed without waiting for any child processes which could potentially leve them runnign forever.

5. In the referenced demo code we used WEXITSTATUS(). What information does this provide, and why is it important?

    > **Answer**:  WEXITSTATUS() provies us with the exit code of any terminated process by taking whatever value wait() returns. This is important as it gives us valuable information for error handling and built in commands like rc which we need to display exit status.

6. Describe how your implementation of build_cmd_buff() handles quoted arguments. Why is this necessary?

    > **Answer**:  My implementation of build_cmd_buff() handles quoted arguments by treating that as one piece, detecting single or double quotes and preserving whatever is in between so as to properly parse multiple word arguments as well as maintain any white space.

7. What changes did you make to your parsing logic compared to the previous assignment? Were there any unexpected challenges in refactoring your old code?

    > **Answer**:  Compared to my previous assignment, I needed to provide a new method of parsing inputs as my old method removed much white space which I noticed in this assignment when trying to print out these spaces when using echo. It had to be restructured so quotations would be noticed as a point to stop removing spaces.

8. For this quesiton, you need to do some research on Linux signals. You can use [this google search](https://www.google.com/search?q=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&oq=Linux+signals+overview+site%3Aman7.org+OR+site%3Alinux.die.net+OR+site%3Atldp.org&gs_lcrp=EgZjaHJvbWUyBggAEEUYOdIBBzc2MGowajeoAgCwAgA&sourceid=chrome&ie=UTF-8) to get started.

- What is the purpose of signals in a Linux system, and how do they differ from other forms of interprocess communication (IPC)?

    > **Answer**:  Signals in a Linux system are asynchronous notifications sent to processes so to indicate any events. Signals differ from other forms of IPC in that they are light and don't require any direct channels between the processes.

- Find and describe three commonly used signals (e.g., SIGKILL, SIGTERM, SIGINT). What are their typical use cases?

    > **Answer**:  SIGKILL will terminate a process in way that it cannot be caught or ignored, forcefully stopping it. SIGTERM is a more gentle form of SIGKILL in which it will allow a process to close properly, cleaning up resources before exit. SIGINT is the signal that is sent when pressing Ctrl C in the terminal, being the method to interrupt any running process.

- What happens when a process receives SIGSTOP? Can it be caught or ignored like SIGINT? Why or why not?

    > **Answer**:  When a process receives SIGSTOP it is paused immediately. Similar to SIGKILL this cannot be caught or ingored and will be carried out. This is done in the kernel to ensure that the process can be stopped.
