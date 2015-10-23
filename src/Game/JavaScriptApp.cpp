//
//  JavaScriptApp.cpp
//  Mural
//
//  Created by Sean on 10/22/15.
//
//

#include "JavaScriptApp.hpp"

JavaScriptApp::JavaScriptApp() {
  jsContext = duk_create_heap_default();
}

JavaScriptApp::~JavaScriptApp() {
  duk_destroy_heap(jsContext);
}

static void register_MuImage(duk_context *jsContext) {
  duk_push_global_object(jsContext);
}

void JavaScriptApp::init() {
  register_MuImage(jsContext);
}
