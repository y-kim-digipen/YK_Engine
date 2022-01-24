//
// Created by yoonki on 1/23/22.
//

#ifndef ENGINE_FBODEBUGCONTENT_H
#define ENGINE_FBODEBUGCONTENT_H

#include "GUIContent.h"

namespace GUI {
    class FBODebugContent : public GUI_Content {
    public:
        void Init() override {};
        void Render() override;
        void CleanUp() override{};
    };
}
#endif //ENGINE_FBODEBUGCONTENT_H
