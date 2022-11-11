/****************************************************************************
*              YOUR_INITIALS testing ext2 file system                       *
*****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <sys/stat.h>
#include <time.h>

#include "type.h"
#include "cd_ls_pwd.c"
#include "symlink.c"
#include "write_cp.c"
#include "mount_umount.c"
#include "misc1.c"

char gpath[128]; // global for tokenized components
char *pname[32];  // assume at most 32 components in pathname
int   n;         // number of component strings
char pathname[128];

int fd;
int dev;

int nblocks, ninodes, bmap, imap, iblk;

MINODE *iget();

int fs_init()
{
  int i,j;
  for (i=0; i<NMINODE; i++) // initialize all minodes as FREE
    minode[i].refCount = 0;

  for (i=0; i<NMTABLE; i++) // initialize mtables as FREE
    mtable[i].dev = 0;

  for (i=0; i<NOFT; i++) // initialize ofts as FREE
    oft[i].refCount = 0;

  for (i=0; i<NPROC; i++){ // initialize PROCs

    proc[i].status = READY; // ready to run

    proc[i].pid = i; // pid = 0 to NPROC-1

    proc[i].uid = i; // P0 is a superuser process

    for (j=0; j<NFD; j++)
      proc[i].fd[j] = 0; // all file descriptors are NULL
  
    proc[i].next = &proc[i+1]; // link list
  }

  proc[NPROC-1].next = &proc[0]; // circular list

  running = &proc[0]; // P0 runs first
}

int mount_root(char *rootdev)
{
  // int i;
  // MTABLE *mp;
  // SUPER *sp;
  // GD *gp;
  // char buf[BLKSIZE];

  // globalDev = open(rootdev, O_RDWR);

  // if (globalDev < 0){
  //   printf("panic : can’t open root device\n");
  //   exit(1);
  // }

  // /* get super block of rootdev */
  // get_block(globalDev, 1, buf);
  // sp = (SUPER *)buf;

  // /* check magic number */
  // if (sp->s_magic != SUPER_MAGIC){
  //   printf("super magic=%x : %s is not an EXT2 filesys\n",
  //   sp->s_magic, rootdev);
  //   exit(0);
  // }

  // // fill mount table mtable[0] with rootdev information
  // mp = &mtable[0]; // use mtable[0]
  // mp->dev = globalDev;

  // // copy super block info into mtable[0]
  // ninodes = mp->ninodes = sp->s_inodes_count;
  // nblocks = mp->nblocks = sp->s_blocks_count;
  // strcpy(mp->devName, rootdev);
  // strcpy(mp->mntName, "/");
  // get_block(globalDev, 2, buf);
  // gp = (GD *)buf;
  // bmap = mp->bmap = gp->bg_block_bitmap;
  // imap = mp->imap = gp->bg_inode_bitmap;
  // iblock = mp->iblock = gp->bg_inode_table;
  // printf("bmap=%d imap=%d iblock=%d\n", bmap, imap, iblock);

  // // call iget(), which inc minode’s refCount
  
  // root = iget(globalDev, 2); // get root inode
  // mp->mntDirPtr = root; // double link
  // root->mptr = mp;

  // // set proc CWDs
  // for (i=0; i<NPROC; i++) // set proc’s CWD
  //   proc[i].cwd = iget(globalDev, 2); // each inc refCount by 1

  // printf("mount : %s mounted on / \n", rootdev);
  // return 0;

  root = iget(dev, 2);
}

int quit()
{
  int i;

  for (i=0; i<NMINODE; i++)
  {
    MINODE *mip = &minode[i];
    if (mip->refCount && mip->dirty)
    {
      mip->refCount = 1;
      iput(mip);
    }
  }

  exit(0);
}

int main(int argc, char *argv[ ])
{
  int ino, linefd, linep, lineposition, linenbytes, lineuid, mode;
  char buf[BLKSIZE];
  char line[128], cmd[32];

  char pathname2[128];

  char diskname[128];

  char *disk = rootdev;

  char *old_file = strtok (pathname, "");
  char *new_file = strtok(NULL, "");

  printf("checking EXT2 FS ... ");
  if((fd = open(disk, O_RDWR)) < 0)
  {
    printf("open %s failed \n", disk);
    exit(1);
  }

  dev = fd;                               // global dev same as this fd

  /********** read super block  ****************/
  get_block(dev, 1, buf);
  sp = (SUPER *)buf;

  if (argc > 1)
  {
    disk = argv[1];

    rootdev = disk;
  }

  /*********** verify it's an ext2 file system ***********/
  if (sp->s_magic != 0xEF53){
      printf("magic = %x is not an ext2 filesystem\n", sp->s_magic);
      exit(1);
  }     
  printf("EXT2 FS OK\n");
  ninodes = sp->s_inodes_count;
  nblocks = sp->s_blocks_count;

  get_block(dev, 2, buf); 
  gp = (GD *)buf;

  bmap = gp->bg_block_bitmap;
  imap = gp->bg_inode_bitmap;
  iblk = gp->bg_inode_table;
  printf("bmp=%d imap=%d inode_start = %d\n", bmap, imap, iblk);

  mount_root(rootdev);
  printf("root refCount = %d\n", root->refCount);

  printf("creating P0 as running process\n");
  running = &proc[0];
  running->cwd = iget(dev, 2);
  printf("root refCount = %d\n", root->refCount);

  // printf("\n\n\nIf you've gotten here, congratulations! The program has compiled.\n\n");
  // printf("There are several utility functions that are not yet complete-\nwhich means beyond this point, you won't be able to properly load up the filesystem.\n\n");
  // printf("Take a look around at the comments to figure out what's missing-\nwhere you can find it, and what you're going to have to do yourself.\n\n");
  // printf("Good luck and take care!\n\n\n\n");

  while(1){
    printf("\n[ls|cd|pwd|mkdir|rmdir|creat|link|unlink|symlink|readlink|quit] \n");
    printf("input command : ");
    fgets(line, 128, stdin);
    line[strlen(line)-1] = 0;


    if (line[0]==0)
       continue;
    pathname[0] = 0;

    sscanf(line, "%s %s", cmd, pathname);
    printf("cmd=%s pathname=%s\n", cmd, pathname);
    
    if (!strcmp(cmd, "ls"))
      my_ls(pathname);
    if (!strcmp(cmd, "cd"))
      chdir(pathname);
    if (!strcmp(cmd, "pwd"))
      pwd(running->cwd);
    if (strcmp(cmd, "mkdir"))
      return 0;
    if (strcmp(cmd, "rmdir"))
      return 0;
    if (strcmp(cmd, "creat"))
      return 0;
    if (strcmp(cmd, "link"))
      my_link(old_file,new_file);
    if (strcmp(cmd, "unlink"))
      my_unlink(pathname);
    if (strcmp(cmd, "symlink"))
       return 0;
    if (strcmp(cmd, "readlink"))
      return 0;
    if (!strcmp(cmd, "quit"))
      quit();
  }
}
