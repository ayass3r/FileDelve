#include <QChar>
#include <QDir>
#include <QString>

#include <dirent.h> //Directory Entries Format
#include <unistd.h> //All ffollowing 3 required for lstat system call
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <errno.h>
#include <getopt.h>
#include <QDebug>
#include <stdio.h>
#include <string.h>

//Declaring FileType. Using typedef enum to prevent other types from being added
typedef enum {Direct, File, Character, Block, SystemLink, Unknown} FileType;

//Declaring File Statistics
struct FileStat{
  char Name[2048];
  FileType Type;
  long Inode;
  long Link;
  char User[64];
  char Group[64];
  long long Size;
  long long TotalSize;
  long long NumberOfBlocks;
};


//Prototypes
int StatFile( char *Path, char *Name, struct FileStat *StatBuffer, int Flags);
void Print(struct FileStat *Stats, int Flags, bool &More , char* WholePath);
long long Directory(char *RDirectory, int flags);
int HumanFormat(long long Number, char *Format);
int Traverse(int argc, char **argv);

int main (int argc, char **argv)
{
  freopen("example.txt", "wt", stdout);
  //DU.c-> example.txt; //Revise Enterprize
  char *Directory;
  char Path[2048];
  char HFormat[10];
  DIR *DirectoryQT;
  bool More = 0;
  struct dirent *DirInode;  //Enterprize??
  int Flags = 0;
  int Options;
  int argvCount = 0;
  int i; //for loop, needed later on
  long TotalSize = 0;
  struct stat Buffer; //Enterprize???
  struct FileStat StatBuffer;
  int Stat;


  for(i = 0; i < argc; i++)
    if(argv[i][0] != '-')
      argvCount++;
    while ((Options = getopt(argc, argv, "irbsdaAtloHh")) != -1) {
      switch (Options) {
        case 'i':
          Flags |= 1;
          break;
        case 'r':
          Flags |= 2;
          break;
        case 'b':
          Flags |= 4;
          break;
        case 's':
          Flags |= 8;
          break;
        case 'd':
          Flags |= 16;
          break;
        case 'a':
          Flags |= 32;
          break;
        case 'A':
          Flags |= 64;
          break;
        case 't':
          Flags |= 128;
          break;
        case 'l':
          Flags |= 256;
          break;
        case 'o':
          Flags |= 512;
          break;
        case 'H':
          Flags |= 1024;
          break;
        case 'h':
          printf("Disk Analyzer 0.1\n");
          printf("Usage: %s [-options] <path>\n", argv[0]);
          printf("Options: \n");
          printf("-H\tShow sizes in human readable format\n");
          printf("-f\tCalculate regular files and directoris only\n");
          printf("-h\tDisplay this help\n");
          printf("-i\tShow Inode value\n");
          printf("-r\tAnalyze directories recursively\n");
          printf("-b\tShow occupied blocks on the disk\n");
          printf("-s\tShow file Size\n");
          printf("-d\tShow directories\n");
          printf("-a\tShow all files including hidden files\n");
          printf("-t\tShow file Type\n");
          printf("-l\tShow Link\n");
          printf("-o\tShow file owner\n");
          exit(0);
          break;
        case 'f':
          Flags |= 2048;
          break;
        default:
           fprintf(stderr, "Invalid option %c, Usage: %s [-options] <Path>\n", Options, argv[0]);
           exit(EXIT_FAILURE);
      }
  }


  if (argvCount > 1){
    fprintf(stderr, "Usage: %s [-options] <path>\n", argv[0]);
    exit(1);
  }else if (argvCount == 0)
    Directory = "/";
  else
    Directory = argv[0];

  if(strcmp(Directory, ".") != 0 && strcmp(Directory, "..") != 0){


      if (lstat(Directory, &Buffer) == -1) {
               perror("Stat");
      }

      TotalSize = (long long) Buffer.st_size;

  }

  Directory = "/home/ayass3r/CJF/";  ///UNTIL GUI THEN REMOVE THIS LINE AND SEND THE PATH IN THE ARGV!! MAKE SURE YOU PASS THE ENDING SLASH!!
    DirectoryQT = opendir(Directory);
   qDebug() << errno;
  if(DirectoryQT == NULL){
    perror("opendir");
    exit(1);
  }


  while((DirInode = readdir(DirectoryQT)) != NULL){

    if(DirInode->d_name[0] != '.' || (Flags & 32)){

      if(StatFile(Directory, DirInode->d_name, &StatBuffer, Flags) != 0){
        sprintf(Path, "Stat error while opening %s%s", Directory, DirInode->d_name);
        perror(Path);
      }else{
        Print(&StatBuffer, Flags, More, Path);
        TotalSize += StatBuffer.TotalSize;
        if (More = 1)
            bool Success = Traverse(argc, argv);
      }

    }
  }

  if (Flags & 1024){
      HumanFormat(TotalSize, HFormat);
      printf("Total directory Size: %s\n", HFormat);
  }else{
      printf("Total directory Size: %lldB\n", TotalSize);
  }

  closedir(DirectoryQT);

  return 0;
}


