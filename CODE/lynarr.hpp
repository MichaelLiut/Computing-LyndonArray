#ifndef _lynarr_hpp
#define _lynarr_hpp

// returns the length of the longest Lyndon prefix of x   ---------------
long MaxLyn(Lstring& x,long beg=0) {
   long i;
   long p=1;
   long max=1;

   for(i = beg+1; i < x.getLen(); i++) {
      //printf("i=%ld,p=%ld,i-p=%ld\n",i-beg,p,i-p);
      if (x[i-p] != x[i]) {
         if (x[i] < x[i-p]) 
            return max;
         else{
            max = i-beg+1; 
            p = i-beg+1;
         }
      }
   }
   return max;
}

// check if the substring from..to is a max Lyndon in the string
bool isMaxLyn(Lstring& x,long from,long to) {
  long j = MaxLyn(x,from);
  if (from+j == to+1)
	return true;
  else
	return false;
}


// compute Lyndon array --------------------------------
long* idla(Lstring& x) {
   long i;
   long *la = new long[x.getLen()];
   for(i = 0; i < x.getLen(); i++)
     la[i] = MaxLyn(x,i);
   return la;
}

// compute Lyndon array --------------------------------
void idla1(Lstring& x,long* la) {
   long i;
   //printf("******************************************\n");
   for(i = 0; i < x.getLen(); i++) {
     //if ((i%1000)==0) printf("doing %lu\n",i);
     la[i] = MaxLyn(x,i);
   }
}

#endif // _lynarr_hpp
