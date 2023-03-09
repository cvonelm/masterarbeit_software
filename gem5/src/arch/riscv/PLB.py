from m5.SimObject import SimObject
from m5.params import *
from m5.proxy import *

class PLB(SimObject):
    type = "PLB"
    cxx_header = 'arch/riscv/plb.hh'
    cxx_class = 'gem5::PLB'

    size =  Param.Int(64, "Number of PLB entries")
