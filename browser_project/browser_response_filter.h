//#include "include/cef_client.h"

#include <list>
#include <iostream>
#include "include/cef_response_filter.h"
#include "include/cef_client.h"
using namespace std;

class BrowserResponseFilter : public CefResponseFilter {
  public:
   BrowserResponseFilter(CefRefPtr<CefRequest> request, CefRefPtr<CefResponse> response,CefRefPtr<CefFrame> frame){
      request_ = request;
      response_ = response;
      frame_ = frame;
   } 
   ~BrowserResponseFilter();
   virtual CefResponseFilter::FilterStatus Filter( void* data_in, size_t data_in_size, size_t& data_in_read, void* data_out, size_t data_out_size, size_t& data_out_written ) OVERRIDE;
   virtual bool InitFilter() OVERRIDE;
   CefRefPtr<CefRequest> request_;
   CefRefPtr<CefResponse> response_;
   CefRefPtr<CefFrame> frame_;
   std::vector<unsigned char*> contents;
   std::vector<int>  lengths;
   int length;
   IMPLEMENT_REFCOUNTING(BrowserResponseFilter);
};
