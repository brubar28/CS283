1. In this assignment I suggested you use `fgets()` to get user input in the main while loop. Why is `fgets()` a good choice for this application?

    > **Answer**:  Using fgets() is a good choice for this application as it reads the whole input including any spaces and also allows you to provide a buffer size. This helps to avoid buffer overflows as well as handle EOF and newline characters.

2. You needed to use `malloc()` to allocte memory for `cmd_buff` in `dsh_cli.c`. Can you explain why you needed to do that, instead of allocating a fixed-size array?

    > **Answer**: Malloc allows for dynamic memory allcoations and since the size the input cannot be determine this allows for flexibility. A fixed array would not be good in this case and would likely result in inputs being cut off.


3. In `dshlib.c`, the function `build_cmd_list(`)` must trim leading and trailing spaces from each command before storing it. Why is this necessary? If we didn't trim spaces, what kind of issues might arise when executing commands in our shell?

    > **Answer**:  Not trimming these spaces could lead to commands being incorrectly parsed or just improper execution. For example, spaces in the beginning or end could lead to incorrect lookups or improper interpretations by programs.

4. For this question you need to do some research on STDIN, STDOUT, and STDERR in Linux. We've learned this week that shells are "robust brokers of input and output". Google _"linux shell stdin stdout stderr explained"_ to get started.

- One topic you should have found information on is "redirection". Please provide at least 3 redirection examples that we should implement in our custom shell, and explain what challenges we might have implementing them.

    > **Answer**:  Input redirection - Providing the input from a file. Output redirection - Ridirecting the command output to a file.  Error Redirection - Redirecting error messages to a file. Challenges that would come out of doing this would be trying to correctly parse redirection characters as well as handling multiple redirections at the same time for both outputs and erros.

- You should have also learned about "pipes". Redirection and piping both involve controlling input and output in the shell, but they serve different purposes. Explain the key differences between redirection and piping.

    > **Answer**:  Redirection transfers inputs or outputs to and from files. Piping connects the output of a command to the input of another command without using any storage, providing flexibilty and a way of chaining commands. Redirection usually consists of managing files while piping is not used here.

- STDERR is often used for error messages, while STDOUT is for regular output. Why is it important to keep these separate in a shell?

    > **Answer**:  This enables you to distinguish between the two. It makes it much easier to debug as well as handle errors without confusing them for regular outputs.

- How should our custom shell handle errors from commands that fail? Consider cases where a command outputs both STDOUT and STDERR. Should we provide a way to merge them, and if so, how?

    > **Answer**:  These messages should be displayed separately to avoid any confusion. However, by merging the two you ensure that both of these are received and you don't miss any errors. There might also be cases where we want to write both to a file so merging them would make this easier. Using the redirection command 2>&1, we can merge the error output and the regular output into a single stream.
