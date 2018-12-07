// #include "type.h"
int my_touch(char *pathname)
{
    int ino;
    int newmode = 0;
    MINODE *mip = running->cwd;
    MINODE *t_mip = NULL;
    INODE *ip = NULL;
    char path[128];
    strcpy(path, pathname);

    if (!strcmp(pathname, "")) {
        printf("no filename given\n");
        return 1;
    }
    printf("path: %s\n", pathname);
    ino = getino(running->cwd, pathname);
    if (ino) { // the file exists. touch it
        printf("Target exists: touching %s\n", pathname);
        t_mip = iget(dev, ino);
        ip = &t_mip->INODE;

        ip->i_mtime = time(0L); // update mtime
        t_mip->dirty = 1;

        iput(t_mip);
        return;
    } else {
        // target doesn't exist. just call creat to make it
        printf("targer doesn't exist. Creating new file...\n");
        my_creat(path);
    }
    return;
}