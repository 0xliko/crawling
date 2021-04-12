// Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "browser_handler.h"
#include <string>
#include <fstream>
#include "include/base/cef_logging.h"
#include "include/cef_browser.h"
#include "include/cef_string_visitor.h"
#include "include/wrapper/cef_helpers.h"
#include "global.h"


void BrowserHandler::PlatformTitleChange(CefRefPtr<CefBrowser> browser,
                                        const CefString& title) {
  return;
}
