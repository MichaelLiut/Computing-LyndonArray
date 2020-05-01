#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>

#define MAX (LONG_MAX-2)
#define min(a,b) ((a<=b) ? (a) : (b))

//#define _stand_alone
// _stand_alone defines if this is a subprogram or a stand alone program
// in other words, it either blocks or unblocks main()

//#define _debug  
// _debug controls displaying debugging info

#include "Lstring.hpp"
#include "lynarr.hpp"

/*******************************************************************************
 *******************************************************************************
 ***************************  bsla code ****************************************
 *******************************************************************************
 *******************************************************************************/



/* *******************************************************************
   *               global data for bsla                              *
   *******************************************************************/
long N;
long nil=-1;
long* prev;
long* Gprev;
long* Gnext;
long* Gstart;
long* Gcntxt;
long* Cprev;
long* Cnext;
long* Gmemb;
long* Gval;
long* auxA;
long* auxB;
long* auxC;

long Cstart;
long last_group_number;

long* prev_stack;  // it is only used to setup the initial prev[]

long prevTop;


/* ***************************************************************************
   *                    auxiliary functions                                  *
   ***************************************************************************/

#define push_prev(a) prev_stack[++prevTop]=a
#define pop_prev() prev_stack[prevTop--]
#define top_prev() ((prevTop<0)?(nil):(prev_stack[prevTop]))

#ifdef _debug
// show contents of the stack used for computing the initial prev[]
void show_prev_stack(Lstring& istring,FILE *fp=stdout) {
  fprintf(fp,"prev_stack:\n");
  if (prevTop<0) {
    fprintf(fp,"<empy>\n");
    return;
  }
  fprintf(fp,"%d",istring[prev_stack[0]]);
  for(long i=1; i<=prevTop; i++)
    fprintf(fp,",%d",istring[prev_stack[i]]);
  fputc('\n',fp);
}//end show_prev_stack
#endif

#ifdef _debug
// only used for testing -- show the contents of prev[]
void show_prev(Lstring& prev,FILE* fp=stdout) {
  fprintf(fp,"prev:\n");
  for(long i=0; i<N; i++)
    if (prev[i]==nil)
      fprintf(fp,"nil ");
    else
      fprintf(fp,"%d ",prev[i]);
}//end show_prev
#endif

#ifdef _debug
// only used for testing -- show contents of Gmemb
void show_Gmemb(FILE* fp=stdout) {
  long i;
  fprintf(fp,"Gmemb:\n");
  for(i=0; i<N; i++)
    if (Gmemb[i]==nil)
      fprintf(fp,"- ");
    else
      fprintf(fp,"%d ",Gmemb[i]);
  fprintf(fp,"\n");
}//end show_Gmemb
#endif

//macros
#define getGend(gr) Gprev[Gstart[gr]]
#define setGend(gr,a) Gprev[Gstart[gr]]=a
#define getGsize(gr) Gnext[Gprev[Gstart[gr]]]
#define setGsize(gr,a) Gnext[Gprev[Gstart[gr]]]=a
#define incGsize(gr) Gnext[Gprev[Gstart[gr]]]=Gnext[Gprev[Gstart[gr]]]+1

inline void startGroup(long gr,long i)  {
  Gstart[gr]=i; 
  setGend(gr,i); 
  setGsize(gr,1);
  Gmemb[i]=gr;
}//end startGroup

inline void removeFromGroup(long gr,long i) {
  long s, e, size;
  size=getGsize(gr);
  if (size==1) {
    // remove gr from conf
    if (gr==Cstart) {             // gr is first in conf
      Cstart=Cnext[gr];
      Cprev[Cstart]=nil;
    }else if (Cnext[gr]==nil) {
      Cnext[Cprev[gr]]=nil;     // gr is last in conf
    }else{                        // gr is in the middle of conf
      Cnext[Cprev[gr]]=Cnext[gr];
      Cprev[Cnext[gr]]=Cprev[gr];
    }
    Gcntxt[gr]=Gnext[Gstart[gr]]=Gprev[Gstart[gr]]=nil;  // delete gr
    return;
  }
  if (i==Gstart[gr]) {  // removing first index
    s=Gnext[i];       // will be new start
    e=getGend(gr);
    Gstart[gr]=s;
    setGend(gr,e);
    setGsize(gr,size-1);
    return;
  }        
  e=getGend(gr);        // removing last index
  if (i==e) {
    e=Gprev[e];
    setGend(gr,e);
    setGsize(gr,size-1);
    return;
  }
  s=Gprev[i];           // removing middle index
  e=Gnext[i];
  Gnext[s]=e;
  Gprev[e]=s;
  setGsize(gr,size-1);
  return;
}//end removeFromGroup

