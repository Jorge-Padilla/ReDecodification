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

#include "base/bitunion.hh"
#include "dev/x86/i8042.hh"
#include "mem/packet.hh"
#include "mem/packet_access.hh"

// The 8042 has a whopping 32 bytes of internal RAM.
const uint8_t RamSize = 32;
const uint8_t NumOutputBits = 14;
const uint8_t X86ISA::PS2Keyboard::ID[] = {0xab, 0x83};
const uint8_t X86ISA::PS2Mouse::ID[] = {0x00};
const uint8_t CommandAck = 0xfa;
const uint8_t CommandNack = 0xfe;
const uint8_t BatSuccessful = 0xaa;

void
X86ISA::I8042::addressRanges(AddrRangeList &range_list)
{
    range_list.clear();
    range_list.push_back(RangeSize(dataPort, 1));
    range_list.push_back(RangeSize(commandPort, 1));
}

void
X86ISA::I8042::writeData(uint8_t newData, bool mouse)
{
    DPRINTF(I8042, "Set data %#02x.\n", newData);
    dataReg = newData;
    statusReg.outputFull = 1;
    statusReg.mouseOutputFull = (mouse ? 1 : 0);
    if (!mouse && commandByte.keyboardFullInt) {
        DPRINTF(I8042, "Sending keyboard interrupt.\n");
        keyboardIntPin->raise();
        //This is a hack
        keyboardIntPin->lower();
    } else if (mouse && commandByte.mouseFullInt) {
        DPRINTF(I8042, "Sending mouse interrupt.\n");
        mouseIntPin->raise();
        //This is a hack
        mouseIntPin->lower();
    }
}

void
X86ISA::PS2Device::ack()
{
    bufferData(&CommandAck, sizeof(CommandAck));
}

void
X86ISA::PS2Device::nack()
{
    bufferData(&CommandNack, sizeof(CommandNack));
}

void
X86ISA::PS2Device::bufferData(const uint8_t *data, int size)
{
    assert(data || size == 0);
    while (size) {
        outBuffer.push(*(data++));
        size--;
    }
}

uint8_t
X86ISA::I8042::readDataOut()
{
    uint8_t data = dataReg;
    statusReg.outputFull = 0;
    statusReg.mouseOutputFull = 0;
    if (keyboard.hasData()) {
        writeData(keyboard.getData(), false);
    } else if (mouse.hasData()) {
        writeData(mouse.getData(), true);
    }
    return data;
}

bool
X86ISA::PS2Keyboard::processData(uint8_t data)
{
    if (lastCommand != NoCommand) {
        switch (lastCommand) {
          case LEDWrite:
            DPRINTF(I8042, "Setting LEDs: "
                    "caps lock %s, num lock %s, scroll lock %s\n",
                    bits(data, 2) ? "on" : "off",
                    bits(data, 1) ? "on" : "off",
                    bits(data, 0) ? "on" : "off");
            ack();
            lastCommand = NoCommand;
            break;
          case TypematicInfo:
            DPRINTF(I8042, "Setting typematic info to %#02x.\n", data);
            ack();
            lastCommand = NoCommand;
            break;
        }
        return hasData();
    }
    switch (data) {
      case LEDWrite:
        DPRINTF(I8042, "Got LED write command.\n");
        ack();
        lastCommand = LEDWrite;
        break;
      case DiagnosticEcho:
        panic("Keyboard diagnostic echo unimplemented.\n");
      case AlternateScanCodes:
        panic("Accessing alternate scan codes unimplemented.\n");
      case ReadID:
        DPRINTF(I8042, "Got keyboard read ID command.\n");
        ack();
        bufferData((uint8_t *)&ID, sizeof(ID));
        break;
      case TypematicInfo:
        DPRINTF(I8042, "Setting typematic info.\n");
        ack();
        lastCommand = TypematicInfo;
        break;
      case Enable:
        DPRINTF(I8042, "Enabling the keyboard.\n");
        ack();
        break;
      case Disable:
        DPRINTF(I8042, "Disabling the keyboard.\n");
        ack();
        break;
      case DefaultsAndDisable:
        DPRINTF(I8042, "Disabling and resetting the keyboard.\n");
        ack();
        break;
      case AllKeysToTypematic:
        panic("Setting all keys to typemantic unimplemented.\n");
      case AllKeysToMakeRelease:
        panic("Setting all keys to make/release unimplemented.\n");
      case AllKeysToMake:
        panic("Setting all keys to make unimplemented.\n");
      case AllKeysToTypematicMakeRelease:
        panic("Setting all keys to "
                "typematic/make/release unimplemented.\n");
      case KeyToTypematic:
        panic("Setting a key to typematic unimplemented.\n");
      case KeyToMakeRelease:
        panic("Setting a key to make/release unimplemented.\n");
      case KeyToMakeOnly:
        panic("Setting key to make only unimplemented.\n");
      case Resend:
        panic("Keyboard resend unimplemented.\n");
      case Reset:
        panic("Keyboard reset unimplemented.\n");
      default:
        panic("Unknown keyboard command %#02x.\n", data);
    }
    return hasData();
}

