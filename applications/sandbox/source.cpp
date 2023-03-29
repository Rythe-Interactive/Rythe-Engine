//#define LEGION_ENTRY
//#define LEGION_LOG_DEBUG
//
//#if defined(NDEBUG)
//    #define LEGION_KEEP_CONSOLE
//#endif
//
//#include <core/core.hpp>
//#include <application/application.hpp>
//#include <graphics/graphics.hpp>
//#include "module/examplemodule.hpp"
//
//#ifdef RYTHE_AUDIO
//#include <audio/audio.hpp>
//#endif
//
//void LEGION_CCONV reportModules(legion::Engine* engine)
//{
//    using namespace legion;
//    engine->reportModule<app::ApplicationModule>();
//    engine->reportModule<gfx::RenderingModule>();
//    engine->reportModule<ExampleModule>();
//
//#ifdef RYTHE_AUDIO
//    engine->reportModule<audio::AudioModule>();
//#endif
//}

//#define LEGION_ENTRY

#include <iostream>

#include <core/core.hpp>

struct StartFeature : public rythe::feature {
    void invoke() {}
};

struct UpdateFeature : public rythe::feature {
    void invoke() {}
};

struct RaycastFeature : public rythe::feature {
    void invoke() {}
};

struct RaycastTerrainFeature : public rythe::feature {
    void invoke() {}
};


struct RenderFeature : public rythe::feature {
    void invoke() {}
};

template<typename T>
class GameModule : public rythe::Module<T, rythe::feature_set<StartFeature, UpdateFeature>> {
};

template<typename T>
class PhysicsModule : public rythe::Module<T, rythe::feature_set<RaycastFeature, RaycastTerrainFeature>> {
};

class Game : public rythe::Program<Game, GameModule, PhysicsModule> {
};

template<typename T>
void logType() {
    std::cout << rsl::type_name<T>() << '\n';
}

template<typename... Types>
void logTypeSequenceImpl(rsl::type_sequence<Types...>) {
    (logType<Types>(), ...);
}

template<rsl::type_sequence_c Seq>
void logTypeSequence() {
    logTypeSequenceImpl(Seq{});
}

template<rythe::feature_set_c Seq>
void logFeatureSet() {
    logTypeSequenceImpl(typename Seq::type_sequence{});
}

template<rythe::feature_set_c Seq, rythe::feature_c feature>
void checkFeature() {
    static_assert(rsl::type_sequence_contains_v<typename Seq::type_sequence, feature>, "Missing feature!");
}

int main() {
   using t = rsl::concat_sequence_t<rsl::type_sequence<StartFeature, UpdateFeature>, rsl::type_sequence<RaycastFeature, RaycastTerrainFeature>>;

    logTypeSequence<t>();

    logFeatureSet<Game::features>();

    checkFeature<Game::features, StartFeature>();
    checkFeature<Game::features, UpdateFeature>();
    checkFeature<Game::features, RaycastFeature>();
    checkFeature<Game::features, RaycastTerrainFeature>();
    checkFeature<Game::features, RenderFeature>();

    return 0;
}
