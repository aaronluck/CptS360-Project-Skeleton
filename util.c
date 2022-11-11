/*********** util.c file ****************/

#ifndef __UTIL_C__
#define __UTIL_C__

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

extern char line[128], pathname[128];

int get_block(int dev, int blk, char *buf)
{
  // Code in Chapter 11.7.2

  int n;
  
  lseek(dev, blk*BLKSIZE, SEEK_SET);
  n = read(dev, buf, BLKSIZE);
  if(n < 0)
  {
    printf("get_block [%d %d] error \n", dev, blk);
  }
}

int put_block(int dev, int blk, char *buf)
{
  // Code in Chapter 11.7.2

  int n;

  lseek(dev, blk*BLKSIZE, SEEK_SET);
  n = write(dev, buf, BLKSIZE);
  if(n != BLKSIZE)
  {
    printf("put_block [%d %d] error \n", dev, blk);
  }

  return 1;
}

int tst_bit(char *buf, int bit) // pg. 333
{
  return buf[bit / 8] & (1 << (bit % 8));
}

int set_bit(char *buf, int bit) // pg. 333
{
  if(buf[bit/8] |= (1 << (bit % 8)))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

int clr_bit(char *buf, int bit) // pg. 306
{
  if(buf[bit / 8] &= ~(1 << bit % 8))
  {
    return 1;
  }
  else
  {
    return 0;
  }
}

MTABLE * get_mtable(int dev)
{
  return 0;
}

int incFreeInodes(int dev)
{
  // Code in Chapter 11.8.4

  char buf[BLKSIZE];

  get_block(dev,1,buf);
  sp = (SUPER*)buf;
  sp->s_free_inodes_count++;
  put_block(dev,1,buf);

  get_block(dev,2,buf);
  gp = (GD*)buf;
  gp->bg_free_inodes_count++;
  put_block(dev,2,buf);
}

// same as incFreeInodes but change to free_blocks_count
int incFreeBlocks(int dev)
{
  char buf[BLKSIZE];

  get_block(dev,1,buf);
  sp = (SUPER*)buf;
  sp->s_free_blocks_count++;
  put_block(dev,1,buf);

  get_block(dev,2,buf);
  gp=(GD*)buf;
  gp->bg_free_blocks_count++;
  put_block(dev,2,buf);
}

int decFreeInodes(int dev)
{
  // Code in Chapter 11.8.1
  // dec free inodes count in SUPER and GD

  char buf[BLKSIZE];

  get_block(dev, 1, buf);
  sp = (SUPER *)buf;
  sp->s_free_inodes_count--;
  put_block(dev,1,buf);

  get_block(dev,2,buf);
  gp = (GD*)buf;
  gp->bg_free_inodes_count--;
  put_block(dev,2,buf);
}

// same as decFreeInodes but change to free_blocks_count
int decFreeBlocks(int dev)
{
  char buf[BLKSIZE];

  get_block(dev,1,buf);
  sp = (SUPER*)buf;
  sp->s_free_blocks_count--;
  put_block(dev,1,buf);

  get_block(dev,2,buf);
  gp=(GD*)buf;
  gp->bg_free_blocks_count--;
  put_block(dev,2,buf);
}

// allocate a free minode for use
MINODE *mialloc()
{
  // Code in Chapter 11.7.1

  int i;

  for(i = 0; i < NMINODE; i = i + 1)
  {
    MINODE *mp = &minode[i];
    if(mp->refCount == 0)
    {
      mp->refCount = 1;
      return mp;
    }
  }

  printf("FS panic: out of minodes \n");
  return 0; // Eventually: Return entire MINODE *
}

// release a used minode
int midalloc(MINODE *mip)
{
  // Code in Chapter 11.7.1
  mip->refCount = 0;
  return 0;
}

int tokenize(char *pathname)
{
  // copy pathname into gpath[]; tokenize it into name[0] to name[n-1]
  // Code in Chapter 11.7.2

  char *s;
  int i;

  strcpy(gpath, pathname);
  nname = 0;
  s = strtok(gpath, "/");

  while(s)
  {
    name[nname++] = s;
    s = strtok(0, "/");
  }
  name[nname] = 0;

  // Eventually: Return number of names in name[]
  for (i = 0; i < nname; i = i + 1)
  {
    printf("%s ", name[i]);
  }
  printf("/n");
}


/*************************************************
 * i get func
 * return pointer to the in mem minode containing INODE of (dev, ino)
 * only one copy of INODE exists in mem
 * normally minode is locked but for our case it is uneeded
*************************************************/
MINODE *iget(int dev, int ino)
{
  // return minode pointer of loaded INODE=(dev, ino)
  // Code in Chapter 11.7.2

  MINODE *mip;
  MTABLE *mp;
  INODE *ip;
  int i, block, offset;
  char buf[BLKSIZE];

  // serach in-memory minodes first
  for (i=0; i<NMINODE; i++)
  {
    // MINODE *mip = &MINODE[i];
    mip = &minode[i];
    if (mip->refCount && (mip->dev==dev) && (mip->ino==ino))
    {
      mip->refCount++;
      return mip;
    }
  }

  // needed INODE=(dev,ino) not in memory
  mip = mialloc();                        // allocate a FREE minode
  mip->dev = dev; mip->ino = ino;         // assign to (dev, ino)
  block = (ino-1)/8 + iblock;             // disk block containing this inode
  offset= (ino-1)%8;                      // which inode in this block
  get_block(dev, block, buf);
  ip = (INODE *)buf + offset;
  mip->INODE = *ip;                       // copy inode to minode.INODE
  
  // initialize minode
  mip->refCount = 1;
  mip->mounted = 0;
  mip->dirty = 0;
  mip->mptr = 0;
  return mip;

  // Eventually: Return entire MINODE *
}

/*************************************************
 * i put func
 * releases a used minode pointed by mip
 * each minodes has refCount which is num of users using minode
 * iput dec refCount by 1
 * if refCount is nonzero i.e. has other uses, caller simply returns
 * if caller is last user of minode(refCount = 0) INODE is written back to disk
*************************************************/
void iput(MINODE *mip)
{
  // dispose of minode pointed by mip
  // Code in Chapter 11.7.2

  INODE *ip; 
  int i, block, offset;
  char buf[BLKSIZE];

  if(mip == 0)
  {
    return;
  }

  mip->refCount --;                   // dec refCount by 1

  if(mip->refCount > 0)               // still has user
  {
    return;
  }
  if(mip->dirty == 0)                 // no need to write back
  {
    return;
  }

  // write INODE back to disk
  block = (mip->ino - 1) / 8;
  offset = (mip->ino - 1) % 8;

  // get block containing this inode
  get_block(mip->dev, block, buf);
  ip = (INODE*)buf + offset;          // ip points at INODE
  *ip = mip->INODE;                   // copy INODE to inode in block
  put_block(mip->dev, block, buf);    // write back to disk
  midalloc(mip);                      // mip->refCount = 0;
}


int search(MINODE *mip, char *name)
{
  // search for name in (DIRECT) data blocks of mip->INODE
  // return its ino
  // Code in Chapter 11.7.2

  int i;
  char *cp, temp[256], sbuf[BLKSIZE];
  DIR *dp;

  for (i=0; i<12; i++)
  { // search DIR direct blocks only
    if (mip->INODE.i_block[i] == 0)
    {
      return 0;
    }

    get_block(mip->dev, mip->INODE.i_block[i], sbuf);
    dp = (DIR *)sbuf;
    cp = sbuf;

    while (cp < sbuf + BLKSIZE)
    {
      strncpy(temp, dp->name, dp->name_len);                    // dp->name is not a str
      temp[dp->name_len] = 0;                                   // temp is a str

      printf("%8d%8d%8u %s\n",
          dp->inode, dp->rec_len, dp->name_len, temp);          // print temp

      if (strcmp(name, temp)==0)                                // compare names with temp
      {
        printf("found %s : inumber = %d\n", name, dp->inode);
        return dp->inode;
      }

      cp += dp->rec_len;
      dp = (DIR *)cp;
    }
  }
  return 0;

  // Eventually: Return inode number
}


/*************************************************
 * Get ino func
 * Impliments file sys tree traversal algo
 * Returns INODE num(ino) of specified pathname
 * Uses tokenize func to breka up pathname
 * Uses search func to look for token str in dir
 * Essentially returns (dev,ino) of a pathname
*************************************************/
int getino(char *pathname)
{
  // return ino of pathname
  // Code in Chapter 11.7.2

  MINODE *mip;
  int i, ino;

  printf("getino: pathname=%s\n", pathname);

  if (strcmp(pathname, "/")==0)
  {
    return 2; // return root ino=2
  }

  if (pathname[0] == '/')
  {
    mip = root; // if absolute pathname: start from root
  }
  else
  {
    mip = running->cwd; // if relative pathname: start from CWD
  }

  mip->refCount++; // in order to iput(mip) later

  tokenize(pathname); // assume: name[ ], nname are globals

  for (i=0; i<nname; i++)
  { // search for each component string

    printf("===========================================\n");
    printf("getino: i=%d name[%d]=%s\n", i, i, name[i]);

    if (!S_ISDIR(mip->INODE.i_mode))
    { // check DIR type
      printf("%s is not a directory\n", name[i]);
      iput(mip);
      return 0;
    }

    ino = search(mip, name[i]);

    if (ino == 0)
    {
      printf("no such component name %s\n", name[i]);
      iput(mip);
      return 0;
    }

    iput(mip); // release current minode
    mip = iget(dev, ino); // switch to new minode
  }

  iput(mip);
  return ino;
  // Eventually: Return inode number
}

int findmyname(MINODE *parent, u32 myino, char *myname) 
{
  // WRITE YOUR code here:
  // search parent's data block for myino;
  // copy its name STRING to myname[ ];

  int i; 
  char *cp, c, sbuf[BLKSIZE], temp[256];
  DIR *dp;
  INODE *ip;
  MINODE *mip = parent;

  for (i= 0; i < 12; i = i + 1)
  {
     if(mip->INODE.i_block[i] == 0)
     {
       // no remaining info
       return 0;
     }

     // book 330 and search func

     get_block(mip->dev, mip->INODE.i_block[i], sbuf);
     dp = (DIR *)sbuf;
     cp = sbuf;

     while(cp < sbuf + BLKSIZE)
     {
       strncpy(temp, dp->name, dp->name_len);
       temp[dp->name_len] = 0;
       
       
       if(dp->inode == myino)
       {
         strncpy(myname, dp->name, dp->name_len);
         myname[dp->name_len] = 0;
         return 0;
       }

       cp += dp->rec_len;
       dp = (DIR *)cp;
     }
  }
}

int findino(MINODE *mip, u32 *myino) // myino = ino of . return ino of ..
{
  // mip points at a DIR minode
  // WRITE your code here: myino = ino of .  return ino of ..
  // all in i_block[0] of this DIR INODE.

  char buf[BLKSIZE], *temp;
  DIR *dp;
  
  // get i_block[0] of DIR INODE into buf
  get_block(mip->dev, mip->INODE.i_block[0], buf);
  
  temp = buf;
  dp = (DIR *)buf;
  *myino = dp->inode;

  // advance temp by rec_len
  // get next file in dir
  temp = temp + dp->rec_len;
  
  // dp goes to next item
  dp = (DIR *)temp;

  return dp->inode;
}

#endif