inline void prependGroup(long gr,long i) {
  long s, e, size;
  s=Gstart[gr];
  size=getGsize(gr);
  e=getGend(gr);
  Gstart[gr]=i;
  Gnext[i]=s;
  Gprev[s]=i;
  setGend(gr,e);
  setGsize(gr,size+1);
  Gmemb[i]=gr;
}//end prependGroup

#ifdef _debug
// only used for testing
void show_group(long gr,Lstring& istring,FILE* fp=stdout) {
  long i, k, l, e;
  i=Gstart[gr]; // first element of gr
  if (i==nil) {
    fprintf(fp,"group %d=<empty>\n",gr);
    return;
  }
  l=Gcntxt[gr];
  fprintf(fp,"group %d<cntxt %d",gr,istring[i]);
    for(k=1; k<l; k++) fprintf(fp," %d",istring[i+k]);
  fprintf(fp,"><gr.size %d>={%d",getGsize(gr),i);
  e=getGend(gr);
  while(true) {
    if (e==i) break;
    i=Gnext[i];
    fprintf(fp,",%d",i);
  }
  fprintf(fp,"}\n");
}//show group
#endif


#ifdef _debug
// if procgr=nil -- initial conf
// otherwise after procgr processed
void show_conf(Lstring& istring,long procgr=-2,FILE* fp=stdout) {
  long i, j, s, e, last_group;
  fprintf(fp,"Configuration: --------------------------------\n");
  for(j=Cstart; j!=nil; j=Cnext[j]) {
    show_group(j,istring);
    last_group=j;
  }
  if (procgr>-2) {
    if (procgr==nil) {
      s=Gstart[last_group];
      e=getGend(last_group);
      for(i=s; ; i=Gnext[i]) {
        fprintf(fp,"prev[%d]=%d\n",i,prev[i]);
        if (i==e) break;
      }
    }else if (procgr==Cstart) {
      fprintf(fp,"no prev[] as proc. group %d is the first\n",procgr);
    }else{
      j=Cprev[procgr];
      s=Gstart[j];
      e=getGend(j);
      if (s!=nil) {
  for(i=s; ; i=Gnext[i]) {
    fprintf(fp,"prev[%d]=%d\n",i,prev[i]);
    if (i==e) break;
  }
      }
    }
  }
  fprintf(fp,"End of Configuration ---------------------------\n");
}//end show_conf
#endif

// prototypes for testing functions
#ifdef _debug
void check_conf(Lstring&,long);
#endif



/* *****************************************************************************
   *                      Function bsla                                        *
   *                      input: string in integer alphabet                    *
   *****************************************************************************/
