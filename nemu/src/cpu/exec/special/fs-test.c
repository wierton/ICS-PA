#include "cpu/exec/helper.h"
#include "monitor/monitor.h"
#include "SDL/SDL.h"

inline void stop_nemu();
uint32_t swaddr_read(swaddr_t, size_t, uint8_t);

static uint8_t data[4096];

typedef struct {
	char *name;
	uint32_t size;
	uint32_t disk_offset;
} file_info;

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

typedef struct {
	bool opened;
	uint32_t offset;
} Fstate;

Fstate file_state[NR_FILES];

make_helper(fs_read_test)
{
	int i;
	int ret = cpu.eax;
	int fd = cpu.ebx;
	int buf = cpu.ecx;
	int len = cpu.edx;

	Assert(len >= 0, "len should be nonegative!\n");
	if(file_state[fd].offset > file_table[fd].size)
		Assert(ret == 0, "offset exceed size!\n");
	int end_pos = len + file_state[fd].offset;
	if(end_pos > file_table[fd].size)
	{
		len = file_table[fd].size - file_state[fd + 3].offset;
		if(len < 0) len = 0;
		Assert(len == ret, "return value not match!\n");
	}

	FILE * fp = fopen(file_table[fd].name, "rb");

	assert(fp != NULL);
	fseek(fp, file_state[fd].offset, SEEK_SET);
	fread(data, len, 1, fp);

	for(i = 0; i < len; i++)
	{
		uint8_t tmp = swaddr_read(4, buf + i, R_DS);
		assert(tmp == data[i]);
	}
	fclose(fp);
	return 1;
}

make_helper(fs_lseek_record)
{
	int fd = cpu.ebx;
	int offset = cpu.ecx;
	int whence = cpu.edx;
	if(fd < 0 || fd >= NR_FILES || file_state[fd + 3].opened == false)
	{
		printf("%d\n", fd);
		printf(" lseek file not exist or not open!\n");
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

	return 1;
}
