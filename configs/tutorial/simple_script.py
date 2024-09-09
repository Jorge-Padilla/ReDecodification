import argparse

from caches import *

import m5
from m5.objects import *

"""
Simulation Parser for parameters
Using the argparse python library
"""
parser = argparse.ArgumentParser(
    description="Simple CPU OOO with 2-level Cache."
)
parser.add_argument(
    "binary",
    default="",
    nargs="?",
    type=str,
    help="Path to the binary to execute",
)
parser.add_argument(
    "--l1i_size", help=f"L1 Instruction Cache size. Default: 16kB"
)
parser.add_argument("--l1d_size", help=f"L1 Data Cache size. Default: 64kB")
parser.add_argument("--l2_size", help=f"L2 Cache size. Default: 256kB")

options = parser.parse_args()

"""
System SimObject
Contains functional (not timing-level) information, such as
    memory ranges, root clock domain, root voltage domain
"""
system = System()

"""
Clock Domain definitions
SrcClockDomain() makes the clk_domain variable a Clock Domain defined by Gem5
clock will keep the frequency
voltage_domain will keep a voltage, the default is obtained by VoltageDomain()
"""
system.clk_domain = SrcClockDomain()
system.clk_domain.clock = "1GHz"
system.clk_domain.voltage_domain = VoltageDomain()

"""
Memory Definition
mem_mode defines the "simulation mode" of the memory
    timing is the most used
mem_ranges defines the size, we use AddrRange() for the creation of a whole range with the defined value
"""
system.mem_mode = "timing"
system.mem_ranges = [AddrRange("512MB")]

"""
CPU Definition
Basic example, X86TimingSimpleCPU
    timing-based, each instruction in a single clock cycle (except mem)
CPU Naming Format
    {ISA}{Type}CPU
"""
system.cpu = X86O3CPU()

"""
Memory Bus Definition
System-Wide memory bus with SystemXBar()
"""
system.membus = SystemXBar()

"""
Cache Definition
ICache and DCache are defined individualy
    If no Cache, conect to membus
Two types of Cache
    Clasic
        Regular Simple Cache
    Ruby
        Designed for Coherence
Cache Steps
    Instanciate the Caches
    Conect them to the ports

Hierarchy
    CPU to L1s
    L1s to L2 bus
    L2 bus to L2
    L2 to membus
"""
# L1s
system.cpu.icache = L1ICache(options.l1i_size)
system.cpu.dcache = L1DCache(options.l1d_size)

system.cpu.icache.connectCPU(system.cpu)
system.cpu.dcache.connectCPU(system.cpu)

# L2 bus
system.l2bus = L2XBar()

system.cpu.icache.connectBus(system.l2bus)
system.cpu.dcache.connectBus(system.l2bus)

# L2
system.l2cache = L2Cache(options.l2_size)
system.l2cache.connectCPUSideBus(system.l2bus)
system.l2cache.connectMemSideBus(system.membus)

# Use this instead if no caches are used, which conect directly to the membus
# system.cpu.icache_port = system.membus.cpu_side_ports
# system.cpu.dcache_port = system.membus.cpu_side_ports

"""
Ports Definition
Each mem object can have two kinds of ports
Request Ports
    From Request port to Response port
Response Ports
    From Response port to Request port
Must be conected that way
    cpu.request_port = cache.response_port
We can assign an array of ports to a port as well

Required Ports
    I/O Controller conected to the memory bus
    Special Functional Only port conected to the memory bus (mem reads and writes)
x86 Ports
    PIO conected to the memory bus
    Interrupt port conected to the memory bus
        requestor conected to cpu side
        responder conected to mem side
The Interrupt Controller creation is a function of the cpu class, no need to assign to an atribute
"""
system.cpu.createInterruptController()
system.cpu.interrupts[0].pio = system.membus.mem_side_ports
system.cpu.interrupts[0].int_requestor = system.membus.cpu_side_ports
system.cpu.interrupts[0].int_responder = system.membus.mem_side_ports

system.system_port = system.membus.cpu_side_ports

"""
Memory Controller Definition
A mem controller should be conected to the membus on the mem side
We must define a dram type and its range
"""
system.mem_ctrl = MemCtrl()
system.mem_ctrl.dram = DDR3_1600_8x8()
system.mem_ctrl.dram.range = system.mem_ranges[0]
system.mem_ctrl.port = system.membus.mem_side_ports

"""
Process to Execute
Syscall Emulation emulates only CPU and Mem, it only needs the CPU to point to a compiled executable
Fullsystem Emulation emulates the entire hardware system and runs a kernel (Like a Virtual Machine)

Process is a SimObject
    The Processes command is set to the command tu run
    Like an argv
        First position is the executable
        Next positions are the arguments
    Set the CPU to use the process and its workload
    Create the functional execution context
"""
binary = "tests/test-progs/hello/bin/x86/linux/hello"

system.workload = SEWorkload.init_compatible(binary)

process = Process()
process.cmd = [binary]
system.cpu.workload = process
system.cpu.createThreads()

"""
System Instance
Needs the creation of the Root object
The instantiaton process creates a C++ equivalent of every SimObject in Python
"""
root = Root(full_system=False, system=system)
m5.instantiate()

"""
Sinulation Execution
"""
print("Start of Simulation!")
exit_event = m5.simulate()
print("End of Simulation!")
print(f"Exit Status: {exit_event.getCause()} @ {m5.curTick()} ticks")
