/************* cd_ls_pwd.c file **************/

#ifndef __CDLSPWD_C__
#define __CDLSPWD_C__

#include "type.h"
#include "util.c"

char *t1 = "xwrxwrxwr-------";
char *t2 = "----------------";

int chdir(char *pathname)   
{
  printf("under construction READ textbook HOW TO chdir!!!!\n");
  // READ Chapter 11.7.3 HOW TO chdir

  return 1;
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

char *rpwd(MINODE *wd)
{
  printf("pwd: READ HOW TO pwd in textbook!!!!\n");
}

char *pwd(MINODE *wd)
{
  printf("pwd: READ HOW TO pwd in textbook!!!!\n");
}

#endif