int StatFile( char *Path, char *Name, struct FileStat *StatBuffer, int Flags)
{
  int FileDirectory;
  struct stat Buffer;
  char FullPath[2048];

 // if (Path[strlen(Path) - 1] != '/')  //Add a missing / if absent
 //   strcat(Path, "/");

  sprintf(FullPath, "%s%s", Path, Name);


  if ((FileDirectory=open(FullPath, O_RDONLY)) < 0){
     return -1;
  }

  if (lstat(FullPath, &Buffer) == -1) {
    return -1;
  }

 strcpy(StatBuffer->Name, FullPath);
 switch (Buffer.st_mode & S_IFMT) {
   case S_IFBLK: StatBuffer->Type = Block;   break;
   case S_IFCHR: StatBuffer->Type = Character;  break;
   case S_IFDIR: StatBuffer->Type = Direct;   break;
   case S_IFLNK: StatBuffer->Type = SystemLink;   break;
   case S_IFREG: StatBuffer->Type = File;   break;
   default:  StatBuffer->Type = Unknown;  break;
 }

 StatBuffer->Inode = (long)Buffer.st_ino;
 StatBuffer->Link = (long) Buffer.st_nlink;
 strcpy(StatBuffer->User, getpwuid(Buffer.st_uid)->pw_name);
 strcpy(StatBuffer->Group, getgrgid(Buffer.st_gid)->gr_name);

 if (((!(Flags & 2048)) || ((Buffer.st_mode & S_IFMT) == S_IFDIR) || ((Buffer.st_mode & S_IFMT) == S_IFREG)) && !(Buffer.st_blocks == 0)){
   StatBuffer->Size = (long long) Buffer.st_size;
   StatBuffer->TotalSize = (long long) Buffer.st_size;
 }else{
   StatBuffer->Size = 0;
   StatBuffer->TotalSize = (long long) 0;
 }

 if (((Flags & 2) || (Flags & 64)) && StatBuffer->Type == Direct){
    StatBuffer->TotalSize += Directory(Path, Flags);
 }

 StatBuffer->NumberOfBlocks = (long long) Buffer.st_blocks;

 return close(FileDirectory);
}

void Print(struct FileStat *Stats, int Flags, bool &More , char* WholePath)
{
  char HFormat[10];

  if(Flags & 128 || Flags & 64)
    switch (Stats->Type) {
     case Block:  printf("b ");   break;
     case Character:  printf("c ");   break;
     case Direct:  printf("d ");   break;
     case SystemLink:  printf("l ");   break;
     case File:  printf("f ");   break;
     default:       printf("- ");   break;
   }
   if(Flags & 1 || Flags & 64)
      printf("%8ld\t\t", Stats->Inode);
   if(Flags & 256 || Flags & 64)
      printf("%8ld\t\t", Stats->Link);
   if(Flags & 512 || Flags & 64)
      printf("%4s:%4s\t\t", Stats->User, Stats->Group);
   if(Flags & 8 || Flags & 64){
      if (Flags & 1024){
        HumanFormat(Stats->Size, HFormat);
        printf("%s\t\t", HFormat);
      }else
        printf("%lldB\t\t", Stats->Size);

   }
   if(Flags & 2 || Flags & 64){
      if (Flags & 1024){
        HumanFormat(Stats->TotalSize, HFormat);
        printf("%s\t\t", HFormat);
      }else
         printf("%lldB\t\t", Stats->TotalSize);

   }
   if(Flags & 4 || Flags & 64)
      printf("%5lldb\t\t", Stats->NumberOfBlocks);
   printf("%s\n", Stats->Name);

   if (Stats->Type == Direct)
       More = 1;
   else
       More = 0;

  WholePath = Stats->Name;
}

long long Directory(char *RDirectory, int Flags)
{
  DIR *DirectoryQT;
  struct dirent *DirInode;
  struct stat Buffer;
  char Path[2048];
  long long TotalSize = 0;

  if (RDirectory[strlen(RDirectory) - 1] != '/')
    strcat(RDirectory, "/");

  if((DirectoryQT = opendir(RDirectory)) == NULL){
    perror("opendir");
    printf("%s\n", RDirectory);
    return 0;
  }


  while((DirInode = readdir(DirectoryQT)) != NULL){
    if(strcmp(DirInode->d_name, ".") != 0 && strcmp(DirInode->d_name, "..") != 0){
      strcpy(Path, RDirectory);
      strcat(Path, DirInode->d_name);
      if (lstat(Path, &Buffer) == -1) {
                perror("Stat");
                continue;
        }

        if (((!(Flags & 8)) || ((Buffer.st_mode & S_IFMT) == S_IFDIR) || ((Buffer.st_mode & S_IFMT) == S_IFREG)) && !(Buffer.st_blocks == 0))
          TotalSize += (long long) Buffer.st_size;
        if((Buffer.st_mode & S_IFMT & S_IFDIR) && !((Buffer.st_mode & S_IFMT & S_IFLNK))){
            TotalSize += Directory(Path, Flags);
        }
    }
    }

  closedir(DirectoryQT);
  return TotalSize;
}