//function bsla ---------------------------------------------------------
void bsla(Lstring& istring,long*& space) {
  long i, j, e, s, a, b, size;
  long procgr;
  long val;

  // allocate memory for the data structures
  if (space==0) {
    prev=new long[12*N];
    space=prev;
  }else{
    prev=space;
  }
  Gprev=prev+N;
  Gnext=Gprev+N;
  Gstart=Gnext+N;
  Gcntxt=Gstart+N;
  Cprev=Gcntxt+N;
  Cnext=Cprev+N;
  Gmemb=Cnext+N;
  Gval=Gmemb+N;
  auxA=Gval+N;
  auxB=auxA+N;
  auxC=auxB+N;
  // and initialize it
  for(i=0; i<N; i++)
    Gstart[i]=Gnext[i]=Gprev[i]=Gmemb[i]=Cnext[i]=Cprev[i]=Gcntxt[i]=nil;

  /* ***************************************************************************
   *                         setup initial prev                                *
   *****************************************************************************/
  prev_stack=auxA;
  // compute prev for initial config
  prev[0]=nil;
  prevTop=nil;
  push_prev(0);
  //show_prev_stack(istring);
  for(i=1; i<N; i++) {
  //case going up
  if (istring[i-1]<istring[i]) {
    prev[i]=i-1;
    push_prev(i);
    //show_prev_stack(istring);
    continue;
  }//end going up

  // case going straight
  if (istring[i-1]==istring[i]) {
    prev[i]=prev[i-1];
    if (prev_stack[prevTop]==i-1) {
    prev_stack[prevTop]=i;
    //show_prev_stack(istring);
    }
    continue;
  }//end going straight

  // case going down
  if (istring[i-1]>istring[i]) {
    while(true) {
    j=top_prev();
    if (j==nil) {
      push_prev(i);
      //show_prev_stack(istring);
      prev[i]=nil;
      break;
    }
    if (istring[j] > istring[i]) {
      pop_prev();
      //show_prev_stack(istring);
      continue;
    }else if (istring[j] == istring[i]) {
      prev_stack[prevTop]=i;
      //show_prev_stack(istring);
      prev[i]=prev[j];
      break;
    }else{ 
      prev[i]=j;
      push_prev(i);
      //show_prev_stack(istring);
      break;
    }
    }
  }//end case going down
  }//end for
  /* ***************************************************************************
   *                    end of setup initial prev                              *
   *****************************************************************************/



  /* ***************************************************************************
   *                    setup initial config                                   *
   *****************************************************************************/

  // group by first letter
  last_group_number=0;
  for(i=0; i<N; i++) {
    j=istring[i];
    if (last_group_number < j) last_group_number=j;
    if (Gstart[j]==nil) {  // start group j
      Gstart[j]=i;
      Gnext[i]=nil;
      Gprev[i]=i;
      Gmemb[i]=j;
      Gcntxt[j]=1;
      setGsize(j,1);
      setGend(j,i);
    }else{                // append group j
      e=getGend(j); 
      s=getGsize(j);
      Gnext[e]=i;
      Gprev[i]=e;
      Gprev[Gstart[j]]=i;
      Gmemb[i]=j;
      setGsize(j,s+1);
      setGend(j,i);
    }
  }
  // setup config
  Cstart=0;
  for(i=0; i<=last_group_number; i++) {
    if (i < last_group_number) {
      Cnext[i]=i+1;
      if (i>0) 
        Cprev[i]=i-1;
      else
        Cprev[i]=nil;
    }else if (i==last_group_number) {
      Cnext[i]=nil;
      if (i>0)
        Cprev[i]=i-1;
      else
        Cprev[i]=nil;
    }
  }
  /* ***************************************************************************
   *                    end of setup initial conf                              *
   *****************************************************************************/


  /* ***************************************************************************
   *                    the refinement loop                                    *
   *****************************************************************************/
  procgr=last_group_number;

  #ifdef _debug
  show_conf(istring,nil);
  check_conf(istring,nil);
  #endif

  long currenttargetgr;
  long currentchildgr;
  long currentval, i1, i2, chunksize, gr;

  while(true) {                                    // refinement loop
    s=Gstart[procgr];
    e=Gprev[s];

    // if procgr has size 1, all this can be skipped
    if (getGsize(procgr)==1) {
      j=Gstart[procgr];
      if (prev[j]==nil)  {  // no refinement taking place
        #ifdef _debug
        printf("processing of group %d complete\n",procgr);
        show_conf(istring,procgr);
        check_conf(istring,procgr);
        #endif
        procgr=Cprev[procgr];
        if (procgr==nil) break;
        if (procgr==Cstart) break;
        continue;
      }else{
        auxA[0]=j;
        a=1;
        Gval[j]=1;
        goto process_loop;
      }
    }

    // first compute valences and check if any refinement will take place *****************
    val=1;
    a=0;
    for(i=e;   ; i=Gprev[i]) {
      if (prev[i]!=nil) a=1;
      if (i==s) {
        Gval[i]=val;
        break;
      }else{
        if (i==Gcntxt[procgr]+Gprev[i]) {
          val++;
          Gval[i]=nil;
        }else{
          Gval[i]=val;
          val=1;
        }
      }
    }
    if (a==0) { // no refinement taking place
      #ifdef _debug
      printf("processing of group %d complete\n",procgr);
      show_conf(istring,procgr);
      check_conf(istring,procgr);
      #endif
      procgr=Cprev[procgr];
      if (procgr==nil) break;
      if (procgr==Cstart) break;
      continue;
    }
    #ifdef _debug
    printf("valencies:\n");
    for(i=s;   ; i=Gnext[i]) {
      if (Gval[i]==nil) {
    if (i==e) break;
    continue;
    }
      printf("%d valence %d\n",i,Gval[i]);
      if (i==e) break;
    }
    #endif
    // end compute valences ****************************************************

    
    // compute buckets starts for target groups in auxC  ***********************
    // first blank auxA
    for(i=s;  ; i=Gnext[i]) {
      if (prev[i]==nil || Gval[i]==nil) {
        if (i==e) break;
        continue;
      }
      j=Gmemb[prev[i]];
      auxA[j]=nil;
      if (i==e) break;
    }
    // now compute group translations
    val=0;
    for(i=s;  ; i=Gnext[i]) {
      if (prev[i]==nil || Gval[i]==nil) {
        if (i==e) break;
        continue;
      }
      j=Gmemb[prev[i]];
      if (auxA[j]==nil) {
        auxA[j]=val;
        auxB[val]=0;
        val++;
      }
      if (i==e) break;
    }
    #ifdef _debug
    printf("target group translations:\n");
    for(i=s;  ; i=Gnext[i]) {
      if (prev[i]==nil || Gval[i]==nil) {
        if (i==e) break;
        continue;
      }
      j=Gmemb[prev[i]];
      printf("target group %d for %d translates to %d\n",j,i,auxA[j]);
      if (i==e) break;
    }
    #endif
    // compute target group frequencies in auxB
    for(i=s;  ; i=Gnext[i]) {
      if (prev[i]==nil || Gval[i]==nil) {
        if (i==e) break;
        continue;
      }
      j=auxA[Gmemb[prev[i]]];
      auxB[j]++;
      if (i==e) break;
    }
    #ifdef _debug
    printf("target group frequencies:\n");
    for(i=0; i<val; i++) {
      printf("translated target group %d has frequency %d\n",i,auxB[i]);
      if (i==s) break;
    }
    #endif
    // now translate the frequencies to bucket polongers
    a=0;
    for(i=0; i<val; i++) {
      b=auxB[i];
      auxB[i]=a;
      a += b;
    }
    #ifdef _debug
    printf("bucket starts for translated target groups:\n");
    for(i=0; i<val; i++) {
      printf("bucket for translated target group %d starts at %d\n",i,auxB[i]);
    }
    #endif
    // transform it to auxC
    for(i=s;  ; i=Gnext[i]) {
      if (prev[i]==nil || Gval[i]==nil) {
        if (i==e) break;
        continue;
      }
      a=Gmemb[prev[i]];
      j=auxA[a];  // a translates to j
      auxC[a]=auxB[j];
      if (i==e) break;
    }
    #ifdef _debug
    printf("bucket starts for groups:\n");
    for(i=s;  ; i=Gnext[i]) {
      if (prev[i]==nil || Gval[i]==nil) {
        if (i==e) break;
        continue;
      }
      a=Gmemb[prev[i]];
      printf("group %d bucket starts at %d\n",a,auxC[a]);
      if (i==e) break;
    }
    #endif
    // end compute buckets starts for target groups in auxC  *******************
  
    // order indices by valencies, store them in auxB **************************
    // blanc out auxA to use as counters for valencies
    size=getGsize(procgr);
    for(i=0; i<=size; i++) auxA[i]=nil;
    for(i=s;  ; i=Gnext[i]) { 
      if (prev[i]==nil || Gval[i]==nil) {
        if (i==e) break;
        continue;
      }
      auxA[Gval[i]]=0;
      if (i==e) break;
    }
    // compute frequencies of valencies, i.e. bucket sizes
    for(i=s;  ; i=Gnext[i]) { 
      if (prev[i]==nil || Gval[i]==nil) {
        if (i==e) break;
        continue;
      }
      auxA[Gval[i]]++;
      if (i==e) break;
    }
    #ifdef _debug
    printf("val frequencies:\n");
    for(i=s;  ; i=Gnext[i]) { 
      if (prev[i]==nil || Gval[i]==nil) {
        if (i==e) break;
        continue;
      }
      if (auxA[i]>0) printf("val %d occurs %d x\n",i,auxA[i]);
      if (i==e) break;
    }
    #endif
    //compute valencies bucket starts, use val for cumulation
    val=0;
    for(i=size; i>=0; i--) {
      if (auxA[i]==nil || auxA[i]==0) continue;
      a=auxA[i];
      auxA[i]=val;
      val += a;
    }
    #ifdef _debug
    printf("bucket starts for valencies:\n");
    for(i=e; ; i=Gprev[i]) {
      if (prev[i]==nil || Gval[i]==nil) {
        if (i==s) break;
        continue;
      }
      if (auxA[i]!=nil) printf("val %d bucket starts at %d\n",i,auxA[i]);
      if (i==s) break;    
    }
    #endif
    // fill in the buckets, the buckets are in auxB
    b=0;
    for(i=e;  ; i=Gprev[i]) {
      if (prev[i]==nil || Gval[i]==nil) {
        if (i==s) break;
        continue;
      }
      j=auxA[Gval[i]];
      auxB[j]=i;
      auxA[Gval[i]]=j+1;
      b++;
      if (i==s) break;
    }
    // auxB[0..b-1] contains relevant indices of procgr ordered by valencies in reverse
    #ifdef _debug
    printf("procgr indices ordered by valencies:\n");
    for(i=0; i<b; i++)
      printf("%d val %d\n",auxB[i],Gval[auxB[i]]);
    printf("\n");
    #endif
    // end order indices by valencies, store them in auxB **********************

    // store in auxA indices, grouped by target groups, descending ordered *************
    // by val, descending order
    // auxC contains target group bucket starts
    // auxB contains indices ordered by valencies
    // we will move them to target group buckets in auxA
    a=0;
    for(i=0; i<b; i++) {
      j=Gmemb[prev[auxB[i]]]; // target group
      auxA[auxC[j]++]=auxB[i];
      a++;
    }
    #ifdef _debug
    printf("relevant indices grouped by targetgr, descending val, descending order:\n");
    for(i=0; i<a; i++) {
      printf("%d targetgr=%d val=%d prev=%d prev[%d]=%d\n",auxA[i],Gmemb[prev[auxA[i]]],Gval[auxA[i]],prev[auxA[i]],prev[auxA[i]],prev[prev[auxA[i]]]);
    }
    #endif
    // now we can use auxB and auxC, and process the indices as they are store in auxA

  process_loop:
    currenttargetgr=Gmemb[prev[auxA[0]]];
    currentval=Gval[auxA[0]];
    i1=0;
    while(i1<a) {                                 // process loop
      // determine chunk
      for(i2=i1+1; i2<a && currenttargetgr==Gmemb[prev[auxA[i2]]] && currentval==Gval[auxA[i2]]; i2++);
      i2--;
      // so auxA[i1..i2-1] are indices with the same target group, i1 <= i2
      chunksize=i2-i1+1;

      //update prev[prev[]] for the indices in the chunk
      prevTop=nil;
      for(i=i1; i<=i2; i++) {
        j=prev[auxA[i]];
        gr=Gmemb[j];
        #ifdef _debug
        printf("updating prev[%d]=%d in group %d\n",j,prev[j],gr);
        #endif
        if (j==Gstart[gr]) {  // cannot improve it
          #ifdef _debug
          printf("prev[%d]=%d unchanged\n",j,prev[j]);
          #endif
          while(prevTop>=0) {
            if (prev[j]>prev[auxB[prevTop]]) {
              #ifdef _debug
              printf("prev[%d]updated from %d to %d\n",auxB[prevTop],prev[auxB[prevTop]],prev[j]);
              #endif
              prev[auxB[prevTop--]]=prev[j];
            }else{
              prevTop--;
            }
          }
          continue;
        }
        // so we have Gprev[j] and it can be a better candidate, as long as it is not in the same chunk
        if (i==i2 || Gprev[j]!=prev[auxA[i+1]]) { // auxA[i+1] is not in the same chunk and so Gprev[j]
          if (Gprev[j]>prev[j]) {               // is a very good candidate for prev[j]
            #ifdef _debug
            printf("prev[%d]updated from %d to ",j,prev[j]);
            #endif
            prev[j]=Gprev[j];
            #ifdef _debug
            printf("%d\n",prev[j]);
            #endif
            while(prevTop>=0) {
              if (prev[j]>prev[auxB[prevTop]]) {
                #ifdef _debug
                printf("prev[%d]updated from %d to %d\n",auxB[prevTop],prev[auxB[prevTop]],prev[j]);
                #endif
                prev[auxB[prevTop--]]=prev[j];
              }else{
                prevTop--;
              }
            }
          }else{ // so we did not find a better candidate for prev[j], so put it in queue
            auxB[++prevTop]=j;           // put it in queue
          }
        }else{ // so Gprev[j]==prev[auxA[i+1]]  i.e. Gprev[j] is in the same chunk
          auxB[++prevTop]=j;                // put it in queue
        }
      }
      // if the target gr is of the same size as the chunk, we can just update its
      // context and are done
      if (chunksize==getGsize(currenttargetgr)) {
        Gcntxt[currenttargetgr]=Gcntxt[currenttargetgr]+currentval*Gcntxt[procgr];
        currentchildgr=currenttargetgr;
        #ifdef _debug
        for(i=i1; i1<=i2; i1++) {
          printf("moving %d (index=%d, procgr=%d, val=%d) from group %d to recycled group %d\n",
            prev[auxA[i]],auxA[i],procgr,currentval,currenttargetgr,currentchildgr);
          show_conf(istring);
        }
        #endif
      }else{
        currentchildgr=++last_group_number;
        currentval=Gval[auxA[i1]];
        j=prev[auxA[i1]];
        removeFromGroup(currenttargetgr,j);
        startGroup(currentchildgr,j);
        Gcntxt[currentchildgr]=Gcntxt[currenttargetgr]+currentval*Gcntxt[procgr];
        // place the new group in conf
        s=Cnext[currenttargetgr];
        Cnext[currenttargetgr]=currentchildgr;
        Cprev[currentchildgr]=currenttargetgr;
        Cnext[currentchildgr]=s;
        Cprev[s]=currentchildgr;
        #ifdef _debug
        printf("moving %d (index=%d, procgr=%d, val=%d) from group %d to new group %d\n",
            j,auxA[i1],procgr,currentval,currenttargetgr,currentchildgr);
        show_conf(istring);
        #endif
        for(i=i1+1; i<=i2; i++) {
          j=prev[auxA[i]];
          removeFromGroup(currenttargetgr,j);
          prependGroup(currentchildgr,j);
          #ifdef _debug
          printf("moving %d (index=%d, procgr=%d, val=%d) from group %d to existing group %d\n",
              j,auxA[i],procgr,currentval,currenttargetgr,currentchildgr);
          show_conf(istring);
          #endif
        }
      }// end process chunk
      i1=i2+1;
      if (i1>=a) break;
      currenttargetgr=Gmemb[prev[auxA[i1]]];
      currentval=Gval[auxA[i1]];
    }//endwhile process loop
    #ifdef _debug
    printf("processing of group %d complete\n",procgr);
    show_conf(istring,procgr);
    check_conf(istring,procgr);
    #endif
    procgr=Cprev[procgr];
    if (procgr==nil) break;
    if (procgr==Cstart) break;
  }//endwhile refinement loop
  /* ***************************************************************************
   *                    end of the refinement loop                             *
   *****************************************************************************/
}//end bsla
/*******************************************************************************
 *******************************************************************************
 ********************* end of bsla code ****************************************
 *******************************************************************************
 *******************************************************************************/

