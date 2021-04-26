// https://github.com/CedricGuillemet/ImGuizmo
// v 1.61 WIP
//
// The MIT License(MIT)
// 
// Copyright(c) 2016 Cedric Guillemet
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and / or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions :
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
// -------------------------------------------------------------------------------------------
// History :
// 2019/11/03 View gizmo
// 2016/09/11 Behind camera culling. Scaling Delta matrix not multiplied by source matrix scales. local/world rotation and translation fixed. Display message is incorrect (X: ... Y:...) in local mode.
// 2016/09/09 Hatched negative axis. Snapping. Documentation update.
// 2016/09/04 Axis switch and translation plan autohiding. Scale transform stability improved
// 2016/09/01 Mogwai changed to Manipulate. Draw debug cube. Fixed inverted scale. Mixing scale and translation/rotation gives bad results.
// 2016/08/31 First version
//
// -------------------------------------------------------------------------------------------
// Future (no order):
//
// - Multi view
// - display rotation/translation/scale infos in local/world space and not only local
// - finish local/world matrix application
// - OPERATION as bitmask
// 
// -------------------------------------------------------------------------------------------
// Example 
#pragma once

#define USE_IMGUI_API
#ifdef USE_IMGUI_API
#include "imconfig.h"
#include <imgui/imgui.h>
#endif
#ifndef IMGUI_API
#define IMGUI_API
#endif

namespace imgui {
    namespace gizmo
    {
        // call inside your own window and before Manipulate() in order to draw gizmo to that window.
        // Or pass a specific ImDrawList to draw to (e.g. ImGui::GetForegroundDrawList()).
        IMGUI_API void SetDrawlist(ImDrawList* drawlist = nullptr);

        // call BeginFrame right after ImGui_XXXX_NewFrame();
        IMGUI_API void BeginFrame();

        // return true if mouse cursor is over any gizmo control (axis, plan or screen component)
        IMGUI_API bool IsOver();

        // return true if mouse IsOver or if the gizmo is in moving state
        IMGUI_API bool IsUsing();

        // enable/disable the gizmo. Stay in the state until next call to Enable.
        // gizmo is rendered with gray half transparent color when disabled
        IMGUI_API void Enable(bool enable);

        // helper functions for manualy editing translation/rotation/scale with an input float
        // translation, rotation and scale float points to 3 floats each
        // Angles are in degrees (more suitable for human editing)
        // example:
        // float matrixTranslation[3], matrixRotation[3], matrixScale[3];
        // ImGuizmo::DecomposeMatrixToComponents(gizmoMatrix.m16, matrixTranslation, matrixRotation, matrixScale);
        // ImGui::InputFloat3("Tr", matrixTranslation, 3);
        // ImGui::InputFloat3("Rt", matrixRotation, 3);
        // ImGui::InputFloat3("Sc", matrixScale, 3);
        // ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, gizmoMatrix.m16);
        //
        // These functions have some numerical stability issues for now. Use with caution.
        IMGUI_API void DecomposeMatrixToComponents(const float* matrix, float* translation, float* rotation, float* scale);
        IMGUI_API void RecomposeMatrixFromComponents(const float* translation, const float* rotation, const float* scale, float* matrix);

        IMGUI_API void SetRect(float x, float y, float width, float height);
        // default is false
        IMGUI_API void SetOrthographic(bool isOrthographic);

        // Render a cube with face color corresponding to face normal. Usefull for debug/tests
        IMGUI_API void DrawCubes(const float* view, const float* projection, const float* matrices, int matrixCount);
        IMGUI_API void DrawGrid(const float* view, const float* projection, const float* matrix, const float gridSize);

        // call it when you want a gizmo
        // Needs view and projection matrices. 
        // matrix parameter is the source matrix (where will be gizmo be drawn) and might be transformed by the function. Return deltaMatrix is optional
        // translation is applied in world space
        enum OPERATION
        {
            TRANSLATE,
            ROTATE,
            SCALE,
            BOUNDS,
        };

        enum MODE
        {
            LOCAL,
            WORLD
        };

