#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define UNIX
#define SORT
#define FLW 0
#define REV 1

#define MREN 512

#ifdef UNIX
#include <dirent.h>
#else
#include "windows.h"
#endif

/**  **/
int getDirectory(int,int,FILE *,int,char *,FILE *);
int readOpenPose(int,char *,FILE *);
int comSpaceCut(char *);
static int mojiSorter(int, char **,int);

int main(argc,argv)
int argc;
char *argv[];
{
    FILE *fo,*fd;
    int no;
    int kind;

    if(argc <= 4) {
      fprintf(stderr,"USAGE kind readFolder folderName outFname\n");
      fprintf(stderr,"  kind == pose  left Pose\n");
      fprintf(stderr,"  kind == face  Face\n");
      fprintf(stderr,"  kind == right Right Hand\n");
      fprintf(stderr,"  kind == left  Left Hand\n");
      exit(-9);
    }

         if(!strcmp("pose", argv[1])) kind=0;
    else if(!strcmp("face", argv[1])) kind=1;
    else if(!strcmp("right",argv[1])) kind=2;
    else if(!strcmp("left", argv[1])) kind=3;
    else {
      fprintf(stderr,"cannot use kind Name=[%s]\n",argv[1]);
      exit(-9);
    }

    if(!(fo=fopen(argv[3],"w"))) {
      fprintf(stderr,"cannot write file=[%s]\n",argv[3]);
      exit(-1);
    }
    if(argc == 5) {
      if(!(fd=fopen(argv[4],"w"))) {
        fprintf(stderr,"cannot write openPose file=[%s]\n",argv[4]);
        exit(-1);
      }
    }
    else {
      fd = NULL;
    }
    no=getDirectory(0,kind,fo,0,argv[2],fd);
    fprintf(fo,"--end-- %d number of files under=%s\n",no,argv[2]);

    fclose(fo);
    if(fd) fclose(fd);
 
  return (0);
}
/******************
  get Directory
*******************/
int getDirectory(int no,int kind,FILE *fo,int num,char *fname,FILE *fd)
{
    int i;
    int len;
    char ext[256*2];
    int id;
    int ret;
    static char *fileKind[4]={"pose.yml","face.yml","hand_right.yml","hand_left.yml"};
    int cnt;
    int lenKind[4],elen;
#ifdef SORT
    char *pathList[1024*8];
#endif

#ifdef UNIX
  	DIR *dir;
  	struct dirent *dp;
    char path[256];
#else
	  WIN32_FIND_DATA lp;
	  HANDLE h;
    char path[256];
    int n;
#endif

    for(i=0;i<4;i++) {
      lenKind[i] = strlen(fileKind[i]);
    }
    cnt=0;
#ifdef UNIX
	  dir=opendir(fname);
    if(!dir) return(-1);

	  for(dp=readdir(dir);dp!=NULL;dp=readdir(dir)){
		  printf("%s\n",dp->d_name);
      if((dp->d_type == DT_DIR) && 
         (strcmp(dp->d_name,"..")!=0 && strcmp(dp->d_name,".")!=0)) {
        /* directory and not .. or . */
        strcpy(path,fname);
        strcat(path,"/");
        strcat(path,dp->d_name);
        for(i=0;i<num;i++) fprintf(fo,"-");
        fprintf(fo,"(DIR)%s/%s\n",fname,dp->d_name);
        /* next sub Directory */
        no=getDirectory(no,kind,fo,num+1,path,fd);
      }
      else
      if(strcmp(dp->d_name,"..")!=0 && strcmp(dp->d_name,".")!=0) {
        /* regular file name */
        no++;
        for(i=0;i<num;i++) fprintf(fo,"-");
        fprintf(fo,"%s/%s",fname,dp->d_name);   /* 改行無し */
        len=strlen(dp->d_name);
        //if(fd && !strcmp(&lp.cFileName[len-14],"hand_right.yml")) {
        if(fd && !strcmp(&dp->d_name[len-lenKind[kind]],fileKind[kind])) {
          /* OpenPose data name format= XXXXX_00000000NN_pose.yml */
          strcpy(path,fname);
          strcat(path,"/");
          strcat(path,dp->d_name);
#ifdef SORT

          pathList[cnt]=(char *)malloc(strlen(path)+1);
          strcpy(pathList[cnt],path);
          cnt++;
#else
          strcpy(ext,dp->d_name);
          elen=strlen(fileKind[kind]);
          ext[len-elen-1]='\0';
          len=strlen(ext);
          id = atoi(&ext[len-12]);

          ret=readOpenPose(id,path,fd);
          if(ret < 0) fprintf(fo,"!!!ERROR cannot read");
          else        fprintf(fo,"::readed");
#endif
        }
        fprintf(fo,"\n");  /* 改行 */
      }

    }

	  closedir(dir);
#else
    n=0;
    strcpy(path,fname);
    strcat(path,"\\*.*");  /* wild card */
    /* scan start */
    if ((h = (void *)FindFirstFile(path,&lp)) == -1) return 1;
    do {
      if((lp.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && 
         (strcmp(lp.cFileName,"..")!=0 && strcmp(lp.cFileName,".")!=0)) {
        /* directory and not .. or . */
        strcpy(path,fname);
        strcat(path,"\\");
        strcat(path,lp.cFileName);
        for(i=0;i<num;i++) fprintf(fo,"-");
        fprintf(fo,"(DIR)%s\\%s\n",fname,lp.cFileName);
        /* next sub Directory */
        no=getDirectory(no,kind,fo,num+1,path,fd);
      }
      else
      if(strcmp(lp.cFileName,"..")!=0 && strcmp(lp.cFileName,".")!=0) {
        /* regular file name */
        no++;
        for(i=0;i<num;i++) fprintf(fo,"-");
        fprintf(fo,"%s\\%s",fname,lp.cFileName);   /* 改行無し */
        len=strlen(lp.cFileName);
        //if(fd && !strcmp(&lp.cFileName[len-14],"hand_right.yml")) {
        if(fd && !strcmp(&lp.cFileName[len-lenKind[kind]],fileKind[kind])) {
          /* OpenPose data name format= XXXXX_00000000NN_pose.yml */
          strcpy(path,fname);
          strcat(path,"\\");
          strcat(path,lp.cFileName);

#ifdef SORT

          pathList[cnt]=(char *)malloc(strlen(path)+1);
          strcpy(pathList[cnt],path);
          cnt++;
#else
          strcpy(ext,lp.cFileName);
          elen=strlen(fileKind[kind]);
          ext[len-elen-1]='\0';
          len=strlen(ext);
          id = atoi(&ext[len-12]);

          ret=readOpenPose(id,path,fd);
          if(ret < 0) fprintf(fo,"!!!ERROR cannot read");
          else        fprintf(fo,"::readed");
#endif
        }
        fprintf(fo,"\n");  /* 改行 */
      }

    }  while(FindNextFile(h,&lp));  /* next scan */
    /* file scan close */
    FindClose(h);
#endif

#ifdef SORT
    if(cnt) mojiSorter(cnt,pathList,FLW);
    for(i=0;i<cnt;i++) {
      len=strlen(pathList[i]);
      strcpy(ext,pathList[i]);
      elen=strlen(fileKind[kind]);
      ext[len-elen-1]='\0';
      len=strlen(ext);
      id = atoi(&ext[len-12]);
      //printf("id=%d ext=%s\n",id,ext); 

      ret=readOpenPose(i,pathList[i],fd);
      if(ret < 0) fprintf(fo,"!!!ERROR cannot read[%s]\n",pathList[i]);
      else        fprintf(fo,"::readed");
      //free(pathList[i]);
    }
    for(i=0;i<cnt;i++) free(pathList[i]);
#endif
    return(no);
}
/****************
  Read Pose File
*****************/
int readOpenPose(id,fname,fd)
int id;
char *fname;
FILE *fd;
{
   FILE *fp;
   char record[MREN],*pc,*adr;
   int i,num;
   double data[18*3*10];
   int size[3*10];
   char word[128];

   if(!(fp = fopen(fname,"r"))) {
     return(-1);
   }

   while(fgets(record,MREN,fp)) {
     record[strlen(record)-1]='\0';
     pc=strtok(record,":");
     while(pc) {
       strcpy(word,pc);
       comSpaceCut(word);
       if(!strcmp(word,"data")) {
         pc=strtok(NULL,"[");
         pc=strtok(NULL,",");
         i=0;
         while(1) {
           while(pc) {
             //strcpy(word,pc);
             //comSpaceCut(word);
             if((adr=strchr(pc,(int)']'))) {
               *adr='\0';
               data[i++]=atof(pc);
               num=i;
               break;
             }
             data[i++]=atof(pc);
             pc=strtok(NULL,",");
           }
           if(!fgets(record,MREN,fp)) {
             goto readEnd;
           }
           record[strlen(record)-1]='\0';
           pc=strtok(record,",");
         }
       }

       if(!strcmp(word,"sizes")) {
         pc=strtok(NULL,"[");
         pc=strtok(NULL,",");
         i=0;
         while(pc) {
           //strcpy(word,pc);
           //comSpaceCut(word);
           //if(!strcmp(word,"]")) break;
           size[i++]=atoi(pc);
           pc=strtok(NULL,",]");
         }
       }
       pc = strtok(NULL,":");
     }
   }
readEnd:
   fclose(fp);

   /* 書き出し */
   fprintf(fd,"%3d,",id);
   /* There is both types which are size[0] = 1 or 2 */
   //if(size[0] != 1) num=54;
   for(i=0;i < num;i++) {
     fprintf(fd,"%lf",data[i]);
     if(i < num-1) fprintf(fd,",");
   }
   fprintf(fd,"\n");

   return(0);

}
/***************
  space cut
****************/
int comSpaceCut(str)
char *str;
{
    int i=0,j;

    while(str[i] != '\0') {
      if(str[i] == ' ') {
        j=i+1;
        while(str[j] == ' ') j++;
        str[i]=str[j];
        str[j]=' ';
      }
      if(str[i] == '\0') break;
      i++;
    }
    return(i);

}
/**************
  sorter
***************/
static int mojiSorter(n,moji,order)
int n;
char *moji[];
int  order;
{
   int gap,i,j;
   char  *cwork;

   for(gap = n/2;gap > 0;gap /= 2) {
     for(i = gap;i < n; i++) { 
       for(j = i-gap; j >= 0; j -= gap) {
    	 if((order == FLW && strcmp(moji[j],moji[j+gap]) <= 0) ||
	        (order != FLW && strcmp(moji[j],moji[j+gap]) >  0)) break;
         cwork = moji[j];
         moji[j] = moji[j+gap];
         moji[j+gap]=cwork;
       } 
     }
   }
   return(n);
}
