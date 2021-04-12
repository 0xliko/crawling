#include "../browser_handler.h"
#include "../global.h"



bool BrowserHandler::OnBeforeBrowse( CefRefPtr< CefBrowser > browser, 
                               CefRefPtr< CefFrame > frame, 
                               CefRefPtr< CefRequest > request, 
                               bool user_gesture, 
                               bool is_redirect )
{
    
    if(is_freeze) return true;
    return false;
}

