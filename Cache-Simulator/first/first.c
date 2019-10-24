#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

long** createCache(int sets, int lines);
int readC(long** cache, int sets, int lines, long int tagindex, long int setindex);
long** readL(long** cache, int sets, int lines, long int tagindex,long int setindex);
long** writeC(long** cache, int sets, int lines, long int tagindex, long int setindex, char* policy);


int main(int argc, char** argv){
  /*   if(argv != 5){
    printf("error\n");
    return 0;
  }
  */
  int Csize = atoi(argv[1]);
  int Bsize = atoi(argv[2]);
  char* policy = argv[3];
  char* assoc = argv[4];
  FILE* trace = fopen(argv[5], "r");
  if(trace == NULL){
    printf("error\n");
    return 0;
  }
  
  int read = 0;
  int write=0;
  int hit=0;
  int miss=0;

  int lines=1;
  int sets=1;
  long int** cache;
  int i;

  //creating an nway assoc cache
  if(strlen(assoc) > 6){
    int n =0;
    i = 6;
    char x[10];
    for(i = 7; i <= strlen(assoc); i++){
	x[i-7] = assoc[i -1];
    }	
    x[i-7] = assoc[i - 1];
    n = atoi(x);
    lines = Csize/Bsize;
     sets = lines/n;
    lines = n;
    cache = createCache(sets, lines);
  } 

  //creating a direct cache
  else if(strcmp(assoc, "direct") == 0){ 
      sets = Csize/Bsize;
      cache = createCache(sets, lines);
  }

  //creating a fully assoc cache
  else{
    lines = Csize/Bsize;
    cache = createCache(sets, lines);
  }

  int s = log2(sets);
  int b = log2(Bsize);


  long int setmask = ((1<<s)-1);
  char c;
  long long int data;

  while(fscanf(trace, "%c" "%llx", &c, &data) != EOF){
  
    long int setindex =(data>>b)&setmask;
    long int tagindex = data>>(b+s);   
    if(c == 'R'){//do a READ
      int a = readC(cache, sets, lines, tagindex, setindex);
      if(a == 1){
	hit++;
	
	if(strcmp(policy, "fifo") != 0){
	  cache = readL(cache, sets, lines, tagindex, setindex);
	}
	
      }
      else{
	//check if set is full, if yes, do replacement
        cache = writeC(cache, sets, lines, tagindex, setindex, policy);
	miss++;
	read++;
      }
    }
    else if(c == 'W'){  //do a WRITE 
      write++;
      int a = readC(cache, sets, lines, tagindex, setindex);
      if(a == 1){
	hit++;
	if(strcmp(policy, "lru") == 0)
	  cache = readL(cache, sets, lines, tagindex, setindex);
	  
      }	
      else{
	//check if set is full, if yes, do replacement
        cache = writeC(cache, sets, lines, tagindex, setindex, policy);
	miss++;
        read++;
      }
    }
  }
  

  
  printf("Memory reads: %d\n", read);
  printf("Memory writes: %d\n", write);
  printf("Cache hits: %d\n", hit);
  printf("Cache misses: %d\n", miss);
 
  return 0;
}






long int** createCache(int sets, int lines){
  //use sets or lines as cols, tag as data at row
  int i, j;
  long int** cache = (long int**)malloc(sizeof(long int*)* sets);
  for(i = 0; i < sets; i++){
    cache[i] =(long int*)malloc(sizeof(long int)* lines);
  }
  for(i=0; i<sets; i++)
    for(j=0; j<lines; j++)
      cache[i][j]=0;
  return cache;
}

int readC(long int** cache, int sets, int lines, long int tagindex, long int setindex){
  int i, j;
  for(i = 0; i < sets; i++){
    if(i == setindex){
      //printf("above second loop in read\n");
      for(j = 0; j < lines; j++){
	  //location found
	  if(cache[i][j] == tagindex){
	    //    printf("second if in read is hit\n");
	    return 1;
	  }
       }
     }
   }

  // printf("read miss i guess\n");
  return 0;
  }
  
  //cache miss
  
// printf("error in read\n");
 

long int** writeC(long int** cache, int sets, int lines, long int tagindex, long int setindex, char* policy){
 int i, j;
  for(i = 0; i < sets; i++){
    if(i == setindex){
      for(j = 0; j < lines; j++){
	  if(cache[i][j] == 0){
	    // printf("about to write in tag\n");
	    //miss, so write it in
	    cache[i][j] = tagindex;
	    // printf("cache[set][tag] = %lx\n", cache[i][j]);
	    return cache;
 	 }
      }
      for(j = 0; j<lines; j++){
	  if((j + 1) < lines)
	    cache[i][j] = cache[i][j+1];
	  else
	    cache[i][j] = tagindex;
	  // return cache;
	}
	return cache;
      //  printf("about to do replacement:\n");
      //FIFO
      /*
      if(strcmp(policy, "fifo") == 0){
	for(j = 0; j<lines; j++){
	  if((j + 1) < lines)
	    cache[i][j] = cache[i][j+1];
	  else
	    cache[i][j] = tagindex;
	  // return cache;
	}
	return cache;
      }
      //LRU
      else{
	for(j=lines-1; j>=0; j--){
	  if(j == 0){
	    cache[i][j] = tagindex;
	    return cache;
	  }
	  else
	    cache[i][j] = cache[i][j-1];	 	  
	}
	return cache;	
      }


      */
      /*
      else {
	for(j = 0; j<lines; j++){
	  if((j + 1) < lines){
	     cache[i][j+1] = cache[i][j];
	     if(j == 0)
	       cache[i][j] = tagindex;
	  }
	}
       return cache;
      }
      */
    }
  }
  return cache;
}

long** readL(long** cache, int sets, int lines, long int tagindex, long int setindex){    
  int i, j, x;
  for(i = 0; i < sets; i++){
    if(i == setindex){
      //printf("above second loop in read\n");
      for(j = 0; j < lines; j++){
	  //location found
	  if(cache[i][j] == tagindex){
	    x =1;
	    while(j < lines){
	      if(j+1 < lines){
	      long int tmp = cache[i][j+1];
	      cache[i][j+1] = cache[i][j];
	      cache[i][j] = tmp;
	      }
	      j++;
	    }
	    break; 
	  }
	  if(x ==1)
	    break;
       }
     }
   }
  return cache;
  }
  


