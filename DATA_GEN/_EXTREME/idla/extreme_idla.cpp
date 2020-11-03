#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void genbad(long size,FILE *fp) {
  for(long i=0; i < size; i++) {
    fprintf(fp,".%lu",i);
  }
  fprintf(fp,"\n");
}


int main() {
  long size;
  char infile[100];
  bool alt;
  
  size=10; 
  alt=true; 
  while(size <= 5000000) {
    sprintf(infile,"extreme_idla_%lu.dat",size);
    printf("doing %s\n",infile);
    FILE *fp=fopen(infile,"w");
    genbad(size,fp);
    fclose(fp);
    if (alt) {
      size = 5*size;
      alt = false;
    }else{
      size = size/5;
      size = 10*size;
      alt = true;
    }
  }
  return 0;
}