        IMGUI_API bool Manipulate(const float* view, const float* projection, OPERATION operation, MODE mode, float* matrix, float* deltaMatrix = NULL, const float* snap = NULL, const float* localBounds = NULL, const float* boundsSnap = NULL);
        //
        // Please note that this cubeview is patented by Autodesk : https://patents.google.com/patent/US7782319B2/en
        // It seems to be a defensive patent in the US. I don't think it will bring troubles using it as
        // other software are using the same mechanics. But just in case, you are now warned!
        //
        IMGUI_API void ViewManipulate(float* view, float length, ImVec2 position, ImVec2 size, ImU32 backgroundColor);

        IMGUI_API void SetID(int id);

        // return true if the cursor is over the operation's gizmo
        IMGUI_API bool IsOver(OPERATION op);
        IMGUI_API void SetGizmoSizeClipSpace(float value);


        inline void EditTransform(const float* cameraView, const float* cameraProjection, float* matrix, bool editTransformDecomposition)
        {
            static OPERATION mCurrentGizmoOperation(TRANSLATE);
            static MODE mCurrentGizmoMode(LOCAL);
            static bool useSnap = false;
            static float snap[] = { 1.f, 1.f, 1.f };
            static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
            static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
            static bool boundSizing = false;
            static bool boundSizingSnap = false;

            ImGuiIO& io = ImGui::GetIO();
            if (editTransformDecomposition)
            {
                if (!io.WantCaptureKeyboard)
                {
                    if (ImGui::IsKeyPressed(90))
                        mCurrentGizmoOperation = TRANSLATE;
                    if (ImGui::IsKeyPressed(69))
                        mCurrentGizmoOperation = ROTATE;
                    if (ImGui::IsKeyPressed(82)) // r Key
                        mCurrentGizmoOperation = SCALE;
                }

                if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == TRANSLATE))
                    mCurrentGizmoOperation = TRANSLATE;
                ImGui::SameLine();
                if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ROTATE))
                    mCurrentGizmoOperation = ROTATE;
                ImGui::SameLine();
                if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == SCALE))
                    mCurrentGizmoOperation = SCALE;
                float matrixTranslation[3], matrixRotation[3], matrixScale[3];
                DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
                ImGui::InputFloat3("Tr", matrixTranslation);
                ImGui::InputFloat3("Rt", matrixRotation);
                ImGui::InputFloat3("Sc", matrixScale);
                RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

                if (mCurrentGizmoOperation != SCALE)
                {
                    if (ImGui::RadioButton("Local", mCurrentGizmoMode == LOCAL))
                        mCurrentGizmoMode = LOCAL;
                    ImGui::SameLine();
                    if (ImGui::RadioButton("World", mCurrentGizmoMode == WORLD))
                        mCurrentGizmoMode = WORLD;
                }

                if (!io.WantCaptureKeyboard)
                    if (ImGui::IsKeyPressed(83))
                        useSnap = !useSnap;

                ImGui::Checkbox("", &useSnap);
                ImGui::SameLine();

                switch (mCurrentGizmoOperation)
                {
                case TRANSLATE:
                    ImGui::InputFloat3("Snap", &snap[0]);
                    break;
                case ROTATE:
                    ImGui::InputFloat("Angle Snap", &snap[0]);
                    break;
                case SCALE:
                    ImGui::InputFloat("Scale Snap", &snap[0]);
                    break;
                }
                ImGui::Checkbox("Bound Sizing", &boundSizing);
                if (boundSizing)
                {
                    ImGui::PushID(3);
                    ImGui::Checkbox("", &boundSizingSnap);
                    ImGui::SameLine();
                    ImGui::InputFloat3("Snap", boundsSnap);
                    ImGui::PopID();
                }
            }
            SetRect(0, 0, io.DisplaySize.x, io.DisplaySize.y);
            Manipulate(cameraView, cameraProjection, mCurrentGizmoOperation, mCurrentGizmoMode, matrix, NULL, useSnap ? snap : NULL, boundSizing ? bounds : NULL, boundSizingSnap ? boundsSnap : NULL);
        }

    };
}
