

int init() {
    //set the proc pointer to allocate size for proc
    running = malloc(sizeof(PROC));

    // set proc 0 settings
    proc[0].pid = 1;
    proc[0].uid = 0;
    proc[0].cwd = 0;

    // set proc 1 settings
    proc[1].pid = 2;
    proc[1].uid = 1;
    proc[1].cwd = 0;

    //set running pointer to point to proc[0]
    running = &proc[0];

    //go through all of the minodes and set the ref count to 0
    for (int i = 0; i < 64; i++) {
        minode[i].refCount = 0;
    }

    //also setting the MINODE root = 0
    root = 0;
}

int mount_root(char device[64]) {
    //make a buffer to store the names of pointers
    char buf[BLKSIZE];

    //  
    dev = open(device, O_RDONLY);

    //checking if the open worked
    if (dev < 0) {
        printf("error: disk unable to be opened\n");
        exit(0);
    }

    //checking here that the super block is an ext2fs
    get_block(dev, SUPERBLOCK, buf);
    //setting the super block
    sp = (SUPER *) buf;
    //this check sees if the s_magic matches that for
    //a ext2 fs
    if (sp->s_magic != 0xEF53) {
        printf("error: file system given is not an ext2 fs\n");
        exit(0);
    }

    //now to set some variables, since the file 
    //system has been verified and opened
    
    //set the number of inodes and blocks to vars
    ninodes = sp->s_inodes_count;
    nblocks = sp->s_blocks_count;

    //now set the group desc pointer
    get_block(dev, GDBLOCK, buf);
    gp = (GD *)buf;

    //now saving the imap and bmap from group desc 
    imap = gp->bg_inode_bitmap;
    bmap = gp->bg_block_bitmap;

    //saving the inode table to the starting var
    inode_start = gp->bg_inode_table;

    //get root inode
    root = iget(dev, 2);

    //set the cwd for both the p0 and p1 to point
    //at the root minode
    proc[0].cwd = root;
    proc[1].cwd = root;

    //now proc's have been set, update root refCount
    root->refCount = 3;

    printf("success: device mounted\n");

    return 0;
}