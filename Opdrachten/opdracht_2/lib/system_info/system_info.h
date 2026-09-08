/*
 * system_info.h
 *
 * Revision: V2.0
 * Modified: 07-09-2026
 */ 

#ifndef SYSTEM_INFO_H
#define SYSTEM_INFO_H

void RegisterSystemInfoCommands();
void PrintMemoryInfo();
void PrintTaskStats();
void PrintTasksInfo();
void PrintCPUInfo(void);
void PrintVersion(void);

#endif
