//#include "type.h"

int ls_dir(MINODE *mip)
{
    DIR *dir;
    char *cur_pointer;
    INODE *inode = &mip->INODE;
    int entry_num, myino;
    MINODE *temp;
    char buf[BLKSIZE], temp_char[BLKSIZE];

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
            temp = iget(dev, dir->inode); // get the block and put it into temp
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

int ls_file(MINODE *mip, char* path)
{
    // mip = iget(dev, ino);
    int i, type;
    INODE *ip = &mip->INODE;
    struct stat fstat;

    if (stat(path, &fstat) < 0)
    {
        return 1;
    }
    char *permissions = "rwxrwxrwx";

    // grab information from the file
    u16 mode = ip->i_mode;
    u16 links = ip->i_links_count;
    u16 uid = ip->i_uid;
    u16 gid = ip->i_gid;
    u32 size = ip->i_size;

    // set time to current time
    char *time = ctime((time_t *)&ip->i_mtime);
    // remove \r from time

    time[strlen(time) - 1] = 0; // set last char to 0

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
    printf((fstat.st_mode & S_IRUSR) ? "r" : "-");
    printf((fstat.st_mode & S_IWUSR) ? "w" : "-");
    printf((fstat.st_mode & S_IXUSR) ? "x" : "-");
    printf((fstat.st_mode & S_IRGRP) ? "r" : "-");
    printf((fstat.st_mode & S_IWGRP) ? "w" : "-");
    printf((fstat.st_mode & S_IXGRP) ? "x" : "-");
    printf((fstat.st_mode & S_IROTH) ? "r" : "-");
    printf((fstat.st_mode & S_IWOTH) ? "w" : "-");
    printf((fstat.st_mode & S_IXOTH) ? "x" : "-");

    printf(" %d %d %d %d %s %s\n", links, gid, uid, size, time, path);

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
