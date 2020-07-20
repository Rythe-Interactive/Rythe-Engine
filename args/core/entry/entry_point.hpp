#pragma once
#include <core/engine/engine.hpp>
#include <iostream>

/**
 * @file entry_point.hpp
 * @brief When ARGS_ENTRY is defined, this file will create a function with signature main(int,char**) -> int
 *        implementing the common main function of a c++ program.
 * @note When defining ARGS_ENTRY do no create your own CRT_STARTUP such as main()->int, main(int,char**)->int,wmain(), etc...
 * @note When using ARGS_ENTRY you must instead implement reportModules(args::core::Engine*).
 * @note When not using ARGS_ENTRY you must call creation and initialisation of the engine manually.
 */


/**@brief Reports engine modules to the engine, must be implemented by you.
 * @param [in] engine The engine object as ptr *
 * @ref args::core::Engine::reportModule<T,...>()
 */
extern void reportModules(args::core::Engine* engine);

#ifdef ARGS_ENTRY
int main(int argc, char** argv)
{
	args::core::Engine engine;
	reportModules(&engine);

	engine.init();

#ifdef _DEBUG
	std::cin.ignore().get();
#endif // DEBUG
	return 0;
}
#endif