#ifndef _RELATION_LOADER_H
#define _RELATION_LOADER_H
#include "../header/structs.h"
#include <stdint.h>

Relations* init_relations();
Relations* load_relations(char** ,int );

#endif