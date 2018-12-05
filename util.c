/**************************************
 * CptS 360 Final Project
 * 
 * Utility Functions File
 * 
 * Programmers:
 * Amariah Del Mar 11504395
 * Elizabeth Viele
 * 
 * Last Modified: 12/4/18
*/



/************** util.c file ****************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

typedef struct ext2_group_desc_desc GD;
typedef struct ext2_super_block SUPER;
typedef struct ext2_inode INODE;
typedef struct ext2_dir_entry_2 DIR;



#define BLKSIZE 1024
#define BLKOFFSET(block) (BLKSIZE + block-1)*BLKSIZE
#define NMINODE 100
#define NPROC 2
#define NFD 10
#define NOFT 40

// #include "type.h"

/**** globals defined in main.c file ****/

extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC proc[NPROC], *running;
extern char gpath[128];
extern char *name[64];
extern int n;
extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, inode_start;
extern char line[256], cmd[32], pathname[256];



typedef struct minode
{
    INODE INODE;
    int mptr;
    int mountptr;
    // disk inode
    int dev, ino;
    int refCount;
    // use count
    int dirty;
    // modified flag
    int mounted;
    // mounted flag
    struct mount *mntPtr;
    // mount table pointer
    // int lock;
    // ignored for simple FS
} MINODE;

typedef struct oft
{
    int mode;
    int refCount;
    MINODE *mptr;
    int offset;
} OFT;

typedef struct proc
{
    struct Proc *next;
    int pid;
    int uid;
    int gid;
    int ppid;
    int status;
    struct minode *cwd;
    OFT *fd[NFD];
} PROC;
PROC proc[NPROC], *running;


int get_block(int dev, int blk, char *buf)
{
    lseek(dev, (long)blk * BLKSIZE, 0);
    n = read(dev, buf, BLKSIZE);
    if (n < 0) {
        printf("get_block(%d %d) error\n", dev, blk);
    }
}

int put_block(int dev, int blk, char *buf)
{
    lseek(dev, (long)blk * BLKSIZE, 0);
    n = write(dev, buf, BLKSIZE);
    if (n < 0) {
        printf("put_block(%d %d) error\n", dev, blk);
    }
}

int i_count = 0, n = 0;

int tokenize(char *pathname)
{
    // tokenize pathname into n components: name[0] to name[n-1];
    printf("\nPathname:\t%s\n", pathname);
    name[0] = strtok(pathname, "/");
    printf("\tname[0]:\t%s\n", name[0]);

    while(name[i_count]) {
        i_count++;
        name[i_count] = strtok(NULL, "/");
        printf("\tname[%d]:\t%s\n", i_count, name[i_count]);
    }
    n = i_count;
    printf("\tn:\t%d\n", n);
}

MINODE *iget(int dev, int ino)
{

    int ipos = 0;
    int i = 0;
    int offset = 0;
    char buf[1024];
    INODE *ip = NULL;
    MINODE *mip = malloc(sizeof(MINODE));

    // return minode pointer to loaded INODE
    // (1). Search minode[ ] for an existing entry (refCount > 0) with
    //   the needed (dev, ino):
    //   if found: inc its refCount by 1;
    //return pointer to this minode;
    for (i = 0; i < NMINODE; i++) {
        mip = &minode[i];
        if(mip->dev == dev && mip->ino == ino) {
            mip->refCount++;
            printf("minode[%d]->refCount incremented\n", i);
            return mip;
        }
    }
    //   (2). // needed entry not in memory:
    //       find a FREE minode(refCount = 0);
    //   Let mip->to this minode;
    //   set its refCount = 1;
    //   set its dev, ino

    //  mailmans algorithm:
    ipos = (ino - 1) / 8 + inode_start;     // get INODE of ino a char buf[BLKSIZE]    
    offset = (ino - 1) % 8;

    //  get the block
    get_block(dev, ipos, buf);
    // load inode
    ip = (INODE *)buf + offset;

    for(i = 0; i < NMINODE; i++) {
        mip = &minode[i];
        printf("minode[%d].refCount = %d\n", i, minode[i].refCount);

        if(!mip->refCount) {
            printf("using minode[%d]\n", i);
            mip->INODE = *ip;    // load INODE of(dev, ino) into mip->INODE
            mip->dev = dev;
            mip->ino;
            mip->refCount++;

            return mip;
        }
    }

    //   (3)

    //  get INODE of ino a char buf[BLKSIZE]

}

int iput(MINODE *mip) // dispose a used minode by mip
{
    int ino = 0, offset, ipos;
    char buf[1024];
    INODE *ip = NULL;

    ino = mip->ino;

    // decrement refCount by 1 to equal 0. refcount == 0 if FREE
    mip->refCount--;

    // check refcount to see if it is in use
    // check dirty to see if it's been changed, dirty == 1 if changed
    // if refCount > 0 or dirty return
    if (mip->refCount > 0)
        return;
    if (!mip->dirty)
        return;

    // Write YOUR CODE to write mip->INODE back to disk
    // use mail man's algorithm to determine whick disk block & inode in the block
    ipos = (ino - 1) / 8 + inode_start;
    offset = (ino - 1) % 8;

    // read block
    get_block(mip->dev, ipos, buf);

    // copy minode's inode into the inode area in the block
    ip = (INODE*)buf + offset;
    *ip = mip->INODE;

    // write block back to disk
    put_block(mip->dev, ipos, buf);
    mip->dirty = 0;

}

