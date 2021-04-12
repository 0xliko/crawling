#include "../browser_handler.h"
#include "../global.h"



CefRefPtr<CefResponseFilter> BrowserHandler::GetResourceResponseFilter( CefRefPtr< CefBrowser > browser, 
	                                                                    CefRefPtr< CefFrame > frame, 
	                                                                    CefRefPtr< CefRequest > request, 
	                                                                    CefRefPtr< CefResponse > response )
{
     if(request->GetMethod() == "POST")
        return nullptr;
     CefRefPtr<BrowserResponseFilter> filter =  new BrowserResponseFilter(request,response,frame);
     return filter;
}