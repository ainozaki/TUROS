#include "x86.h"

const unsigned long long gdt[] = {
        0x0000000000000000,// NULL descriptor
        0x00af9a000000ffff,// Code Segment(r-x), base=0, limit=4GB
        0x00cf93000000ffff // Data Segment(rw-), base=0, limit=4GB
};

unsigned long long gdtr[2];

unsigned char io_read(unsigned short address) {
  unsigned char value;
  __asm__ volatile("inb %[address], %[value]"
                   : [value] "=a"(value)
                   : [address] "d"(address));
  return value;
}

unsigned int io_read32(unsigned short address) {
  unsigned int value;
  __asm__ volatile("inl %[address], %[value]"
                   : [value] "=a"(value)
                   : [address] "d"(address));
  return value;
}

void io_write(unsigned short address, unsigned char value) {
  __asm__ volatile("outb %[value], %[address]" ::[value] "a"(value),
                   [address] "d"(address));
}

void io_write32(unsigned short address, unsigned int value) {
  __asm__ volatile("outl %[value], %[address]" ::[value] "a"(value),
                   [address] "d"(address));
}

void gdt_init() {
  // Set GDTR
  gdtr[0] = ((unsigned long long) gdt << 16) | (sizeof(gdt) - 1);
  gdtr[1] = (unsigned long long) gdt >> 48;
  __asm__ volatile("lgdt gdtr");

  // Set DS(Data Segment) and SS(Stack Segment)
  __asm__ volatile(
          "movw $2*8, %ax\n"
          "movw %ax, %ds\n"
          "movw %ax, %ss\n");

  // Set CS(Code Stack)
  // Switch current CS
  unsigned short selector = SS_KERNEL_CODE;
  unsigned long long dummy;
  __asm__ volatile(
          "pushq %[selector];"
          "leaq ret_label(%%rip), %[dummy];"
          "pushq %[dummy];"
          "lretq;"
          "ret_label:"
          : [dummy] "=r"(dummy)
          : [selector] "m"(selector));
}

void enable_cpu_intr() {
  __asm__ volatile("sti");
}
