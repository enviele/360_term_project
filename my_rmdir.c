
// #include "type.h"
int my_rmdir(char *pathname)
{
    int i;
	int ino, parent_ino;
	MINODE *mip;
	MINODE *p_mip;
	INODE *ip;
	INODE *pip;
	char temp[64], child[64];

	//Checks
	if(!pathname)
	{
		printf("ERROR: no pathname given\n");
		return;
	}

	strcpy(temp, pathname);
	strcpy(child, basename(temp));

	//get the ino of the child and ensure it exists
	ino = getino(running->cwd, pathname);
	printf("%s ino is %d\n", pathname, ino);
	mip = iget(dev, ino);

	if(!mip)
	{
		printf("ERROR: mip does not exist\n");
		return;
	}

	//check if dir
	if(!S_ISDIR(mip->INODE.i_mode))
	{
		printf("ERROR: %s is not a directory\n", pathname);
		return;
	}

	//check if empty
	if(mip->INODE.i_links_count > 2)
	{
		printf("ERROR: directory not empty\n");
		return 0;
	}

	printf("Starting remove\n");

	ip = &mip->INODE;

	//get parent ino
	findino(mip, &ino, &parent_ino);
	printf("ino is %d\nparent ino is%d\n", ino, parent_ino);
	p_mip = iget(dev, parent_ino);
	pip = &p_mip->INODE;

	//go through blocks deallocating them

	//deallocate blocks
	for(i = 0; i < 15 && ip->i_block[i] != 0; i++)
	{
		bdealloc(dev, ip->i_block[i]);
	}

	//deallocate inode
	idealloc(dev, ino);

	//remove entry from parent dir
	rm_child(p_mip, child);

	//update parent
	pip->i_links_count--;
	pip->i_atime = time(0L);
	pip->i_mtime = time(0L);
	p_mip->dirty = 1;

	//write parent changes to disk
	iput(p_mip);
	//write changes to deleted directory to disk
	mip->dirty = 1;
	iput(mip);

	return;
}

int rm_child(MINODE *parent, char *name)
{
    int i, start, end;
    DIR *dir, *prev_dir, *last_dir;
    INODE *pip = &parent->INODE;
    char buf[1024], *cp, tmp[64], *last_cp;

    printf("removing %s ....\n", name);
    printf("parent's size is %d\n", pip->i_size);

    // iterate through parent's blocks to find the child
    for (i = 0; i < 12; i++)
    {
        if (!pip->i_block[i])
            return;

        get_block(dev, pip->i_block[i], buf);
        cp = buf;
        dir = (DIR *)buf;

        printf("dir at %s\n", dir->name);

        while (cp < buf + BLKSIZE)
        {
            strncpy(tmp, dir->name, dir->name_len);
            tmp[dir->name_len] = 0;

            printf("dir is at %s\n", tmp);

            if (!strcmp(tmp, name))
            {
                printf("I found the child!\n");
                if (cp == buf & cp + dir->rec_len == buf + BLKSIZE)
                {
                    // child is the first and only entry, need to delete the entire block
                    free(buf);
                    bdealloc(dev, ip->i_block[i]);

                    pip->i_size -= BLKSIZE;

                    // shift blocks left

                    while (pip->i_block[i + 1] && i + 1 < 12)
                    {
                        i++;
                        get_block(dev, pip->i_block[i], buf);
                        put_block(dev, pip->i_block[i - 1], buf);
                    }
                }
                else if (cp + dir->rec_len == buf + BLKSIZE)
                { // child is the last entry
                    // just need to remove the last entry
                    printf("removing last entry\n");
                    prev_dir->rec_len += dir->rec_len;
                    put_block(dev, pip->i_block[i], buf);
                }
                else
                {
                    // child is in the middle
                    printf("before dir is %s\n", dir->name);

                    last_dir = (DIR *)buf;
                    last_cp = buf;

                    // step into last entry

                    while (last_cp + last_dir->rec_len < buf + BLKSIZE)
                    {
                        printf("lst_dir: %s\n", last_dir->name);
                        last_cp += last_dir->rec_len;
                        last_dir = (DIR *)last_cp;
                    }
                    printf("%s and $s\n", dir->name, last_dir->name);
                    last_dir->rec_len += dir->rec_len;

                    start = cp + dir->rec_len;
                    end = buf + BLKSIZE;

                    memmove(cp, start, end - start); // built in function, move memory left

                    put_block(dev, pip->i_block[i], buf);
                }
                parent->dirty = 1;
                iput(parent);
                return;
            } // end of child found

            prev_dir = dir; // look at next file/dir
            cp += dir->rec_len;
            dir = (DIR *)cp;
        }
        //end of while loop
    }
    return;
}