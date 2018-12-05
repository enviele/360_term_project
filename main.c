#include "util.c"


char line[128], command[128], pathname[256];
char *commands[] = {"ls", "pwd", "cd", "mkdir", "rmdir", "creat", "link", "symlink", "unlink", "chmod", "menu"};



int menu()
{
    printf("*********************************\n");
    printf("************** MENU *************\n");
    printf("** ls pwd cd mkdir rmdir creat **\n");
    printf("*** link syslink unlink chmod ***\n");
    printf("*********************************\n");

    return 0;
}

int findCmd(command) {
    // cycle through commands stored in the commands[] array and return it's index
    for (int i = 0; i < 10; i++) {
        if (strcmp(command, commands[i]) == 0) {
            return i;
        }
    }
    return -1; // return -1 to run the default option
}


int main(int argc, char *argv[])
{
    int index, quitting = 0;

    //initialize();

    while(!quitting) {
        printf("input a command:\t");
        fgets(line, 128, stdin);
        line[strlen(line)-1] = NULL; // get rid of NULL at the end
        sscanf(line, "%s %s", command, pathname);
        index = findCmd(command);
        if (index != -1) {
            quitting = fptr[index](pathname);
        } else {
            printf("invalid command %s\n", command);
        }
    }
    
    return 0;
}