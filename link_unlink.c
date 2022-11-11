/************* link_unlink.c file **************/

#ifndef __LINKUNLINK_C__
#define __LINKUNLINK_C__

#include "mkdir_creat.c"
#include "rmdir.c"
#include "alloc_dalloc.c"

/**** globals defined in main.c file ****/
extern MINODE minode[NMINODE];
extern MINODE *root;
extern PROC   proc[NPROC], *running;

extern char gpath[128];
extern char *name[64];
extern int n;

extern int fd, dev;
extern int nblocks, ninodes, bmap, imap, iblk;

extern char line[128], pathname[128];

// link old_file new_file
int my_link(char* old_file, char* new_file)
{
    // (1). // verify old_file exists and is not a DIR;
    // oino = getino(old_file);
    // omip = iget(dev, oino);
    // check omip->INODE file type (must not be DIR).

    // (2). // new_file must not exist yet:
    // getino(new_file) must return 0;

    // (3). creat new_file with the same inode number of old_file:
    // parent = dirname(new_file); child = basename(new_file);
    // pino = getino(parent);
    // pmip = iget(dev, pino);
    // // creat entry in new parent DIR with same inode number of old_file
    // enter_name(pmip, oino, child);

    // (4). omip->INODE.i_links_count++; // inc INODE’s links_count by 1
    // omip->dirty = 1; // for write back by iput(omip)
    // iput(omip);
    // iput(pmip);

    MINODE* omip, *pmip;
    char* parent, *child;
    int oino, pino;

    if(pathname[0] == '/')
    {
        dev = root->dev;
    }
    else
    {
        dev = running->cwd->dev;
    }

    // 1.0 verify old file exists and is not a dir
    oino = getino(old_file);
    if(oino == 0)
    {
        return -1;
    }

    omip = iget(dev,oino);

    // 1.1 check omip->INODE file type (must not be DIR).
    if(S_ISDIR(omip->INODE.i_mode))
    {
        printf("Error: %s is a dir \n", old_file);
        return -1;
    }

    // 2.0 new file must not exist yet
    if(getino(new_file) == 0) // getino must return 0
    {
        return 0;
    }
    else
    {
        printf("Error: %s exists already \n", new_file);
        return -1;
    }

    // 3.0 creat new_file with the same inode number of old_file:
    parent = dirname(new_file);
    child = basename(new_file);
    pino = getino(parent);
    pmip = iget(dev,pino);

    // 3.1 creat entry in new parent DIR with same inode number of old_file
    enter_name(pmip,oino,child);


    // 4.0 
    // inc INODES link_count by 1
    omip->INODE.i_links_count++;

    // for write back by iput(omip)
    omip->dirty = 1;

    iput(omip);
    iput(pmip);
}

// int link_file(char *pathname)
// {
//     return 1;
// }


/*************************************************
 * unlink func
 * dec file's link_count by 1
 * deletes file name from parent DIR
 * when a files link_count = 0, file is completly removed
 * this is done by deallocating data blocks and inode
*************************************************/
int my_unlink(char *filename)
{

    // (1). get filenmae’s minode:
    // ino = getino(filename);
    // mip = iget(dev, ino);
    // check it’s a REG or symbolic LNK file; can not be a DIR

    // (2). // remove name entry from parent DIR’s data block:
    // parent = dirname(filename); child = basename(filename);
    // pino = getino(parent);
    // pimp = iget(dev, pino);
    // rm_child(pmip, ino, child);
    // pmip->dirty = 1;
    // iput(pmip);
    
    // (3). // decrement INODE’s link_count by 1
    // mip->INODE.i_links_count--;
    
    // (4). if (mip->INODE.i_links_count > 0)
    // mip->dirty = 1; // for write INODE back to disk
    
    // (5). else{ // if links_count = 0: remove filename
    // deallocate all data blocks in INODE;
    // deallocate INODE;
    // }
    // iput(mip); // release mip
    //     return 1;

    MINODE *mip, *pmip;

    int ino, pino;
    int i;

    char* parent, *child;

    if(pathname[0] == '/')
    {
        dev = root->dev;
    }
    else
    {
        dev = running->cwd-dev;
    }

    // 1.0 get filenames minode
    ino = getino(filename);
    if(ino == 0)
    {
        return -1;
    }

    mip = iget(dev,ino);

    // 1.1 check if its a reg or symbolic link, cant be dir
    if(S_ISDIR(mip->INODE.i_mode))
    {
        printf("Erorr: %s is a dir \n", filename);
        return -1;
    }

    // 2.0 remove name entry from parent DIR's data block
    parent = dirname(filename);
    child = basename(filename);
    pino = getino(parent);
    pmip = iget(dev,pino);
    rm_child(pmip,child);
    pmip->dirty = 1;
    iput(pmip);

    // 3.0 dec INODES link count by 1
    mip->INODE.i_links_count--;

    // 4.0
    if(mip->INODE.i_links_count > 0)
    {
        mip->dirty = 1;
    }
    // 5.0
    else // if link count = 0
    {
        // remove filename
        INODE *ip = &mip->INODE;

        // deallocate all data blocks in inode
        for(i = 0; i < 12; i = i + 1)
        {
            if(ip->i_block[i] == 0)
            {
                break;
            }

            // deallocate inode
            balloc(ip->i_block[i]);
            ip->i_block[i] = 0;
        }
        idalloc(mip->dev,mip->ino);
    }

    // release mip
    iput(mip);
}

// int my_rm(MINODE *mip, char *pathname)
// {
//     return 1;
// }

#endif