/************* cd_ls_pwd.c file **************/

#ifndef __CDLSPWD_C__
#define __CDLSPWD_C__

#include "type.h"
#include "util.c"

extern int dev;

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

int chdir(char *pathname)   
{
  // READ Chapter 11.7.3 HOW TO chdir

  // (1). int ino = getino(pathname);           // return error if ino=0
  // (2). MINODE *mip = iget(dev, ino);
  // (3). Verify mip->INODE is a DIR            // return error if not DIR
  // (4). iput(running->cwd);                   // release old cwd
  // (5). running->cwd = mip;                   // change cwd to mip

  int ino; 
  ino = getino(pathname);                     // step 1.0

  if(ino = 0)
  {
    printf("Error: ino cannot be found \n");  // step 1.1
    return 0;
  }

  MINODE *mip = iget(dev,ino);                // step 2.0

  if(S_ISDIR(mip->INODE.i_mode))              // step 3.0
  {
    iput(running->cwd);                       // step 4.0
    running->cwd = mip;                       // step 5.0
    return 1;
  }
  else                                        // step 3.1
  {
    printf("Error: mip is not a dir \n");
    return 0;
  }
}

int ls_file(MINODE *mip, char *name)
{
  
}

int ls_dir(MINODE *mip)
{
  printf("ls_dir: list CWD's file names; YOU do it for ls -l\n");

  int i, ino, j;

  char buf[BLKSIZE], temp[256];
  
  int ibuf[256], dbuf[256];
  DIR *dp;
  char *cp;

  return 1;
}

int my_ls(char *pathname)  
{
  printf("ls %s\n", pathname);
  printf("YOU do it for ANY pathname\n");

  return 1;
}

void rpwd(MINODE *wd)
{

  // (1). if (wd==root) return;
  // (2). from wd->INODE.i_block[0], get my_ino and parent_ino
  // (3). pip = iget(dev, parent_ino);
  // (4). from pip->INODE.i_block[ ]: get my_name string by my_ino as LOCAL
  // (5). rpwd(pip); // recursive call rpwd(pip) with parent minode
  // (6). print "/%s", my_name;

  int ino, parent_ino;
  char my_name[256];

  if(wd == root)                            // step 1.0
  {
    return;
  }

  parent_ino = findino(wd, &ino);           // step 2.0

  MINODE *pip = iget(dev, parent_ino);      // step 3.0

  findmyname(pip, ino, my_name);            // step 4.0

  rpwd(pip);                                // step 5.0

  printf("%s \n", my_name);                 // step 6.0
  return;

}

char *pwd(MINODE *wd)
{
  if (wd == root) 
  {
    printf("/\n");
  }
  else 
  {
    rpwd(wd);
  }
}
// pwd start:
// pwd(running->cwd);


#endif