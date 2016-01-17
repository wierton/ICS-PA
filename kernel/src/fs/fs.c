#include "common.h"

typedef struct {
	char *name;
	uint32_t size;
	uint32_t disk_offset;
} file_info;

enum {SEEK_SET, SEEK_CUR, SEEK_END};

/* This is the information about all files in disk. */
static const file_info file_table[] = {
	{"stdin", -1, -1}, {"stdout", -1, -1}, {"stderr", -1, -1},
	{"1.rpg", 188864, 1048576}, {"2.rpg", 188864, 1237440},
	{"3.rpg", 188864, 1426304}, {"4.rpg", 188864, 1615168},
	{"5.rpg", 188864, 1804032}, {"abc.mkf", 1022564, 1992896},
	{"ball.mkf", 134704, 3015460}, {"data.mkf", 66418, 3150164},
	{"desc.dat", 16027, 3216582}, {"fbp.mkf", 1128064, 3232609},
	{"fire.mkf", 834728, 4360673}, {"f.mkf", 186966, 5195401},
	{"gop.mkf", 11530322, 5382367}, {"map.mkf", 1496578, 16912689},
	{"mgo.mkf", 1577442, 18409267}, {"m.msg", 188232, 19986709},
	{"mus.mkf", 331284, 20174941}, {"pat.mkf", 8488, 20506225},
	{"rgm.mkf", 453202, 20514713}, {"rng.mkf", 4546074, 20967915},
	{"sss.mkf", 557004, 25513989}, {"voc.mkf", 1997044, 26070993},
	{"wor16.asc", 5374, 28068037}, {"wor16.fon", 82306, 28073411},
	{"word.dat", 5650, 28155717},
};

#define NR_FILES (sizeof(file_table) / sizeof(file_table[0]))

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);

/* TODO: implement a simplified file system here. */

int prints(char[]);
int printx(uint32_t);

typedef struct {
	bool opened;
	uint32_t offset;
} Fstate;

Fstate file_state[NR_FILES];

int fs_open(const char *pathname, int flags)
{
	int i,j;
	for(i = 0; i < NR_FILES; i++)
	{
		j = 0;
		while(pathname[j] != 0 && file_table[i].name[j] != 0)
		{
			if(pathname[j] != file_table[i].name[j])
				break;
			j++;
		}
		if(pathname[j] == file_table[i].name[j])
		{
			file_state[i].opened = true;
			file_state[i].offset = 0;
			nemu_assert(i >= 3);
			return i;
		}
	}
	prints("error file:");
	prints((void *)pathname);
	prints("\n");
	nemu_assert(0);
	return -1;
}

int fs_read(int fd, void *buf, int len)
{
	if(fd < 0 || fd >= NR_FILES)
	{
		printx(fd);
		prints(" read file not exist!\n");
		nemu_assert(0);
		return -1;
	}
	if(file_state[fd].opened == false)
	{
		printx(fd);
		prints(" read file not open!\n");
		nemu_assert(0);
		return -1;
	}
	if(file_state[fd].offset > file_table[fd].size)
		return 0;
	int end_pos = len + file_state[fd].offset;
	if(end_pos > file_table[fd].size)
	{
		printx(fd);
		prints(" th file read exceed the boundary, len:");
		printx(len);
		prints(", off:");
		printx(file_state[fd].offset);
		prints(", off:");
		printx(file_table[fd].size);
		prints("\n");
//		nemu_assert(0);
		len = file_table[fd].size - file_state[fd].offset;
	}
	if(len < 0) len = 0;
	ide_read(buf, file_table[fd].disk_offset + file_state[fd].offset, len);

	file_state[fd].offset += len;
	return len;
}

int fs_write(int fd, void *buf, int len)
{
	nemu_assert(0);
	if(fd < 0 || fd >= NR_FILES || file_state[fd].opened == false)
	{
		printx(fd);
		prints(" write file not exist or not open!\n");
		nemu_assert(0);
		return -1;
	}
	int end_pos = len + file_state[fd].offset;
	if(end_pos > file_table[fd].size)
	{
		printx(fd);
		prints(" write exceed the boundary!\n");
		nemu_assert(0);
		len = file_table[fd].size - file_state[fd].offset;
	}
	if(len < 0) len = 0;
	ide_write(buf, file_table[fd].disk_offset + file_state[fd].offset, len);
	file_state[fd].offset += len;
	return len;
}


int fs_lseek(int fd, int offset, int whence)
{
	if(fd < 0 || fd >= NR_FILES || file_state[fd].opened == false)
	{
		printx(fd);
		prints(" lseek file not exist or not open!\n");
		nemu_assert(0);
		return 0;
	}
	switch(whence)
	{
		case SEEK_SET:file_state[fd].offset = offset;break;
		case SEEK_CUR:file_state[fd].offset += offset;break;
		case SEEK_END:
					  file_state[fd].offset = file_table[fd].size + offset;
					  break; 
	}
	
	return file_state[fd].offset;
}


int fs_close(int fd)
{
	if(fd < 0 || fd >= NR_FILES || file_state[fd].opened == false)
		return -1;
	file_state[fd].opened = false;
	return 0;
}
