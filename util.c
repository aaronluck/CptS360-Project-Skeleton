/*********** util.c file ****************/

#ifndef __UTIL_C__
#define __UTIL_C__

#include "type.h"

int get_block(int dev, int blk, char *buf)
{
  // Code in Chapter 11.7.2

  return 1;
}

int put_block(int dev, int blk, char *buf)
{
  // Code in Chapter 11.7.2

  return 1;
}

int tst_bit(char *buf, int bit) // pg. 333
{
  return 1; // Eventually: return proper test value
}

int set_bit(char *buf, int bit) // pg. 333
{
  return 1;
}

int clr_bit(char *buf, int bit) // pg. 306
{
  return 1;
}

MTABLE * get_mtable(int dev)
{
  return 0;
}

int incFreeInodes(int dev)
{
  // Code in Chapter 11.8.4

  return 1;
}

int idalloc(int dev, int ino)
{
  // Code in Chapter 11.8.4

  return 1;
}

int incFreeBlocks(int dev)
{
  return 1;
}

int bdalloc(int dev, int bno)
{
  return 1;
}

int decFreeInodes(int dev)
{
  // Code in Chapter 11.8.1

  return 1;
}

int decFreeBlocks(int dev)
{
  return 1;
}

int ialloc(int dev)
{
  // Code in Chapter 11.8.1

  return 1; // Eventually: Return inode number
}

int balloc(int dev)
{
  return 1; // Eventually: Return block number
}

MINODE *mialloc()
{
  // Code in Chapter 11.7.1

  return 0; // Eventually: Return entire MINODE *
}

int midalloc(MINODE *mip)
{
  // Code in Chapter 11.7.1

  return 1;
}

int tokenize(char *pathname)
{
  // copy pathname into gpath[]; tokenize it into name[0] to name[n-1]
  // Code in Chapter 11.7.2

  return 1; // Eventually: Return number of names in name[]
}

MINODE *iget(int dev, int ino)
{
  // return minode pointer of loaded INODE=(dev, ino)
  // Code in Chapter 11.7.2

  return 0; // Eventually: Return entire MINODE *
}

void iput(MINODE *mip)
{
  // dispose of minode pointed by mip
  // Code in Chapter 11.7.2

  return;
}


int search(MINODE *mip, char *name)
{
  // search for name in (DIRECT) data blocks of mip->INODE
  // return its ino
  // Code in Chapter 11.7.2

  return 0; // Eventually: Return inode number
}

int getino(char *pathname)
{
  // return ino of pathname
  // Code in Chapter 11.7.2

  return 0; // Eventually: Return inode number
}

int findmyname(MINODE *parent, u32 myino, char *myname) 
{
  // WRITE YOUR code here:
  // search parent's data block for myino;
  // copy its name STRING to myname[ ];

  return 1;
}

int findino(MINODE *mip, u32 *myino) // myino = ino of . return ino of ..
{
  // mip->a DIR minode. Write YOUR code to get mino=ino of .
  //                                         return ino of ..
  return 1; // Eventually: Return parent inode number
}

#endif