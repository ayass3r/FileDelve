int main (int argc, char **argv)
{
  char *Directory;
  char Path[2048];
  char HFormat[10];
  QDir *DirectoryQT;
  struct dirent *DirInode;  //Enterprize??
  int Flags = 0;
  int Options;
  int argvCount = 0;
  int i; //for loop, needed later on
  long TotalSize = 0;
  struct stat Buffer; //Enterprize???
  struct FileStat StatBuffer;
  int Stat;

  {
    /* data */
  };

  for(i = 1; i < argc; i++)
    if(argv[i][0] != '-')
      argv_count++;
    while ((Option = getopt(argc, argv, "irbsdaAtloHhf")) != -1) {
      switch (Option) {
        case 'i':
          Flags |= FLAG_SHOW_INODE;
          break;
        case 'r':
          Flags |= FLAG_RECURSE_MASK;
          break;
        case 'b':
          Flags |= FLAG_SHOW_BLOCKS;
          break;
        case 's':
          Flags |= FLAG_SHOW_SIZE;
          break;
        case 'd':
          Flags |= FLAG_SHOW_DIRECTORY;
          break;
        case 'a':
          Flags |= FLAG_SHOW_ALL_FILES;
          break;
        case 'A':
          Flags |= FLAG_SHOW_ALL_ATTRS;
          break;
        case 't':
          Flags |= FLAG_SHOW_FILE_TYPE;
          break;
        case 'l':
          Flags |= FLAG_SHOW_LINKS;
          break;   
        case 'o':
          Flags |= FLAG_SHOW_OWNER;
          break;
        case 'H':
          Flags |= FLAG_SHOW_HUMAN_FORMAT;
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
          printf("-A\tShow all attributes\n");
          printf("-t\tShow file Type\n");
          printf("-l\tShow Link\n");
          printf("-o\tShow file owner\n");
          exit(0);
          break;
        case 'f':
          Flags |= FLAG_ONLY_FILES_AND_DIRS;
          break;   
        default:
           fprintf(stderr, "Invalid option %c, Usage: %s [-options] <path>\n", Option, argv[0]);
           exit(EXIT_FAILURE);
      }
  }

  if (argvCount > 1){
    fprintf(stderr, "Usage: %s [-options] <path>\n", argv[0]);
    exit(1);
  }else if (argvCount == 0)
    Directory = "/";
  else
    Directory = argv[optind];

  if(strcmp(Directory, ".") != 0 && strcmp(Directory, "..") != 0){
      

      if (lstat(Directory, &Buffer) == -1) {
               perror("Stat");
      }

      TotalSize = (long long) Buffer.st_size;
      
  }

  if((DirectoryQT = opendir(Directory)) == NULL){
    perror("opendir");
    exit(1);
  }



  while((DirInode = readdir(DirectoryQT)) != NULL){

    if(DirInode->d_name[0] != '.' || (Flags & FLAG_SHOW_ALL_FILES)){
      

      if(stat_file(Directory, DirInode->d_name, &StatBuffer, Flags) != 0){
        sprintf(Path, "Stat error while opening %s%s", Directory, DirInode->d_name);
        perror(Path);
      }else{
        //print_stat(&StatBuffer, Flags);
        TotalSize += StatBuffer.TotalSize;
      }
        
    }
  }

  if (Flags & FLAG_SHOW_HUMAN_FORMAT){
      get_human_format(TotalSize, h_format);
      printf("Total directory Size: %s\n", h_format);
  }else{
      printf("Total directory Size: %lldB\n", TotalSize);
  }

  closedir(DirectoryQT);  
  
  return 0;

}

int stat_file(char *path, char *Name, struct file_stat *StatBuffer, int Flags){
  int fd;
  struct Stat Buffer;
  char Path[MAX_FULL_PATH_SIZE];

  if (path[strlen(path) - 1] != '/')
    strcat(path, "/");

  sprintf(Path, "%s%s", path, Name);

  if ((fd=open(Path, O_RDONLY)) < 0){
     return -1;
  }

  if (lstat(Path, &Buffer) == -1) {
    return -1;
  }

 strcpy(StatBuffer->Name, Name);
 switch (Buffer.st_mode & S_IFMT) {
   case S_IFBLK: StatBuffer->Type = BLOCK_DEVICE;   break;
   case S_IFCHR: StatBuffer->Type = CHARACTER_DEVICE;  break;
   case S_IFDIR: StatBuffer->Type = DIRECTORY;   break;
   case S_IFIFO: StatBuffer->Type = FIFO_PIPE; break;
   case S_IFLNK: StatBuffer->Type = SYMLINK;   break;
   case S_IFREG: StatBuffer->Type = REGULAR_FILE;   break;
   case S_IFSOCK: StatBuffer->Type = SOCKET;   break;
   default:  StatBuffer->Type = UNKNOWN;  break;
 }

 StatBuffer->Inode = (long)Buffer.st_ino;
 StatBuffer->Link = (long) Buffer.st_nlink;
 strcpy(StatBuffer->User, getpwuid(Buffer.st_uid)->pw_name);
 strcpy(StatBuffer->Group, getgrgid(Buffer.st_gid)->gr_name);
 if (((!(Flags & FLAG_ONLY_FILES_AND_DIRS)) || ((Buffer.st_mode & S_IFMT) == S_IFDIR) || ((Buffer.st_mode & S_IFMT) == S_IFREG)) && !(Buffer.st_blocks == 0)){
   StatBuffer->Size = (long long) Buffer.st_size;
   StatBuffer->TotalSize = (long long) Buffer.st_size;
 }else{
   StatBuffer->Size = 0;
   StatBuffer->TotalSize = (long long) 0;
 }
 if (((Flags & FLAG_RECURSE_MASK) || (Flags & FLAG_SHOW_ALL_ATTRS)) && StatBuffer->Type == DIRECTORY){
   // printf("Recursing: %s\n", Path);
    StatBuffer->TotalSize += get_dir_size(Path, Flags);
 }
 
 StatBuffer->NumberOfBlocks = (long long) Buffer.st_blocks;

 return close(fd);
}