int HumanFormat(long long Number, char *Format)
{
  if (Number <= 1000){
    return sprintf(Format, "%lldB\t\t", Number);
  }else if (Number <= 1000000){
    return sprintf(Format, "%1.1fKBs\t\t", (float)Number / 1000.0);
  }else if(Number <= 1000000000){
    return sprintf(Format, "%1.1fMBs\t\t", (float)Number / 1000000.0);
  }else{
    return sprintf(Format, "%1.1fGBs\t\t", (float)Number / 1000000000.0);
  }
}

int Traverse(int argc, char **argv)
{
  char *Directory;
  char Path[2048];
  char HFormat[10];
  DIR *DirectoryQT;
  struct dirent *DirInode;  //Enterprize??
  int Flags = 0;
  int Options;
  bool More = 0;
  int argvCount = 0;
  int i; //for loop, needed later on
  long TotalSize = 0;
  struct stat Buffer; //Enterprize???
  struct FileStat StatBuffer;
  char *slash = "/";


  for(i = 0; i < argc; i++)
    if(argv[i][0] != '-')
      argvCount++;
    while ((Options = getopt(argc, argv, "irbsdaAtloHh")) != -1) {
      switch (Options) {
        case 'i':
          Flags |= 1;
          break;
        case 'r':
          Flags |= 2;
          break;
        case 'b':
          Flags |= 4;
          break;
        case 's':
          Flags |= 8;
          break;
        case 'd':
          Flags |= 16;
          break;
        case 'a':
          Flags |= 32;
          break;
        case 'A':
          Flags |= 64;
          break;
        case 't':
          Flags |= 128;
          break;
        case 'l':
          Flags |= 256;
          break;
        case 'o':
          Flags |= 512;
          break;
        case 'H':
          Flags |= 1024;
          break;
        case 'h':
          printf("Disk Analyzer 0.1\n");
          printf("Usage: %s [-options] <path>\n", argv[0]);
          printf("Options: \n");
          printf("-H\tShow sizes in human readable format\n");
          printf("-f\tCalculate regular files and directoris only\n");
          printf("-h\tDisplay this help\n");
          printf("-i\tShow Inode value\n");
          printf("-r\tAnalyze directories recursively\n");
          printf("-b\tShow occupied blocks on the disk\n");
          printf("-s\tShow file Size\n");
          printf("-d\tShow directories\n");
          printf("-a\tShow all files including hidden files\n");
          printf("-t\tShow file Type\n");
          printf("-l\tShow Link\n");
          printf("-o\tShow file owner\n");
          exit(0);
          break;
        case 'f':
          Flags |= 2048;
          break;
        default:
           fprintf(stderr, "Invalid option %c, Usage: %s [-options] <Path>\n", Options, argv[0]);
           exit(EXIT_FAILURE);
      }
  }


  if (argvCount > 1){
    fprintf(stderr, "Usage: %s [-options] <path>\n", argv[0]);
    exit(1);
  }else if (argvCount == 0)
    Directory = "/";
  else
    Directory = "/";
  if(strcmp(Directory, ".") != 0 && strcmp(Directory, "..") != 0){


      if (lstat(Directory, &Buffer) == -1) {
               perror("Stat");
      }

      TotalSize = (long long) Buffer.st_size;

  }
    strcat(Directory,slash);
    DirectoryQT = opendir(Directory);
   qDebug() << errno;
  if(DirectoryQT == NULL){
    perror("opendir");
    exit(1);
  }



  while((DirInode = readdir(DirectoryQT)) != NULL){

    if(DirInode->d_name[0] != '.' || (Flags & 32)){

      if(StatFile(Directory, DirInode->d_name, &StatBuffer, Flags) != 0){
        sprintf(Path, "Stat error while opening %s%s", Directory, DirInode->d_name);
        perror(Path);
      }else{
        Print(&StatBuffer, Flags, More, Path );
        TotalSize += StatBuffer.TotalSize;
      }

    }
  }

  if (Flags & 1024){
      HumanFormat(TotalSize, HFormat);
      printf("Total directory Size: %s\n", HFormat);
  }else{
      printf("Total directory Size: %lldB\n", TotalSize);
  }

  closedir(DirectoryQT);

  return 0;
}



/*
Legend: FLAGS!!
Flag Show INode = 1
Flag Recurse Mask = 2
Flag Show Blocks = 4
Flag Show Size = 8
Flag Show Directory = 16
Flag Show All files = 32
Flag Show All Attrs = 64
Flag Show File Type = 128
Flag Show Link = 256
Flag Show Owner = 512
Flag Show Human Format = 1024
Flag Only Files and Directories  = 8
*/
