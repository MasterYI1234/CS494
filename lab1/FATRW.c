#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include "jdisk.h"
#include <math.h>

void* jd;
int S;

unsigned char** fat;

char* flag;

void usage(char *s)
{
  fprintf(stderr, "usage: jdisk_test FATRW diskfile import input-file\n");
  fprintf(stderr, "       FATRW diskfile export starting-block output-file\n");
  if (s != NULL) fprintf(stderr, "%s\n", s);
  exit(1);
}

int getS(unsigned long disk_size) {

  int num_sectors = disk_size / JDISK_SECTOR_SIZE;

  int D = num_sectors - 1;

  int S;

  while(1) {
    int numBytes = (D + 1) * 2;

    S = numBytes / JDISK_SECTOR_SIZE;

    if(numBytes > S * JDISK_SECTOR_SIZE) {
      S++;
    }

    if(S + D <= num_sectors) {
      break;
    }

    D--;
  }

  return S;
}

void setUp(char* fn) {

  jd = jdisk_attach(fn);
  unsigned long disk_size = jdisk_size(jd);
  S = getS(disk_size);
  fat = (unsigned char**) malloc(sizeof(char*) * S);

  flag = (char*) malloc(S);

  for(int i=0; i < S; i++) {
    fat[i] = NULL;
    flag[i] = 0;
  }
}

void freeMemory() {

  for (int i = 0; i < S; ++i)
  {
     if(fat[i] != NULL) {

        free(fat[i]);
     }
  }

  free(fat);

  free(flag);
}



unsigned short Read_Link(int link)
{
   link *= 2;
   unsigned int sector = link / JDISK_SECTOR_SIZE;

   if(fat[sector] == NULL) {

      fat[sector] = (unsigned char*) malloc(sizeof(unsigned char) * JDISK_SECTOR_SIZE);

      jdisk_read(jd, sector, fat[sector]);
   }

   int byteOffset = link % JDISK_SECTOR_SIZE;

   // printf("%d %d %d\n", byteOffset, fat[sector][byteOffset], fat[sector][byteOffset + 1]);

   unsigned short lv = fat[sector][byteOffset] | (fat[sector][byteOffset + 1] << 8);

   return lv;
}

void Write_Link(int link, unsigned short val) {

   link *= 2;
   unsigned int sector = link / JDISK_SECTOR_SIZE;

   if(fat[sector] == NULL) {

      fat[sector] = (unsigned char*) malloc(sizeof(unsigned char) * JDISK_SECTOR_SIZE);

      jdisk_read(jd, sector, fat[sector]);
   }

   int byteOffset = link % JDISK_SECTOR_SIZE;

   unsigned short lv = fat[sector][byteOffset] | (fat[sector][byteOffset + 1] << 8);

   if(lv == val) {

    return;
   }

   fat[sector][byteOffset] = val & 0xff;

   fat[sector][byteOffset + 1] = (val >> 8) & 0xff;

   flag[sector] = 1;

}

void Flush_Links() {

  for (int i = 0; i < S; ++i)
  {
     if(flag[i] == 1) {

       jdisk_write(jd, i, fat[i]);
     }
  }
}


void exportFile(int start_block, char* outFn) {



  FILE* f = fopen(outFn, "w");

  int block = start_block;

  int over = 0;

  unsigned char buf[JDISK_SECTOR_SIZE];

  while(! over) {

     // printf("%d\n", block);

     jdisk_read(jd, block, buf);

     int link = block - S + 1;

     unsigned short lv = Read_Link(link);

     unsigned short n = JDISK_SECTOR_SIZE;

     if(lv == 0) {
        n = 1024;
        over = 1;
     }

     else if(lv == link) {

       over = 1;

       char CONS = 0xff;

       // printf("%d\n", CONS);

       if(((char*) buf)[JDISK_SECTOR_SIZE - 1] == (char)0xff) {
          n = 1023;
       }else{

          // printf("two bytes: %d %d\n", buf[JDISK_SECTOR_SIZE - 1], buf[JDISK_SECTOR_SIZE - 2]);

          n = ((unsigned short*) buf)[JDISK_SECTOR_SIZE / 2 - 1];

          // printf("has %d\n", n);
       }

     } else{
        n = 1024;

        block = S + lv - 1;
     }

     fwrite(buf, sizeof(char) , n, f );
  }

  fclose(f);

}

