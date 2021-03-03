#pragma once
#include<iostream>

class flow_cpuinfo{
public:
    inline flow_cpuinfo():logic_core(0),hard_core(0){
        char vendor[12];
        unsigned regs[4];
        cpuID(0, regs);
        ((unsigned *)vendor)[0] = regs[1]; // EBX
        ((unsigned *)vendor)[1] = regs[3]; // EDX
        ((unsigned *)vendor)[2] = regs[2]; // ECX
        cpu_Vendor = std::string(vendor, 12);

        cpuID(1, regs);
        logic_core = (regs[1] >> 16) & 0xff; // EBX[23:16]

        if (cpu_Vendor == "GenuineIntel") {
          // Get DCP cache info
          cpuID(4, regs);
          hard_core = ((regs[0] >> 26) & 0x3f) + 1; // EAX[31:26] + 1
        } else if (cpu_Vendor == "AuthenticAMD") {
          // Get NC: Number of CPU cores - 1
          cpuID(0x80000008, regs);
          hard_core = ((unsigned)(regs[2] & 0xff)) + 1; // ECX[7:0] + 1
        }
    }
    int logic_core;
    int hard_core;
    std::string cpu_Vendor;
private:
    inline void cpuID(unsigned i, unsigned regs[4]) {
        #ifdef _WIN32
            __cpuid((int *)regs, (int)i);

        #else
        asm volatile
            ("cpuid" : "=a" (regs[0]), "=b" (regs[1]), "=c" (regs[2]), "=d" (regs[3])
            : "a" (i), "c" (0));
        // ECX is set to zero for CPUID function 4
        #endif
    }
};