void print_stat(struct file_stat *st, int Flags){
  char h_format[10];
  if(Flags & FLAG_SHOW_FILE_TYPE || Flags & FLAG_SHOW_ALL_ATTRS)
    switch (st->Type) {
     case BLOCK_DEVICE:  printf("b ");   break;
     case CHARACTER_DEVICE:  printf("c ");   break;
     case DIRECTORY:  printf("d ");   break;
     case FIFO_PIPE:  printf("p ");   break;
     case SYMLINK:  printf("l ");   break;
     case REGULAR_FILE:  printf("f ");   break;
     case SOCKET: printf("s ");   break;
     default:       printf("- ");   break;
   }
   if(Flags & FLAG_SHOW_INODE || Flags & FLAG_SHOW_ALL_ATTRS)
      printf("%8ld ", st->Inode);
   if(Flags & FLAG_SHOW_LINKS || Flags & FLAG_SHOW_ALL_ATTRS)
      printf("%8ld ", st->Link);
   if(Flags & FLAG_SHOW_LINKS || Flags & FLAG_SHOW_ALL_ATTRS)
      printf("%8ld  ", st->Link);
   if(Flags & FLAG_SHOW_OWNER || Flags & FLAG_SHOW_ALL_ATTRS)
      printf("%4s:%4s  ", st->User, st->Group);
   if(Flags & FLAG_SHOW_SIZE || Flags & FLAG_SHOW_ALL_ATTRS){
      if (Flags & FLAG_SHOW_HUMAN_FORMAT){
        get_human_format(st->Size, h_format);
        printf("%s ", h_format);
      }else
        printf("%lldB  ", st->Size);
      
   }
   if(Flags & FLAG_RECURSE_MASK || Flags & FLAG_SHOW_ALL_ATTRS){
      if (Flags & FLAG_SHOW_HUMAN_FORMAT){
        get_human_format(st->TotalSize, h_format);
        printf("%s ", h_format);
      }else
         printf("%lldB  ", st->TotalSize);
      
   }
   if(Flags & FLAG_SHOW_BLOCKS || Flags & FLAG_SHOW_ALL_ATTRS)
      printf("%5lldb ", st->NumberOfBlocks);
   printf("%s \n", st->Name);
}
// proc root run sys tmp
long long get_dir_size(char *r_dir, int Flags){
  DIR *DirectoryQT;
  struct dirent *DirInode;
  struct Stat Buffer;
  char Path[MAX_FULL_PATH_SIZE];
  long long TotalSize = 0;

  if (r_dir[strlen(r_dir) - 1] != '/')
    strcat(r_dir, "/");

  if((DirectoryQT = opendir(r_dir)) == NULL){
    perror("opendir");
    printf("%s\n", r_dir);
    return 0;
  }


  while((DirInode = readdir(DirectoryQT)) != NULL){
    if(strcmp(DirInode->d_name, ".") != 0 && strcmp(DirInode->d_name, "..") != 0){    
      strcpy(Path, r_dir);
      strcat(Path, DirInode->d_name);
      if (lstat(Path, &Buffer) == -1) {
                perror("Stat");
                continue;
        }

        if (((!(Flags & FLAG_ONLY_FILES_AND_DIRS)) || ((Buffer.st_mode & S_IFMT) == S_IFDIR) || ((Buffer.st_mode & S_IFMT) == S_IFREG)) && !(Buffer.st_blocks == 0))
          TotalSize += (long long) Buffer.st_size;
        if((Buffer.st_mode & S_IFMT & S_IFDIR) && !((Buffer.st_mode & S_IFMT & S_IFLNK))){
            TotalSize += get_dir_size(Path, Flags);
        }
    }
    }

  closedir(DirectoryQT);
  return TotalSize;
}

int get_human_format(long long number, char *format){
  if (number <= 1000){
    return sprintf(format, "%lldB", number);
  }else if (number <= 1000000){
    return sprintf(format, "%1.1fKBs", (float)number / 1000.0);
  }else if(number <= 1000000000){
    return sprintf(format, "%1.1fMBs", (float)number / 1000000.0);
  }else{
    return sprintf(format, "%1.1fGBs", (float)number / 1000000000.0);
  }
}




struct FileStat{
  char Name[2048];
  FILE_TYPE Type;
  long Inode;
  long Link;
  char User[64];
  char Group[64];
  long long Size;
  long long TotalSize;
  long long NumberOfBlocks;
};