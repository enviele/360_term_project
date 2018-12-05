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


    // return minode pointer to loaded INODE
    // (1). Search minode[ ] for an existing entry (refCount > 0) with
    //   the needed (dev, ino):
    //   if found: inc its refCount by 1;
    //return pointer to this minode;

    //   (2). // needed entry not in memory:
    //       find a FREE minode(refCount = 0);
    //   Let mip->to this minode;
    //   set its refCount = 1;
    //   set its dev, ino

    //            (3)
    //                .load INODE of(dev, ino) into mip->INODE :

    //   // get INODE of ino a char buf[BLKSIZE]
    blk = (ino - 1) / 8 + inode_start;
    offset = (ino - 1) % 8;

    printf("iget: ino=%d blk=%d offset=%d\n", ino, blk, offset);

    get_block(dev, blk, buf);
    ip = (INODE *)buf + offset;
    mip->INODE = *ip; // copy INODE to mp->INODE

    return mip;
}

int iput(MINODE *mip) // dispose a used minode by mip
{
    mip->refCount--;

    if (mip->refCount > 0)
        return;
    if (!mip->dirty)
        return;

    // Write YOUR CODE to write mip->INODE back to disk
}

// serach a DIRectory INODE for entry with a given name
int search(MINODE *mip, char *name)
{
    // return ino if found; return 0 if NOT
}

// retrun inode number of pathname

int getino(char *pathname)
{
    // SAME as LAB6 program: just return the pathname's ino;
}

// THESE two functions are for pwd(running->cwd), which prints the absolute
// pathname of CWD.

int findmyname(MINODE *parent, u32 myino, char *myname)
{
    // parent -> at a DIR minode, find myname by myino
    // get name string of myino: SAME as search except by myino;
    // copy entry name (string) into myname[ ];
}

int findino(MINODE *mip, u32 *myino)
{
    // fill myino with ino of .
    // retrun ino of ..
}