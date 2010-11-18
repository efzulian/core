// *****************************************************************************
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
// *****************************************************************************
// Title:      tb_ahb_mem.h
//
// ScssId:
//
// Origin:     HW-SW SystemC Co-Simulation SoC Validation Platform
//
// Purpose:    Provide a test bench memory class with AHB slave interface.
//
// Modified on $Date$
//          at $Revision$
//          by $Author$
//
// Principal:  European Space Agency
// Author:     VLSI working group @ IDA @ TUBS
// Maintainer: Soeren Brinkmann
// Reviewed:
// *****************************************************************************

#ifndef TB_AHB_MEM_H
#define TB_AHB_MEM_H

#include <systemc.h>
#include <tlm.h>
#include <map>
#include "amba.h"
#include "grlibdevice.h"

class Ctb_ahb_mem : public sc_module, public amba_slave_base {

    public:
        /// Constructor
        SC_HAS_PROCESS( Ctb_ahb_mem);
        /// @brief Constructor for the test bench memory class
        /// @param haddr AHB address of the AHB slave socket (12 bit)
        /// @param hmask AHB address mask (12 bit)
        /// @param ambaLayer Abstraction layer used (AT/LT)
        /// @param infile File name of a text file to initialize the memory from
        /// @param addr Start address for memory initilization
        Ctb_ahb_mem(const sc_core::sc_module_name nm, const uint16_t haddr_,
                    const uint16_t hmask_ = 0, const amba::amba_layer_ids ambaLayer = amba::amba_LT,
                    const char infile[] = NULL, uint32_t addr = 0);

        /// Destructor
        ~Ctb_ahb_mem();

        // AMBA pnp devices
        CGrlibDevice pnpahb;

        // AMBA master socket
        amba::amba_slave_socket<32, 0> ahb;

        /// @brief Method to read memory contents from a text file
        /// @param infile File name of a text file to initialize the memory from
        /// @param addr Start address for memory initilization
        /// @return 0 on success, 1 on error
        int readmem(const char infile_[], uint32_t addr = 0);

        /// @brief Method dumping the memory contents to a text file
        /// @param outfile File name to dump the memory in
        /// @return 0 on success, 1 on error
        int dumpmem(const char outfile_[]);

        /// TLM blocking transport function
        void b_transport(unsigned int id, tlm::tlm_generic_payload &gp,
                         sc_core::sc_time &delay);

        /// TLM non blocking transport function
        tlm::tlm_sync_enum nb_transport_fw(unsigned int id, tlm::tlm_generic_payload& gp,
                                           tlm::tlm_phase& phase, sc_core::sc_time& delay);

        /// @brief Delete memory content
        void clear_mem() {
            mem.clear();
        }

        /// @brief Method returning the memory's base address at the AHB bus
        /// @return AHB base address of the memory module
        inline sc_dt::uint64 get_base_addr() {
            return ahbBaseAddress;
        }

        /// @brief Method returning the memory's size at the AHB bus
        /// @return Size of the address space occupied by the memory module in bytes
        inline sc_dt::uint64 get_size() {
            // ahbSize holds size in bytes. Add shift if other units are required.
            return ahbSize;
        }

    private:
        /// The actual memory
        std::map<uint32_t, uint8_t> mem;
        /// Method to convert ascii chars into their binary represenation
        uint8_t char2nibble(const char *ch) const;

        /// AHB slave base address and size
        const uint32_t ahbBaseAddress;
        // size is saved in bytes
        const uint32_t ahbSize;
        const uint32_t hmask;
        const uint32_t haddr;
};

#endif
