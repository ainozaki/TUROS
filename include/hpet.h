#ifndef HPET_H_
#define HPET_H_

#include "acpi.h"

#define HPET_INTR_NO 32

struct __attribute__((packed)) HPET_TABLE {
  struct SDTH header;
  unsigned int event_timer_block_id;
  struct ACPI_ADDRESS base_address;
  unsigned char hpet_number;
  unsigned short minimum_tick;
  unsigned char flags;
};

void init_hpet();

void dump_gcidr();
void dump_gcr();
void dump_mcr();

// sleep |us|
void sleep(unsigned long long us);

// do |handler| after |us|
void alert(unsigned long long us);

void ptimer_setup(unsigned long long us);
void ptimer_start();
void ptimer_stop();

#endif// HPET_H_