// to compute LA ---------------------------------------------------------------
void bsla2la(Lstring& istring,long* la,long*& space) {
  N=istring.getLen();
  bsla(istring,space);
  for(long i=0; i<N; i++) la[i]=nil;
  for(long j=Cstart; j!=nil; j=Cnext[j]) {
    long e=getGend(j);
    for(long i=Gstart[j];  ; i=Gnext[i]) {
      if (i==nil) break;
      la[i]=Gcntxt[j];
      if (i==e) break;
    } 
  } 
}//end bsla2la


// stuff for main() ----------------------------------------------------------

#ifdef _debug
// rotate x[from..to] 1 position to the right
void rotate(Lstring& x,long from,long to) {
  long a=x[from];
  for(long i=from+1; i<=to; i++)
    x[i-1]=x[i];
  x[to]=a;
}//end rotate
#endif

#ifdef _debug
// compare x1[form1..to1] and x2[form2..to2] in lexicographic ordering
// returns < 0 if x1[form1..to1] < x2[form2..to2]
// returns 0 if x1[form1..to1] = x2[form2..to2]
// returns > 0 if x1[form1..to1] > x2[form2..to2]
long lex(Lstring& x1,long from1,long to1,Lstring& x2,long from2,long to2) {
  long lcp=to1-from1+1;
  if (to2-from2+1 < lcp) lcp=to2-from2+1;
  for(long i=0; i<lcp; i++) {
    if (x1[from1+i]<x2[from2+i])
      return -1;
    else if (x1[from1+i]>x2[from2+i])
      return 1;
  }
  // so x1[from1..from1+lcp-1]==x2[from2..from2+lcp-1]
  if (to1-from1==to2-from2) return 0;
  if (to1-from1<to2-from2)
    return -1;
  else
    return 1;
}//end lex
#endif

