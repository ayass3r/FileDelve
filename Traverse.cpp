int Traverse(int argc, char **argv)
{
  char *Directory;
  char Path[2048];
  char HFormat[10];
  DIR *DirectoryQT;
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
        Print(&StatBuffer, Flags);
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
