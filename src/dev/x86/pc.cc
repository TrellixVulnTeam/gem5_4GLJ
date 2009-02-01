/*
 * Copyright (c) 2008 The Regents of The University of Michigan
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met: redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer;
 * redistributions in binary form must reproduce the above copyright
 * notice, this list of conditions and the following disclaimer in the
 * documentation and/or other materials provided with the distribution;
 * neither the name of the copyright holders nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Authors: Gabe Black
 */

/** @file
 * Implementation of PC platform.
 */

#include <deque>
#include <string>
#include <vector>

#include "arch/x86/intmessage.hh"
#include "arch/x86/x86_traits.hh"
#include "cpu/intr_control.hh"
#include "dev/terminal.hh"
#include "dev/x86/i82094aa.hh"
#include "dev/x86/i8254.hh"
#include "dev/x86/i8259.hh"
#include "dev/x86/pc.hh"
#include "dev/x86/south_bridge.hh"
#include "sim/system.hh"

using namespace std;
using namespace TheISA;

Pc::Pc(const Params *p)
    : Platform(p), system(p->system)
{
    southBridge = NULL;
    // set the back pointer from the system to myself
    system->platform = this;
}

void
Pc::init()
{
    assert(southBridge);

    /*
     * Initialize the timer.
     */
    I8254 & timer = *southBridge->pit;
    //Timer 0, mode 2, no bcd, 16 bit count
    timer.writeControl(0x34);
    //Timer 0, latch command
    timer.writeControl(0x00);
    //Write a 16 bit count of 0
    timer.writeCounter(0, 0);
    timer.writeCounter(0, 0);

    /*
     * Initialize the I/O APIC.
     */
    I82094AA & ioApic = *southBridge->ioApic;
    I82094AA::RedirTableEntry entry = 0;
    entry.deliveryMode = DeliveryMode::ExtInt;
    entry.vector = 0x20;
    ioApic.writeReg(0x10, entry.bottomDW);
    ioApic.writeReg(0x11, entry.topDW);
    entry.deliveryMode = DeliveryMode::Fixed;
    entry.vector = 0x24;
    ioApic.writeReg(0x18, entry.bottomDW);
    ioApic.writeReg(0x19, entry.topDW);
    entry.mask = 1;
    entry.vector = 0x21;
    ioApic.writeReg(0x12, entry.bottomDW);
    ioApic.writeReg(0x13, entry.topDW);
    entry.vector = 0x20;
    ioApic.writeReg(0x14, entry.bottomDW);
    ioApic.writeReg(0x15, entry.topDW);
    entry.vector = 0x28;
    ioApic.writeReg(0x20, entry.bottomDW);
    ioApic.writeReg(0x21, entry.topDW);
    entry.vector = 0x2C;
    ioApic.writeReg(0x28, entry.bottomDW);
    ioApic.writeReg(0x29, entry.topDW);
}

Tick
Pc::intrFrequency()
{
    panic("Need implementation\n");
    M5_DUMMY_RETURN
}

void
Pc::postConsoleInt()
{
    southBridge->ioApic->signalInterrupt(4);
    southBridge->pic1->signalInterrupt(4);
}

void
Pc::clearConsoleInt()
{
    warn_once("Don't know what interrupt to clear for console.\n");
    //panic("Need implementation\n");
}

void
Pc::postPciInt(int line)
{
    panic("Need implementation\n");
}

void
Pc::clearPciInt(int line)
{
    panic("Need implementation\n");
}

Addr
Pc::pciToDma(Addr pciAddr) const
{
    panic("Need implementation\n");
    M5_DUMMY_RETURN
}

Addr
Pc::calcPciConfigAddr(int bus, int dev, int func)
{
    assert(func < 8);
    assert(dev < 32);
    assert(bus == 0);
    return (PhysAddrPrefixPciConfig | (func << 8) | (dev << 11));
}

Addr
Pc::calcPciIOAddr(Addr addr)
{
    return PhysAddrPrefixIO + addr;
}

Addr
Pc::calcPciMemAddr(Addr addr)
{
    return addr;
}

Pc *
PcParams::create()
{
    return new Pc(this);
}
