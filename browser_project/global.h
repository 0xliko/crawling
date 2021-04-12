// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.
// Entry point function for all processes.



// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#include "include/cef_browser.h"
#include "Base64.h"
#include "string.h"
#include <iostream>
#include "jsonxx.h"
#include <sys/time.h>


namespace jsonxx {
  
    extern bool parse_string(std::istream& input, String& value);
    extern bool parse_number(std::istream& input, Number& value);
    extern bool match(const char* pattern, std::istream& input);
}
using namespace std;
using namespace jsonxx;   
using namespace macaron;



void  sendMessageToClient(const char* msg,bool js_inject=false);


extern CefRefPtr<CefBrowser> main_browser;

extern pthread_t timeThread;
extern pthread_t executionTimeThread;

extern std::string work_path;
extern int view_port_width;
extern int view_port_height;
extern int view_content_height;

extern std::string preload_js_code;
extern int execution_id;
extern bool is_freeze;
extern std::vector<Object*> requestResults;





