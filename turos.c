#include "acpi.h"
#include "common.h"
#include "fs.h"
#include "hpet.h"
#include "intr.h"
#include "kbc.h"
#include "pci.h"
#include "pic.h"
#include "pixel.h"
#include "print.h"
#include "proc.h"
#include "syscall.h"
#include "x86.h"

struct platform_info {
  struct framebuffer fb;
  void *rsdp;
};

// @_t: SystemTable address
void start_kernel(void *_t __attribute__((unused)),
                  struct platform_info *platform_info,
                  void *_fs_start) {
  fb_init(&platform_info->fb);

  // paint screen
  set_screen(64, 224, 208);

  // display character
  puts("HELLO TUROS\r\n");

  fs_init(_fs_start);
  gdt_init();
  intr_init();
  init_syscall();
  pic_init();
  kbc_intr_init();
  nic_init();

  // Dump RSDP signature
  puts("DUMP RSDP SIGNATURE: ");
  char *rsdp = (char *) platform_info->rsdp;
  putc(*rsdp++);
  putc(*rsdp++);
  putc(*rsdp++);
  putc(*rsdp++);
  putc(*rsdp++);
  putc(*rsdp++);
  putc(*rsdp++);
  putc(*rsdp);
  puts(" ...DONE\r\n");

  // Register XSDT
  puts("REGISTER XSDT...");
  init_acpi(platform_info->rsdp);
  puts("DONE\r\n");

  // Initialize HPET
  puts("INITIALIZE HPET...");
  init_hpet();
  puts("DONE\r\n");

  // Dump XSDT entries
  puts("DUMP XSDT ENTRIES: ");
  dump_xsdt_entries();
  puts("...DONE\r\n");

  // Read HPET registers
  puts("DUMP HPET RGISTERS...\r\n");
  dump_gcidr();
  dump_gcr();
  dump_mcr();
  volatile unsigned long long wait = 10000;
  while (wait--)
    ;
  dump_mcr();

  // Sleep
  puts("SLEEP 2 SEC...");
  sleep(2 * 1000000);
  puts("DONE\r\n");

  // Alert
  //puts("ALERT AFTER 5 SEC...\r\n");
  //alert(5 * 1000000);

  //ptimer_setup(1 * 1000000);
  //puts("START PTIMER...\r\n");
  //ptimer_start();

  unsigned long long softirq_ret;
  asm volatile(
          "movq $1, %%rdi\n"
          "movq $2, %%rsi\n"
          "movq $3, %%rdx\n"
          "movq $4, %%rcx\n"
          "int $0x80\n"
          "movq %%rax, %[softirq_ret]"
          : [softirq_ret] "=r"(softirq_ret)
          :);
  puth(softirq_ret, 16);
  puts("\r\n");

  // Enable interrupt CPU
  puts("ENABLE CPU INTR...");
  enable_cpu_intr();
  puts("DONE\r\n");

  puts("OPENING FS...\r\n");
  struct file *hello = open("HELLO.TXT");
  if (hello) {
    puts((char *) hello->filename);
    puts(": ");
    puts((char *) hello->data);
    puts("\r\n");
  }
  struct file *good = open("GOOD.TXT");
  if (good) {
    puts((char *) good->filename);
    puts(": ");
    puts((char *) good->data);
    puts("\r\n");
  }
  puts("DONE\r\n");

  //puts("EXEC...");
  //exec(open("test"));
  //puts("DONE\r\n");

  dump_nic_ims();
  set_nic_reg(NIC_REG_IMS, 0xcafecafe);
  dump_nic_ims();
  set_nic_reg(NIC_REG_IMC, 0xffffffff);
  dump_nic_ims();

  while (1) {
    if (dump_frame() > 0) {
      puts("\r\n");
    }
  }

  while (1) {
    __asm__ volatile("hlt");
  }
}
