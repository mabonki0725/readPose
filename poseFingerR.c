#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>

//#defin UNIX
#define M_REC 1024*8
#define SIZER  1
#define READER 2
#define PI     3.14159263358

int readFile(int,FILE *,double **,int *,int *);

int main(argc,argv)
int argc;
char *argv[];
{
  FILE *fw,*fParm,*fPose,*fLeft,*fRight,*fFace,*fa;
  int i,j,n;
  double **pose;
  double **left;
  double **right;
  double **face;
  int maxPi,maxPj;
  int maxLi,maxLj;
  int maxRi,maxRj;
  int maxFi,maxFj;
  double /* prob,permit,*/ rate;
  int na;

  double xBig,yBig,pBig,xLit,yLit,pLit,angle;
  double xLsold,yLsold,pLsold,xRsold,yRsold,pRsold;
  double xLmouse,yLmouse,pLmouse,xRmouse,yRmouse,pRmouse;
  double xLeye,yLeye,pLeye,xReye,yReye,pReye,xNose,yNose,pNose;
  double distR,distL;
  double xRhand,yRhand,pRhand,xLhand,yLhand,pLhand;
  double xNoseTop,yNoseTop,pNoseTop,xNoseBot,yNoseBot,pNoseBot,angleM,angleN;

  char *p,record[M_REC];
  double *param,parmSolder[4],parmMouse[4],parmEye[4],parmLHand[4],parmRHand[4],parmLFinger[4],parmRFinger[4];
  double parmNose[4];  
  char *pSide;
  int solderP,noseP,mouseP,eyeP,leftFingerP,rightFingerP,leftHandP,rightHandP;

  if(argc < 8) {
    fprintf(stderr,"USAGE command paramFile poseFile faceFile leftHandFile rightHandFile outFile anaFile\n");
    return(-9);
  }
  /**
  prob = atof(argv[1])/100.0;
  permit = atof(argv[2]);
  **/

  na=0;

  if(!(fParm=fopen(argv[++na],"r"))) {
    fprintf(stderr,"cannot read paramFile=[%s]\n",argv[na]);
    return(-11);
  }
  if(!(fPose=fopen(argv[++na],"r"))) {
    fprintf(stderr,"cannot read poseFile=[%s]\n",argv[na]);
    return(-11);
  }
  if(!(fFace=fopen(argv[++na],"r"))) {
    fprintf(stderr,"cannot read faceFile=[%s]\n",argv[na]);
    return(-13);
  }
  if(!(fLeft=fopen(argv[++na],"r"))) {
    fprintf(stderr,"cannot read Left fingerFile=[%s]\n",argv[na]);
    return(-14);
  }
  if(!(fRight=fopen(argv[++na],"r"))) {
    fprintf(stderr,"cannot read Right fingerFile=[%s]\n",argv[na]);
    return(-15);
  }


  if(!(fw=fopen(argv[++na],"w"))) {
    fprintf(stderr,"cannot write outFile=[%s]\n",argv[na]);
    return(-21);
  }
  if(!(fa=fopen(argv[++na],"w"))) {
    fprintf(stderr,"cannot write outFile=[%s]\n",argv[na]);
    return(-22);
  }

  /* Read Param File */
  parmSolder [0]=0.5;
  parmNose   [0]=0.5;
  parmMouse  [0]=0.5;
  parmEye    [0]=0.5;
  parmLHand  [0]=0.5;
  parmRHand  [0]=0.5;
  parmLFinger[0]=0.5;
  parmRFinger[0]=0.5;

  parmSolder [1]=5;
  parmNose   [1]=5;
  parmMouse  [1]=5;
  parmEye    [1]=0.2;
  parmLHand  [1]=100;
  parmRHand  [1]=100;
  parmLFinger[1]=5;
  parmRFinger[1]=5;

  parmSolder [2]=10;
  parmNose   [2]=10;
  parmMouse  [2]=10;
  parmEye    [2]=10;
  parmLHand  [2]=10;
  parmRHand  [2]=10;
  parmLFinger[2]=10;
  parmRFinger[2]=10;

  while(fgets(record,M_REC,fParm)) {
    record[strlen(record)-1]='\0';
    printf("record=%s\n",record);
    if(record[0] == '#') continue;
    p=strtok(record,", ");
    if(!strcmp(p,"SOLDER"))      param=parmSolder;
    if(!strcmp(p,"Nose"))        param=parmNose;
    if(!strcmp(p,"MOUSE"))       param=parmMouse;
    if(!strcmp(p,"EYE"))         param=parmEye;
    if(!strcmp(p,"LEFTHAND"))    param=parmLHand;
    if(!strcmp(p,"RIGHTHAND"))   param=parmRHand;
    if(!strcmp(p,"LEFTFINGER"))  param=parmLFinger;
    if(!strcmp(p,"RIGHTFINGER")) param=parmRFinger;

    p=strtok(NULL,", ");      
    j=0;
    while(p) {
      param[j]=atof(p);
      p=strtok(NULL,", ");
      j++;
    }
  }
  

  readFile(SIZER,fPose,NULL,&maxPi,&maxPj);
  pose=(double **)malloc(sizeof(double *)*maxPi);
  for(i=0;i<maxPi;i++) {
    pose[i]=(double *)malloc(sizeof(double)*maxPj);
    for(j=0;j<maxPj;j++) pose[i][j]=0.0;
  }
  readFile(READER,fPose,pose,NULL,NULL);
  fclose(fPose);
  printf("read pose %d %d\n",maxPi,maxPj);

  readFile(SIZER,fFace,NULL,&maxFi,&maxFj);
  face=(double **)malloc(sizeof(double *)*maxFi);
  for(i=0;i<maxFi;i++) {
    face[i]=(double *)malloc(sizeof(double)*maxFj);
    for(j=0;j<maxFj;j++) face[i][j]=0.0;
  }
  readFile(READER,fFace,face,NULL,NULL);
  fclose(fFace);
  printf("read face %d %d\n",maxFi,maxFj);

  readFile(SIZER,fLeft,NULL,&maxLi,&maxLj);
  left=(double **)malloc(sizeof(double *)*maxLi);
  for(i=0;i<maxLi;i++) {
    left[i]=(double *)malloc(sizeof(double)*maxLj);
    for(j=0;j<maxLj;j++) left[i][j]=0.0;
  }
  readFile(READER,fLeft,left,NULL,NULL);
  fclose(fLeft);
  printf("read Left %d %d\n",maxLi,maxLj);

  readFile(SIZER,fRight,NULL,&maxRi,&maxRj);
  right=(double **)malloc(sizeof(double *)*maxRi);
  for(i=0;i<maxRi;i++) {
    right[i]=(double *)malloc(sizeof(double)*maxRj);
    for(j=0;j<maxRj;j++) right[i][j]=0.0;
  }
  readFile(READER,fRight,right,NULL,NULL);
  fclose(fRight);
  printf("read Right %d %d\n",maxRi,maxRj);


  
  if(maxLi != maxRi) {
    fprintf(stderr,"WARNING Left length[%d] not equal Right length[%d]\n",maxLi,maxRi);
  }
  if(maxPi != maxLi) {
    fprintf(stderr,"WARNING Pose length[%d] not equal Left length[%d]\n",maxPi,maxLi);
  }
  if(maxPi != maxRi) {
    fprintf(stderr,"WARNING Pose length[%d] not equal Right length[%d]\n",maxPi,maxRi);
  }
  if(maxPi != maxFi) {
    fprintf(stderr,"WARNING Pose length[%d] not equal Face length[%d]\n",maxPi,maxFi);
  }

  n = maxPi;
  if(maxLi < n) n=maxLi;
  if(maxRi < n) n=maxRi;
  if(maxFi < n) n=maxFi;


  solderP=noseP=mouseP=eyeP=leftFingerP=rightFingerP=leftHandP=rightHandP=0;

  /**
  for(i=0;i<n;i++) {
    for(j=0;j<maxPj;j++) {
      fprintf(fw,"%lf",pose[i][j]);
      if(j < maxPj -1) fprintf(fw,",");
      else             fprintf(fw,",");
    }
    for(j=0;j<maxFj;j++) {
      fprintf(fw,"%lf",face[i][j]);
      if(j < maxFj -1) fprintf(fw,",");
      else             fprintf(fw,",");
    }
    for(j=0;j<maxLj;j++) {
      fprintf(fw,"%lf",left[i][j]);
      if(j < maxLj -1) fprintf(fw,",");
      else             fprintf(fw,",");
    }
    for(j=0;j<maxRj;j++) {
      fprintf(fw,"%lf",right[i][j]);
      if(j < maxRj -1) fprintf(fw,",");
      else             fprintf(fw,"\n");
    }

  }
  fclose(fw);
  **/

  /****
    parallel test;
  ****/
  /***
   POSE  Data 55 =(17+1)*3+1
   LEFT  Data 64 =(20+1)*3+1
   RIGHT DATA 64 =(20+1)*3+1
   FACE  DATA 211=(69+1)*3+1

   Left Solder  #2   8 =(1+1)*3+1+1
   Right Solder #5   17=(1+4)*3+1+1

   big pos    #4     14 =(1+ 3)*3+1+1 
   little pos #20    62 =(1+19)*3+1+1 
   Left   BIG    69 =55+14     
   Right  BIG    133=55+64+62  
   Left   Lettle 117=55+62    
   Right  Lettle 181=55+64+62  

   LeftEye    #36 #68 #39  206=(1+67)*3+1+1
   RightEye   #42 #69 #45  209=(1+68)*3+1+1
   Nose       #28  86 =(1+27)*3+1+1
   LeftMouse  #48  146=(1+47)*3+1+1    
   RightMouse #54  164=(1+53)*3+1+1

   LeftHand   #4     14 =(1+3)*3+1+1
   RightHand  #7     23 =(1+6)*3+1+1
  ****/

  /*Check Left up down */
  for(i=0;i<n;i++) {
    /* solder */
    xRsold=pose[i][7];
    yRsold=pose[i][8];
    pRsold=pose[i][9];
    xLsold=pose[i][16];
    yLsold=pose[i][17];
    pLsold=pose[i][18];

    if(pRsold < parmSolder[0] || pLsold < parmSolder[0]) {
      pSide="S_UK";
      angle = 0;
    }
    else {
      angle = atan(fabs(yRsold - yLsold)/(xRsold - xLsold))/PI*180.0;
      if(fabs(angle) < parmSolder[1]) pSide="S_OK";
      else                            pSide="S_NG";

      if(i >= n-parmSolder[2]) {
        if(!strcmp(&pSide[strlen(pSide)-2],"OK")) solderP++;
        else                                      solderP--;
      }

    }
    fprintf(fa,"%lf,%lf,%lf,%s,",yLsold,yRsold,angle,pSide);

    /* nose 27 28 29 30 51 57 to mouse 48 54*/
    xNoseTop=face[i][85]; /* 28 */
    yNoseTop=face[i][86];
    pNoseTop=face[i][87];

    /* 30 
    xNoseBot=face[i][91]; 
    yNoseBot=face[i][92];
    pNoseBot=face[i][93]; */

    /* 57 */
    xNoseBot=face[i][172]; 
    yNoseBot=face[i][173];
    pNoseBot=face[i][174];

    if(pNoseTop < parmMouse[0] || pNoseBot < parmMouse[0] ) {
      pSide = "Nos_UK";
      xNoseTop=0;
      xNoseBot=0;
    }
    else {
      angleN = atan(fabs(yNoseTop - yNoseBot)/(xNoseTop - xNoseBot))/PI*180.0;
      if(fabs(angle) -90.0 < parmNose[1]) pSide="Nos_OK";
      else                                pSide="Nos_NG";

      if(i >= n-parmNose[2]) {
        if(!strcmp(&pSide[strlen(pSide)-2],"OK")) noseP++;
        else                                      noseP--;
      }  

    }
    fprintf(fa,"%lf,%lf,%lf,%s,",xNoseTop,xNoseBot,angle,pSide);

    xLmouse=face[i][145];
    yLmouse=face[i][146];
    pLmouse=face[i][147];
    xRmouse=face[i][163];
    yRmouse=face[i][164];
    pRmouse=face[i][165];

    if(pRmouse  < parmMouse[0] || 
       pLmouse  < parmMouse[0] || 
       pNoseTop < parmNose[0]  ||
       pNoseBot < parmNose[0]  ) {
      pSide="M_UK";
      angle = 0;
      yLmouse=0;
      yRmouse=0;
    }
    else {
      angleM = atan(fabs(yLmouse  - yRmouse )/(xLmouse  - xRmouse ))/PI*180.0;
      angleN = atan(fabs(yNoseTop - yNoseBot)/(xNoseTop - xNoseBot))/PI*180.0;
      angle  = fabs(angleN + angleM) - 90.0;
      if(fabs(angle) < parmMouse[1]) pSide="M_OK";
      else                           pSide="M_NG";

      if(i >= n-parmMouse[2]) {
        if(!strcmp(&pSide[strlen(pSide)-2],"OK")) mouseP++;
        else                                      mouseP--;
      }

    }
    fprintf(fa,"%lf,%lf,%lf,%s,",yLmouse,yRmouse,angle,pSide);

    /* eye */
    xLeye=face[i][205];
    yLeye=face[i][206];
    pLeye=face[i][207];
    xReye=face[i][208];
    yReye=face[i][209];
    pReye=face[i][210];

    xNose=face[i][ 85];
    yNose=face[i][ 86];
    pNose=face[i][ 87];

    if(pNose < parmEye[0] || pLeye < parmEye[0] || pReye < parmEye[0]) {
      pSide = "Eye_UK";
      distL=0;
      distR=0;
    }
    else {
      distL=pow(pow((xNose-xLeye),2)+pow((yNose-yLeye),2),0.5);
      distR=pow(pow((xNose-xReye),2)+pow((yNose-yReye),2),0.5);
      if(distR) {
        rate =fabs(distL-distR)/distR;
        if(rate < parmEye[1] ) pSide="Eye_OK";
        else                   pSide="Eye_NG";
      }
      else {
        pSide="Eye_UK";
      }

      if(i >= n-parmEye[2]) {
        if(!strcmp(&pSide[strlen(pSide)-2],"OK")) eyeP++;
        else                                      eyeP--;
      }  

    }
    fprintf(fa,"%lf,%lf,%lf,%s,",distL,distR,rate,pSide);

    /* LeftFinger */
    xBig=left[i][13];
    yBig=left[i][14];
    pBig=left[i][15];
    xLit=left[i][61];
    yLit=left[i][62];
    pLit=left[i][63];
    if(pBig < parmLFinger[0] || pLit < parmLFinger[0]) {
      pSide="LF_UK";
      xBig=0.0;
      xLit=0.0;
    }
    else {
      if(xBig < xLit) pSide="LF_DW";
      else            pSide="LF_UP";

      if(i >= n-parmLFinger[2]) {
        if(!strcmp(&pSide[strlen(pSide)-2],"OK")) leftFingerP++;
        else                                      leftFingerP--;
      } 

    }

    fprintf(fa,"%lf,%lf,%s,",xBig,xLit,pSide);


    /* RightFinger */
    xBig=right[i][13];
    yBig=right[i][14];
    pBig=right[i][15];
    xLit=right[i][61];
    yLit=right[i][62];
    pLit=right[i][63];
    if(pBig < parmRFinger[0] || pLit < parmRFinger[0]) {
      pSide="RF_UK";
      xBig=0.0;
      yBig=0.0;
    }
    else {
      if(xBig < xLit) pSide="RF_UP";
      else            pSide="RF_DW";

      if(i >= n-parmLFinger[2]) {
        if(!strcmp(&pSide[strlen(pSide)-2],"OK")) rightFingerP++;
        else                                      rightFingerP--;
      }

    }
    fprintf(fa,"%lf,%lf,%s,",xBig,xLit,pSide);

    
    xRsold=pose[i][7];
    yRsold=pose[i][8];
    pRsold=pose[i][9];
    xLsold=pose[i][16];
    yLsold=pose[i][17];
    pLsold=pose[i][18];

    /* RightHand */
    xRhand=pose[i][13];
    yRhand=pose[i][14];
    pRhand=pose[i][15];

    xLhand=pose[i][22];
    yLhand=pose[i][23];
    pLhand=pose[i][24];


    if(pRsold < parmRHand[0] || pRhand < parmRHand[0]) {
      pSide="RH_UK";
    }
    else {
      if(fabs(yRsold - yRhand) < parmRHand[1])  pSide="RH_OK";
      else                                      pSide="RH_NG";

      if(i >= n-parmRHand[2]) {
        if(!strcmp(&pSide[strlen(pSide)-2],"OK")) rightHandP++;
        else                                      rightHandP--;
      }

    }
    fprintf(fa,"%lf,%lf,%s,",yRsold,yRhand,pSide);

    if(pLsold < parmLHand[0] || pLhand < parmLHand[0]) {
      pSide="LH_UK";
    }
    else {
      if(fabs(yLsold - yLhand) < parmLHand[1])  pSide="LH_OK";
      else                                      pSide="LH_NG";

      if(i >= n-parmLHand[2]) {
        if(!strcmp(&pSide[strlen(pSide)-2],"OK")) leftHandP++;
        else                                      leftHandP--;
      }
    }
    fprintf(fa,"%lf,%lf,%s,",yLsold,yLhand,pSide);  
    
    fprintf(fa,"\n");
  }
  fclose(fa);

  fprintf(stderr,"SOLD,%d,%lf\n",solderP,     solderP/     parmSolder [2]);
  fprintf(stderr,"NOSE,%d,%lf\n",noseP,       noseP/       parmNose   [2]);
  fprintf(stderr,"MOUS,%d,%lf\n",mouseP,      mouseP/      parmMouse  [2]);
  fprintf(stderr,"EYES,%d,%lf\n",eyeP,        eyeP/        parmEye    [2]);
  fprintf(stderr,"LFIN,%d,%lf\n",leftFingerP, leftFingerP/ parmLFinger[2]);
  fprintf(stderr,"RFIN,%d,%lf\n",rightFingerP,rightFingerP/parmRFinger[2]);
  fprintf(stderr,"LHND,%d,%lf\n",leftHandP,   leftHandP/   parmLHand  [2]);
  fprintf(stderr,"RHND,%d,%lf\n",rightHandP,  rightHandP/  parmRHand  [2]);

  fprintf(fw,"SOLD,%d,%lf\n",solderP,     solderP/     parmSolder [2]);
  fprintf(fw,"NOSE,%d,%lf\n",noseP,       noseP/       parmNose   [2]);
  fprintf(fw,"MOUS,%d,%lf\n",mouseP,      mouseP/      parmMouse  [2]);
  fprintf(fw,"EYES,%d,%lf\n",eyeP,        eyeP/        parmEye    [2]);
  fprintf(fw,"LFIN,%d,%lf\n",leftFingerP, leftFingerP/ parmLFinger[2]);
  fprintf(fw,"RFIN,%d,%lf\n",rightFingerP,rightFingerP/parmRFinger[2]);
  fprintf(fw,"LHND,%d,%lf\n",leftHandP,   leftHandP/   parmLHand  [2]);
  fprintf(fw,"RHND,%d,%lf\n",rightHandP,  rightHandP/  parmRHand  [2]);
  fclose(fw);



  /****
    Free
  ****/
  for(i=0;i<maxPi;i++) {
    free(pose[i]);
  }
  free(pose);
  for(i=0;i<maxFi;i++) {
    free(face[i]);
  }
  free(face);
  for(i=0;i<maxLi;i++) {
    free(left[i]);
  }
  free(left);
  for(i=0;i<maxRi;i++) {
    free(right[i]);
  }
  free(right);



  return(0);

}
/**********

**********/
int readFile(flag,fp,data,pLen,pRec)
int flag; /* SIZE READ */
FILE *fp;
double **data;
int *pLen;
int *pRec;
{
  int i,j;
  char *p,record[M_REC];

  i=0;
  rewind(fp);

  if(flag == SIZER) {
    *pRec=0;
    while(fgets(record,M_REC,fp)) {
      record[strlen(record)-1]='\0';
      p=strtok(record,", ");
      j=0;
      while(p) {
        j++;
        p=strtok(NULL,", ");
      }
      if(*pRec < j) *pRec = j;
      i++;
    }
    *pLen = i;
  }
  else {
    while(fgets(record,M_REC,fp)) {
      record[strlen(record)-1]='\0';
      p=strtok(record,", ");
      j=0;
      while(p) {
        data[i][j]=atof(p);
        j++;
        p=strtok(NULL,", ");
      }
      i++;
    }
  }
  return(0);
}
