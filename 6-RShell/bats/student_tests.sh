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

@test "Start server and check it is listening" {
    ./dsh -s -p 5678 &
    sleep 2
    run netstat -tulnp | grep 5678
    [ "$status" -eq 0 ]
}

@test "Start client and check connection" {
    ./dsh -s -p 5678 &
    sleep 2
    run ./dsh -c -p 5678 <<< "exit"
    [ "$status" -eq 0 ]
}

@test "Execute remote command ls" {
    ./dsh -s -p 5678 &
    sleep 2
    run ./dsh -c -p 5678 <<< "ls"
    [ "$status" -eq 0 ]
    [[ "$output" =~ "rsh_server.c" ]]
}

@test "Execute remote command pwd" {
    ./dsh -s -p 5678 &
    sleep 2
    run ./dsh -c -p 5678 <<< "pwd"
    [ "$status" -eq 0 ]
}

@test "Change directory remotely" {
    ./dsh -s -p 5678 &
    sleep 2
    run ./dsh -c -p 5678 <<< "cd /tmp; pwd"
    [ "$status" -eq 0 ]
    [[ "$output" =~ "/tmp" ]]
}

@test "Stop the server remotely" {
    ./dsh -s -p 5678 &
    sleep 2
    run ./dsh -c -p 5678 <<< "stop-server"
    [ "$status" -eq 0 ]
}

@test "Execute a command with pipes" {
    ./dsh -s -p 5678 &
    sleep 2
    run ./dsh -c -p 5678 <<< "ls | grep .c"
    [ "$status" -eq 0 ]
    [[ "$output" =~ "rsh_server.c" ]]
}

@test "Execute a command with redirection" {
    ./dsh -s -p 5678 &
    sleep 2
    run ./dsh -c -p 5678 <<< "echo 'Hello World' > testfile.txt; cat testfile.txt"
    [ "$status" -eq 0 ]
    [[ "$output" =~ "Hello World" ]]
}

@test "Handle an invalid command" {
    ./dsh -s -p 5678 &
    sleep 2
    run ./dsh -c -p 5678 <<< "invalidcommand"
    [ "$status" -ne 0 ]
}
