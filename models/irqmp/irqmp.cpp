//*********************************************************************
// Copyright 2010, Institute of Computer and Network Engineering,
//                 TU-Braunschweig
// All rights reserved
// Any reproduction, use, distribution or disclosure of this program,
// without the express, prior written consent of the authors is 
// strictly prohibited.
//
// University of Technology Braunschweig
// Institute of Computer and Network Engineering
// Hans-Sommer-Str. 66
// 38118 Braunschweig, Germany
//
// ESA SPECIAL LICENSE
//
// This program may be freely used, copied, modified, and redistributed
// by the European Space Agency for the Agency's own requirements.
//
// The program is provided "as is", there is no warranty that
// the program is correct or suitable for any purpose,
// neither implicit nor explicit. The program and the information in it
// contained do not necessarily reflect the policy of the 
// European Space Agency or of TU-Braunschweig.
//*********************************************************************
// Title:      irqmp.tpp
//
// ScssId:
//
// Origin:     HW-SW SystemC Co-Simulation SoC Validation Platform
//
// Purpose:    implementation of irqmp module
//             is included by irqmp.h template header file
//
// Method:
//
// Modified on $Date$
//          at $Revision$
//          by $Author$
//
// Principal:  European Space Agency
// Author:     VLSI working group @ IDA @ TUBS
// Maintainer: Rolf Meyer
// Reviewed:
//*********************************************************************

/// @addtogroup irqmp
/// @{

#include "irqmp.h"
#include "verbose.h"
#include <string>

/// Constructor
CIrqmp::CIrqmp(sc_core::sc_module_name name, int _paddr, int _pmask, int _ncpu, int _eirq, unsigned int pindex) :
            gr_device(name, //sc_module name
                    gs::reg::ALIGNED_ADDRESS, //address mode (options: aligned / indexed)
                    0xFF, //dword size (of register file)
                    NULL //parent module
            ),
            APBDevice(pindex, 0x01, 0x00D, 3, 0, APBDevice::APBIO, _pmask, false, false, _paddr),
            apb_slv(
                    "APB_SLAVE", //name
                    r, //register container
                    (_paddr & _pmask) << 8, // start address
                    (((~_pmask & 0xfff) + 1) << 8), // register space length
                    ::amba::amba_APB, // bus type
                    ::amba::amba_LT, // communication type / abstraction level
                    false // not used
            ), 
            rst(&CIrqmp::onreset, "RESET"), 
            cpu_rst("CPU_RESET"), irq_req("CPU_REQUEST"), 
            irq_ack(&CIrqmp::acknowledged_irq,"IRQ_ACKNOWLEDGE"), 
            irq_in(&CIrqmp::incomming_irq, "IRQ_INPUT"), 
            ncpu(_ncpu), eirq(_eirq), cc(20, SC_NS) {

            forcereg = new uint32_t[ncpu];
   // Display APB slave information
   v::info << name << "APB slave @0x" << hex << v::setw(8)
           << v::setfill('0') << apb_slv.get_base_addr() << " size: 0x" << hex
           << v::setw(8) << v::setfill('0') << apb_slv.get_size() << " byte"
           << endl;

    // create register | name + description
    r.create_register("level", "Interrupt Level Register",
            // offset
            0x00,
            
            // config
            gs::reg::STANDARD_REG | gs::reg::SINGLE_IO | gs::reg::SINGLE_BUFFER
                    | gs::reg::FULL_WIDTH,
            
            // init value
            0x00000000,
            
            // write mask
            CIrqmp::IR_LEVEL_IL,
            
            // reg width (maximum 32 bit)
            32,
            
            // lock mask: Not implementet, has to be zero.
            0x00);
    
    r.create_register("pending", "Interrupt Pending Register", 0x04,
            gs::reg::STANDARD_REG | gs::reg::SINGLE_IO | 
            gs::reg::SINGLE_BUFFER | gs::reg::FULL_WIDTH, 0x00000000, 
            IR_PENDING_EIP | IR_PENDING_IP, 32, 0x00);
    //Following register is part of the manual, but will never be used.
    // 1) A system with 0 cpus will never be implemented
    // 2) If there were 0 cpus, no cpu would need an IR force register
    // 3) The IR force register for the first CPU ('CPU 0') will always be located at address 0x80
    if (ncpu == 0) {
        r.create_register("force", "Interrupt Force Register", 0x08,
                gs::reg::STANDARD_REG | gs::reg::SINGLE_IO
                        | gs::reg::SINGLE_BUFFER | gs::reg::FULL_WIDTH,
                0x00000000, IR_FORCE_IF, 32, 0x00);
    }
    r.create_register("clear", "Interrupt Clear Register", 0x0C,
            gs::reg::STANDARD_REG | gs::reg::SINGLE_IO | gs::reg::SINGLE_BUFFER
                    | gs::reg::FULL_WIDTH, 0x00000000, IR_CLEAR_IC, 32,
            0x00);
    r.create_register("mpstat", "Multiprocessor Status Register", 0x10,
            gs::reg::STANDARD_REG | gs::reg::SINGLE_IO | gs::reg::SINGLE_BUFFER
                    | gs::reg::FULL_WIDTH, 0x00000000, MP_STAT_NCPU
                    | MP_STAT_EIRQ | MP_STAT_STAT(), 32, 0x00);
    r.create_register("broadcast", "Interrupt broadcast Register", 0x14,
            gs::reg::STANDARD_REG | gs::reg::SINGLE_IO | gs::reg::SINGLE_BUFFER
                    | gs::reg::FULL_WIDTH, 0x00000000, BROADCAST_BM, 32,
            0x00);

    for (int i = 0; i < ncpu; ++i) {
        r.create_register(gen_unique_name("mask", false),
                "Interrupt Mask Register", 0x40 + 0x4 * i,
                gs::reg::STANDARD_REG | gs::reg::SINGLE_IO
                        | gs::reg::SINGLE_BUFFER | gs::reg::FULL_WIDTH,
                0xFFFFFFFE, PROC_MASK_EIM | CIrqmp::PROC_MASK_IM, 32, 0x00);
        r.create_register(gen_unique_name("force", false),
                "Interrupt Force Register", 0x80 + 0x4 * i,
                gs::reg::STANDARD_REG | gs::reg::SINGLE_IO
                        | gs::reg::SINGLE_BUFFER | gs::reg::FULL_WIDTH,
                0x00000000, PROC_IR_FORCE_IFC | PROC_IR_FORCE_IF, 32,
                0x00);
        r.create_register(gen_unique_name("eir_id", false),
                "Extended Interrupt Identification Register", 0xC0 + 0x4 * i,
                gs::reg::STANDARD_REG | gs::reg::SINGLE_IO
                        | gs::reg::SINGLE_BUFFER | gs::reg::FULL_WIDTH,
                0x00000000, PROC_EXTIR_ID_EID, 32, 0x00);
    }

    SC_THREAD(launch_irq); 
}

