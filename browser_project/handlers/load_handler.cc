#include "../browser_handler.h"
#include "../global.h"

void BrowserHandler::OnLoadError(CefRefPtr<CefBrowser> browser,
                                CefRefPtr<CefFrame> frame,
                                ErrorCode errorCode,
                                const CefString& errorText,
                                const CefString& failedUrl) 
{
  CEF_REQUIRE_UI_THREAD();
  
  
  // Don't display an error for downloaded files.
  if (errorCode == ERR_ABORTED)
    return;
  pthread_cancel(timeThread);
  timeThread = 0;
  std::stringstream ss;
  ss << "{\"action\":\"open\",\"result\":\"network error\",\"code\":" << errorCode << "}";
  frame->ExecuteJavaScript("var cef_browser_callback = function(identify,arg){var ret = {action:\"inject_javascript\",result:{}}; ret.result.data = arg; var data = JSON.stringify(ret);if(identify == window.execution_id) cef_browser_after_callback(data);}",frame->GetURL(),0);                 
  sendMessageToClient(ss.str().c_str()); 
  
  return;
  //frame->LoadURL(GetDataURI(ss.str(), "text/html"));
}

void BrowserHandler::CloseAllBrowsers(bool force_close) {
  if (!CefCurrentlyOn(TID_UI)) {
    // Execute on the UI thread.
    CefPostTask(TID_UI, base::Bind(&BrowserHandler::CloseAllBrowsers, this,
                                   force_close));
    return;
  }

  if (browser_list_.empty())
    return;

  BrowserList::const_iterator it = browser_list_.begin();
  for (; it != browser_list_.end(); ++it)
    (*it)->GetHost()->CloseBrowser(force_close);
}



void BrowserHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         int httpStatusCode){
      
      if(httpStatusCode == 0) return;
      if(httpStatusCode == 200){

         if(timeThread && frame->IsMain()){
            pthread_cancel(timeThread);
            timeThread = 0;
            sendMessageToClient("{\"action\":\"open\",\"result\":\"ok\"}");
         }        
            frame->ExecuteJavaScript("var cef_browser_callback = function(identify,arg){var ret = {action:\"inject_javascript\",result:{}}; ret.result.data = arg; var data = JSON.stringify(ret);if(identify == window.execution_id) cef_browser_after_callback(data);}",frame->GetURL(),0);                 

           
          
      }
      else{

         if(timeThread && frame->IsMain()){
           pthread_cancel(timeThread);
           timeThread = 0;
           std::stringstream ss;
           ss << "{\"action\":\"open\",\"result\":" << httpStatusCode << "}";
           sendMessageToClient(ss.str().c_str());
         }
         frame->ExecuteJavaScript("var cef_browser_callback = function(identify,arg){var ret = {action:\"inject_javascript\",result:{}}; ret.result.data = arg; var data = JSON.stringify(ret);if(identify == window.execution_id) cef_browser_after_callback(data);}",frame->GetURL(),0);                 
         //sendMessage(std::string("{\"action\":\"open\",\"result\":\"") +  std::itos(httpStatusCode) + std::string("\"}"));
         
      }

}
void BrowserHandler::OnLoadStart( CefRefPtr< CefBrowser > browser, 
                            CefRefPtr< CefFrame > frame, 
                            CefLoadHandler::TransitionType transition_type){
     if(!preload_js_code.empty()){
       frame->ExecuteJavaScript(preload_js_code,frame->GetURL(),0);
     }

}