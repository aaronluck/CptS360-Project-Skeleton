// ialloc/balloc, idalloc/bdalloc functions

#include "type.h"

/**** globals defined in main.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;

extern char gpath[128];
extern char *name[64];
extern int n;

extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, iblk;

int ialloc(int dev)
{
    // Code in Chapter 11.8.1

    int i;
    char buf[BLKSIZE];

    MTABLE *mp = (MTABLE*)get_mtable(dev);
    get_block(dev, mp->imap, buf);
    for(i = 0; i < mp->ninodes; i++)
    {
        if(tst_bit(buf, i) == 0)
        {
            set_bit(buf, i);
            put_block(dev,mp->imap,buf);
            decFreeInodes(dev);
            return(i + 1);
        }
    }
    return 0; // Out of free inodes
}

int idalloc(int dev, int ino)
{
    // Code in Chapter 11.8.4

    int i;
    char buf[BLKSIZE];
    MTABLE *mp = (MTABLE*)get_mtable(dev);
    if(ino > mp->ninodes) // inodes global
    {
        printf("inumber %d out of range \n", ino);
        return;
    }

    // get inode bitmap block
    get_block(dev,mp->imap, buf);
    clr_bit(buf, ino-1);

    // wrtie buf back
    put_block(dev,mp->imap,buf);

    // update free inode count in SUPER and GD
    incFreeInodes(dev);
}

int bdalloc(int dev, int ino)
{
    return 1;
}

// allocates a free disk block (num) from a dev
int balloc(int dev)
{
    int i;
    char buf[BLKSIZE];

    get_block(dev,bmap,buf);

    for(i = 0; i < nblocks; i = i + 1)
    {
        if(tst_bit(buf,i) == 0)
        {
            set_bit(buf,i);
            decFreeBlocks(dev);
            put_block(dev,bmap,buf);
            return (i+1);
        }
    }
    return 0; // Eventually: Return block number
}