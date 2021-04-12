// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFBrowser_Browser_APP_H_
#define CEF_TESTS_CEFBrowser_Browser_APP_H_

#include "include/cef_app.h"
// Implement application-level callbacks for the browser process.
class BrowserApp : public CefApp, public CefBrowserProcessHandler, public CefRenderProcessHandler {
 public:
  BrowserApp();



  // CefApp methods:
  virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler()
      OVERRIDE {
    return this;
  }
   virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() {
    return this;
  }
  virtual void OnContextCreated(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                CefRefPtr<CefV8Context> context) OVERRIDE;
  // CefBrowserProcessHandler methods:
  virtual void OnContextInitialized() OVERRIDE;
  virtual bool OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) OVERRIDE;
  
  
  
 private:
  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(BrowserApp);
};

#endif  // CEF_TESTS_CEFBrowser_Browser_APP_H_
