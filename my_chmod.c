//#include "type.h"

int my_chmod(char *mode, char *pathname) {
    //var def
    int ino, new_mode;
    MINODE *mip = running->cwd;
    INODE *inode;

    printf("in my_chmod\n");
    
    printf("mode: %s\n", mode);
    printf("pathname: %s\n", pathname);
    //check that a mode is specified
    if (!strcmp(mode, "")) {
        printf("error: no mode specified.\n");
        return 1;
    }

    //we have a mode, so convert it from string to
    //octal int for easy readability
    new_mode = (int) strtol(mode, (char **)NULL, 8);

    //start by getting the ino
    ino = getino(mip, pathname);

    //make sure that the file exists from pathname
    if (ino == 0) {
        printf("error: pathname doesn't exist\n");
        return 1;
    }

    //get the MINODE at the ino
    mip = iget(dev, ino);

    //set inode to the mip inode
    inode = &mip->INODE;
    
    //set this inode's mode as the new altered mode
    inode->i_mode |= new_mode;

    //done with mip, so set it to dirty and put it back
    mip->dirty = 1;
    iput(mip);

    return 0;
}