//
// Created by yoonki on 1/23/22.
//

#include "TextureContent.h"
#include "../engine.h"
namespace GUI{
    TextureContent::TextureContent() : TextureContent("") { }
    TextureContent::TextureContent(const std::string &usingTextureStr) {
        mUsingTextureStr = usingTextureStr;
        if(!mUsingTextureStr.empty())
        {
            std::string titleName = mUsingTextureStr;
            int i = 0;
            while(!engine::GetGUIManager().HasWindow(titleName))
            {
                titleName = mUsingTextureStr + std::to_string(++i);
            }
            m_pParent->SetTitle(titleName);
        }
    }
    void GUI::TextureContent::Render() {
            auto& TextureComponents = engine::GetTextureManager().GetTextureObjects();
            if(ImGui::BeginCombo("Texture", mUsingTextureStr.c_str()))
            {
                for(auto& texture : TextureComponents)
                {
                    const std::string textureStr = texture.first;
                    bool isSelected = (mUsingTextureStr == textureStr);
                    if(ImGui::Selectable(textureStr.c_str(), isSelected))
                    {
                        mUsingTextureStr = textureStr;
                        std::string titleName = mUsingTextureStr;
                        int i = 0;
                        while(engine::GetGUIManager().HasWindow(titleName))
                        {
                            titleName = mUsingTextureStr + std::to_string(++i);
                        }
                        m_pParent->SetTitle(titleName);
                    }
                    if(isSelected)
                    {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            // Using a Child allow to fill all the space of the window.
            // It also alows customization
            ImGui::BeginChild("GameRender");
            // Get the size of the child (i.e. the whole draw size of the windows).
            ImVec2 wsize = ImGui::GetWindowSize();
            // Because I use the texture from OpenGL, I need to invert the V from the UV.
            auto pTextureObj = engine::GetTextureManager().FindTextureByName(mUsingTextureStr);
            if(pTextureObj)
            {
                auto handle = (void*)pTextureObj->GetHandle();
                ImGui::Image(handle, wsize, ImVec2(0, 1), ImVec2(1, 0));
            }
            ImGui::EndChild();
    }
}

