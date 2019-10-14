//
// Project: KTRW
// Author:  Brandon Azad <bazad@google.com>
//
// Copyright 2019 Google LLC
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//

#ifndef GDB_STUB__H_
#define GDB_STUB__H_

#include <mach-o/loader.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

// ---- Platform configuration --------------------------------------------------------------------

#include "gdb_platform.h"

// ---- Functions needed by the GDB stub ----------------------------------------------------------

// Reset state to prepare for a new connection.
void gdb_stub_reset_state(void);

// Read any pending data sent from GDB to the GDB stub.
size_t gdb_stub_serial_read(void *data, size_t size);

// Write data generated by the GDB stub out to GDB.
size_t gdb_stub_serial_write(const void *data, size_t size);

// Called when the GDB stub wants to set or clear a hardware breakpoint.
bool gdb_stub_set_hardware_breakpoint(uint64_t address);
bool gdb_stub_clear_hardware_breakpoint(uint64_t address);

// Called when the GDB stub wants to set or clear a hardware watchpoint. The type parameter
// specifies the type of watchpoint: 'r' for read, 'w' for write, or 'a' for access.
bool gdb_stub_set_hardware_watchpoint(uint64_t address, size_t size, char type);
bool gdb_stub_clear_hardware_watchpoint(uint64_t address, size_t size, char type);

// Called when the GDB stub wants to interrupt the specified CPU. The function should not wait for
// the CPU to halt.
void gdb_stub_interrupt_cpu(int cpu_id);

// Called when the GDB stub wants to resume the specified CPU. The function should not wait for the
// CPU to resume.
void gdb_stub_resume_cpu(int cpu_id);

// Called when the GDB stub wants to single-step the specified CPU. The function should not wait
// for the CPU to resume or to halt.
void gdb_stub_step_cpu(int cpu_id);

// Called when the GDB stub wants to read the PC register on a specific CPU.
uint64_t gdb_stub_cpu_pc(int cpu_id);

// Called when the GDB stub wants to read the registers on a specific CPU.
void gdb_stub_read_registers(int cpu_id, struct gdb_registers *registers);

// Called when the GDB stub wants to write the registers on a specific CPU.
void gdb_stub_write_registers(int cpu_id, const struct gdb_registers *registers);

// Called when the GDB stub wants to read memory from a specific CPU.
size_t gdb_stub_read_memory(int cpu_id, uint64_t address, void *data, size_t length);

// Called when the GDB stub wants to write memory from a specific CPU.
size_t gdb_stub_write_memory(int cpu_id, uint64_t address, const void *data, size_t length);

// Called when the GDB stub wants to allocate memory with the specified permissions.
uint64_t gdb_stub_allocate_jit_memory(size_t size, int perm);

// Called when the GDB stub wants to deallocate memory previously allocated with
// gdb_stub_allocate_jit_memory().
bool gdb_stub_deallocate_jit_memory(uint64_t address);

// ---- The GDB stub entry points -----------------------------------------------------------------

// Call this once at the beginning to initialize the GDB stub state.
void gdb_stub_init(uint32_t cpu_mask, uint32_t halted_mask);

// Call this once to initialize the Mach-O header of the main kernel.
void gdb_stub_set_mach_header(const struct mach_header_64 *mach_header);

// Call this once to set the number of available hardware watchpoints.
void gdb_stub_set_hardware_watchpoint_count(unsigned hardware_watchpoint_count);

// Call this when a CPU halts due to a hardware breakpoint. This function simply records the
// breakpoint information in the GDB stub, it does not begin processing the event.
void gdb_stub_hit_hardware_breakpoint(int cpu_id);

// Call this when a CPU halts due to a hardware watchpoint. This function simply records the
// watchpoint information in the GDB stub, it does not begin processing the event.
void gdb_stub_hit_hardware_watchpoint(int cpu_id, uint64_t address);

// Call this function when a CPU halts because it completed a single-step operation. This function
// simply records the step information in the GDB stub, it does not begin processing the event.
void gdb_stub_did_step(int cpu_id);

// Call this function when a CPU halts because it was interrupted. This function simply records the
// halt information in the GDB stub, it does not begin processing the event.
void gdb_stub_did_halt(int cpu_id);

// Call this when information about all current halts has been recorded with the
// gdb_stub_hit_hardware_breakpoint(), ..., gdb_stub_did_halt() functions and the GDB stub should
// now process that information and act accordingly.
// The halted_mask argument is a mask of the CPUs that are halted. This is a debugging check for
// the GDB stub that it is managing state correctly.
void gdb_stub_process_halts(uint32_t halted_mask);

// Call this when a packet becomes available over serial.
void gdb_stub_packet(void);

// This function can be called to log a critical event over serial. It may interfere with the
// normal operation of the debugger, so only do so for debugging critical issues.
void gdb_stub_log(const char *message, ...);

#endif
