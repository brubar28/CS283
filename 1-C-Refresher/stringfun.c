#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
//add additional prototypes here
int  count_words(char *, int, int);
void reverse_string(char *, int);
void print_words(char *, int);

int setup_buff(char *buff, char *user_str, int len){
    //TODO: #4:  Implement the setup buff as per the directions
    char *src = user_str;
    char *dest = buff;
    int space = 1;
    int count = 0;

    while (*src != '\0') {
        if (isspace(*src)) {
            if (!space && count < len) {
                *dest++ = ' ';
                count++;
                space = 1;
            }
        } else {
            if (count < len){
                *dest++ = *src;
                count++;
                space = 0;
            } else {
                return -1;
            }
        }
        src++;
    }

    while (count < len) {
        *dest++ = '.';
        count++;
    }

    return count;
}

int count_words(char *buff, int len, int str_len){
    char *ptr = buff;
    int count = 0;
    int isin = 0;

    //loop through string looking for space to count up words
    for (int i = 0; i < str_len; i++) {
        if (*ptr != ' ' && *ptr != '.') {
            if (!isin) {
                isin = 1;
                count++;
            }
        } 
        else {
            isin = 0;
        }
        ptr++;
    }

    return count;
}

//ADD OTHER HELPER FUNCTIONS HERE FOR OTHER REQUIRED PROGRAM OPTIONS
// Reverse the string in place
void reverse_string(char *buff, int str_len) {
    char *start = buff;
    char *end = buff + str_len - 1;

    //store beginning of string in temp so it can be replaced by end and repeat
    while (start < end) {
        char temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }
}

void print_words(char *buff, int str_len) {
    char *ptr = buff;
    int index = 1;
    int length = 0;

    printf("Word Print\n----------\n");
    for (int i = 0; i < str_len; i++) {
        if (*ptr != ' ' && *ptr != '.') {
            if(length == 0){
                printf("%d. ", index++);
            }
            putchar(*ptr);
            length++;
        } 
        else if (length > 0) {
            printf(" (%d)\n", length);
            length = 0;
        }
        ptr++; 
    }

    if(length > 0){
        printf(" (%d)\n", length);
    }
}

void usage(char *exename) {
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
    printf("Options:\n");
    printf("  -h    Prints help about the program\n");
    printf("  -c    Counts the number of words in the string\n");
    printf("  -r    Reverses the characters in the string\n");
    printf("  -w    Prints each word and its length in the string\n");
    printf("  -x    Search and replace a substring (extra credit)\n");
}

void print_buff(char *buff, int len) {
    printf("Buffer: [");
    for (int i = 0; i < len; i++) {
        putchar(*(buff + i));
    }
    printf("]\n");
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    //It is safe because it checks for a valid option and that there are enough arguements. Will terminate if there isnt enough
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag

    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    //Makes sure the string input is given as the second arguement.
    //The program must have a valid string or it will not perform, so it terminates instead.
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    // CODE GOES HERE FOR #3
    buff = (char *)malloc(BUFFER_SZ);
    if (!buff) {
        fprintf(stderr, "Failed to allocate memory\n");
        exit(99);
    }


    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);     //see todos
    if (user_str_len < 0){
        fprintf(stderr, "Buffer Error, error = %d\n", user_str_len);
        free(buff);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);  //you need to implement
            if (rc < 0){
                printf("Word Count Error, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        //TODO:  #5 Implement the other cases for 'r' and 'w' by extending
        //       the case statement options
        case 'r':
            reverse_string(buff, user_str_len);
            printf("Reversed String: ");
            for (int i = 0; i < user_str_len; i++) {
                putchar(buff[i]);
            }
            putchar('\n');
            break;

        case 'w':
            print_words(buff, user_str_len);
            break;

        default:
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    //TODO:  #6 Dont forget to free your buffer before exiting
    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(1);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//By providing both the pointer and length it provides for flexibility.
//It should not be assumed that the buffer has a fixed size as this could lead to overflows.
