#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class ciEjectaApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;
};

void ciEjectaApp::setup()
{
}

void ciEjectaApp::mouseDown( MouseEvent event )
{
}

void ciEjectaApp::update()
{
}

void ciEjectaApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( ciEjectaApp, RendererGl )
