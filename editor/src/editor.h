#pragma once
#include <beet/engine.h>

namespace beet {

class Editor {
   public:
    Editor();
    void run();

   private:
    std::unique_ptr<Engine> m_engine;
};

}  // namespace beet
