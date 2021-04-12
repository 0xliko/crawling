// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "browser_handler.h"
#include <sstream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/cef_parser.h"
#include "include/views/cef_browser_view.h"
#include "include/views/cef_window.h"
#include "include/wrapper/cef_closure_task.h"
#include "include/wrapper/cef_helpers.h"
#include "global.h"


CefRefPtr<CefBrowser> main_browser;

namespace {

BrowserHandler* g_instance = nullptr;

}


BrowserHandler::BrowserHandler(bool use_views)
    :use_views_(use_views), is_closing_(false) {
  DCHECK(!g_instance);
  g_instance = this;
}

BrowserHandler::~BrowserHandler() {
  g_instance = nullptr;
}

// static
BrowserHandler* BrowserHandler::GetInstance() {
  return g_instance;
}


bool BrowserHandler::OnProcessMessageReceived(

    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {

  
  // Check the message name.
  const std::string& message_name = message->GetName();
   
  if (message_name == "get_screenshot") {
        
        
        
        CefRefPtr<CefListValue> args = message->GetArgumentList();
        int height = args->GetInt(0);
        view_content_height = height;
        cerr << "force redraw" << endl; 
        //browser->GetHost()->NotifyMoveOrResizeStarted();
        browser->GetHost()->NotifyScreenInfoChanged();
        usleep(1000*50);
        requireCapture = true;
        
        browser->GetHost()->Invalidate(PET_VIEW);
        return true;
   }
  
  
  
  return false;
}