#ifdef _debug
// returns true if x[from..to] is Lyndon, false otherwise
bool isLyndon(Lstring& x,long from,long to) {
  long i, j;
  Lstring y=Lstring(N);
  for(i=from; i<=to; i++) y[i]=x[i];
  //len=to-from+1, so need to rotate to-from times
  for(i=0; i<to-from; i++) {
    rotate(y,from,to);
    j=lex(x,from,to,y,from,to);
    if (j>=0) return false;
  }
  return true;
}//end isLyndon
#endif

#ifdef _debug
// returns < 0 if in the conf, gr1 precedes gr2
// returns 0 if gr1=gr2
// returns > 0 if in the conf, gr1 succeeds gr2
long grcmp(long gr1,long gr2,Lstring& istring) {
  long i;
  if (gr1>last_group_number) {
    printf("grcmp error: %d > %d\n",gr1,last_group_number);
    show_conf(istring);
    getchar();
    exit(0);
  }
  if (gr2>last_group_number) {
    printf("grcmp error: %d > %d\n",gr2,last_group_number);
    show_conf(istring);
    getchar();
    exit(0);
  }
  if (gr1==gr2)
    return 0;
  if (gr1==Cstart)
    return -1;
  else if (gr2==Cstart)
    return 1;
  for(i=Cstart; i!=nil; i=Cnext[i]) {
    if (i==gr1) 
      return -1;
    else if (i==gr2) 
      return 1;
  }
  printf("grcmp error\n");
  show_conf(istring);
  getchar();
  exit(0);
}//end grcmp
#endif


