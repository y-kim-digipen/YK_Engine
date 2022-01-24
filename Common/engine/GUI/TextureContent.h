//
// Created by yoonki on 1/23/22.
//

#ifndef ENGINE_TEXTURECONTENT_H
#define ENGINE_TEXTURECONTENT_H

#include "GUIContent.h"
#include <string>

namespace GUI {
    class TextureContent : public GUI_Content {
    public:
        TextureContent();
        TextureContent(const std::string& usingTextureStr);
        void Init() override {};
        void Render() override;
        void CleanUp() override{};

    private:
        std::string mUsingTextureStr;
    };
}
#endif //ENGINE_TEXTURECONTENT_H
