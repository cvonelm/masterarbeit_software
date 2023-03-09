from m5.SimObject import SimObject
from m5.params import *
from m5.proxy import *

class MLB(SimObject):
    type = "MLB"
    cxx_header = 'arch/riscv/mlb.hh'
    cxx_class = 'gem5::MLB'

    size =  Param.Int(64, "Number of MLB entries")
