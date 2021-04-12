
#include "global.h"
#include "include/base/cef_logging.h"
#include <iostream>
#include <csignal>
#include <ctime>
#include <iomanip>
#include <cassert>
#include <sstream>
#include <string>
#include <fstream>
#include <pthread.h>
#include <thread>
#include <unistd.h>
#include "browser_handler.h"
#include <X11/Xatom.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "include/cef_cookie.h"

extern std::vector<Object*> requestResults;


pthread_t timeThread;
pthread_t executionTimeThread;
std::string preload_js_code;
int execution_id;
bool is_freeze;
std::vector<Object*> requestResults;

typedef std::vector<std::string> CookieVector;
CookieVector globalCookies;



void  sendMessageToClient(const char* msg,bool js_inject)
{
   if(js_inject && executionTimeThread){
      cerr << "terminate execution timeout thread" << executionTimeThread << endl;
      pthread_cancel(executionTimeThread);
      executionTimeThread = 0;
   }
   std::string encoded = Base64::Encode(msg);
   cout <<  encoded << endl; 
}



void close_app(int s) {
	
	exit(0);
}

void * timeoutThreadHanlder(void* m){
  
  int * ptimeout = (int*)m;
  int timeout = *ptimeout;
  delete ptimeout;
  usleep(timeout*1000*1000);
  cerr << "-------stop loading -------" << endl;
  main_browser->StopLoad();
  sendMessageToClient("{\"action\":\"open\",\"result\":\"timeout\"}");
  return 0;
}

void * executionTimeoutThreadHandler(void* m){
  
  int * ptimeout = (int*)m;
  int timeout = *ptimeout;
  delete ptimeout;
  usleep(timeout*1000);
  if(pthread_self() != executionTimeThread || executionTimeThread == 0) {
    cerr << "this is not current exectuion thread " << executionTimeThread << endl;
    return 0;
  }
  
  //cerr << "-------stop execution: timeout ------- "  << timeout << " current exection thread is: " << executionTimeThread << " current thread id: " << pthread_self() << endl;
  //main_browser->StopLoad();
  sendMessageToClient("{\"action\":\"inject_javascript\",\"result\":null}");
  return 0;
}

std::string convertCookieToString(const CefCookie& cookie_read){
       const std::string& name = CefString(&cookie_read.name);
       const std::string& value = CefString(&cookie_read.value);
       const std::string& domain = CefString(&cookie_read.domain);
       const std::string& path = CefString(&cookie_read.path);
       const cef_time_t& t = cookie_read.expires;
       struct tm tm;
       tm.tm_mday = t.day_of_month;
       tm.tm_wday = t.day_of_week;
       tm.tm_hour = t.hour ;
       tm.tm_min = t.minute;
       tm.tm_sec = t.second;
       tm.tm_mon = t.month -1 ;
       tm.tm_year = t.year - 1900;
       tm.tm_isdst = -1;
       //cerr << t.year << "-" << t.month << "-" << t.day_of_month << endl;
       std::stringstream ss;
       char str[100] = {0};
       strftime(str,100,"%a, %d-%b-%Y %H:%M:%S",&tm);
       std::string timeStr = str;
       if(t.year == 0 && t.month == 0) timeStr = "Session";
       ss << name << "=" <<  value << "; expires=" << timeStr << "; domain=" << domain << "; path=" << path;
       return ss.str();
}

void initCookie(CefCookie& cookie,Object  obj){
       
       if(obj.has<String>("name"))
          CefString(&cookie.name).FromASCII(obj.get<String>("name").c_str());

       if(obj.has<String>("value"))
          CefString(&cookie.value).FromASCII(obj.get<String>("value").c_str());

       if(obj.has<String>("domain"))
          CefString(&cookie.domain).FromASCII(obj.get<String>("domain").c_str());

       if(obj.has<String>("path"))
          CefString(&cookie.path).FromASCII(obj.get<String>("path").c_str());   

       if(obj.has<String>("expires")){
          cookie.has_expires = true;
          struct tm tm;
          std::istringstream iss(obj.get<String>("expires"));
          iss >> std::get_time(&tm, "%a, %d-%b-%Y %H:%M:%S %Z");
          
          
          cef_time_t t;
          t.day_of_month = tm.tm_mday;
          t.day_of_week = tm.tm_wday;
          t.hour = tm.tm_hour;
          t.minute = tm.tm_min;
          t.second = tm.tm_sec;
          t.month = tm.tm_mon+1;
          t.year = tm.tm_year + 1900;
          cookie.expires = t;
       }       
   
}

void returnCookies(){
   Object obj;
   obj << "action" << "get_cookies";
   Array arr;	
   for(size_t i = 0 ; i < globalCookies.size() ; i++){
   	   Value v = globalCookies.at(i); 
       arr.append(v);
   }
   obj << "cookies" << arr;
   sendMessageToClient(obj.json().c_str());
}


