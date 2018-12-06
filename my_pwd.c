//#include "type.h"

int rpwd(MINODE *wd) {
    //var def
    char buf[BLKSIZE], my_name[128];
    INODE *inode = &wd->INODE;
    MINODE  *pip;
    int parent_ino, my_ino;

    //base case: if we have reach root
    if (wd == root) {
        //in this case, just return
        return;
    }
    //next we get the parent ino and the current ino based off of wd
    findino(wd, &my_ino, &parent_ino);
    
    //set wd to the MINODE from parent_ino
    wd = iget(dev, parent_ino);

    //find the name of the current ino and put it in my_name
    findmyname(wd, my_ino, my_name);

    //recursively call again for the parent, set as pip
    rpwd(wd);

    //wait until the recursive function call is done, and print
    printf("/%s", my_name);
}

int pwd(MINODE *wd) {
    
    //base case: recursively go back until hitting root
    if (wd == root) {
        //since we're in the root, print / for root
        printf("/");
    }
    else {
        //recursively call rpwd
        rpwd(wd);
    }
}

int my_pwd(char *pathname) {
    printf("in my_pwd\n");
    
    //this starts with the cwd, so give that as
    //param to pwd
    pwd(running->cwd);
    printf("\n");

    return 0;
}