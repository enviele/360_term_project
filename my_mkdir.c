// #include "type.h"

int my_mkdir(char *pathname) {
    printf("in my_mkdir\n");
    int i, ino;
    MINODE *pmip;
    INODE *pip;

    char buf[1024], tmp1[1024],tmp2[1024], parent[1024], child[1024];

    // copyy pathname so we don't destroy it
    strcpy(tmp1, pathname);
    strcpy(tmp2, pathname);

    // get child and parent names
    strcpy(parent, dirname(tmp1));
    strcpy(child, basename(tmp2));
    printf("parent:\t%s\nchild:\t%s\n", parent, child);

    // get parent's ino
    ino = getino(running->cwd, parent);
    printf("ino is %d\n", ino);
    pmip = iget(dev, ino);
    pip = &pmip->INODE;

    // check to see if parent exists
    if(!pmip) {
        printf("ERROR:\t parent does not exist\n");
        return 1;
    } if (pmip == root) {
        printf("in root\n");
    }
    // check if is dir
    if(!S_ISDIR(pip->i_mode)) {
        printf("ERROR:\tparent is not a directory\n");
        return 1;
    }
    // check if dir already exists
    if(getino(running->cwd, pathname)) {
        printf("ERROR:\t%s already exists\n", pathname);
        return 1;
    }

    // prerequirements have been satisfied
    // run my_mkdir
    my_mkdir_fun(pmip, child);

    //increment the parents link count and adjust the time
    pip->i_links_count++;
    pip->i_atime = time(0L);
    pmip->dirty = 1;
    // set dirty to true and iput

    iput(pmip);

    return 0;
}

// puts name in parent dir
int set_name(MINODE *mip, int myino, char *myname) {
    int i, required_len=0, ideal_len=0, left=0, bno=0, block_size = 1024;
    INODE *parent = &mip->INODE;
    char buf[1024], *cp;
    DIR *dir;

    for (i = 0; parent->i_size / BLKSIZE; i++) {
        if (!parent->i_block[i])  break;
        bno = parent->i_block[i];
        get_block(dev, bno, buf);

        dir = (DIR *)buf;
        cp = buf;

        // get length
        required_len = 4 * ((8+strlen(myname) + 3) / 4);
        printf("len is %d\n", required_len);

        // step into last dir entry
        while (cp + dir->rec_len < buf + BLKSIZE) {
            cp += dir->rec_len;
            dir = (DIR*)cp;
        }

        printf("last entry is %s\n", dir->name);
        cp = (char*)dir;

        // ideal length uses name len of last dir entry
        ideal_len = 4 * ((8+dir->name_len + 3) / 4);

        // what's left
        left = dir->rec_len - ideal_len;
        printf("what's left is %d\n", left);

        if(left >= required_len) {
            // set rec_len to ideal
            dir->rec_len = ideal_len;
            cp += dir->rec_len;
            dir = (DIR*)cp;

            // sets the dirpointer inode to the given myino
            dir->inode = myino;
            dir->rec_len = block_size - ((u32)cp - (u32)buf);
            printf("rec len is %d\n", dir->rec_len);
            dir->name_len = strlen(myname);
            dir->file_type = EXT2_FT_DIR;
            // sets the np name to the given name
            strcpy(dir->name, myname);

            // puts the block back
            put_block(dev, bno, buf);

            return 1;
        }

    }

    printf("number is %d\n", i);

    // if there is no space in the existing data blocks, we need to allocate space in the next block
    bno = ballco(dev);
    parent->i_block[i] = bno;
    parent->i_size += BLKSIZE;
    mip->dirty = 1;

    get_block(dev, bno, buf);

    dir = (DIR*)buf;
    cp = buf;

    printf("dir name is %s\n", dir->name);

    dir->inode = myino;
    dir->rec_len = 1024;
    dir->name_len = strlen(myname);
    dir->file_type = EXT2_FT_DIR;
    strcpy(dir->name, myname);

    // put block back
    put_block(dev, bno, buf);

    return 1;
}

void my_mkdir_fun(MINODE *pmip, char *child) {
    int ino = ialloc(dev);
    int bno = balloc(dev);
    int i;

    printf("device is %d\n", dev);
    printf("ino is %d\nbno is %d\n", ino, bno);

    MINODE *mip = iget(dev, ino);
    INODE *ip = &mip->INODE;
    char *cp, buf[1024];

    DIR *dir;

    ip->i_mode = 0x41ED; // DIR type and permissions
    printf("mode is %d\n", ip->i_mode);
    ip->i_uid = running->uid; // owner user id
    printf("user id is %d\n", ip->i_uid);
    ip->i_gid = running->gid; //group id
    printf("group id is %d\n", ip->i_gid);

    // we set the size to blksize too because that's the size of a directory
    ip->i_size = BLKSIZE; //size in bytes
    ip->i_links_count = 2; // link count=2 because of . and ..
    ip->i_atime = time(0L); // set access time to current time
    ip->i_ctime = time(0L); // set creation time ""         ""
    ip->i_mtime = time(0L); // set modify time ""           ""

    // now for . and ..
    ip->i_blocks = 2; // LINUX: blocks count in 512-byte chunks 
    ip->i_block[0] = bno;

    // set the rest of the blocks to 0
    for (i = i; i < 15; i++) {
        ip->i_block[i] = 0;
    }
    mip->dirty = 1; // set dirty to true then iput
    iput(mip);

    // breate data block for new DIR containing . and ..
    get_block(dev, bno, buf);

    dir = (DIR*)buf;
    cp = buf;

    dir->inode = ino;
    dir->rec_len = 4 * (( 8 + 1 + 3) / 4);
    dir->name_len = strlen(".");
    dir->file_type = (u8)EXT2_FT_DIR;
    dir->name[0] = '.';

    cp += dir->rec_len;
    dir = (DIR *)cp;

    // for the parent ".."
    dir->inode = pmip->ino;
    dir->rec_len = 1012; // will alwas be 12 in this case
    printf("rec_len is %d\n", dir->rec_len);
    dir->name_len = strlen("..");
    dir->file_type = (u8)EXT2_FT_DIR;
    dir->name[0] = '.';
    dir->name[1] = '.'; // name is now ".."

    // write buf to disk block bno
    put_block(dev, bno, buf);

    // enter name entry into parent's directory
    set_name(pmip, ino, child);
    return 1;

}