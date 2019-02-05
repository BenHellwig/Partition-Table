#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>


typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;

struct partition {
  u8 drive;
  u8 head;
  u8 sector;
  u8 cylinder;

  u8 sys_type;

  u8 end_head;
  u8 end_sector;
  u8 end_cylinder;

  u32 start_sector;
  u32 nr_sectors;
};

int getsector(int fd, int sector, char buf[])
{
  lseek(fd, (long)(sector*512), SEEK_SET);
  read(fd, buf, 512);
}


int main()
{
  int fd;
  char buf[512];
  struct partition *p;
  int extStart;
  struct partition *e;
  int relStart;    /* beginSector relative to EXT type's 
                      beginSector (extStart)*/

  int flag = 0;
  int adjustedStart;
  int i = 0;

  fd = open("vdisk", O_RDONLY);
  getsector(fd, 0, buf);

  p = (struct partition *)&buf[0x1BE];

 
  for (i=1; i <=4; ++i)
    {
        printf("%d start_sector=%d end_sector=%d type=%x\n", i, p->start_sector, (p->start_sector + p->nr_sectors - 1), p->sys_type);
	

	if (p->sys_type == 5)
	  {
	    extStart = p->start_sector;
            getsector(fd, extStart, buf);
            relStart = extStart;
	    flag = 0;
	    adjustedStart = 0;
	    do
	    {
	      ++i; // Keeps track of partition number
	      e = (struct partition *)&buf[0x1BE];
	      relStart = extStart + e->start_sector;
	      printf("%d start_sector=%d end_sector=%d type=%x\n", i,extStart + adjustedStart + e->start_sector , (extStart + adjustedStart + e->start_sector + e->nr_sectors - 1), e->sys_type);
	      ++e;
	      if (e->start_sector == 0)
	      {
		  flag = 1;
	      }
	      else
		{
	      relStart = extStart + e->start_sector;
	      adjustedStart = e->start_sector;
	      getsector(fd, relStart, buf);
		}
	    } while (flag == 0);
	    
	  }
	++p;
    }
  
}
