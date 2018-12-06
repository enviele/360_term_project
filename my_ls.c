int ls_dir() {

}

int ls_file(int ino) {

}

int my_ls(char *pathname) {
    //vars for the function
    struct stat path_stat; 
    DIR * dir;


    printf("in my_ls\n");
    
    //start by analyzing path name
    //if nothing is specified, then it's for cwd
    if (!strcmp(pathname, "")) {
        ls_dir();
    }
    //if only / is written, then it's for cwd
    //now checking the written path name


    return 0;
}
