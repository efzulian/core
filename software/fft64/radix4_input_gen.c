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
// Title:      mmu_cache.h
//
// ScssId:
//
// Origin:     HW-SW SystemC Co-Simulation SoC Validation Platform
//
// Purpose:    64 Point Radix-4 FFT Implementation
//             Simple test for Leon ISS / regular computation
//             / word load-store
//             (Version without File I/O - All input/output diverted
//             to headers)
//
// Modified on $Date: 2011-10-14 17:05:38 +0200 (Fri, 14 Oct 2011) $
//          at $Revision: 531 $
//          by $Author: HWSWSIM $
//
// Principal:  European Space Agency
// Author:     VLSI working group @ IDA @ TUBS
// Maintainer: Thomas Schuster
// Reviewed:
//*********************************************************************

// datatyp definitions 
#include "defines.h"

#include <asm-leon/irq.h>

// input stimuli
#include "./input/data.h"
#include "results.h"
#include "stdio.h"

#ifdef LINUX_TEST
// linux input output files
#define OUTFILENAME "./outputs/spectrum_linux.dat"
#else
// for platform simulation
#define OUTFILENAME "com3"
#endif // LINUX_TEST

// allow tolerance in precision
#define tol 10 

#include "../grlib_tests/irqmp.h"

struct irqmp *irqmp_base;
static volatile int irqtbl[18];
volatile int inputdev_irq = 0;
volatile int socwire_irq = 0;

void irqhandler_f(int irq) {

  //irqtbl[irqtbl[0]] = irq + 0x10;
  //irqtbl[0]++;

  if (irq == 13) {

    inputdev_irq=1;
  } 
  else if (irq == 10) {

    socwire_irq = 1;
  }
}

void init_irqmp(struct irqmp *lr) {
    lr->irqlevel = 0;   /* clear level reg */
    lr->irqmask = 0x0;  /* mask all interrupts */
    lr->irqclear = -1;  /* clear all pending interrupts */
    irqtbl[0] = 1;      /* init irqtable */
}

void socw_transfer(unsigned int address, unsigned int length) {

  printf("Start SoCWire DMA action\n");

  // Write TX descriptor to mem
  unsigned int * descriptor_base;
  descriptor_base = 0xa0001000;
  *descriptor_base = 0x00070000 | (length & 0xffff);
  *(descriptor_base + 1) = address;

  // Write RX descriptor to mem (loopback)
  *(descriptor_base+256) = 0x00030000 | (length & 0xffff);
  *(descriptor_base+257) = address;

  // Start transmission
  unsigned int * socw_addr;
  socw_addr = 0x80000a00;

  // Init Transmit descriptor pointer reg
  *(socw_addr+5) = (unsigned int)descriptor_base;

  // Init Receive descriptor pointer reg
  *(socw_addr+6) = (unsigned int)(descriptor_base+256);

  // Codec enabled (SE) / Codec Interrupt (PI)
  *socw_addr = 0x00000503;

}

void fft_radix4(complex32 samples[], complex32 twiddles[], complex32 inplace[], int N);

//int start() {
int main() {

  int i;
  int r=0;
  int frames = 0;

  unsigned int irqmpaddr = 0x8001F000;

  struct irqmp *lr = (struct irqmp *)irqmpaddr;
  irqmp_base = lr;
  init_irqmp(lr);
  
  // Install interrupt handlers
  for(i=1;i<16;i++) {

    catch_interrupt(irqhandler_f, i);

  }

  // Unmask all interrupts
  lr->irqmask = 0x0fffe;

  while(frames < 10) {

    printf("Processor waiting for data\n");

    while(inputdev_irq==0) {
      // Wait for IRQ from input device
    }
    
    printf("Received IRQ\n");
    inputdev_irq=0;

    // Computation function (samples: input stimuli, twiddles: twiddle factors, inplace: space for results)
    fft_radix4(samples, twiddles, inplace, N);

    printf("Completed FFT\n");

    // Upload data with SoCWire DMA
    socw_transfer(0xA0000000, 512);

    frames++;

  }

  printf("Test completed\n");

  return r;
}

