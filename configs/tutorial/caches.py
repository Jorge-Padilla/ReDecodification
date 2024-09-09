from m5.objects import Cache

"""
L1 Cache Definition
"""


class L1Cache(Cache):
    assoc = 2
    tag_latency = 2
    data_latency = 2
    response_latency = 2
    mshrs = 4
    tgts_per_mshr = 20

    def __init__(self, size=None):
        super().__init__()
        pass

    def connectCPU(self, cpu):
        # To be Overrited on subclasses
        raise NotImplementedError

    def connectBus(self, bus):
        self.mem_side = bus.cpu_side_ports


class L1ICache(L1Cache):
    size = "16kB"

    def __init__(self, size=None):
        super().__init__(size)
        if not size:
            return
        self.size = size

    def connectCPU(self, cpu):
        self.cpu_side = cpu.icache_port


class L1DCache(L1Cache):
    size = "64kB"

    def __init__(self, size=None):
        super().__init__(size)
        if not size:
            return
        self.size = size

    def connectCPU(self, cpu):
        self.cpu_side = cpu.dcache_port


"""
L2 Cache Definition
"""


class L2Cache(Cache):
    size = "256kB"
    assoc = 8
    tag_latency = 20
    data_latency = 20
    response_latency = 20
    mshrs = 20
    tgts_per_mshr = 12

    def __init__(self, size=None):
        super().__init__()
        if not size:
            return
        self.size = size

    def connectCPUSideBus(self, cpu):
        self.cpu_side = cpu.mem_side_ports

    def connectMemSideBus(self, bus):
        self.mem_side = bus.cpu_side_ports