#ifdef _debug
// returns true if x[from..to] is maximal Lyndon, false otherwise
bool isMaxLyndon(Lstring& x,long from,long to,long N) {
  if (! isLyndon(x,from,to)) return false;
  for(long i=to+1; i<N; i++)
    if (isLyndon(x,from,i)) return false;
  return true;
}//end isMaxLyndon
#endif

#ifdef _debug
// if procgr=nil --- initial configuration
// otherwise after processing of procgr
// function check_conf --------------------------------------------------------
void check_conf(Lstring& istring,long procgr) {
  long* x=new long[N];
  long* y=new long[N];
  long i, j, s, e, k, size, a1, a2, i1, i2, last_group;

  for(i=0; i<N; i++) x[i]=nil;

  for(j=Cstart; j!=nil; j=Cnext[j]) { // traverse conf
    last_group=j;
    size=0;
    s=Gstart[j]; 
    e=getGend(j);
  if (s==nil) 
    continue;
    //check Gmemb
    for(i=s;  ; i=Gnext[i]) {
      if (Gmemb[i]!=j) {
        printf("check_conf error: Gmemb error for gr %d and pos %d\n",j,i);
        getchar();
        exit(0);
      }
      if (i==e) break;
    }
    // check the group is the same both ways
    for(i=0; i<N; i++) y[i]=nil;
    for(i=s;  ; i=Gnext[i]) {
      y[i]=1;
      if (i==e) break;
    }
    for(i=e;  ; i=Gprev[i]) {
      y[i]++;
      if (i==s) break;
    }
    for(i=0; i<N; i++) {
      if (y[i]==1) {
        printf("check_conf error: group link error gr=%d\n",j);
        getchar();
        exit(0);
      }
    }
    for(i=s;  ; i=Gnext[i]) {
      x[i]=1;
      size++;
      for(k=0; k<Gcntxt[j]; k++) {
        if (istring[i+k]!=istring[s+k]) {
          printf("check_conf error: Group %d wrong context %d, discrepancy between position %d and position %d\n",j,Gcntxt[j],s,i);
          getchar();
          exit(0);
        }
      }
      if (i==e) break;
    }
    if (size!=getGsize(j)) {
      printf("check_conf error: Group %d wrong size (records %d, should be %d)\n",j,getGsize(j),size);
      show_conf(istring,procgr);
      getchar();
      exit(0);
    }
    if (j>Cstart) {
      a1=Gcntxt[Cprev[j]];
      i1=Gstart[Cprev[j]];
      a2=Gcntxt[j];
      i2=Gstart[j];
      // need to show that istring[i1..i1+a1-1] < istring[i2..i2+a2-1]
      if (lex(istring,i1,i1+a1-1,istring,i2,i2+a2-1)>=0) {
        printf("check_conf error: lex order error: group %d context <",Cprev[j]);
        for(long u=0; u<a1-1; u++) printf("%d ",istring[i1+u]);
        printf("%d> is not smaller than group %d context <",istring[i1+a1-1],j);
        for(long u=0; u<a2-1; u++) printf("%d ",istring[i2+u]);
        printf("%d>\n",istring[i2+a2-1]);
        getchar();
        exit(0);
      }
    }
  }//end traverse conf
  for(i=0; i<N; i++) {
    if (x[i]==nil) {
      printf("check_conf error: groups do not form partitioning\n");
      getchar();
      exit(0);
    }
  }
  // check conf linking
  for(i=0; i<N; i++) x[i]=nil;
  for(i=Cstart; i!=nil ; i=Cnext[i]) {
    if (Cnext[i]!=nil) e=Cnext[i];
    x[i]=1;
  }
  for(i=e; i!=nil; i=Cprev[i]) {
    x[i]++;
  }
  for(i=0; i<N; i++) {
    if (x[i]==1) {
      printf("check_conf error: conf linking error for gr %d\n",i);
      getchar();
      exit(0);
    }
  }

  //check that all contexts are Lyndon
  for(j=Cstart; j!=nil ; j=Cnext[j]) {
    if (! isLyndon(istring,Gstart[j],Gstart[j]+Gcntxt[j]-1)) {
      printf("check_conf error: Context of gr %d not Lyndon\n",j);
      getchar();
      exit(0);
    }
  }

  //check that all contexts are maxLyndon from procgr-- up
  if (procgr==nil)
    j=last_group;
  else
    j=Cprev[procgr];
  for(  ; j!=nil ; j=Cnext[j]) {
  if (Gstart[j]==nil) 
    continue;
    if (! isMaxLyndon(istring,Gstart[j],Gstart[j]+Gcntxt[j]-1,N)) {
      printf("check_conf error: Context of gr %d not maxLyndon\n",j);
      getchar();
      exit(0);
    }
  }
  // check that all groups from procgr up are complete
  if (procgr==nil)
    j=last_group;
  else
    j=Cprev[procgr];
  for( ; j!=nil; j=Cnext[j]) {
    //istring[Gstart[j]..Gstart[j]+Gcntxt[j]-1]
    for(i=0; i+Gcntxt[j]<N; i++) {
      if (lex(istring,i,i+Gcntxt[j]-1,istring,Gstart[j],Gstart[j]+Gcntxt[j]-1)==0) {
        if (isMaxLyndon(istring,i,i+Gcntxt[j]-1,N)) {
          if (Gmemb[i]!=j) {
            printf("check_conf error: Group %d should be complete but does not contain %d\n",j,i);
            getchar();
            exit(0);
          }
        }
      }
    }
  }
  //check prev
  for(i=0; i<N; i++) {
    if (i==0) {
      if (prev[i]!=nil) {
        printf("check_conf error: prev error: prev[0]=%d\n",prev[0]);
        getchar();
        exit(0);
      }
      continue;
    }
    j=prev[i];
    if (j!=nil) {
      if (grcmp(Gmemb[j],Gmemb[i],istring)>=0) {
        printf("check_conf error: prev error: prev[%d]=%d but %d in gr=%d and %d i gr=%d\n",i,j,i,Gmemb[i],j,Gmemb[j]);
        getchar();
        exit(0);
      }
    }
    // so prev[i] is good, but could it be better?
    for(s=0; s<i; s++) {
      if (grcmp(Gmemb[s],Gmemb[i],istring)<0) {
        // s potentially prev[i]
        if (j==nil || s>j) {
          printf("check_conf error: prev[%d]=%d error as %d would be better prev\n",i,j,s);
          getchar();
          exit(0);
        }
      }
    }
  }
  printf("check_conf: check_conf for proc. group %d successful\n",procgr);
}//end check_conf
#endif



