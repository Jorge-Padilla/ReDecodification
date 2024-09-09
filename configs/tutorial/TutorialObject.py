from m5.params import *
from m5.SimObject import SimObject


class TutorialObject(SimObject):
    type = "TutorialObject"
    cxx_header = "configs/tutorial/tutorial_object.hh"
    cxx_class = "gem5::TutorialObject"
