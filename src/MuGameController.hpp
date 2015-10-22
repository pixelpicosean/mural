//
//  MuGameController.hpp
//  Mural
//
//  Created by Sean on 10/22/15.
//
//

#pragma once

namespace mural {

  using ci::app::MouseEvent;
  using ci::app::KeyEvent;

  class MuGameController {
    public:
      virtual void init() {}
      virtual void update() {}
      virtual void draw() {}

      virtual void mouseDown(MouseEvent event) {}
      virtual void mouseMove(MouseEvent event) {}
      virtual void mouseUp(MouseEvent event) {}

      virtual void keyDown(KeyEvent event) {}
      virtual void keyUp(KeyEvent event) {}
  };

}
