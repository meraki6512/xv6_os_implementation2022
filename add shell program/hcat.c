#include "types.h"
#include "stat.h"
#include "user.h"

int line;
char buf[512];

void
cat(int fd)
{
  int n;
  int cnt = 1;
  
  while((n = read(fd, buf, 1) > 0)) {

	  if (write(1, buf, n) != n) {
		  printf(1, "cat: write error\n");
		  exit();
	  }

	  if (buf[0]=='\n')
		  cnt++;

	  if (cnt > line)
		  break;
  }

  if(n < 0){
    printf(1, "hcat: read error\n");
    exit();
  }

}

int
main(int argc, char *argv[])
{
  int fd, i;

  if(argc < 3){
	  printf(1, "use: hcat line_number file_name\n");
	  exit();
  }

  for(i = 2; i < argc; i++){
    if((fd = open(argv[i], 0)) < 0){
      printf(1, "hcat: cannot open %s\n", argv[i]);
      exit();
    }
  	line = atoi(argv[1]);
    cat(fd);
    close(fd);
  }
  exit();
}
