
#include "tokeniser.h"
#include "parser.h"
#include "libchibi.h"
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char* argv[]) {
    int flagC = 0;
    bool compile_only = false;
    bool assemble_only = false;
    if (argc == 4 && strcmp(argv[1], "-c") == 0) {
        flagC += 1;
        compile_only = true;
    } else if (argc == 4 && strcmp(argv[1], "-a") == 0) {
        flagC += 1;
        assemble_only = true;
    } else if (argc != 3) {
        fprintf(stderr, "Error, incorrect arguments\n");
        fprintf(stderr, "Usage: ChibiLisp <source file> <executable name>\n");
        return 1;
    }
    char* filepath = argv[1+flagC];
    char* executable_name = argv[2+flagC];

    //open the file
    FILE* source_file = fopen(filepath, "r");
    if (!source_file) {
        fprintf(stderr, "Error: failed to read source file\n");
        return 1;
    }
    //get the length of the input file
    fseek(source_file, 0, SEEK_END);
    long size = ftell(source_file);
    rewind(source_file);

    //allocate the memory for our source string
    char* source_string = calloc(1, size+1);
    fread(source_string, sizeof(char), size, source_file);

    //close the file
    fclose(source_file);

    ObjectList program = parse_program(tokenise(source_string));

    resection_objects(program);
    DebugFile df = {.file_name = filepath, .file_num = 0};
    register_debug_files(&df, 1);

    if (compile_only) {
        cProg(program, executable_name);
        return 0;
    }
    if (assemble_only) {
        aProg(program, executable_name);
        return 0;
    }

    char object_template[] = "/tmp/chibilisp-XXXXXX.o";
    int fd = mkstemps(object_template, 2);
    if (fd < 0) {
        fprintf(stderr, "Error: failed to make temporary object file\n");
        return 1;
    }
    close(fd);

    aProg(program, object_template);

    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "Error: failed to fork process\n");
        return 1;
    }
    if (pid == 0) {
        //Child process: exec gcc
        execlp("gcc", "gcc", "-o", executable_name, object_template, NULL);
        //if exec fails:
        fprintf(stderr, "Error: failed to execute gcc");
        _exit(1);
    }
    //Parent process
    //wait for gcc to end
    int status;
    waitpid(pid, &status, 0);
    //check for error with gcc
    if (!WIFEXITED(status) || WEXITSTATUS(status) != 0) {
        fprintf(stderr, "Error: gcc failed\n");
        return 1;
    }

    //clean up the temporary object file
    unlink(object_template);
}

#define TOKENISER_C
#include "tokeniser.h"
#define PARSER_C
#include "parser.h"
#define HELPERS_C
#include "helpers.h"