bool
X86ISA::PS2Mouse::processData(uint8_t data)
{
    if (lastCommand != NoCommand) {
        switch(lastCommand) {
          case SetResolution:
            DPRINTF(I8042, "Mouse resolution set to %d.\n", data);
            resolution = data;
            ack();
            lastCommand = NoCommand;
            break;
          case SampleRate:
            DPRINTF(I8042, "Mouse sample rate %d samples "
                    "per second.\n", data);
            sampleRate = data;
            ack();
            lastCommand = NoCommand;
            break;
          default:
            panic("Not expecting data for a mouse command.\n");
        }
        return hasData();
    }
    switch (data) {
      case Scale1to1:
        DPRINTF(I8042, "Setting mouse scale to 1:1.\n");
        status.twoToOne = 0;
        ack();
        break;
      case Scale2to1:
        DPRINTF(I8042, "Setting mouse scale to 2:1.\n");
        status.twoToOne = 1;
        ack();
        break;
      case SetResolution:
        DPRINTF(I8042, "Setting mouse resolution.\n");
        lastCommand = SetResolution;
        ack();
        break;
      case GetStatus:
        DPRINTF(I8042, "Getting mouse status.\n");
        ack();
        bufferData((uint8_t *)&(status), 1);
        bufferData(&resolution, sizeof(resolution));
        bufferData(&sampleRate, sizeof(sampleRate));
        break;
      case ReadData:
        panic("Reading mouse data unimplemented.\n");
      case ResetWrapMode:
        panic("Resetting mouse wrap mode unimplemented.\n");
      case WrapMode:
        panic("Setting mouse wrap mode unimplemented.\n");
      case RemoteMode:
        panic("Setting mouse remote mode unimplemented.\n");
      case ReadID:
        DPRINTF(I8042, "Mouse ID requested.\n");
        ack();
        bufferData(ID, sizeof(ID));
        break;
      case SampleRate:
        DPRINTF(I8042, "Setting mouse sample rate.\n");
        lastCommand = SampleRate;
        ack();
        break;
      case DisableReporting:
        DPRINTF(I8042, "Disabling data reporting.\n");
        status.enabled = 0;
        ack();
        break;
      case EnableReporting:
        DPRINTF(I8042, "Enabling data reporting.\n");
        status.enabled = 1;
        ack();
        break;
      case DefaultsAndDisable:
        DPRINTF(I8042, "Disabling and resetting mouse.\n");
        sampleRate = 100;
        resolution = 4;
        status.twoToOne = 0;
        status.enabled = 0;
        ack();
        break;
      case Resend:
        panic("Mouse resend unimplemented.\n");
      case Reset:
        DPRINTF(I8042, "Resetting the mouse.\n");
        sampleRate = 100;
        resolution = 4;
        status.twoToOne = 0;
        status.enabled = 0;
        ack();
        bufferData(&BatSuccessful, sizeof(BatSuccessful));
        bufferData(ID, sizeof(ID));
        break;
      default:
        warn("Unknown mouse command %#02x.\n", data);
        nack();
        break;
    }
    return hasData();
}


Tick
X86ISA::I8042::read(PacketPtr pkt)
{
    assert(pkt->getSize() == 1);
    Addr addr = pkt->getAddr();
    if (addr == dataPort) {
        uint8_t data = readDataOut();
        //DPRINTF(I8042, "Read from data port got %#02x.\n", data);
        pkt->set<uint8_t>(data);
    } else if (addr == commandPort) {
        //DPRINTF(I8042, "Read status as %#02x.\n", (uint8_t)statusReg);
        pkt->set<uint8_t>((uint8_t)statusReg);
    } else {
        panic("Read from unrecognized port %#x.\n", addr);
    }
    return latency;
}

