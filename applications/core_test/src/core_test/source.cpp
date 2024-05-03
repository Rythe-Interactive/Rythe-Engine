#define RYTHE_ENTRY

#include <rsl/time>
#include <rsl/math>
#include <rsl/logging>
#include <chrono>
#include <iostream>

#include <core/core.hpp>

void RYTHE_CCONV reportModules(rythe::core::Program* program)
{
    rsl::log::debug("Initilizing Core-Application");
    program->addEngineInstance();
}
