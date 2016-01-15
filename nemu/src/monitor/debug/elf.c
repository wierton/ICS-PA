#include "common.h"
#include <stdlib.h>

#include "monitor/readelf.h"

char *exec_file = NULL;

#define INF_FUNC 9999
uint64_t total_record = 0;
uint64_t record[10000] = {0};
static int h[10000] = {0};
uint64_t pfunc = INF_FUNC;

static char *strtab = NULL;
static Elf32_Sym *symtab = NULL;
static int nr_symtab_entry;
static int nr_strtab = 0;

void load_elf_tables(int argc, char *argv[]) {
	int ret;
	Assert(argc == 2, "run NEMU with format 'nemu [program]'");
	exec_file = argv[1];

	FILE *fp = fopen(exec_file, "rb");
	Assert(fp, "Can not open '%s'", exec_file);

	uint8_t buf[sizeof(Elf32_Ehdr)];
	ret = fread(buf, sizeof(Elf32_Ehdr), 1, fp);
	assert(ret == 1);

	/* The first several bytes contain the ELF header. */
	Elf32_Ehdr *elf = (void *)buf;
	char magic[] = {ELFMAG0, ELFMAG1, ELFMAG2, ELFMAG3};

	/* Check ELF header */
	assert(memcmp(elf->e_ident, magic, 4) == 0);		// magic number
	assert(elf->e_ident[EI_CLASS] == ELFCLASS32);		// 32-bit architecture
	assert(elf->e_ident[EI_DATA] == ELFDATA2LSB);		// littel-endian
	assert(elf->e_ident[EI_VERSION] == EV_CURRENT);		// current version
	assert(elf->e_ident[EI_OSABI] == ELFOSABI_SYSV || 	// UNIX System V ABI
			elf->e_ident[EI_OSABI] == ELFOSABI_LINUX); 	// UNIX - GNU
	assert(elf->e_ident[EI_ABIVERSION] == 0);			// should be 0
	assert(elf->e_type == ET_EXEC);						// executable file
	assert(elf->e_machine == EM_386);					// Intel 80386 architecture
	assert(elf->e_version == EV_CURRENT);				// current version


	/* Load symbol table and string table for future use */

	/* Load section header table */
	uint32_t sh_size = elf->e_shentsize * elf->e_shnum;
	Elf32_Shdr *sh = malloc(sh_size);
	fseek(fp, elf->e_shoff, SEEK_SET);
	ret = fread(sh, sh_size, 1, fp);
	assert(ret == 1);

	/* Load section header string table */
	char *shstrtab = malloc(sh[elf->e_shstrndx].sh_size);
	fseek(fp, sh[elf->e_shstrndx].sh_offset, SEEK_SET);
	ret = fread(shstrtab, sh[elf->e_shstrndx].sh_size, 1, fp);
	assert(ret == 1);

	int i;
	for(i = 0; i < elf->e_shnum; i ++) {
		if(sh[i].sh_type == SHT_SYMTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".symtab") == 0) {
			/* Load symbol table from exec_file */
			symtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(symtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
			nr_symtab_entry = sh[i].sh_size / sizeof(symtab[0]);
		}
		else if(sh[i].sh_type == SHT_STRTAB && 
				strcmp(shstrtab + sh[i].sh_name, ".strtab") == 0) {
			/* Load string table from exec_file */
			nr_strtab = sh[i].sh_size;
			strtab = malloc(sh[i].sh_size);
			fseek(fp, sh[i].sh_offset, SEEK_SET);
			ret = fread(strtab, sh[i].sh_size, 1, fp);
			assert(ret == 1);
		}
	}

	free(sh);
	free(shstrtab);

	assert(strtab != NULL && symtab != NULL);

	fclose(fp);
}