#ifdef _stand_alone
// translate to integer alphabet
char* toia(char* x) {
  static char buf[500];
  char alpha[500];
  char *y=x;
  long i, j;
  for(i=0; i<500; i++)
  buf[i]=alpha[i]=-1;
  while(*y != '\0') {
  buf[*y]++;
  y++;
  }
  for(j=i=0; i<500; i++)
  if (buf[i]>=0) alpha[j++]=(char) i;
  y=x;
  buf[0]='\0';
  while(*y != '\0') {
  // translate *y
  strcat(buf,".");
  for(i=0; alpha[i]>=0; i++) {
    if (alpha[i]==*y) {
    sprintf(buf+strlen(buf),"%u",i);
    break;
    }
  }
  y++;
  }
  return buf;
}


// function main -----------------------------------------------
int main(int argc,char* argv[]) {
  char infile[100];
  char* line;
  long linesize;
  long i, d, ds;
  char c;

  if (argc != 2) {
    printf("usage -- %s <infile>\n",argv[0]);
    return 0;
  }
  strcpy(infile,argv[1]);
  FILE* fp;
  fp=fopen(infile,"r");
  if (fp==0) {
    printf("can't open \"%s\"\n",infile);
    return 0;
  }else{
    printf("input file \"%s\" successfully opened\n",infile);
    printf("warning -- all the strings in the file \"%s\" are assumed"
           "to be of the same length\n",infile);
  }

  // determine the length of the strings
  // read the first line to determine the length of the string
  N=0;
  while(true) {
    c=fgetc(fp);
    if (feof(fp)) break;
    if (c=='\n'||c=='\r') break;
    if (c=='.') N++;
  }
  rewind(fp);
  d=N;
  ds=0;
  while(d > 0) {
    ds++;
    d=d/10;
  }
  linesize = (ds+1)*N+1;
  line=new char[linesize];

  Lstring *s=new Lstring(N);

  // process the file
  while(fgets(line,linesize,fp)) {
    for(i=strlen(line); i>=0; i--) {
      if (line[i]=='\0' || line[i]=='\n' || line[i]=='\r') {
        line[i]='\0';
        continue;
      }else{
        break;
      }
    }//endfor
 
    (*s)=line;
    s->shownl();
    long* space=0;
    long* la = new long[s->getLen()];
    bsla2la(*s,la,space);
    for(long i=0; i<N;i++)
      printf("%d ",la[i]);
    putchar('\n');

    // compute Lyndon array directly
    long* LA;
    LA = idla(*s);

    for(long i=0; i<N; i++) {
      if (la[i]!=LA[i]) {
        printf("ERROR\n");
        getchar();
        return 0;
      }
    }
    printf("PERFECT\n");
  }//end while

  fclose(fp);
  printf("done\n");
  return 0;
}//end main
#endif
