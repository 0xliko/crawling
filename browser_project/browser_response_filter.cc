#include "browser_response_filter.h"
#include <sstream>
#include <fstream>
#include <string>
#include <sys/stat.h>
#include "global.h"

CefResponseFilter::FilterStatus BrowserResponseFilter::Filter( void* data_in, size_t data_in_size, size_t& data_in_read, void* data_out, size_t data_out_size, size_t& data_out_written ) {
   
   if(data_in_size == 0) return RESPONSE_FILTER_DONE;

     size_t count = data_in_size;
   if(data_in_size >  data_out_size)
       count = data_out_size;
    length +=  count;
    
    data_out_written =  count;
    data_in_read =  count;
    
    memcpy(data_out,data_in,count);
    //content = realloc(content,length);
    //memcpy((void*)((char*)content+length-count),data_in,count);
    unsigned char* pData = new unsigned char[count];
    memcpy((void*)pData,data_in,count);
    contents.push_back(pData);
    lengths.push_back(count);
    if(count != data_in_size){
        return RESPONSE_FILTER_NEED_MORE_DATA;
    }
    return RESPONSE_FILTER_DONE;
}
bool BrowserResponseFilter::InitFilter(){
     length = 0;
     return true;
}
BrowserResponseFilter::~BrowserResponseFilter(){
  

   Object *obj = new Object();
   std::string url = std::string(request_->GetURL()); 
   *obj << "src" << url;
   if(frame_->IsMain()) *obj << "iframeid" << "null";
   else *obj << "iframeid" << frame_->GetIdentifier();
   *obj << "length" << length;
   std::string mimeType = std::string(response_->GetMimeType());
   *obj << "mime" << mimeType;
   struct timeval tv;
   gettimeofday(&tv,NULL);
   unsigned long long  millisecondsSinceEpoch = 
     (unsigned long long) (tv.tv_sec) * 1000 + (unsigned long long )(tv.tv_usec)/1000;
   *obj << "load_time" << millisecondsSinceEpoch;
   unsigned char* pData = new unsigned char[length];
  while(contents.size()){
      unsigned char* chunk =  contents.at(0);
      int count = lengths.at(0);
      memcpy(pData,chunk,count);
      pData += count;
      delete chunk;
      contents.erase(contents.begin());
      lengths.erase(lengths.begin());
  }
  pData =pData-length;
  size_t pos = url.find_last_of(".");
  std::string ext;
  
  if(string::npos == pos) ext = "";
  else ext = url.substr(pos);
  
  srand(time(NULL));
  
  mkdir(std::string(work_path + "/dump/").c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
  string filename = work_path + "/dump/resource_" + std::to_string(rand()) + ext;
  ofstream file(filename.c_str(),ios::out | ios::binary);
  if(file){
     file.write((char*)pData,length);
     file.close();
     delete pData;
     *obj << "content" << filename;
  } else {
     *obj << "content" << "FILE_WRITE_ERROR"; }
  requestResults.push_back(obj);

}