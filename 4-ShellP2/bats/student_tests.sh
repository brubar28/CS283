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

@test "Check exit command terminates shell" {
    run ./dsh <<EOF
exit
EOF
    
    [ "$status" -eq 0 ]
}

@test "Check cd command changes directory" {
    run ./dsh <<EOF
cd /tmp
pwd
EOF
    
    [[ "$output" == "/tmp"* ]]
}

@test "Check cd with too many arguments" {
    run ./dsh <<EOF
cd /tmp /var
EOF
    
    [[ "$output" == *"cd: too many arguments"* ]]
}

@test "Check invalid command handling" {
    run ./dsh <<EOF
invalid_command
EOF
    
    [[ "$output" == *"execvp failed"* ]]
}

@test "Check empty command does nothing" {
    run ./dsh <<EOF

EOF
    
    [ "$status" -eq 0 ]
}

@test "Check built-in rc command returns last exit status" {
    run ./dsh <<EOF
ls
rc
EOF
    
    [[ "$output" == *"0"* ]]
}

@test "Check multiple commands execution" {
    run ./dsh <<EOF
echo hello
echo world
EOF
    
    [[ "$output" == *"hello"* ]]
    [[ "$output" == *"world"* ]]
}

@test "Check command with arguments" {
    run ./dsh <<EOF
echo test argument
EOF
    
    [[ "$output" == *"test argument"* ]]
}


@test "Check piped commands execution" {
    run ./dsh <<EOF
echo hello | grep h
EOF
    
    [[ "$output" == *"hello"* ]]
}

@test "Check sequential commands execution" {
    run ./dsh <<EOF
echo first; echo second
EOF
    
    [[ "$output" == *"first"* ]]
    [[ "$output" == *"second"* ]]
}
