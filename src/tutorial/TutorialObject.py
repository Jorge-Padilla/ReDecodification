from m5.params import *
from m5.SimObject import SimObject


class TutorialObject(SimObject):
    type = "TutorialObject"
    cxx_header = "tutorial/tutorial_object.hh"
    cxx_class = "gem5::TutorialObject"
    latency = Param.Latency("Time before hitting the event")
    iterations = Param.Int(1, "Number of times to fire the event")
    extra_object = Param.ExtraObject("An Extra Object")
