int my_creat(char *pathname) {
    printf("in my_creat\n");

    int i, ino;
	MINODE *pmip;
	INODE *pip;

	char buf[1024];
	char temp1[1024], temp2[1024];
	char parent_name[1024], child_name[1024];

	//copy path so we don't destroy it
	strcpy(temp1, pathname);
	strcpy(temp2, pathname);

	//get parent and child name
	strcpy(parent_name, dirname(temp1));
	strcpy(child_name, basename(temp2));
	//printf("parent: %s\nchild: %s\n", parent_name, child_name);

	//get parent's ino
	ino = getino(running->cwd, parent_name);
	printf("ino is %d\n", ino);
	pmip = iget(dev, ino);
	pip = &pmip->INODE;

	//check if parent exists
	if(!pmip)
	{
		printf("ERROR: parent does not exist\n");
		return;
	}

	if(pmip == root)
		printf("in root\n");

	//check if dir
	if(!S_ISDIR(pip->i_mode))
	{
		printf("ERROR: parent is not directory\n");
		return;
	}

	//check if file already exists
	if(getino(running->cwd, pathname) != 0)
	{
		printf("ERROR: %s already exists\n", pathname);
		return;
	}

	printf("running my_creat\n");

    my_creat_function(pmip, child_name);

    pip->i_atime = time(0L);

    // set dirty and iput
    pmip->dirty = 1;

    iput(pmip);

    return 0;
}

int my_creat_function(MINODE *pip, char *name) {
    int i;
	//allocate inode for new file
	int ino = ialloc(dev);

	//create it in memory
	MINODE *mip = iget(dev, ino);
	INODE *ip = &mip->INODE;

	ip->i_mode = 0x81A4; //file type
	ip->i_uid  = running->uid; // Owner uid
    ip->i_gid  = running->gid; // Group Id
		//We set the size to 0 because it is an empty file
    ip->i_size = 0;	// Size in bytes
    ip->i_links_count = 1; // Links to parent directory
    ip->i_atime = time(0L); // Set last access to current time
	ip->i_ctime = time(0L);
	ip->i_mtime = time(0L);

	//Set to 0 cause we dont need . and .. cause it isnt a dir
	ip->i_blocks = 0;

	//set data blocks to 0
	for(i = 0; i < 15; i++)
	{
		ip->i_block[i] = 0;
	}

	//set dirty and iput
	mip->dirty = 1;
	iput(mip);

	//enters the files name in the parent directory
	enter_name(pip, ino, name);//enter name defined in mkdir.c

	return ino;
}