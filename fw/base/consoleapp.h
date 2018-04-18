#ifndef __CONSOLEAPP_H
#define __CONSOLEAPP_H

#include <windows.h>
#include "runtime.h"

extern int _main(NS_FW_BASE::Map*);

char* getWorkingDir();

int main(int, char**);

#endif