void process_message(std::string& s){
   
   std::string message;
   Base64::Decode(s,message);
   istringstream input(message);
   Object o;
   bool success = o.parse(input);
   if(success){
      string type =  o.get<String>("action");
      if(type == "open"){
         
         //cerr << "------------ main browser on ipc --------------------"  << endl;
         //cerr << main_browser << endl;
         while(requestResults.size()){
            delete requestResults.at(0);
            requestResults.erase(requestResults.begin());
         }
      	 std::string url = o.get<String>("url");
      	 if(o.has<Array>("cookies")){
      	 	 CefRefPtr<CefCookieManager> manager =  CefCookieManager::GetGlobalManager(nullptr);
      	 	 Array cookies = o.get<Array>("cookies");
      	 	 for(size_t i = 0 ; i < cookies.size() ; i++){
                CefCookie cefCookie;
                Object cookie  = cookies.get<Object>(i);
                initCookie(cefCookie,cookie);
                bool result = manager->SetCookie(CefString(url),cefCookie,nullptr);
                cerr << "cookie set result " << result << endl;
                
      	 	 }
      	 }

         if(o.has<String>("preload")){
            preload_js_code = o.get<String>("preload");            
         } else preload_js_code.clear();
         is_freeze = false;
         main_browser->GetMainFrame()->LoadURL(url);
         int timeout = o.get<Number>("timeout");
         int* arg = new int[1];
         arg[0] = timeout;
         pthread_create(&timeThread, NULL, timeoutThreadHanlder, (void *)(arg));

      } 
      else if(type=="get_screenshot"){
          // Create the message object.
          CefRefPtr<CefProcessMessage> msg= CefProcessMessage::Create("get_screenshot");

          // Retrieve the argument list object.
          CefRefPtr<CefListValue> args = msg->GetArgumentList();

          // Populate the argument values.
          args->SetString(0, "get_screenshot");
          args->SetInt(0, 10);
          
          
          // Send the process message to the main frame in the render process.
          // Use PID_BROWSER instead when sending a message to the browser process.
          main_browser->GetMainFrame()->SendProcessMessage(PID_BROWSER, msg);          
          return;
         
      } 
      else if(type == "get_cookies"){
      	   CefRefPtr<CefCookieManager> manager =  CefCookieManager::GetGlobalManager(nullptr);
      	   class TestVisitor : public CefCookieVisitor {
  			     public:
  			      explicit TestVisitor() {}
  			      ~TestVisitor() override {
  			       
  			      }

  			      bool Visit(const CefCookie& cookie,
  			                 int count,
  			                 int total,
  			                 bool& deleteCookie) override {
                      globalCookies.push_back(convertCookieToString(cookie));
  			        if((size_t)total == globalCookies.size())
  			           returnCookies();
  			        return true;
  			      }
                   public:
                   	
  			     private:			      
  			      IMPLEMENT_REFCOUNTING(TestVisitor);
		       };
           globalCookies.clear();
      	   manager->VisitAllCookies(new TestVisitor());  
      }
      else if(type == "inject_javascript"){
         
         if(o.has<String>("content")){
            std::string url = main_browser->GetMainFrame()->GetURL();
            std::string content = o.get<String>("content");  
            
            /// start execution timeout process thread
            int timeout;
            if(o.has<Number>("timeout"))
              timeout = o.get<Number>("timeout");
            else timeout = 1000;                
            
            int* arg = new int[1];
            arg[0] = timeout;
            pthread_create(&executionTimeThread, NULL, executionTimeoutThreadHandler , (void *)(arg));
            size_t index = 0;
            execution_id = rand()%10000;
            while (true) {
                 /* Locate the substring to replace. */
                 index = content.find("cef_browser_callback(", index);
                 if (index == std::string::npos) break;
                 content.replace(index,strlen("cef_browser_callback("), "cef_browser_callback(" + std::to_string(execution_id) + ",");

                 /* Advance index forward so the next iteration doesn't pick it up as well. */
                 index += strlen("cef_browser_callback(");
            }
            
            main_browser->GetMainFrame()->ExecuteJavaScript("window.execution_id="+std::to_string(execution_id),url, 0);
            main_browser->GetMainFrame()->ExecuteJavaScript(content,url, 0);
            
            
         } else {
            sendMessageToClient("{\"type\":\"inject_javascript\",\"result\":\"there are no injection code\"}");
         }       
         
      }
      else if(type == "freeze"){
         is_freeze = true;
         sendMessageToClient("{\"action\":\"freeze\",\"result\":\"ok\"}");
      }
      else if(type == "get_requests"){
         Object object;
         Array arr;
         for(int t = 0 ; t < (int)requestResults.size() ; t++ ){
            arr << *(requestResults.at(t));
         }
         Object result;
         result << "data" << arr;
         object << "action" << "get_requests";
         object <<  "result" << result;
         sendMessageToClient(object.json().c_str()); 
         while(requestResults.size()){
            delete requestResults.at(0);
            requestResults.erase(requestResults.begin());
         }
      }
      else if(type=="close"){
        sendMessageToClient("{\"type\":\"close\",\"result\":\"closed\"}");
        exit(0);
      }
   }
   else{
      sendMessageToClient("{\"type\":\"json-error\",\"result\":\"invalid json\"}");
   }  
   
}

void * received(void * m)
{
	while(1)
	{
        std::string message;
        cin >> message;
        if(message.size())
        process_message(message);            
		usleep(10);
	}
	return 0;
}
bool runIPCServer(){
  std::signal(SIGINT, close_app);
  pthread_t msg;
  if( pthread_create(&msg, NULL, received, (void *)0) == 0)
   return true;
  else
    return false; 

}