#include "testModule.hpp"
#include <iostream>

using namespace args;

priority_type TestModule::priority()
{
    return 0;
}

void TestModule::init()
{
    std::cout << "this is a test module." << std::endl;
}
