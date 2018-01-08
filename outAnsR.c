#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define RTN  return
//#define RTN  exit

int main(argc,argv)
int argc;
char *argv[];
{
  int i;
  FILE *fp;
  char *p,record[2048];
  double val[32];
  double conf;

  if(argc < 4) {
    fprintf(stderr,"command anaFile confidence ITEM(SOLD,NOSE,MOUS,EYES,LFIN,RFIN,LHND,RHND)\n");
    fprintf(stderr,"Return 0:UNKOWN 1:NOGOOD 2:GOOD\n");
    RTN(-99);
  }
  if(!(fp=fopen(argv[1],"r"))) {
    fprintf(stderr,"cannnot read inputFile=[%s]\n",argv[3]);
    RTN(-98);
  }
  conf=atof(argv[2]);
  if(!conf) conf=0.5;


  val[0]=0;
  val[1]=0;
  i=-1;
  while(fgets(record,2048,fp)) {
    record[strlen(record)-1]='\0';
    p=strtok(record,", ");
    if(strcmp(p,argv[3]) != 0) continue;
    p=strtok(NULL,", ");
    i=0;
    while(p) {
      val[i]=atof(p);
      p=strtok(NULL,", ");
      i++;
    }
  }
  if(i < 0) {
    fprintf(stderr,"cannot Find item=[%s] in (SOLD,MOUS,EYES,LFIN,RFIN,LHND,RHND)\n",argv[3]);
    RTN(-97);
  }

  if(fabs(val[1]) <= conf) {
    fprintf(stderr,"%s UNKOWN %lf\n",argv[3],val[1]);
    RTN(0);
  }
  else {
    if(val[1] > 0) {
      fprintf(stderr,"%s GOOD %lf\n",argv[3],val[1]);
      RTN(2);
    }
    else  {
      fprintf(stderr,"%s NOGOOD %lf\n",argv[3],val[1]);
      RTN(1);
    }
  }
}

    
  
