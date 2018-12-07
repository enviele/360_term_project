

int my_cd(char *pathname) {
    //var def
    int ino;
    MINODE *mip = running->cwd;
    MINODE *move_node = NULL;


    printf("in my_cd\n");
    
    //if there was no pathname specified
    if (!(strcmp(pathname, "")) || !(strcmp(pathname, "/"))) {
        //then that means cd into the root, so we can 
        //just set that directly here
        running->cwd = root;
        return 0;
    }
        
    //this gets the ino num from the pathname
    ino = getino(mip, pathname);
    //if ino is zero, the file doesn't exist
    if (ino == 0) {
        printf("error: pathname does not exist\n");
        return 1;
    }
    //now set the new mip to the block from the ino
    move_node = iget(dev, ino);
    //now checking that move_node is a dir
    if (S_ISDIR(move_node->INODE.i_mode)) {
        //now that we have the new location and it is a dir, 
        //we take the current directory running and put it back
        iput(running->cwd);

        //now change the current dir to the new dir
        //cd'd into
        running->cwd = move_node;
    }
    else {
        printf("error: cannot cd into non-dir");
        return 1;
    }
    return 0;
}

