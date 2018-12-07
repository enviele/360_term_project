int ls_dir(MINODE *mip)
{
    DIR *dir;
    char *cur_pointer;
    INODE *inode = &mip->INODE;
    int entry_num, myino;
    MINODE *temp;
    char buf[BLKSIZE], temp_char[BLKSIZE];
    int device = running->cwd->dev;

    entry_num = inode->i_size / BLKSIZE; // how many blocks in inode
    for (int i = 0; i < entry_num; i++)
    {                                           // for each block in inode
        get_block(dev, inode->i_block[i], buf); // get the block and put it into buf
        dir = (DIR *)buf;                       // init dir
        cur_pointer = buf;

        while (cur_pointer < buf + BLKSIZE)
        {                                                 // goes through each entry in the directory
            strncpy(temp_char, dir->name, dir->name_len); // copy directory name into temp_char
            temp_char[dir->name_len] = 0;                 // get rid of \n
            temp = iget(dev, dir->inode);                 // get the block and put it into temp
            if (temp)
            { // if you have the block, print out the file's contents
                ls_file(temp, temp_char);
                iput(temp); // put the block back
            }
            else
            {
                printf("Error: cannot print data for this MINODE\n");
            }
            memset(temp_char, 0, 1024); // empty contents of temp_char
            cur_pointer += dir->rec_len;
            dir = (DIR *)cur_pointer;
        }
    }
    printf("\n");
    return 0;
}

int my_ls(char *pathname)
{
    int ino, offset;
    MINODE *mip = running->cwd;
    char name[64][64], temp[64];
    char buf[1024];

    printf("pathname:\t%s\n", pathname);
    //ls cwd
    if (!strcmp(pathname, ""))
    {
        //print_dir(mip->INODE);
        ls_dir(mip);
        return;
    }

    //ls root dir
    if (!strcmp(pathname, "/"))
    {
        //print_dir(root->INODE);
        ls_dir(root);
        return;
    }

    //if there's a pathname, ls pathname
    if (pathname)
    {
        //check if path starts at root
        if (pathname[0] == '/')
        {
            mip = root;
        }

        //search for path to print
        ino = getino(mip, pathname);
        if (ino == 0)
        {
            return;
        }

        mip = iget(dev, ino);
        if (!S_ISDIR(mip->INODE.i_mode))
        {
            printf("%s is not a directory!\n", pathname);
            iput(mip);
            return;
        }

        //print_dir(mip->INODE);
        ls_dir(mip);
        iput(mip);
    }
    else
    {
        //print root dir
        //print_dir(root->INODE);
        ls_dir(root);
    }
    return 0;
}

// // print information on a file
void ls_file(MINODE *mip, char *namebuf)
{
    char *Time;
    unsigned short mode;
    int type;
    struct stat fstat;
    stat(namebuf, &fstat);

    mode = mip->INODE.i_mode;
    // print out info in the file in same format as ls -l in linux
    // print the file type
    if ((mode & 0120000) == 0120000)
    {
        printf("l");
        type = LINK;
    }
    else if ((mode & 0040000) == 0040000)
    {
        printf("d");
        type = DIRECTORY;
    }
    else if ((mode & 0100000) == 0100000)
    {
        printf("-");
        type = FILE;
    }

    // print the permissions
    if ((mode & (1 << 8)))
        printf("r");
    else
        printf("-");
    if ((mode & (1 << 7)))
        printf("w");
    else
        printf("-");
    if ((mode & (1 << 6)))
        printf("x");
    else
        printf("-");

    if ((mode & (1 << 5)))
        printf("r");
    else
        printf("-");
    if ((mode & (1 << 4)))
        printf("w");
    else
        printf("-");
    if ((mode & (1 << 3)))
        printf("x");
    else
        printf("-");

    if ((mode & (1 << 2)))
        printf("r");
    else
        printf("-");
    if ((mode & (1 << 1)))
        printf("w");
    else
        printf("-");
    if (mode & 1)
        printf("x");
    else
        printf("-");
    //     // print the permissions

    // print the file info
    printf(" %d %d %d %d", mip->INODE.i_links_count, mip->INODE.i_uid, mip->INODE.i_gid, mip->INODE.i_size);
    Time = ctime(&(mip->INODE.i_mtime));
    Time[strlen(Time) - 1] = 0;
    printf(" %s %s", Time, namebuf);

    // if this is a symlink file, show the file it points to
    if ((mode & 0120000) == 0120000)
    {
        printf(" => ");
        for (int j = 0; mip->INODE.i_block[j]; j++)
        {
            printf("%c", mip->INODE.i_block[j]);
        }
        printf("\n");

    }
    else
        printf("\n");

    iput(mip); // cleanup
}
