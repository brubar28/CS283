#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}

@test "Test simple command: echo" {
    run ./dsh <<EOF
echo "Hello, world"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Hello, world"* ]]
}

@test "Test built-in command: exit" {
    run ./dsh <<EOF
exit
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"exiting..."* ]]
}

@test "Test built-in command: cd" {
    run ./dsh <<EOF
cd /tmp
pwd
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"/tmp"* ]]
}

@test "Test piped commands: ls | grep .c" {
    run ./dsh <<EOF
ls | grep ".c"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *".c"* ]]
}

@test "Test multiple pipes: ls | grep .c | wc -l" {
    run ./dsh <<EOF
ls | grep ".c" | wc -l
EOF
    [ "$status" -eq 0 ]
}

@test "Test redirection: output to file" {
    run ./dsh <<EOF
echo "test output" > out.txt
cat out.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"test output"* ]]
}

@test "Test redirection: append to file" {
    run ./dsh <<EOF
echo "line 1" > out.txt
echo "line 2" >> out.txt
cat out.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"line 1"* ]]
    [[ "$output" == *"line 2"* ]]
}

@test "Test invalid command handling" {
    run ./dsh <<EOF
invalid_command
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"execvp failed: No such file or directory"* ]]
}

@test "Test multiple commands execution" {
    run ./dsh <<EOF
echo "first"
echo "second"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"first"* ]]
    [[ "$output" == *"second"* ]]
}

@test "Test handling of extra spaces in commands" {
    run ./dsh <<EOF
   echo     "extra   spaces  test"   
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"extra   spaces  test"* ]]
}

@test "Test redirecting input and output at the same time" {
    echo "input content" > input.txt
    run ./dsh <<EOF
cat < input.txt > output.txt
cat output.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"input content"* ]]
}

@test "Test overwriting a file with > redirection" {
    echo "old content" > testfile.txt
    run ./dsh <<EOF
echo "new content" > testfile.txt
cat testfile.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"new content"* ]]
    [[ "$output" != *"old content"* ]]
}

@test "Test redirection with append mode >>" {
    echo "first line" > append_test.txt
    run ./dsh <<EOF
echo "second line" >> append_test.txt
cat append_test.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"first line"* ]]
    [[ "$output" == *"second line"* ]]
}

@test "Test rc (last exit status)" {
    run ./dsh <<EOF
ls
rc
EOF
    [ "$status" -eq 0 ]
}

@test "Test cd with invalid directory" {
    run ./dsh <<EOF
cd /does/not/exist
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"cd failed: No such file or directory"* ]]
}

@test "Test command execution with a non-existent file as input" {
    run ./dsh <<EOF
cat < non_existent.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"open: No such file or directory"* ]]
}

@test "Test using pipes with redirection" {
    echo "alpha" > temp.txt
    echo "beta" >> temp.txt
    echo "gamma" >> temp.txt
    run ./dsh <<EOF
cat temp.txt | grep "beta" > filtered.txt
cat filtered.txt
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"beta"* ]]
    [[ "$output" != *"alpha"* ]]
    [[ "$output" != *"gamma"* ]]
}

@test "Test using echo with a long argument string" {
    run ./dsh <<EOF
echo "Very long sentence that is very long and is actually very long super long nice string"
EOF
    [ "$status" -eq 0 ]
    [[ "$output" == *"Very long sentence that is very long and is actually very long super long nice string"* ]]
}
