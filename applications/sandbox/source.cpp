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

class GameModule : public rythe::module<rythe::feature_set<StartFeature, UpdateFeature, RenderFeature>> {
};

class PhysicsModule : public rythe::module<rythe::feature_set<RaycastFeature, RaycastTerrainFeature>> {
};

class Game : public rythe::program<GameModule, PhysicsModule> {
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

template<typename ProgramT, rythe::feature_c feature>
void checkFeature() {
    static_assert(ProgramT::template has_feature<feature>(), "Missing feature!");
}

int main() {
   using t = rsl::concat_sequence_t<rsl::type_sequence<StartFeature, UpdateFeature>, rsl::type_sequence<RaycastFeature, RaycastTerrainFeature, RenderFeature>>;
   std::cout << "type sequence:\n";
    logTypeSequence<t>();

    std::cout << "\nfeature set:\n";
    logFeatureSet<Game::features>();

    checkFeature<Game, StartFeature>();
    checkFeature<Game, UpdateFeature>();
    checkFeature<Game, RaycastFeature>();
    checkFeature<Game, RaycastTerrainFeature>();
    if constexpr (!Game::has_feature<RenderFeature>()) {
        std::cout << "Game does not have \"" << rsl::type_name<RenderFeature>() << "\"\n";
    }
    else {
        std::cout << "Game has \"" << rsl::type_name<RenderFeature>() << "\"\n";
    }

    return 0;
}
