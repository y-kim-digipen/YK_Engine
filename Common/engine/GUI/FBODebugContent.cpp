//
// Created by yoonki on 1/23/22.
//

#include "FBODebugContent.h"
#include "../engine.h"
namespace GUI{
    void GUI::FBODebugContent::Render() {

            // Using a Child allow to fill all the space of the window.
            // It also alows customization
            ImGui::BeginChild("GameRender");
            // Get the size of the child (i.e. the whole draw size of the windows).
            ImVec2 wsize = ImGui::GetWindowSize();
            // Because I use the texture from OpenGL, I need to invert the V from the UV.
            auto pTextureObj = engine::GetTextureManager().FindTextureByName("viewPosBuffer");

//            engine::GetTextureManager().BindTexture(pTextureObj);
//            auto handle = (void*)engine::gBuffer.GetFBOHandle();
            auto handle = (void*)engine::gBuffer.GetTextureAttachment(GL_COLOR_ATTACHMENT0)->GetHandle();
            ImGui::Image(handle, wsize, ImVec2(0, 1), ImVec2(1, 0));
            ImGui::EndChild();
    }
}

