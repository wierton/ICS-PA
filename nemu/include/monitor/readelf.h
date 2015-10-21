#ifndef __READELF_H__
#define __READELF_H__

static char *strtab = NULL;
static Elf32_Sym *symtab = NULL;
static int nr_symtab_entry;

void show_symtab();

#endif
