/*
htop - linux/Platform.c
(C) 2014 Hisham H. Muhammad
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "Platform.h"
#include "IOPriority.h"
#include "IOPriorityPanel.h"
#include "LinuxProcess.h"
#include "LinuxProcessList.h"
#include "Battery.h"

#include "Meter.h"
#include "CPUMeter.h"
#include "MemoryMeter.h"
#include "SwapMeter.h"
#include "TasksMeter.h"
#include "LoadAverageMeter.h"
#include "UptimeMeter.h"
#include "PressureStallMeter.h"
#include "ClockMeter.h"
#include "HostnameMeter.h"
#include "zfs/ZfsArcMeter.h"
#include "zfs/ZfsCompressedArcMeter.h"
#include "LinuxProcess.h"

#include <math.h>
#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>


ProcessField Platform_defaultFields[] = { PID, USER, PRIORITY, NICE, M_SIZE, M_RESIDENT, (int)M_SHARE, STATE, PERCENT_CPU, PERCENT_MEM, TIME, COMM, 0 };

//static ProcessField defaultIoFields[] = { PID, IO_PRIORITY, USER, IO_READ_RATE, IO_WRITE_RATE, IO_RATE, COMM, 0 };

int Platform_numberOfFields = LAST_PROCESSFIELD;

const SignalItem Platform_signals[] = {
   { .name = " 0 Cancel",    .number = 0 },
   { .name = " 1 SIGHUP",    .number = 1 },
   { .name = " 2 SIGINT",    .number = 2 },
   { .name = " 3 SIGQUIT",   .number = 3 },
   { .name = " 4 SIGILL",    .number = 4 },
   { .name = " 5 SIGTRAP",   .number = 5 },
   { .name = " 6 SIGABRT",   .number = 6 },
   { .name = " 6 SIGIOT",    .number = 6 },
   { .name = " 7 SIGBUS",    .number = 7 },
   { .name = " 8 SIGFPE",    .number = 8 },
   { .name = " 9 SIGKILL",   .number = 9 },
   { .name = "10 SIGUSR1",   .number = 10 },
   { .name = "11 SIGSEGV",   .number = 11 },
   { .name = "12 SIGUSR2",   .number = 12 },
   { .name = "13 SIGPIPE",   .number = 13 },
   { .name = "14 SIGALRM",   .number = 14 },
   { .name = "15 SIGTERM",   .number = 15 },
   { .name = "16 SIGSTKFLT", .number = 16 },
   { .name = "17 SIGCHLD",   .number = 17 },
   { .name = "18 SIGCONT",   .number = 18 },
   { .name = "19 SIGSTOP",   .number = 19 },
   { .name = "20 SIGTSTP",   .number = 20 },
   { .name = "21 SIGTTIN",   .number = 21 },
   { .name = "22 SIGTTOU",   .number = 22 },
   { .name = "23 SIGURG",    .number = 23 },
   { .name = "24 SIGXCPU",   .number = 24 },
   { .name = "25 SIGXFSZ",   .number = 25 },
   { .name = "26 SIGVTALRM", .number = 26 },
   { .name = "27 SIGPROF",   .number = 27 },
   { .name = "28 SIGWINCH",  .number = 28 },
   { .name = "29 SIGIO",     .number = 29 },
   { .name = "29 SIGPOLL",   .number = 29 },
   { .name = "30 SIGPWR",    .number = 30 },
   { .name = "31 SIGSYS",    .number = 31 },
};

const SignalItem Platform_limit[] = {
   { .name = " 0",   .number = 0 },
{ .name = " 1",   .number = 1 },
{ .name = " 2",   .number = 2 },
{ .name = " 3",   .number = 3 },
{ .name = " 4",   .number = 4 },
{ .name = " 5",   .number = 5 },
{ .name = " 6",   .number = 6 },
{ .name = " 7",   .number = 7 },
{ .name = " 8",   .number = 8 },
{ .name = " 9",   .number = 9 },
{ .name = " 10",  .number = 10 },
{ .name = " 11",  .number = 11 },
{ .name = " 12",  .number = 12 },
{ .name = " 13",  .number = 13 },
{ .name = " 14",  .number = 14 },
{ .name = " 15",  .number = 15 },
{ .name = " 16",  .number = 16 },
{ .name = " 17",  .number = 17 },
{ .name = " 18",  .number = 18 },
{ .name = " 19",  .number = 19 },
{ .name = " 20",  .number = 20 },
{ .name = " 21",  .number = 21 },
{ .name = " 22",  .number = 22 },
{ .name = " 23",  .number = 23 },
{ .name = " 24",  .number = 24 },
{ .name = " 25",  .number = 25 },
{ .name = " 26",  .number = 26 },
{ .name = " 27",  .number = 27 },
{ .name = " 28",  .number = 28 },
{ .name = " 29",  .number = 29 },
{ .name = " 30",  .number = 30 },
{ .name = " 31",  .number = 31 },
{ .name = " 32",  .number = 32 },
{ .name = " 33",  .number = 33 },
{ .name = " 34",  .number = 34 },
{ .name = " 35",  .number = 35 },
{ .name = " 36",  .number = 36 },
{ .name = " 37",  .number = 37 },
{ .name = " 38",  .number = 38 },
{ .name = " 39",  .number = 39 },
{ .name = " 40",  .number = 40 },
{ .name = " 41",  .number = 41 },
{ .name = " 42",  .number = 42 },
{ .name = " 43",  .number = 43 },
{ .name = " 44",  .number = 44 },
{ .name = " 45",  .number = 45 },
{ .name = " 46",  .number = 46 },
{ .name = " 47",  .number = 47 },
{ .name = " 48",  .number = 48 },
{ .name = " 49",  .number = 49 },
{ .name = " 50",  .number = 50 },
{ .name = " 51",  .number = 51 },
{ .name = " 52",  .number = 52 },
{ .name = " 53",  .number = 53 },
{ .name = " 54",  .number = 54 },
{ .name = " 55",  .number = 55 },
{ .name = " 56",  .number = 56 },
{ .name = " 57",  .number = 57 },
{ .name = " 58",  .number = 58 },
{ .name = " 59",  .number = 59 },
{ .name = " 60",  .number = 60 },
{ .name = " 61",  .number = 61 },
{ .name = " 62",  .number = 62 },
{ .name = " 63",  .number = 63 },
{ .name = " 64",  .number = 64 },
{ .name = " 65",  .number = 65 },
{ .name = " 66",  .number = 66 },
{ .name = " 67",  .number = 67 },
{ .name = " 68",  .number = 68 },
{ .name = " 69",  .number = 69 },
{ .name = " 70",  .number = 70 },
{ .name = " 71",  .number = 71 },
{ .name = " 72",  .number = 72 },
{ .name = " 73",  .number = 73 },
{ .name = " 74",  .number = 74 },
{ .name = " 75",  .number = 75 },
{ .name = " 76",  .number = 76 },
{ .name = " 77",  .number = 77 },
{ .name = " 78",  .number = 78 },
{ .name = " 79",  .number = 79 },
{ .name = " 80",  .number = 80 },
{ .name = " 81",  .number = 81 },
{ .name = " 82",  .number = 82 },
{ .name = " 83",  .number = 83 },
{ .name = " 84",  .number = 84 },
{ .name = " 85",  .number = 85 },
{ .name = " 86",  .number = 86 },
{ .name = " 87",  .number = 87 },
{ .name = " 88",  .number = 88 },
{ .name = " 89",  .number = 89 },
{ .name = " 90",  .number = 90 },
{ .name = " 91",  .number = 91 },
{ .name = " 92",  .number = 92 },
{ .name = " 93",  .number = 93 },
{ .name = " 94",  .number = 94 },
{ .name = " 95",  .number = 95 },
{ .name = " 96",  .number = 96 },
{ .name = " 97",  .number = 97 },
{ .name = " 98",  .number = 98 },
{ .name = " 99",  .number = 99 },
{ .name = " 100", .number = 100 }
};

const unsigned int Platform_numberOfSignals = sizeof(Platform_signals)/sizeof(SignalItem);

static Htop_Reaction Platform_actionSetIOPriority(State* st) {
   Panel* panel = st->panel;

   LinuxProcess* p = (LinuxProcess*) Panel_getSelected(panel);
   if (!p) return HTOP_OK;
   IOPriority ioprio1 = p->ioPriority;
   Panel* ioprioPanel = IOPriorityPanel_new(ioprio1);
   void* set = Action_pickFromVector(st, ioprioPanel, 21, true);
   if (set) {
      IOPriority ioprio2 = IOPriorityPanel_getIOPriority(ioprioPanel);
      bool ok = MainPanel_foreachProcess((MainPanel*)panel, (MainPanel_ForeachProcessFn) LinuxProcess_setIOPriority, (Arg){ .i = ioprio2 }, NULL);
      if (!ok)
         beep();
   }
   Panel_delete((Object*)ioprioPanel);
   return HTOP_REFRESH | HTOP_REDRAW_BAR | HTOP_UPDATE_PANELHDR;
}

void Platform_setBindings(Htop_Action* keys) {
   keys['i'] = Platform_actionSetIOPriority;
}

MeterClass* Platform_meterTypes[] = {
   &CPUMeter_class,
   &ClockMeter_class,
   &LoadAverageMeter_class,
   &LoadMeter_class,
   &MemoryMeter_class,
   &SwapMeter_class,
   &TasksMeter_class,
   &UptimeMeter_class,
   &BatteryMeter_class,
   &HostnameMeter_class,
   &AllCPUsMeter_class,
   &AllCPUs2Meter_class,
   &AllCPUs4Meter_class,
   &LeftCPUsMeter_class,
   &RightCPUsMeter_class,
   &LeftCPUs2Meter_class,
   &RightCPUs2Meter_class,
   &LeftCPUs4Meter_class,
   &RightCPUs4Meter_class,
   &BlankMeter_class,
   &PressureStallCPUSomeMeter_class,
   &PressureStallIOSomeMeter_class,
   &PressureStallIOFullMeter_class,
   &PressureStallMemorySomeMeter_class,
   &PressureStallMemoryFullMeter_class,
   &ZfsArcMeter_class,
   &ZfsCompressedArcMeter_class,
   NULL
};

int Platform_getUptime() {
   double uptime = 0;
   FILE* fd = fopen(PROCDIR "/uptime", "r");
   if (fd) {
      int n = fscanf(fd, "%64lf", &uptime);
      fclose(fd);
      if (n <= 0) return 0;
   }
   return (int) floor(uptime);
}

void Platform_getLoadAverage(double* one, double* five, double* fifteen) {
   int activeProcs, totalProcs, lastProc;
   *one = 0; *five = 0; *fifteen = 0;
   FILE *fd = fopen(PROCDIR "/loadavg", "r");
   if (fd) {
      int total = fscanf(fd, "%32lf %32lf %32lf %32d/%32d %32d", one, five, fifteen,
         &activeProcs, &totalProcs, &lastProc);
      (void) total;
      assert(total == 6);
      fclose(fd);
   }
}

int Platform_getMaxPid() {
   FILE* file = fopen(PROCDIR "/sys/kernel/pid_max", "r");
   if (!file) return -1;
   int maxPid = 4194303;
   int match = fscanf(file, "%32d", &maxPid);
   (void) match;
   fclose(file);
   return maxPid;
}

double Platform_setCPUValues(Meter* this, int cpu) {
   LinuxProcessList* pl = (LinuxProcessList*) this->pl;
   CPUData* cpuData = &(pl->cpus[cpu]);
   double total = (double) ( cpuData->totalPeriod == 0 ? 1 : cpuData->totalPeriod);
   double percent;
   double* v = this->values;
   v[CPU_METER_NICE] = cpuData->nicePeriod / total * 100.0;
   v[CPU_METER_NORMAL] = cpuData->userPeriod / total * 100.0;
   if (this->pl->settings->detailedCPUTime) {
      v[CPU_METER_KERNEL]  = cpuData->systemPeriod / total * 100.0;
      v[CPU_METER_IRQ]     = cpuData->irqPeriod / total * 100.0;
      v[CPU_METER_SOFTIRQ] = cpuData->softIrqPeriod / total * 100.0;
      v[CPU_METER_STEAL]   = cpuData->stealPeriod / total * 100.0;
      v[CPU_METER_GUEST]   = cpuData->guestPeriod / total * 100.0;
      v[CPU_METER_IOWAIT]  = cpuData->ioWaitPeriod / total * 100.0;
      Meter_setItems(this, 8);
      if (this->pl->settings->accountGuestInCPUMeter) {
         percent = v[0]+v[1]+v[2]+v[3]+v[4]+v[5]+v[6];
      } else {
         percent = v[0]+v[1]+v[2]+v[3]+v[4];
      }
   } else {
      v[2] = cpuData->systemAllPeriod / total * 100.0;
      v[3] = (cpuData->stealPeriod + cpuData->guestPeriod) / total * 100.0;
      Meter_setItems(this, 4);
      percent = v[0]+v[1]+v[2]+v[3];
   }
   percent = CLAMP(percent, 0.0, 100.0);
   if (isnan(percent)) percent = 0.0;

   v[CPU_METER_FREQUENCY] = cpuData->frequency;

   return percent;
}

void Platform_setMemoryValues(Meter* this) {
   ProcessList* pl = (ProcessList*) this->pl;
   long int usedMem = pl->usedMem;
   long int buffersMem = pl->buffersMem;
   long int cachedMem = pl->cachedMem;
   usedMem -= buffersMem + cachedMem;
   this->total = pl->totalMem;
   this->values[0] = usedMem;
   this->values[1] = buffersMem;
   this->values[2] = cachedMem;
}

void Platform_setSwapValues(Meter* this) {
   ProcessList* pl = (ProcessList*) this->pl;
   this->total = pl->totalSwap;
   this->values[0] = pl->usedSwap;
}

void Platform_setZfsArcValues(Meter* this) {
   LinuxProcessList* lpl = (LinuxProcessList*) this->pl;

   ZfsArcMeter_readStats(this, &(lpl->zfs));
}

void Platform_setZfsCompressedArcValues(Meter* this) {
   LinuxProcessList* lpl = (LinuxProcessList*) this->pl;

   ZfsCompressedArcMeter_readStats(this, &(lpl->zfs));
}
char* Platform_getProcessEnv(pid_t pid) {
   char procname[32+1];
   xSnprintf(procname, 32, "/proc/%d/environ", pid);
   FILE* fd = fopen(procname, "r");
   char *env = NULL;
   if (fd) {
      size_t capacity = 4096, size = 0, bytes;
      env = xMalloc(capacity);
      while ((bytes = fread(env+size, 1, capacity-size, fd)) > 0) {
         size += bytes;
         capacity *= 2;
         env = xRealloc(env, capacity);
      }
      fclose(fd);
      if (size < 2 || env[size-1] || env[size-2]) {
         if (size + 2 < capacity) {
            env = xRealloc(env, capacity+2);
         }
         env[size] = 0;
         env[size+1] = 0;
      }
   }
   return env;
}

void Platform_getPressureStall(const char *file, bool some, double* ten, double* sixty, double* threehundred) {
   *ten = *sixty = *threehundred = 0;
   char procname[128+1];
   xSnprintf(procname, 128, PROCDIR "/pressure/%s", file);
   FILE *fd = fopen(procname, "r");
   if (!fd) {
      *ten = *sixty = *threehundred = NAN;
      return;
   }
   int total = fscanf(fd, "some avg10=%32lf avg60=%32lf avg300=%32lf total=%*f ", ten, sixty, threehundred);
   if (!some) {
      total = fscanf(fd, "full avg10=%32lf avg60=%32lf avg300=%32lf total=%*f ", ten, sixty, threehundred);
   }
   (void) total;
   assert(total == 3);
   fclose(fd);
}
