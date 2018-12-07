int my_unlink(char *pathname) {
    printf("in my_unlink\n");
    MINODE *mip, *p_mip;
    int i, ino, p_ino;
    INODE *ip, *p_ip;
    char temp[64], dname[64], bname[64];

    if (!pathname) {
        printf("Eror: no pathname given\n");
        return 1;
    }
    // get ino and check that it exists
    ino = getino(running->cwd, pathname);
    if (!ino) {
        printf("Error: path doesn't exist\n");
        return 1;
    }
    // get minode and check to make sure that it is a file
    mip = iget(dev, ino);

    if (!mip) {
        printf("Error: can't find minode\n");
        return 1;
    }
    // make sure pathname isn't a directory
    // can only delete links and reg files
    if(S_ISDIR(mip->INODE.i_mode)) {
        printf("Error: cannot unlink a directory. Try using rmdir\n");
        return 1;
    }
    printf("unlinking ....");
    ip = &mip->INODE;

    // decrement link count
    ip->i_links_count--;
    printf("new links: %d\n", ip->i_links_count);

    // deallocate the blocks
    for (i = 0; i < 12 && ip->i_block[i]; i++) {
        bdealloc(dev, ip->i_block[i]);
    }

    // deallocate the inode
    idealloc(dev, ino);
    strcpy(temp, pathname); // copy into temp so we don't lost the pathname
    strcpy(dname, dirname(temp));

    strcpy(temp, pathname);
    strcpy(bname, basename(temp));

    printf("dname: %s\tbname:\t%s\n", dname, bname);

    // get parent and remove target from the parent
    p_ino = getino(running->cwd, dname);
    p_mip = iget(dev, p_ino);
    p_ip = &p_mip->INODE;

    // removes child from parent
    printf("removing %s from %s\n", bname, dname);
    rm_child(p_mip, bname);

    // update time, set dirty, and put back
    p_ip->i_atime = time(0L);
    p_ip->i_mtime = time(0L);
    p_mip->dirty = 1;

    iput(p_mip);

    mip->dirty = 1;
    iput(mip);
    return 0;
}
