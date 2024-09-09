from m5.params import *
from m5.SimObject import SimObject


class ExtraObject(SimObject):
    type = "ExtraObject"
    cxx_header = "tutorial/extra_object.hh"
    cxx_class = "gem5::ExtraObject"
    buffer_size = Param.MemorySize("1kB", "Size of the buffer to fill")
    write_bandwidth = Param.MemoryBandwidth(
        "100MB/s", "Bandwidth to fill the buffer"
    )