void importFile(char* fn) {

  FILE* f = fopen(fn, "r");

  unsigned char buf[JDISK_SECTOR_SIZE];

  unsigned short check_lv = Read_Link(0);

  unsigned short lv = Read_Link(0);

  int first_block = S + lv - 1;

  // printf("%d %d %d\n", S, lv, first_block);

  int preBlock;

  int firstFreeBlockLv;

  int over = 0;

  int numFree = 0;

  unsigned int sectors_number = 0;

  unsigned int fileSize;

  fseek(f,0L,SEEK_END);
  fileSize = ftell(f);
  rewind(f);


  unsigned int FAT_sectors_needed = ceil(fileSize *1.0 / JDISK_SECTOR_SIZE);

  while(check_lv != 0) {

      int link = check_lv;

      check_lv = Read_Link(link);

      sectors_number++;


  }



  if( sectors_number < FAT_sectors_needed){


        fprintf(stderr,"%s is too big for the disk  (%d vs %d)\n" , fn ,fileSize, 1024 * sectors_number);


    //freeMemory();

    exit(0);

  }else{
 while(lv != 0 && over != 1) {

    int block = S + lv - 1;

    // read file
    size_t res = fread(buf, 1, JDISK_SECTOR_SIZE, f);

    if(res == JDISK_SECTOR_SIZE) {

      // read link value
      int link = block - S + 1;
      lv = Read_Link(link);

    }else if(res == 0) {

      // previous link
      int preLink = preBlock - S + 1;

      // first free block value
      firstFreeBlockLv = lv;

      // write link
      Write_Link(preLink, 0);

      over = 1;

      break;

    }else{

      int link = block - S + 1;

      if(res == 1023) {

        // write last byte
        buf[JDISK_SECTOR_SIZE - 1] = 0xff;

      }else{

        // write last byte
        buf[JDISK_SECTOR_SIZE - 1] = (res >> 8) & 0xff;

        buf[JDISK_SECTOR_SIZE - 2] = res & 0xff;
      }

      // int link = block - S + 1;

      // read link value
      lv = Read_Link(link);

      firstFreeBlockLv = lv;

      // indicate last block and less than 1024 bytes
      Write_Link(link, link);

      over = 1;

    }


    // write to disk
    jdisk_write(jd, block, buf);

    preBlock = block;

    numFree++;

  }

    if(ftell(f) == fileSize){
        over = 1;
      }

  if(over == 1) {

    // write first free block
    Write_Link(0, firstFreeBlockLv);

    // flush links
    Flush_Links();

    printf("New file starts at sector %d\n", first_block);

  }else{

    int numAdd = 0;

    // compute additional blocks needed
    while(1) {

      size_t res = fread(buf, 1, JDISK_SECTOR_SIZE, f);

      if(res == 0) {
        break;
      }

      numAdd++;

      if(res < JDISK_SECTOR_SIZE) {
        break;
      }
    }

    printf("Not enough free sectors (%d) for %s, which needs %d.\n", numFree, fn, numFree + numAdd);

    freeMemory();

    exit(0);
  }
 }


}



int main(int argc, char **argv)
{

  if(argc != 4 && argc != 5) {

    usage(NULL);
  }



  if (strcmp(argv[2], "export") == 0) {

     int start_block;

     if (sscanf(argv[3], "%d", &start_block) == 0)
         usage((char *) "Start block must be an integer.\n");

     setUp(argv[1]);

     exportFile(start_block, argv[4]);

     freeMemory();
  }

  else if(strcmp(argv[2], "import") == 0) {

     setUp(argv[1]);

     importFile(argv[3]);

     freeMemory();

  }else{

     usage((char *) "Must be import or export.\n");
  }

  printf("Reads: %ld\n", jdisk_reads(jd));
  printf("Writes: %ld\n", jdisk_writes(jd));
  exit(0);
}