void fft_radix4(complex32 samples[], complex32 twiddles[], complex32 inplace[], int N) {

  int input0I, input0Q, input1I, input1Q, input2I, input2Q, input3I, input3Q;
  int intdft0I, intdft0Q, intdft1I, intdft1Q, intdft2I, intdft2Q, intdft3I, intdft3Q;
  int twiddle1I, twiddle1Q, twiddle2I, twiddle2Q, twiddle3I, twiddle3Q;
  int tmp0I,tmp0Q,tmp1I,tmp1Q,tmp2I,tmp2Q,tmp3I,tmp3Q;

  int mask, c;
  int k, k0, k1, k2, k3;

  // first stage with bitinvers load
  
  for(k=0;k<N;k+=4){

    k0 = 1;
    k1 = 1;
    k2 = 1;
    k3 = 1;

    // reuse vars
    tmp1I = k + 1;
    tmp2I = k + 2;
    tmp3I = k + 3;

    mask = 1;

    // calculate bitinvers index
    k0 += (k & mask) << 5;
    k1 += (tmp1I & mask) << 5;
    k2 += (tmp2I & mask) << 5;
    k3 += (tmp3I & mask) << 5;
    mask <<= 1;

    k0 += (k & mask) << 3;
    k1 += (tmp1I & mask) << 3;
    k2 += (tmp2I & mask) << 3;
    k3 += (tmp3I & mask) << 3;
    mask <<= 1;

    k0 += (k & mask) << 1;
    k1 += (tmp1I & mask) << 1;
    k2 += (tmp2I & mask) << 1;
    k3 += (tmp3I & mask) << 1;
    mask <<= 1;

    k0 += (k & mask) >> 1;
    k1 += (tmp1I & mask) >> 1;
    k2 += (tmp2I & mask) >> 1;
    k3 += (tmp3I & mask) >> 1;
    mask <<= 1;

    k0 += (k & mask) >> 3;
    k1 += (tmp1I & mask) >> 3;
    k2 += (tmp2I & mask) >> 3;
    k3 += (tmp3I & mask) >> 3;
    mask <<= 1;

    k0 += (k & mask) >> 5;
    k1 += (tmp1I & mask) >> 5;
    k2 += (tmp2I & mask) >> 5;
    k3 += (tmp3I & mask) >> 5;

    // simplified butterflies of first stage (no mults)
    input0I = samples[k0].real;
    input0Q = samples[k0].imag;
    input2I = samples[k1].real;
    input2Q = samples[k1].imag;
    input1I = samples[k2].real;
    input1Q = samples[k2].imag;
    input3I = samples[k3].real;
    input3Q = samples[k3].imag;

    intdft0I = input0I+input2I;
    intdft1I = input1I+input3I;
    intdft2I = input0I-input2I;
    intdft3I = input1Q-input3Q;
    intdft0Q = input0Q+input2Q;
    intdft1Q = input1Q+input3Q;
    intdft2Q = input0Q-input2Q;
    intdft3Q = -input1I+input3I;

    inplace[tmp1I].real = (intdft0I+intdft1I);
    inplace[tmp1I].imag = (intdft0Q+intdft1Q);
    inplace[tmp2I].real = (intdft2I+intdft3I);
    inplace[tmp2I].imag = (intdft2Q+intdft3Q);
    inplace[tmp3I].real = (intdft0I-intdft1I);
    inplace[tmp3I].imag = (intdft0Q-intdft1Q);
    inplace[k+4].real = (intdft2I-intdft3I);
    inplace[k+4].imag = (intdft2Q-intdft3Q);

  }  

  // ********************************************************************
  // second stage

  k0 = 0;
    
  // perform 4 x 4 = N/4 butterflies
  for (c=0;c<=12;c+=4) {

    // load new twiddle factor
    twiddle1I = twiddles[c+1].real;
    twiddle1Q = twiddles[c+1].imag;
    twiddle2I = twiddles[(c<<1)+1].real;
    twiddle2Q = twiddles[(c<<1)+1].imag;
    twiddle3I = twiddles[3*c+1].real;
    twiddle3Q = twiddles[3*c+1].imag;

    // first butterfly of twiddlefactor-block
    
    // calculate indizes of butterfly inputs
    k0=(c>>2)+1;
    k1=(c>>2)+5;
    k2=(c>>2)+9;
    k3=(c>>2)+13;

    // load data from memory
    input0I = inplace[k0].real;
    input0Q = inplace[k0].imag;
    input2I = inplace[k1].real;
    input2Q = inplace[k1].imag;
    intdft2I = (input2I*twiddle2I - input2Q*twiddle2Q) >> scale;
    intdft2Q = (input2I*twiddle2Q + input2Q*twiddle2I) >> scale;  
    input1I = inplace[k2].real;
    input1Q = inplace[k2].imag;
    intdft1I = (input1I*twiddle1I - input1Q*twiddle1Q) >> scale;
    intdft1Q = (input1I*twiddle1Q + input1Q*twiddle1I) >> scale;
    input3I = inplace[k3].real;
    input3Q = inplace[k3].imag;
    intdft3I = (input3I*twiddle3I - input3Q*twiddle3Q) >> scale;
    intdft3Q = (input3I*twiddle3Q + input3Q*twiddle3I) >> scale;

    tmp0I = input0I + intdft2I;
    tmp0Q = input0Q + intdft2Q;
    tmp2I = intdft1I + intdft3I;
    tmp2Q = intdft1Q + intdft3Q; 
    inplace[k0].real = tmp0I + tmp2I;
    inplace[k2].real = tmp0I - tmp2I;
    inplace[k0].imag = tmp0Q + tmp2Q;
    inplace[k2].imag = tmp0Q - tmp2Q;
    tmp1I = input0I - intdft2I;
    tmp1Q = input0Q - intdft2Q;
    tmp3I = intdft1I - intdft3I;
    tmp3Q = intdft1Q - intdft3Q;
    inplace[k1].real = tmp1I + tmp3Q;
    inplace[k3].real = tmp1I - tmp3Q;
    inplace[k1].imag = tmp1Q - tmp3I;
    inplace[k3].imag = tmp1Q + tmp3I;

    // second butterfly of twiddlefactor-block
    
    // calculate indizes of butterfly inputs
    k0+=16;
    k1+=16;
    k2+=16;
    k3+=16;

    // load data from memory
    input0I = inplace[k0].real;
    input0Q = inplace[k0].imag;
    input2I = inplace[k1].real;
    input2Q = inplace[k1].imag;
    intdft2I = (input2I*twiddle2I - input2Q*twiddle2Q) >> scale;
    intdft2Q = (input2I*twiddle2Q + input2Q*twiddle2I) >> scale;  
    input1I = inplace[k2].real;
    input1Q = inplace[k2].imag;
    intdft1I = (input1I*twiddle1I - input1Q*twiddle1Q) >> scale;
    intdft1Q = (input1I*twiddle1Q + input1Q*twiddle1I) >> scale;
    input3I = inplace[k3].real;
    input3Q = inplace[k3].imag;
    intdft3I = (input3I*twiddle3I - input3Q*twiddle3Q) >> scale;
    intdft3Q = (input3I*twiddle3Q + input3Q*twiddle3I) >> scale;

    tmp0I = input0I + intdft2I;
    tmp0Q = input0Q + intdft2Q;
    tmp2I = intdft1I + intdft3I;
    tmp2Q = intdft1Q + intdft3Q; 
    inplace[k0].real = tmp0I + tmp2I;
    inplace[k2].real = tmp0I - tmp2I;
    inplace[k0].imag = tmp0Q + tmp2Q;
    inplace[k2].imag = tmp0Q - tmp2Q;
    tmp1I = input0I - intdft2I;
    tmp1Q = input0Q - intdft2Q;
    tmp3I = intdft1I - intdft3I;
    tmp3Q = intdft1Q - intdft3Q;
    inplace[k1].real = tmp1I + tmp3Q;
    inplace[k3].real = tmp1I - tmp3Q;
    inplace[k1].imag = tmp1Q - tmp3I;
    inplace[k3].imag = tmp1Q + tmp3I;

    // third butterfly of twiddlefactor-block

    // calculate indizes of butterfly inputs
    k0+=16;
    k1+=16;
    k2+=16;
    k3+=16;

    // load data from memory
    input0I = inplace[k0].real;
    input0Q = inplace[k0].imag;
    input2I = inplace[k1].real;
    input2Q = inplace[k1].imag;
    intdft2I = (input2I*twiddle2I - input2Q*twiddle2Q) >> scale;
    intdft2Q = (input2I*twiddle2Q + input2Q*twiddle2I) >> scale;  
    input1I = inplace[k2].real;
    input1Q = inplace[k2].imag;
    intdft1I = (input1I*twiddle1I - input1Q*twiddle1Q) >> scale;
    intdft1Q = (input1I*twiddle1Q + input1Q*twiddle1I) >> scale;
    input3I = inplace[k3].real;
    input3Q = inplace[k3].imag;
    intdft3I = (input3I*twiddle3I - input3Q*twiddle3Q) >> scale;
    intdft3Q = (input3I*twiddle3Q + input3Q*twiddle3I) >> scale;

    tmp0I = input0I + intdft2I;
    tmp0Q = input0Q + intdft2Q;
    tmp2I = intdft1I + intdft3I;
    tmp2Q = intdft1Q + intdft3Q; 
    inplace[k0].real = tmp0I + tmp2I;
    inplace[k2].real = tmp0I - tmp2I;
    inplace[k0].imag = tmp0Q + tmp2Q;
    inplace[k2].imag = tmp0Q - tmp2Q;
    tmp1I = input0I - intdft2I;
    tmp1Q = input0Q - intdft2Q;
    tmp3I = intdft1I - intdft3I;
    tmp3Q = intdft1Q - intdft3Q;
    inplace[k1].real = tmp1I + tmp3Q;
    inplace[k3].real = tmp1I - tmp3Q;
    inplace[k1].imag = tmp1Q - tmp3I;
    inplace[k3].imag = tmp1Q + tmp3I;

    // fourth butterfly of twiddlefactor-block

    // calculate indizes of butterfly inputs
    k0+=16;
    k1+=16;
    k2+=16;
    k3+=16;

    // load data from memory
    input0I = inplace[k0].real;
    input0Q = inplace[k0].imag;
    input2I = inplace[k1].real;
    input2Q = inplace[k1].imag;
    intdft2I = (input2I*twiddle2I - input2Q*twiddle2Q) >> scale;
    intdft2Q = (input2I*twiddle2Q + input2Q*twiddle2I) >> scale;  
    input1I = inplace[k2].real;
    input1Q = inplace[k2].imag;
    intdft1I = (input1I*twiddle1I - input1Q*twiddle1Q) >> scale;
    intdft1Q = (input1I*twiddle1Q + input1Q*twiddle1I) >> scale;
    input3I = inplace[k3].real;
    input3Q = inplace[k3].imag;
    intdft3I = (input3I*twiddle3I - input3Q*twiddle3Q) >> scale;
    intdft3Q = (input3I*twiddle3Q + input3Q*twiddle3I) >> scale;

    tmp0I = input0I + intdft2I;
    tmp0Q = input0Q + intdft2Q;
    tmp2I = intdft1I + intdft3I;
    tmp2Q = intdft1Q + intdft3Q; 
    inplace[k0].real = tmp0I + tmp2I;
    inplace[k2].real = tmp0I - tmp2I;
    inplace[k0].imag = tmp0Q + tmp2Q;
    inplace[k2].imag = tmp0Q - tmp2Q;
    tmp1I = input0I - intdft2I;
    tmp1Q = input0Q - intdft2Q;
    tmp3I = intdft1I - intdft3I;
    tmp3Q = intdft1Q - intdft3Q;
    inplace[k1].real = tmp1I + tmp3Q;
    inplace[k3].real = tmp1I - tmp3Q;
    inplace[k1].imag = tmp1Q - tmp3I;
    inplace[k3].imag = tmp1Q + tmp3I;
 
  }

  // ********************************************************************
  // third stage

  k1 = 16;
  k2 = 32;
  k3 = 48;

  // perform N/4 butterflies/stage
  for (c=0;c<16;c++) {

    // load new twiddle factor
    twiddle1I = twiddles[c+1].real;
    twiddle1Q = twiddles[c+1].imag;
    twiddle2I = twiddles[(c<<1)+1].real;
    twiddle2Q = twiddles[(c<<1)+1].imag;
    twiddle3I = twiddles[(3*c)+1].real;
    twiddle3Q = twiddles[(3*c)+1].imag;

    // calculate indizes of butterfly inputs
    k0=c+1;
    k1++;
    k2++;
    k3++;

    // load data from memory
    input0I = inplace[k0].real;
    input0Q = inplace[k0].imag;
    input2I = inplace[k1].real;
    input2Q = inplace[k1].imag;
    intdft2I = (input2I*twiddle2I - input2Q*twiddle2Q) >> scale;
    intdft2Q = (input2I*twiddle2Q + input2Q*twiddle2I) >> scale;  
    input1I = inplace[k2].real;
    input1Q = inplace[k2].imag;
    intdft1I = (input1I*twiddle1I - input1Q*twiddle1Q) >> scale;
    intdft1Q = (input1I*twiddle1Q + input1Q*twiddle1I) >> scale;
    input3I = inplace[k3].real;
    input3Q = inplace[k3].imag;
    intdft3I = (input3I*twiddle3I - input3Q*twiddle3Q) >> scale;
    intdft3Q = (input3I*twiddle3Q + input3Q*twiddle3I) >> scale;

    tmp0I = input0I + intdft2I;
    tmp0Q = input0Q + intdft2Q;
    tmp2I = intdft1I + intdft3I;
    tmp2Q = intdft1Q + intdft3Q; 
    inplace[k0].real = tmp0I + tmp2I;
    inplace[k2].real = tmp0I - tmp2I;
    inplace[k0].imag = tmp0Q + tmp2Q;
    inplace[k2].imag = tmp0Q - tmp2Q;
    tmp1I = input0I - intdft2I;
    tmp1Q = input0Q - intdft2Q;
    tmp3I = intdft1I - intdft3I;
    tmp3Q = intdft1Q - intdft3Q;
    inplace[k1].real = tmp1I + tmp3Q;
    inplace[k3].real = tmp1I - tmp3Q;
    inplace[k1].imag = tmp1Q - tmp3I;
    inplace[k3].imag = tmp1Q + tmp3I;
	
  }
}


