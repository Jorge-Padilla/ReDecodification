#include <iostream>

#include "tutorial/tutorial_object.hh"

namespace gem5
{

    TutorialObject::TutorialObject(const TutorialObjectParams &p)
     : SimObject(p)
    {
        std::cout << "Hello World! From a SimObject!" << std::endl;
    }

}   //namespace gem5