CIrqmp::~CIrqmp() {
    GC_UNREGISTER_CALLBACKS();
    delete[] forcereg;
}

void CIrqmp::end_of_elaboration() {
    // send interrupts to processors after write to pending / force regs
    GR_FUNCTION(CIrqmp, pending_write); // args: module name, callback function name
    GR_SENSITIVE(r[IR_PENDING].add_rule(gs::reg::POST_WRITE,"pending_write", gs::reg::NOTIFY));

    // unset pending bits of cleared interrupts
    GR_FUNCTION(CIrqmp, clear_write);
    GR_SENSITIVE(r[IR_CLEAR].add_rule(gs::reg::POST_WRITE, "clear_write", gs::reg::NOTIFY));

    // unset force bits of cleared interrupts
    for (int i_cpu = 0; i_cpu < ncpu; i_cpu++) {
        GR_FUNCTION(CIrqmp, force_write);
        GR_SENSITIVE(r[PROC_IR_FORCE(i_cpu)].add_rule(
                gs::reg::POST_WRITE, gen_unique_name("force_write", false), gs::reg::NOTIFY));
    }

    // manage cpu run / reset signals after write into MP status reg
    GR_FUNCTION(CIrqmp, mpstat_write);
    GR_SENSITIVE(r[MP_STAT].add_rule(gs::reg::POST_WRITE, "mpstat_write", gs::reg::NOTIFY));
}

