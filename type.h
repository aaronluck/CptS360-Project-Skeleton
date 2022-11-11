/*************** type.h file for LEVEL-1 ****************/

// book 336

#ifndef __TYPE_H__
#define __TYPE_H__

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <ext2fs/ext2_fs.h>
#include <string.h>
#include <libgen.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

typedef unsigned char  u8;
typedef unsigned short u16;
typedef unsigned int   u32;

typedef struct ext2_super_block SUPER;
typedef struct ext2_group_desc  GD;
typedef struct ext2_inode       INODE;
typedef struct ext2_dir_entry_2 DIR;
#define BLKSIZE  1024

SUPER *sp;
GD    *gp;
INODE *ip;
DIR   *dp;   

// Define block numbers of important EXT2 file system blocks on the disk
#define SUPERBLOCK  1
#define GDBLOCK     2
#define ROOT_INODE  2

// Default directories and regular file modes
#define DIR_MODE    0x41ED
#define FILE_MODE   0xB1AE
#define SUPER_MAGIC 0xEF53
#define SUPER_USER  0

// Define process statuses
#define FREE        0 // A free process has status 0
#define BUSY        1 // A busy process has status 1
#define READY       1 // ? (it started as 1)

// File system table sizes
#define NMINODE   100
#define NMTABLE    10
#define NPROC       2
#define NFD        16
#define NOFT       40

typedef struct minode{
  INODE INODE;           // INODE structure on disk
  int dev, ino;          // (dev, ino) of INODE
  int refCount;          // in use count
  int dirty;             // 0 for clean, 1 for modified
  int mounted;           // for level-3
  struct mtable *mptr;  // for level-3
}MINODE;

// Open File Table
typedef struct oft{
int mode;
int refCount;
struct minode *minodePtr;
int offset;
}OFT; 

typedef struct proc{
  struct proc *next;
  int          pid;      // process ID  
  int          uid;      // user ID
  int          gid;
  int          ppid;
  int          status;
  MINODE       *cwd;     // CWD directory pointer
  OFT          *fd[NFD];  
}PROC;


// book 337
typedef struct mtable{
  int dev;              // device number; 0 for FREE
  int ninodes;          // from superblock
  int nblocks;
  int free_blocks;      // from superblock and GD
  int free_inodes;
  int bmap;             // from group descriptor
  int imap;
  int iblock;           // inodes start block
  MINODE *mntDirPtr;    // mount point DIR pointer
  char devName[64];     //device name
  char mntName[64];     // mount point DIR name
}MTABLE;

MINODE minode[NMINODE]; // In memory inodes
MINODE *root; // Root minode
MTABLE mtable[NMTABLE]; // Mount tables

OFT oft[NOFT]; // Opened file instances
PROC proc[NPROC]; // Process structures
PROC *running; //Currently running (executing) process

int ninodes, nblocks, bmap, imap, iblock;
int globalDev;

char *name[64]; // token string pointers
char gline[256]; // holds token strings, each pointed to by a name[i]

int nname; // number of token strings
int inode_start; // Inode start block

char *rootdev = "vdisk"; // Default root_device

#endif