Tick
X86ISA::I8042::write(PacketPtr pkt)
{
    assert(pkt->getSize() == 1);
    Addr addr = pkt->getAddr();
    uint8_t data = pkt->get<uint8_t>();
    if (addr == dataPort) {
        statusReg.commandLast = 0;
        switch (lastCommand) {
          case NoCommand:
            if (keyboard.processData(data)) {
                writeData(keyboard.getData(), false);
            }
            break;
          case WriteToMouse:
            if (mouse.processData(data)) {
                writeData(mouse.getData(), true);
            }
            break;
          case WriteCommandByte:
            commandByte = data;
            DPRINTF(I8042, "Got data %#02x for \"Write "
                    "command byte\" command.\n", data);
            statusReg.passedSelfTest = (uint8_t)commandByte.passedSelfTest;
            break;
          case WriteMouseOutputBuff:
            DPRINTF(I8042, "Got data %#02x for \"Write "
                    "mouse output buffer\" command.\n", data);
            writeData(data, true);
            break;
          default:
            panic("Data written for unrecognized "
                    "command %#02x\n", lastCommand);
        }
        lastCommand = NoCommand;
    } else if (addr == commandPort) {
        DPRINTF(I8042, "Got command %#02x.\n", data);
        statusReg.commandLast = 1;
        // These purposefully leave off the first byte of the controller RAM
        // so it can be handled specially.
        if (data > ReadControllerRamBase &&
                data < ReadControllerRamBase + RamSize) {
            panic("Attempted to use i8042 read controller RAM command to "
                    "get byte %d.\n", data - ReadControllerRamBase);
        } else if (data > WriteControllerRamBase &&
                data < WriteControllerRamBase + RamSize) {
            panic("Attempted to use i8042 read controller RAM command to "
                    "get byte %d.\n", data - ReadControllerRamBase);
        } else if (data >= PulseOutputBitBase &&
                data < PulseOutputBitBase + NumOutputBits) {
            panic("Attempted to use i8042 pulse output bit command to "
                    "to pulse bit %d.\n", data - PulseOutputBitBase);
        }
        switch (data) {
          case GetCommandByte:
            DPRINTF(I8042, "Getting command byte.\n");
            writeData(commandByte);
            break;
          case WriteCommandByte:
            DPRINTF(I8042, "Setting command byte.\n");
            lastCommand = WriteCommandByte;
            break;
          case CheckForPassword:
            panic("i8042 \"Check for password\" command not implemented.\n");
          case LoadPassword:
            panic("i8042 \"Load password\" command not implemented.\n");
          case CheckPassword:
            panic("i8042 \"Check password\" command not implemented.\n");
          case DisableMouse:
            DPRINTF(I8042, "Disabling mouse at controller.\n");
            commandByte.disableMouse = 1;
            break;
          case EnableMouse:
            DPRINTF(I8042, "Enabling mouse at controller.\n");
            commandByte.disableMouse = 0;
            break;
          case TestMouse:
            panic("i8042 \"Test mouse\" command not implemented.\n");
          case SelfTest:
            panic("i8042 \"Self test\" command not implemented.\n");
          case InterfaceTest:
            panic("i8042 \"Interface test\" command not implemented.\n");
          case DiagnosticDump:
            panic("i8042 \"Diagnostic dump\" command not implemented.\n");
          case DisableKeyboard:
            DPRINTF(I8042, "Disabling keyboard at controller.\n");
            commandByte.disableKeyboard = 1;
            break;
          case EnableKeyboard:
            DPRINTF(I8042, "Enabling keyboard at controller.\n");
            commandByte.disableKeyboard = 0;
            break;
          case ReadInputPort:
            panic("i8042 \"Read input port\" command not implemented.\n");
          case ContinuousPollLow:
            panic("i8042 \"Continuous poll low\" command not implemented.\n");
          case ContinuousPollHigh:
            panic("i8042 \"Continuous poll high\" command not implemented.\n");
          case ReadOutputPort:
            panic("i8042 \"Read output port\" command not implemented.\n");
          case WriteOutputPort:
            panic("i8042 \"Write output port\" command not implemented.\n");
          case WriteKeyboardOutputBuff:
            panic("i8042 \"Write keyboard output buffer\" "
                    "command not implemented.\n");
          case WriteMouseOutputBuff:
            DPRINTF(I8042, "Got command to write to mouse output buffer.\n");
            lastCommand = WriteMouseOutputBuff;
            break;
          case WriteToMouse:
            DPRINTF(I8042, "Expecting mouse command.\n");
            lastCommand = WriteToMouse;
            break;
          case DisableA20:
            panic("i8042 \"Disable A20\" command not implemented.\n");
          case EnableA20:
            panic("i8042 \"Enable A20\" command not implemented.\n");
          case ReadTestInputs:
            panic("i8042 \"Read test inputs\" command not implemented.\n");
          case SystemReset:
            panic("i8042 \"System reset\" command not implemented.\n");
          default:
            panic("Write to unknown i8042 "
                    "(keyboard controller) command port.\n");
        }
    } else {
        panic("Write to unrecognized port %#x.\n", addr);
    }
    return latency;
}

X86ISA::I8042 *
I8042Params::create()
{
    return new X86ISA::I8042(this);
}
