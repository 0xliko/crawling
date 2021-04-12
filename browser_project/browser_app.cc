// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "browser_app.h"
#include "global.h"
#include <string>
#include "include/views/cef_browser_view.h"
#include "include/cef_browser.h"
#include "include/cef_command_line.h"
#include "include/views/cef_window.h"
#include "include/views/cef_box_layout.h"
#include "include/wrapper/cef_helpers.h"
#include "include/capi/cef_browser_capi.h"
#include "browser_handler.h"

#include <csignal>
#include <iostream>
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

using namespace std;


std::string work_path;
int view_port_width;
int view_port_height;
int view_content_height;



class BrowserV8Handler : public CefV8Handler {
public:
  BrowserV8Handler() {}

  virtual bool Execute(const CefString& name,
                       CefRefPtr<CefV8Value> object,
                       const CefV8ValueList& arguments,
                       CefRefPtr<CefV8Value>& retval,
                       CefString& exception) OVERRIDE {
    CEF_REQUIRE_UI_THREAD();
    if (name == "cef_browser_after_callback") {

      CefString str = arguments[0]->GetStringValue();
      sendMessageToClient(std::string(str).c_str(),true); 
      //return true;
    }
     
    // Function does not exist.
    return false;
  }

  // Provide the reference counting implementation for this class.
  IMPLEMENT_REFCOUNTING(BrowserV8Handler);
};

namespace {
  
  
  CefRefPtr<CefWindow> main_window;
  
  
// When using the Views framework this object provides the delegate
// implementation for the CefWindow that hosts the Views-based browser.
  

class BrowserWindowDelegate : public CefWindowDelegate {
 public:
  explicit BrowserWindowDelegate(CefRefPtr<CefBrowserView> browser_view)
      : browser_view_(browser_view) {}

  void OnWindowCreated(CefRefPtr<CefWindow> window) OVERRIDE {
    
    window->AddChildView(browser_view_);
    browser_view_->RequestFocus();
     
  }

  void OnWindowDestroyed(CefRefPtr<CefWindow> window) OVERRIDE {
    browser_view_ = nullptr;
  }

  bool CanClose(CefRefPtr<CefWindow> window) OVERRIDE {
    // Allow the window to close if the browser says it's OK.
    CefRefPtr<CefBrowser> browser = browser_view_->GetBrowser();
    if (browser)
      return browser->GetHost()->TryCloseBrowser();
    return true;
  }

  CefSize GetPreferredSize(CefRefPtr<CefView> view) OVERRIDE {
    
    if(view_port_height > 0 && view_port_width > 0)
      return CefSize(view_port_width, view_port_height);
    return CefSize(1280, 768);
  }

 private:
  CefRefPtr<CefBrowserView> browser_view_;

  IMPLEMENT_REFCOUNTING(BrowserWindowDelegate);
  DISALLOW_COPY_AND_ASSIGN(BrowserWindowDelegate);
};

class BrowserBrowserViewDelegate : public CefBrowserViewDelegate {
 public:
  BrowserBrowserViewDelegate() {}

  bool OnPopupBrowserViewCreated(CefRefPtr<CefBrowserView> browser_view,
                                 CefRefPtr<CefBrowserView> popup_browser_view,
                                 bool is_devtools) OVERRIDE {
    // Create a new top-level Window for the popup. It will show itself after
    // creation.
    return true;

  } 
  virtual void OnBrowserCreated(CefRefPtr<CefBrowserView> browser_view, CefRefPtr<CefBrowser> browser) OVERRIDE{
     
  }
 

 private:
  IMPLEMENT_REFCOUNTING(BrowserBrowserViewDelegate);
  DISALLOW_COPY_AND_ASSIGN(BrowserBrowserViewDelegate);
};

}  // namespace




BrowserApp::BrowserApp() {}

void BrowserApp::OnContextInitialized() {
  CEF_REQUIRE_UI_THREAD();
  

  CefRefPtr<CefCommandLine> command_line =
      CefCommandLine::GetGlobalCommandLine();

  
  
  // BrowserHandler implements browser-level callbacks.
  CefRefPtr<BrowserHandler> handler(new BrowserHandler(false));

  // Specify CEF browser settings here.
  CefBrowserSettings browser_settings;

  std::string url;

 

  view_port_width = 1024;
  view_port_height = 768;
  view_content_height = 768;
  work_path = "/tmp/";
  if(command_line->HasSwitch("path"))
     work_path = command_line->GetSwitchValue("path");
  
  if(command_line->HasSwitch("view-port"))
   {
      std::string view_port = command_line->GetSwitchValue("view-port");
      std::istringstream iss(view_port);
      sscanf(view_port.c_str(),"%d,%d",&view_port_width,&view_port_height);
      view_content_height = view_port_height;
   }

  
    // Information used when creating the native window.
    CefWindowInfo window_info;
    window_info.SetAsWindowless(0);
    main_browser = CefBrowserHost::CreateBrowserSync(window_info, handler, url, browser_settings,
                                  nullptr, nullptr);
   
    
    //sendMessageToClient("{\"type\":\"app-ready\"}"); 
  
}


void BrowserApp::OnContextCreated(CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefV8Context> context){

    CEF_REQUIRE_RENDERER_THREAD();


    // Retrieve the context's window object.

    CefRefPtr<CefV8Value> object = context->GetGlobal();

    // Create an instance of my CefV8Handler object.
    CefRefPtr<CefV8Handler> handler = new BrowserV8Handler();

    // Create the "cef_browser_callback" function.
    CefRefPtr<CefV8Value> func = CefV8Value::CreateFunction("cef_browser_after_callback", handler);

    // Add the "cef_browser_callback" function to the "window" object.
    object->SetValue("cef_browser_after_callback", func, V8_PROPERTY_ATTRIBUTE_NONE);


    CefRefPtr<CefV8Value> contentHeight = CefV8Value::CreateUInt(0);

    // Add the "cef_browser_callback" function to the "window" object.
    object->SetValue("browserContentHeight", contentHeight, V8_PROPERTY_ATTRIBUTE_NONE);


    return;
}


bool BrowserApp::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {

  CEF_REQUIRE_UI_THREAD();
  // Check the message name.
  const std::string& message_name = message->GetName();

  if (message_name == "get_screenshot") {

          
          //CefRefPtr<CefListValue> args = message->GetArgumentList();
          CefRefPtr<CefV8Context> context = frame->GetV8Context();  
          std::string js = "(function(){ var body = document.body,html = document.documentElement;var height = Math.max( body.scrollHeight, body.offsetHeight, html.clientHeight, html.scrollHeight, html.offsetHeight ); return height; })()";
          CefRefPtr<CefV8Value> val = CefV8Value::CreateUInt(0);
          CefRefPtr<CefV8Exception> exception;
          context->Eval(js,frame->GetURL(),0,val,exception);
          uint height =  val->GetUIntValue();

          CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("get_screenshot");

          // Retrieve the argument list object.
          CefRefPtr<CefListValue> args = msg->GetArgumentList();

          // Populate the argument values.
          args->SetString(0, "get_screenshot");
          args->SetInt(0, height);

          
          frame->SendProcessMessage(PID_BROWSER, msg);          
          return true;

          
  }
  return false;
}

