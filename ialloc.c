/******* ialloc.c: allocate a free INODE, return its inode number ******/
#include <stdio.h>
#include "type.h"

/********** globals *************/
int fd;
int imap, bmap; // IMAP and BMAP block number
int ninodes, nblocks, nfreeInodes, nfreeBlocks;

// helper functions for ialloc from lab 6 prelab
//

int tst_bit(char *buf, int bit)
{
    int i, j;
    i = bit / 8;
    j = bit % 8;
    if (buf[i] & (1 << j))
        return 1;
    return 0;
}

// sets a bit using OR, used in ialloc
int set_bit(char *buf, int bit)
{
    int i, j;
    i = bit / 8;
    j = bit % 8;
    buf[i] |= (1 << j);
}

int clr_bit(char *buf, int bit)
{
    int i, j;
    i = bit / 8;
    j = bit % 8;
    buf[i] &= ~(1 << j);
}

// decrements the amount of free inodes on the device
// this is used to ensure we don't use more inodes than we have room for in the disk
int decFreeInodes(int dev)
{
    char buf[BLKSIZE];

    // dec free inodes count in SUPER and GD
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_inodes_count--;
    put_block(dev, 1, buf);

    get_block(dev, 2, buf);
    gp = (GD *)buf;
    gp->bg_free_inodes_count--;
    put_block(dev, 2, buf);
}

// end of helper functions

// allocates a free inode number for writing
// taken from lab6 and project help notes
// this is usedby any writing functions that require new inodes
int ialloc(int dev)
{
    int i;
    char buf[BLKSIZE];

    // read inode_bitmap block
    get_block(dev, imap, buf);

    for (i = 0; i < ninodes; i++)
    {
        if (tst_bit(buf, i) == 0)
        {
            set_bit(buf, i);
            decFreeInodes(dev);

            put_block(dev, imap, buf);

            return i + 1;
        }
    }
    printf("ialloc() Error: no more free inodes\n");
    return 0;
}

//taken from prelab 6
// allocates a free block so we can put stuff in it
int balloc(int dev)
{
    int i;
    char buf[BLKSIZE];

    get_block(dev, bmap, buf);

    for (i = 0; i < ninodes; i++)
    {
        if (tst_bit(buf, i) == 0)
        {
            set_bit(buf, i);
            decFreeInodes(dev);

            put_block(dev, bmap, buf);

            return i + 1;
        }
    }
    printf("ialloc() Error: no more free inodes\n");
    return 0;
}

// deallocates an inode for a given ino on the dev
// this is used when we remove stuff
// once deallocated we need to increment the free nodes in the SUPER and GD blocks

int idealloc(int dev, int ino) {
    char buf[1024];
    int byte, bit;

    // clear bit(bmap, bno)
    get_block(dev, imap, buf);

    // use mailman's algorithm to find where it is
    byte = ino / 8;
    bit = ino % 8;

    // negate it

    buf[byte] &= ~(1 << bit);
    // put block back
    put_block(dev, imap, buf);

    // increment free blocks for SUPER
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count++;
    put_block(dev, 1, buf);

    //increment free blocks for GD
    get_block(dev, 2, buf);
    gp = (GD *) buf;
    gp->bg_free_blocks_count++;
    put_block(dev, 2, buf);

}

// deallocate a block
// once we have deallocated the block we also need to increment the numberof free blocks in SUPER and GD

int bdealloc(int dev, int bno) {
    char buf[1024];
    int byte, bit;

    // clear bit(bmap, bno)
    get_block(dev, bmap, buf);
    byte = bno / 8;
    bit = bno % 8;

    buf[byte] &= ~(1 << bit);

    put_block(dev, bmap, buf);

    // increment free blocks for SUPER
    get_block(dev, 1, buf);
    sp = (SUPER *)buf;
    sp->s_free_blocks_count++;
    put_block(dev, 1, buf);

    // increment free blocks for GD
	get_block(dev, 2, buf);
	gp = (GD *)buf;
	gp->bg_free_blocks_count++;
    put_block(dev, 2, buf);

    return 0;
}