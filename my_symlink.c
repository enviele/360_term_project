int my_symlink(char *file1, char *file2) {
    //var def
    int ino, dir_ino, sym_link_ino;
    MINODE *mip, *dir_mip, *sym_link_mip;
    INODE *inode, *sym_link_inode;
    char dir[128], base[128];
    char new_file[64], old_file[64], temp[128];

    printf("in my_syslink\n");

    //start by checking that both of the files are specified
    if (!strcmp(file1, "") || !strcmp(file2, "")) {
        printf("error: not enough args specified: exiting\n");
        return 0;
    }

    //next, we need to verify that the old file exists
    //first, put the names in buffers
    strcpy(new_file, file2);
    strcpy(old_file, file1);

    printf("newfile: %s\n", new_file);
    printf("oldfile: %s\n", old_file);
    //get the ino of the inode specified by the old file
    ino = getino(running->cwd, file1);

    //check if the file exists
    if (ino == 0) {
        printf("error: given file to link does not exist: exiting\n");
        return 0;
    }

    //get the minode from the specified ino
    mip = iget(dev, ino);

    //now we want to make sure the old file is not a link file
    if (S_ISLNK(mip->INODE.i_mode)) {
        printf("error: cannot symlink from a link file: exiting\n");
        return 0;
    }

    //now we can break the old pathname into base and dir
    strcpy(temp, file2);
    strcpy(dir, dirname(temp));
    strcpy(base, basename(file2));

    printf("dir: %s\tbase: %s\n", dir, base);
    printf("oldfile:\t%s", old_file);
    //now we want to get the ino and mip of the dir
    dir_ino = getino(running->cwd, dir);
    dir_mip = iget(dev, dir_ino);

    //now to check more about the directory. 
    //make sure it exists 
    if (!dir_mip) {
        printf("error: directory for file doesn't exist: exiting\n");
        return 0;
    }
    printf("S_ISDIR: %d\n", S_ISDIR(dir_mip->INODE.i_mode));
    //next, making sure the dir is a dir
    if (!S_ISDIR(dir_mip->INODE.i_mode)) {
        printf("error: dir must be a directory: exiting\n");
        return 0;
    }
    //one last check: making sure that the file doesn't exist
    if (getino(running->cwd, base) > 0) {
        printf("error: file cannot already exist: exiting\n");
        return 0;
    }

    //now all the checks are done, its time to make the link
    //file, with the basename specified in args
    sym_link_ino = my_creat_function(dir_mip, base);

    //now get the MINODE of created node
    sym_link_mip = iget(dev, sym_link_ino);

    //save the inode of minode
    sym_link_inode = &sym_link_mip->INODE;

    //set the mode for the link to LNK
    sym_link_inode->i_mode = 0120000;
    
    //set the size, which should be the size of the 
    //old file's name 
    sym_link_inode->i_size = strlen(old_file);
    for (int i = 0; i < strlen(old_file); i++) {
        sym_link_inode->i_block[i] = old_file[i];
        printf("%c", sym_link_inode->i_block[i]);
    }
    //list the mip as dirty and put it back
    sym_link_mip->dirty = 1;
    iput(mip);
    iput(sym_link_mip);

    return 0;
}

int my_readlink(char *pathname, char *buffer) {
    INODE *ip;
    MINODE *mip = running->cwd;
    int ino = 0;
    char temp[128];


    ino = getino(mip, pathname);
    if (!ino) {
        printf("file not found\n");
        return 1;
    }
    mip = iget(dev, ino);
    if(!mip) {
        printf("there was an error retrieving the file\n");
        return 1;
    }
    if(!S_ISLNK(mip->INODE.i_mode)) {
        printf("file needs to be a symlink");
        return 1;
    }
    // printf(" %s ", temp);
    for(int i = 0; mip->INODE.i_block[i]; i++) {
        buffer = mip->INODE.i_block[i];
    }
    return strlen((char*)mip->INODE.i_block);
}