// serach a DIRectory INODE for entry with a given name
int search(MINODE *mip, char *name)
{
    // return ino if found; return 0 if NOT

    //search for name in the date blocks of this inode
    // if found, return name's ino;
    // return 0;

    int i;
    char buf[BLKSIZE], *cp, dir_name[64];
    DIR *dp;

    //make sure it's a directory
    if (!S_ISDIR(mip->INODE.i_mode)) {
        printf("ERROR:\tnot a dirctory\n");
        return 0;
    }
    // search through direct blocks: i_blocks[0-11]
    for (i = 0; i < 12; i++) {
        // if data block has stuff in it
        if(mip->INODE.i_block[i]) {
            // get the block at this i_block
            get_block(dev, mip->INODE.i_block[i], buf);

            dp = (DIR *)buf;
            cp = buf;

            while(cp<buf + BLKSIZE) {
                //null terminate dp->name for strcmp
                if(dp->name_len < 64) {
                    strncpy(dir_name, dp->name, dp->name_len);
                    dir_name[dp->name_len] = 0;
                } else {
                    strncpy(dir_name, dp->name, 64);
                    dir_name[63] = 0;
                }

                printf("current dir:\t%s", dir_name);
                // check if it's the name we're looking for
                if(!strcmp(name, dir_name)) {
                    return dp->inode; // if name matches, return the inode
                }
                cp += dp->rec_len;
                dp = (DIR *)cp;
            }
        }
    }
    // name does not exist,  print error message
    printf("name %s does not exist\n", name);
    strcpy(teststr, "name ");
    strcat(teststr, name);
    strcat(teststr, " does not exist.");
    return 0;
}

// retrun inode number of pathname

int getino(char *pathname)
{
    // SAME as LAB6 program: just return the pathname's ino;
    int ino = 0, i = 0, n = 0, inum, offset;
    char path[64], name[64][64], *temp, buf[1024];
    MINODE *mip = NULL;
    //check if root
    if (!strcmp(pathname, "/")) {
        return 2;
    }
    // check if absolute path
    if (pathname[0] == '/') {
        mip = root;
    }
    if(pathname) {
        // parse string and put it into the var 'name
        strcat(pathname, "/");
        temp = strtok(pathname, "/");

        while(temp != NULL) {
            strcpy(name[i], temp);
            temp = strtok(NULL, "/");
            i++;
            n++;
        }
        // parsing complete
    }
    // searching for minode with name of 'pathname
    for(i = 0; i < n; i++) {
        printf("current inode:\t%d\n", mip->ino);
        printf("Searching for:\t%s\n", name[i]);
        ino = search(mip, name[i]);

        if (ino == 0) {
            // can't find name[i]
            return 0;
        }
        printf("found $s at inode %d\n", name[i], ino);
        mip = iget(dev, ino);
    }
    return ino;
}

// THESE two functions are for pwd(running->cwd), which prints the absolute
// pathname of CWD.

int findmyname(MINODE *parent, u32 myino, char *myname)
{
    // parent -> at a DIR minode, find myname by myino
    // get name string of myino: SAME as search except by myino;
    // copy entry name (string) into myname[ ];
    int i;
    INODE* ip;
    char buf[BLKSIZE];
    char *cp;
    DIR *dp;

    if(myino == root->ino) {
        strcpy(myname, "/");
        return 0;
    }
    if (!parent) {
        printf("ERROR:\tno parent\n\n");
        return 1;
    }

    ip = &parent->INODE;

    if(!S_ISDIR(ip->i_mode)) {
        printf("ERROR:\tnot a directory\n\n");
        return 1;
    }

    for (i = 0; i < 12; i++) {
        if(ip->i_block[i]) {
            get_block(dev, ip->i_block[i], buf);
            dp = (DIR*)buf;
            cp = buf;

            while(cp < buf + BLKSIZE) {
                if (dp->inode == myino) {
                    strncpy(myname, dp->name, dp->name_len);
                    myname[dp->name_len] = 0;
                    return 0;
                } else {
                    cp += dp->rec_len;
                    dp = (DIR*)cp;
                }
            }
        }
    }
    return 1;
}

int findino(MINODE *mip, u32 *myino)
{
    // fill myino with ino of .
    // retrun ino of ..

    INODE *ip;
    char buf[1024];
    char *cp;
    DIR *dp;

    // check if minode exists
    if (!mip) {
        printf("ERROR:\tino does not exist\n");
        return 1;
    }

    // point ip to minode's inode
    ip = &mip->INODE;

    // check if directory
    if(!S_ISDIR(ip->i_mode)) {
        printf("ERROR:\tino not a directory");
        return 1;
    }
    // read the block
    get_block(dev, ip->i_block[0], buf);
    dp = (DIR*)buf;
    cp = buf;

    // return ino of parent

    return dp->inode;
}