void show_symtab()
{
	int i;
	printf(".symtab\n");
	printf("Num:\t");
	printf("Value\t\t");
	printf("Size\t");
	printf("Type\t");
	printf("Bind\t");
	printf("Vis\t");
	printf("Ndx\t");
	printf("NAME\n");
	for(i = 0;i < nr_symtab_entry;i++)
	{
		printf("%d\t", i);
		printf("$0x%08x\t",symtab[i].st_value);
		printf("%u\t",symtab[i].st_size);
		switch(ELF32_ST_TYPE(symtab[i].st_info))
		{
			case STT_NOTYPE	:printf("NOTYPE\t");	break;
			case STT_OBJECT	:printf("OBJECT\t");	break;
			case STT_FUNC	:printf("FUNC\t");		break;
			case STT_SECTION:printf("SECTION\t");	break;
			case STT_FILE	:printf("FILE\t");		break;
			case STT_LOPROC	:printf("LOPROC\t");	break;
			case STT_HIPROC	:printf("HIPROC\t");	break;
			default			:printf("UNKNOWN\t");	break;
		}
		switch(ELF32_ST_BIND(symtab[i].st_info))
		{
			case STB_LOCAL	:printf("LOCAL\t");		break;
			case STB_GLOBAL	:printf("GLOBAL\t");	break;
			case STB_WEAK	:printf("WEAK\t");		break;
			case STB_LOPROC	:printf("LOPROC\t");	break;
			case STB_HIPROC	:printf("HIPROC\t");break;
			default			:printf("UNKNOWN\t");	break;
		}
		switch(ELF32_ST_VISIBILITY(symtab[i].st_other))
		{
			case STV_DEFAULT	:printf("DEFAULT\t");	break;
			case STV_INTERNAL	:printf("INTERNAL\t");	break;
			case STV_HIDDEN		:printf("HIDDEN\t");	break;
			case STV_PROTECTED	:printf("PROTECTED\t");	break;
			default				:printf("UNKNOWN\t");	break;
		}
		printf("%hu\t",symtab[i].st_shndx);
		printf("%d:%s\n",symtab[i].st_name,strtab + symtab[i].st_name);
	}
}

swaddr_t find_var(char symbol[])
{
	int i,j;
	for(i = 0;i < nr_strtab;i++)
	{
		if(strcmp(symbol,strtab + i) == 0)
		{
			for(j = 0;j < nr_symtab_entry;j++)
				if(symtab[j].st_name == i)
					if(ELF32_ST_TYPE(symtab[j].st_info) == STT_OBJECT)
						return symtab[j].st_value;
		}
	}	
	return 0;
}

int find_func_addr(swaddr_t func_addr)
{
	int i;
	for(i = 0;i < nr_symtab_entry;i++)
	{
		if(symtab[i].st_value <= func_addr && func_addr < symtab[i].st_value + symtab[i].st_size)
		{
			if(ELF32_ST_TYPE(symtab[i].st_info) == STT_FUNC)
			{
				pfunc = i;
				return 0;
			}
		}
	}
	pfunc = INF_FUNC;
	return 0;
}

int print_perf()
{
	int i, j;
	FILE *fp = fopen("game.perf", "w+");
	for(i = 0; i <= INF_FUNC; i++)
		h[i] = i;
/*bubble sort*/
	for(j = INF_FUNC; j >= 1; j--)
	{
		for(i = 0; i < j; i++)
		{
			if(record[h[i]] < record[h[i+1]])
			{
				int tmp = h[i];
				h[i] = h[i + 1];
				h[i + 1] = tmp;
			}
		}
	}

	fprintf(fp, "%9lld\t%f%%\t%s\n\n", record[INF_FUNC], 100*(float)record[INF_FUNC]/(float)total_record, "kernel");
	for(i = 0;i <= INF_FUNC;i++)
	{
		if(h[i] < nr_symtab_entry && ELF32_ST_TYPE(symtab[h[i]].st_info) == STT_FUNC)
		{
			fprintf(fp, "%9lld\t%f%%\t%s\n", record[h[i]], 100*(float)record[h[i]]/(float)total_record, strtab + symtab[h[i]].st_name);
		}
	}
	fclose(fp);
	return 0;
}

inline void func_cost_calc()
{
	record[pfunc] ++;
	total_record ++;
}

int find_func(swaddr_t func_addr, char *func_name)
{
	int i;
	for(i = 0;i < nr_symtab_entry;i++)
	{
		if(symtab[i].st_value <= func_addr && func_addr <= symtab[i].st_value + symtab[i].st_size)
		{
			if(ELF32_ST_TYPE(symtab[i].st_info) == STT_FUNC)
			{
				strcpy(func_name, strtab + symtab[i].st_name);
				return 1;
			}
			else
				return 0;
		}
	}
	return 0;
}
