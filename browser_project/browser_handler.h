// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_CEFBrowser_Browser_HANDLER_H_

#define CEF_TESTS_CEFBrowser_Browser_HANDLER_H_

#include "include/cef_client.h"
#include "include/wrapper/cef_resource_manager.h"
#include <list>
#include <iostream>
#include "../browser_response_filter.h"
using namespace std;

class BrowserHandler : public CefClient,
                       public CefDisplayHandler,
                       public CefLifeSpanHandler,
                       public CefLoadHandler,
                       public CefRequestHandler,
                       public CefResourceRequestHandler,
                       public CefRenderHandler                      
                      { 
 public:
  
  
  explicit BrowserHandler(bool use_views);
  ~BrowserHandler();
  int sizeX = 1920;
  int sizeY = 1080;

  
  // Provide access to the single global instance of this object.
  static BrowserHandler* GetInstance();

  // CefClient methods:
  virtual CefRefPtr<CefDisplayHandler> GetDisplayHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler() OVERRIDE {
    return this;
  }
  virtual CefRefPtr<CefLoadHandler> GetLoadHandler() OVERRIDE { return this; }



  CefRefPtr<CefRequestHandler> GetRequestHandler() OVERRIDE { return this; }

  CefRefPtr<CefRenderHandler> GetRenderHandler() OVERRIDE { return this; }

  CefRefPtr<CefResourceRequestHandler> GetResourceRequestHandler(
      CefRefPtr<CefBrowser> browser,
      CefRefPtr<CefFrame> frame,
      CefRefPtr<CefRequest> request,
      bool is_navigation,
      bool is_download,
      const CefString& request_initiator,
      bool& disable_default_handling) OVERRIDE { return this;}

  
  // CefDisplayHandler methods:
  
  virtual void OnTitleChange(CefRefPtr<CefBrowser> browser,
                             const CefString& title) OVERRIDE;

  // CefLifeSpanHandler methods:
  virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual bool DoClose(CefRefPtr<CefBrowser> browser) OVERRIDE;
  virtual void OnBeforeClose(CefRefPtr<CefBrowser> browser) OVERRIDE;

  // CefLoadHandler methods:
  virtual void OnLoadError(CefRefPtr<CefBrowser> browser,
                           CefRefPtr<CefFrame> frame,
                           ErrorCode errorCode,
                           const CefString& errorText,
                           const CefString& failedUrl) OVERRIDE;
  virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int httpStatusCode) OVERRIDE;
  
  virtual void OnLoadStart( CefRefPtr< CefBrowser > browser, 
                            CefRefPtr< CefFrame > frame, 
                            CefLoadHandler::TransitionType transition_type);
 
 // CefRenderHandler methods.
  bool GetRootScreenRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;
  void GetViewRect(CefRefPtr<CefBrowser> browser, CefRect& rect) OVERRIDE;
  bool GetScreenPoint(CefRefPtr<CefBrowser> browser,
                      int viewX,
                      int viewY,
                      int& screenX,
                      int& screenY) OVERRIDE;
  bool GetScreenInfo(CefRefPtr<CefBrowser> browser,
                     CefScreenInfo& screen_info) OVERRIDE;
  void OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) OVERRIDE;
  void OnPopupSize(CefRefPtr<CefBrowser> browser, const CefRect& rect) OVERRIDE;
  void OnPaint(CefRefPtr<CefBrowser> browser,
               CefRenderHandler::PaintElementType type,
               const CefRenderHandler::RectList& dirtyRects,
               const void* buffer,
               int width,
               int height) OVERRIDE;
  void OnAcceleratedPaint(CefRefPtr<CefBrowser> browser,
                          CefRenderHandler::PaintElementType type,
                          const CefRenderHandler::RectList& dirtyRects,
                          void* share_handle) OVERRIDE;
  /*void OnCursorChange(CefRefPtr<CefBrowser> browser,
                      CefCursorHandle cursor,
                      CursorType type,
                      const CefCursorInfo& custom_cursor_info) OVERRIDE;*/
  bool StartDragging(CefRefPtr<CefBrowser> browser,
                     CefRefPtr<CefDragData> drag_data,
                     CefRenderHandler::DragOperationsMask allowed_ops,
                     int x,
                     int y) OVERRIDE;
  void UpdateDragCursor(CefRefPtr<CefBrowser> browser,
                        CefRenderHandler::DragOperation operation) OVERRIDE;
  void OnImeCompositionRangeChanged(
      CefRefPtr<CefBrowser> browser,
      const CefRange& selection_range,
      const CefRenderHandler::RectList& character_bounds) OVERRIDE;

  
  
  virtual bool OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefFrame> frame,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) OVERRIDE;
 
  // CefResourceRequestHandler methods:
  
  virtual CefRefPtr< CefResponseFilter > GetResourceResponseFilter( CefRefPtr< CefBrowser > browser, 
                                                                    CefRefPtr< CefFrame > frame, 
                                                                    CefRefPtr< CefRequest > request, 
                                                                    CefRefPtr< CefResponse > response ) OVERRIDE;







  /// CerResponseFilter methods
 

  virtual bool OnBeforeBrowse( CefRefPtr< CefBrowser > browser, 
                               CefRefPtr< CefFrame > frame, 
                               CefRefPtr< CefRequest > request, 
                               bool user_gesture, 
                               bool is_redirect ) OVERRIDE;

  // Request that all existing browser windows close.
  void CloseAllBrowsers(bool force_close);

  bool IsClosing() const { return is_closing_; }

 public:

     
 private:
  // Platform-specific implementation.
  void PlatformTitleChange(CefRefPtr<CefBrowser> browser,
                           const CefString& title);
  bool requireCapture;

  // True if the application is using the Views framework.
  const bool use_views_;

  // List of existing browser windows. Only accessed on the CEF UI thread.
  typedef std::list<CefRefPtr<CefBrowser>> BrowserList;
  BrowserList browser_list_;
  bool is_closing_;
  

  // Include the default reference counting implementation.
  IMPLEMENT_REFCOUNTING(BrowserHandler);


};

#endif  // CEF_TESTS_CEFBrowser_Browser_HANDLER_H_
