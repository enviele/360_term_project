//#include "type.h"

int my_stat(char * pathname) {
    //var def
    struct stat myst;
    int ino;
    MINODE *mip = running->cwd;
    INODE *inode;
    char *a_time, *m_time, *c_time;

    printf("in my_stat\n");

    //get INODE of filename into memory
    //start by getting the ino from the INODE in path
    ino = getino(mip, pathname);
    //if the ino is 0, the file doesn't exist
    if (ino == 0) {
        printf("error: pathname does not exist\n");
        return 1;
    }

    //now set the local MINODE to the MINODE with the ino number
    //from the path
    mip = iget(dev, ino);   

    //set dev and ino to the dev and ino of stat
    myst.st_dev = dev;

    //set a var to the INode at mip
    inode = &mip->INODE;
    //now we need to copy all the INODE fields
    //from mip to the stat
    myst.st_mode = inode->i_mode;
    myst.st_uid = inode->i_uid;
    myst.st_size = inode->i_size;
    myst.st_atime = inode->i_atime;
    myst.st_ctime = inode->i_ctime;
    myst.st_mtime = inode->i_mtime;
    myst.st_gid = inode->i_gid;
    myst.st_blocks = inode->i_blocks;

    //now that myst is set, print out all of the needed fields
    printf("File: '%s'\n", basename(pathname));
    printf("Size: %d    Blocks: %d \n", myst.st_size, myst.st_blocks);

    //to print out if reg or dir, check
    if (S_ISREG(myst.st_mode)) {
        printf("regular file\n");
    }
    else {
        printf("directory file\n");
    }

    // int my_dev = myst.st_dev, my_ino = myst.st_ino, my_lcount = inode->i_links_count;
    printf("Device: %d\t",  myst.st_dev);
    printf("Ino:\t%d\t", ino);
    printf("Links:\t%d\n", inode->i_links_count);


    a_time = ctime((time_t*)&inode->i_atime);
    m_time = ctime((time_t*)&inode->i_mtime);
    c_time = ctime((time_t*)&inode->i_ctime);

    printf("Access: %s\n", a_time);
    printf("Modify: %s\n", m_time);
    printf("Change: %s\n", c_time);

    //set that the mip is dirty
    mip->dirty = 1;
    
    //put back mip
    iput(mip);

    return 0;
}