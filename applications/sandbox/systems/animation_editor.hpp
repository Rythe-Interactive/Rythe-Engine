#pragma once
#include <variant>
#include <core/core.hpp>
#include <rendering/util/gui.hpp>
#include <rendering/rendering.hpp>
#include <rendering/pipeline/gui/stages/imguirenderstage.hpp>


#include "../data/animation.hpp"
#include <application/input/inputsystem.hpp>

struct copy_key_frame : application::input_action<copy_key_frame>{};

namespace ext
{


    using namespace legion;

    namespace detail {

        class AnimationSequencer : public imgui::sequencer::SequenceInterface
        {
        public:

            using event_t = std::pair<std::shared_ptr<animation_event_base>, id_type>;
            using holder = std::variant<position, rotation, scale, event_t>;


            //////////////////////////////////////////////////////
            ///INTERFACE
            ///
            int GetFrameMin() const override;

            int GetFrameMax() const override;

            int GetItemCount() const override;

            int GetItemTypeCount() const override;
            const char* GetItemTypeName(int type) const override;

            const char* GetItemLabel(int index) const override;

            void Get(int index, int** start, int** end, int* type,
                unsigned* color) override;

            void Add(int type) override;

            void Del(int index) override;

            void Duplicate(int index) override;
            ///
            //////////////////////////////////////////////////////

            /**
             * @brief sets the animation to work with
             */
            void SetAnimation(animation* anim);

            /**
             * @brief gets the modified animation
             */
            animation GetAnimation() const;


            /**
             * @brief gets a particular datapoint at a certain index
             */
           holder& GetDataPointAt(int index);
        private:

            const char* make_label(holder v) const
            {
                return
                std::holds_alternative<position>(v) ? "Position" :
                std::holds_alternative<rotation>(v) ? "Rotation" :
                std::holds_alternative<scale>(v)    ? "Scale":
                                                      "Event";
            }

            struct InternalAnimationData
            {
                bool running, looping;
                float ra;
                float pa;
                float sa;
                float ea;
                index_type ri;
                index_type pi;
                index_type si;
                index_type ei;

            } m_savedAnimationData;

            int m_frameMax = 100;
            int m_frameMin = 0;
            struct SequencerData
            {
                std::vector<std::tuple<int, int, holder>> data;
            }m_currentAnimation;

        };
    }

    class AnimationEditor : public System<AnimationEditor>
    {
        ecs::entity_handle m_cubeEntity;

        //this is for copying the camera position and rotation to the current animator values
        //
        position m_copiedPosition;
        rotation m_copiedRotation;
        bool m_applyCopy = false;

        int m_selectedEntry = 0;
        int m_firstFrame = 0;
        detail::AnimationSequencer m_sequencer;
        bool m_animatorHasControl = false;

        using custom_render_gui_fn = delegate<bool(id_type, animation_event_base*)>;

        static sparse_map<id_type, custom_render_gui_fn> m_guiRenderers;


        //since this is a char buffer we need to explicitly spell out assets://test.anim ... how lovely
        char m_filenameBuffer[512]{ 'a','s','s','e','t','s',':','\\','\\','t','e','s','t','.','a','n','i','m','\0' };

        void setup() override
        {
            //we use _view, thus we need access to the literals namespace
            using namespace filesystem::literals;

            app::window window = world.get_component_handle<app::window>().read();

            rendering::model_handle cubeModel;
            rendering::material_handle vertexColorMaterial;

            {
                //to create models and materials we need to lock the window entity
                //this is in a higher scope, such that the guard is returned on completion
                //of the material & mesh creation
                application::context_guard guard(window);

                cubeModel = rendering::ModelCache::create_model("cube - Animator", "assets://models/cube.obj"_view);
                vertexColorMaterial = rendering::MaterialCache::create_material("color shader - Animator", "assets://shaders/texture.shs"_view);
            }

            // create a new entity that we can attach the animation to
            m_cubeEntity = createEntity();
            //TODO(algo-ryth-mix): In the future this boilerplate code should
            //TODO(cont.)          go in favor of a raycasting system.

            //add mesh & material
            m_cubeEntity.add_components<transform>(position(), rotation(), scale());
            m_cubeEntity.add_components<rendering::mesh_renderable>(mesh_filter(cubeModel.get_mesh()), rendering::mesh_renderer(vertexColorMaterial));

            //create new running & looping animation
            animation anim{ true };

            //load animation data from disk
            filesystem::basic_resource res = fs::view("assets://test.anim").get().except([](auto err)
                {
                    return filesystem::basic_resource("{}");
                });

            //convert to animation
            anim = res.to<animation>();

            //add animation to entity
            m_cubeEntity.add_component<animation>(anim);

            //add custom gui stage
            rendering::ImGuiStage::addGuiRender<AnimationEditor, &AnimationEditor::onGUI>(this);

            application::InputSystem::createBinding<copy_key_frame>(application::inputmap::method::P);
            bindToEvent<copy_key_frame,&AnimationEditor::onCopyKeyFrame>();

        }

        void onCopyKeyFrame(copy_key_frame* evt)
        {
            //get camera
            static auto cameraQuery = createQuery<rendering::camera>();
            cameraQuery.queryEntities();
            if(cameraQuery.size() != 0)
            {

                //copy pos and rot to applycopy
                auto [posH,rotH,_]= cameraQuery[0].get_component_handles<transform>();
                m_copiedRotation = rotH.read();
                m_copiedPosition = posH.read();
                m_applyCopy = true;
            }

        }

        void onGUI(application::window&, rendering::camera
            &cam, const rendering::camera::camera_input& cInput,
            time::span);

    public:


        /**
         * @brief The function signature for a custom render Layer for custom
         *         Animation-Events editors
         */
        using custom_render_layer = custom_render_gui_fn;


        /**
         * @brief Registers a custom GUI Layer for an event type
         * @note if no custom layer is registered, your event will be rendered with the default
         *        Base Layer
         * @param event_id The ID you want to register a custom render layer for
         * @param renderer The Custom render function you want to register.
         *         Please note that this is called in an active gui context, so there is no need
         *         to call imgui::base::Begin() again. if you want the base gui layer to be
         *         rendered in conjunction with your custom layer your function should return
         *         true, otherwise it should return false.
         */
        static void onRenderCustomEventGUI(id_type event_id, custom_render_layer renderer)
        {
            m_guiRenderers[event_id] = renderer;
        }
    };


}