/// Reset registers to default values
/// Process sensitive to reset signal
void CIrqmp::onreset(const bool &value, const sc_core::sc_time &time) {
    if(!value) {
        //mp status register contains ncpu and eirq at bits 31..28 and 19..16 respectively
        uint32_t stat_ncpu = ncpu << 28;
        uint32_t stat_eirq = eirq << 16;

        //initialize registers with values defined above
        r[IR_LEVEL]   = static_cast<uint32_t>(LEVEL_DEFAULT);
        r[IR_PENDING] = static_cast<uint32_t>(PENDING_DEFAULT);
        if(ncpu == 0) {
            r[IR_FORCE] = static_cast<uint32_t>(FORCE_DEFAULT);
        }
        r[IR_CLEAR] = static_cast<uint32_t>(CLEAR_DEFAULT);
        r[MP_STAT] = MP_STAT_DEFAULT | stat_ncpu | stat_eirq;
        r[BROADCAST] = static_cast<uint32_t>(BROADCAST_DEFAULT);
        for(int cpu = 0; cpu < ncpu; cpu++) {
            r[PROC_IR_MASK(cpu)]  = static_cast<uint32_t>(MASK_DEFAULT);
            r[PROC_IR_FORCE(cpu)] = static_cast<uint32_t>(PROC_FORCE_DEFAULT);
            r[PROC_EXTIR_ID(cpu)] = static_cast<uint32_t>(EXTIR_ID_DEFAULT);
            forcereg[cpu] = 0;
        }
    }
}

///  - watch interrupt bus signals (apbi.pirq)
///  - write incoming interrupts into pending or force registers
///
/// process sensitive to apbi.pirq
void CIrqmp::incomming_irq(const bool &value, const uint32_t &irq, const sc_time &time) {
    // A variable with true as workaround for greenreg.
    bool t = true;
    if(!value) {
        // Return if the value turned to false.
        // Interrupts will not be unset this way.
        // So we cann simply ignore a false value.
        return;
    }
    
    // If the incomming interrupt is not listed in the broadcast register 
    // it goes in the pending register
    if(!r[BROADCAST].bit_get(irq)) {
        r[IR_PENDING].bit_set(irq, t);
    }
    
    // If it is not listed n the broadcast register and not an extended interrupt it goes into the force registers.
    // EIRs cannot be forced
    if(r[BROADCAST].bit_get(irq) && (irq < 16)) {
        //set force registers for broadcasted interrupts
        for(int32_t cpu = 0; cpu < ncpu; cpu++) { 
            r[PROC_IR_FORCE(cpu)].bit_set(irq, t);
            forcereg[cpu] |= (t << irq);
        }
    }
    // Pending and force regs are set now. 
    // To call an explicit launch_irq signal is set here
    signal.notify(2 * cc);
}

/// launch irq:
///  - combine pending, force, and mask register
///  - prioritize pending interrupts
///  - send highest priority IR to processor via irqo port
///
/// callback registered on IR pending register,
///                        IR force registers
void CIrqmp::launch_irq() {
    int16_t high;
    uint32_t masked, pending, all;
    bool eirq_en;
    while(1) {
        wait(signal);
        for(int cpu = 0; cpu < ncpu; cpu++) {
            // Pending register for this CPU line.
            pending = r[IR_PENDING] & r[PROC_IR_MASK(cpu)];

            // All relevant interrupts for this CPU line to determ pending extended interrupts
            masked  = pending | (r[PROC_IR_FORCE(cpu)] & IR_FORCE_IF);
            // if any pending extended interrupts
            if(eirq != 0) {
                // Set the pending pit in the pending register.
                eirq_en = masked & IR_PENDING_EIP;
                r[IR_PENDING].bit_set(eirq, eirq_en);
            } else {
                eirq_en = 0;
            }
            // Recalculate relevant interrupts
            all = pending | (eirq_en << eirq) | (r[PROC_IR_FORCE(cpu)] & IR_FORCE_IF);
            
            // Find the highes not extended interrupt on level 1 
            masked = (all & r[IR_LEVEL]) & IR_PENDING_IP;
            for(high = 15; high > 0; high--) {
                if(masked & (1 << high)) {
                    break;
                }
            }

            // If no IR on level 1 found check level 0.
            if(high == 0) {
                // Find the highes not extended interrupt on level 0 
                masked = (all & ~r[IR_LEVEL]) & IR_PENDING_IP;
                for(high = 15; high > 0; high--) {
                    if(masked & (1 << high)) {
                        break;
                    }
                }
            }
            // If an interrupt is selected send it out to the CPU.
            if(high!=0) {
                irq_req.write(1 << cpu, std::pair<uint32_t, bool>(0xF & high, true));
            }
        }
    }
}

