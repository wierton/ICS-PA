#ifndef __READELF_H__
#define __READELF_H__

#include <elf.h>

void show_symtab();
swaddr_t find_var(char symbol[]);

#endif
