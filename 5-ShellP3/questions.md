1. Your shell forks multiple child processes when executing piped commands. How does your implementation ensure that all child processes complete before the shell continues accepting user input? What would happen if you forgot to call waitpid() on all child processes?

_My implementation ensures that all child processes complete before the shell continues byt using the wait function to wait for the processes. I used I loop to do this by waiting and checking the status for each process, iterating over each command in the pipeline. Wait will block the parent process until all child process exit. If you forgot to call wait it could lead to several issues such as child processes completing execution but never being exited and wasting resources._

2. The dup2() function is used to redirect input and output file descriptors. Explain why it is necessary to close unused pipe ends after calling dup2(). What could go wrong if you leave pipes open?

_It is necessary to close unused pipe ends after calling dup2() as this is crucial to managing resources, in which if unused pipe ends are not closed the system could run out of file descriptors. This could also affect pipiline behavior in which if some pipe ends are not closed, child processes might not terminate correctly._

3. Your shell recognizes built-in commands (cd, exit, dragon). Unlike external commands, built-in commands do not require execvp(). Why is cd implemented as a built-in rather than an external command? What challenges would arise if cd were implemented as an external process?

_Cd is implemented as a built-in command becuase if it were external it would ened up changing the directory for the child processes executing the command and not the parent shell process. After executing this command, the shell would still be in the same directory. If cd were implemented as an external process you would have to implement a workaround so it actually does what is intended, making it communicate from the child process to the parent shell the new directory._

4. Currently, your shell supports a fixed number of piped commands (CMD_MAX). How would you modify your implementation to allow an arbitrary number of piped commands while still handling memory allocation efficiently? What trade-offs would you need to consider?

_In order to allow an arbitrary number of piped commands in the shell we would need to modify the current implementation to dynanmically allocate memory for any future commands, specifically in the command list and pipes. Instead of used fixed arrays as we currently are we would introduce realloc to resize the array to accommodate the increase in the number of commands. The trade-offs for this may include an excess of memory being reallocated as you might allocate more memory than needed when trying to avoid reallocating multiple times. if you were to reallocate many times this would impact performance especially if we have a very large number of commands._