///
/// clear acknowledged IRQs                                           (three processes)
///  - interrupts can be cleared
///    - by software writing to the IR_Clear register                 (process 1: clear_write)
///    - by software writing to the upper half of IR_Force registers  (process 2: Clear_forced_ir)
///    - by processor sending irqi.intack signal and irqi.irl data    (process 3: clear_acknowledged_irq)
///  - remove IRQ from pending / force registers
///  - in case of eirq, release eirq ID in eirq ID register
///
/// callback registered on interrupt clear register
void CIrqmp::clear_write() {
    // pending reg only: forced IRs are cleared in the next function
    bool extirq = false;
    for(int cpu = 0; cpu < ncpu; cpu++) {
        if(eirq != 0) {
            r[IR_PENDING] = r[IR_PENDING] & ~(1 << r[PROC_EXTIR_ID(cpu)]);
            r[PROC_EXTIR_ID(cpu)] = 0;
            extirq = true;
        }
    }
    if(extirq) {
        irq_req.write(~0, std::pair<uint32_t, bool>(eirq, false));
    }
    for(int i = 15; i > 0; --i) {
        if((1<<i)&r[IR_CLEAR]) {
            irq_req.write(~0, std::pair<uint32_t, bool>(i, false));
        }
    }
    
    uint32_t cleared_vector = r[IR_PENDING] & ~r[IR_CLEAR];
    r[IR_PENDING] = cleared_vector;
    r[IR_CLEAR]   = 0;
    signal.notify(2 * cc);
}

/// callback registered on interrupt force registers
void CIrqmp::force_write() {
    for(int cpu = 0; cpu < ncpu; cpu++) {
        forcereg[cpu] |= r[PROC_IR_FORCE(cpu)];
        for(int i = 31; i > 0; --i) {
            if((1<<i)&forcereg[cpu]) {
                irq_req.write(~0, std::pair<uint32_t, bool>(i, false));
            }
        }
        //write mask clears IFC bits:
        //IF && !IFC && write_mask
        forcereg[cpu] &= (~(forcereg[cpu] >> 16) & PROC_IR_FORCE_IF);
        r[PROC_IR_FORCE(cpu)] = forcereg[cpu];
    }
    signal.notify(2 * cc);
}

/// process sensitive to ack_irq
void CIrqmp::acknowledged_irq(const uint32_t &irq, const uint32_t &cpu, const sc_core::sc_time &time) {
    bool f = false;
    //extended IR handling: Identify highest pending EIR
    if(eirq != 0 && irq == (uint32_t)eirq && !(eirq&r[BROADCAST])) {
        r[IR_PENDING] = r[IR_PENDING] & ~(1 << r[PROC_EXTIR_ID(cpu)]);
        r[PROC_IR_FORCE(cpu)] = r[PROC_IR_FORCE(cpu)] & ~(1 << r[PROC_EXTIR_ID(cpu)]);
        forcereg[cpu] &= ~(1 << r[PROC_EXTIR_ID(cpu)]) & 0xFFFE;
        irq_req.write(~0, std::pair<uint32_t, bool>(eirq, false));
    } else {
        //clear interrupt from pending and force register
        irq_req.write(~0, std::pair<uint32_t, bool>(irq, false));
        r[IR_PENDING].bit_set(irq, f);
        if(r[BROADCAST].bit_get(irq)) {
            r[PROC_IR_FORCE(cpu)].bit_set(irq, f);
            forcereg[cpu] &= ~(1 << irq) & 0xFFFE;
        }
    }
    r[PROC_EXTIR_ID(cpu)] = 0;
    signal.notify(2 * cc);
}

/// reset cpus after write to cpu status register
/// callback registered on mp status register
void CIrqmp::mpstat_write() {
    cpu_rst.write(0xFFFFFFFF, true);
}

void CIrqmp::pending_write() {
    signal.notify(2 * cc);
}

// Extract basic cycle rate from a sc_clock
void CIrqmp::clk(sc_core::sc_clock &clk) {
    cc = clk.period();
}

// Extract basic cycle rate from a clock period
void CIrqmp::clk(sc_core::sc_time &period) {
    cc = period;
}

// Extract basic cycle rate from a clock period in double
void CIrqmp::clk(double period, sc_core::sc_time_unit base) {
    cc = sc_time(period, base);
}


/// @}
