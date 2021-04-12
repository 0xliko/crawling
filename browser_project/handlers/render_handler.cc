#include "../browser_handler.h"
#include "../global.h"
#include <sys/stat.h>
#include <png.h>


bool write_png_for_image(unsigned char* buffer, int width, int height,const char *filename) 
{
        
        
        FILE *fp;
        png_structp png_ptr;
        png_infop png_info_ptr;
        png_bytep png_row;

    
        fp = fopen(filename, "wb");
        if (fp == NULL) {
          fprintf(stderr, "Could not open file for writing\n");
          
          return false;
        }

        // Initialize write structure
        png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
        if (png_ptr == NULL) {
          fprintf(stderr, "Could not allocate write struct\n");
          
          return false;
        }

        // Initialize info structure
        png_info_ptr = png_create_info_struct(png_ptr);
        if (png_info_ptr == NULL) {
          fprintf(stderr, "Could not allocate info struct\n");
          
          return false;
        }

        // Setup Exception handling
        if (setjmp(png_jmpbuf (png_ptr))) {
          fprintf(stderr, "Error during png creation\n");
          
          return false;
        }

        png_init_io(png_ptr, fp);

        // Write header (8 bit colour depth)
        png_set_IHDR(png_ptr, png_info_ptr, width, height, 8, PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

      
        png_write_info(png_ptr, png_info_ptr);

        // Allocate memory for one row (4 bytes per pixel - RGBA)
        png_row = (png_bytep) malloc(4 * width * sizeof(png_byte));

        for (int y = 0; y < height; y++) {
          for (int x = 0; x < width; x++) {

            unsigned char blue =  buffer[width * y * 4 + x*4];
            unsigned char green = buffer[width * y * 4 + x*4 +1];
            unsigned char red =   buffer[width * y * 4 + x*4 + 2];
            unsigned char alpha = buffer[width * y * 4 + x*4 + 3];

            png_byte *ptr = &(png_row[x * 4]);
            ptr[0] = red;
            ptr[1] = green;
            ptr[2] = blue;
            ptr[3] = alpha;
          }
          png_write_row(png_ptr, png_row);
        }
        png_write_end(png_ptr, NULL);

       fclose(fp);
        if (png_info_ptr != NULL)
          png_free_data(png_ptr, png_info_ptr, PNG_FREE_ALL, -1);
        if (png_ptr != NULL)
          png_destroy_write_struct(&png_ptr, (png_infopp) NULL);
        if (png_row != NULL)
          free(png_row);
       return true;   

}

///// for render handler /////////////

bool BrowserHandler::GetRootScreenRect(CefRefPtr<CefBrowser> browser,
                                         CefRect& rect) 
{
  //cerr << "render handler get root screen rect" << endl;
  CEF_REQUIRE_UI_THREAD();
  return false;
  
}

bool BrowserHandler::GetScreenPoint(CefRefPtr<CefBrowser> browser,
                                      int viewX,
                                      int viewY,
                                      int& screenX,
                                      int& screenY) 
{
  //cerr << "render handler get screen point" << endl;
  CEF_REQUIRE_UI_THREAD();
  return false;
  
}

bool BrowserHandler::GetScreenInfo(CefRefPtr<CefBrowser> browser,
                                     CefScreenInfo& screen_info) 
{
  //cerr << "render handler get screen info" << endl;
  CEF_REQUIRE_UI_THREAD();
  return false;
  
}

void BrowserHandler::OnPopupShow(CefRefPtr<CefBrowser> browser, bool show) {
  //cerr << "render handler on popup show" << endl;
  CEF_REQUIRE_UI_THREAD();
  
}

void BrowserHandler::OnPopupSize(CefRefPtr<CefBrowser> browser,
                                   const CefRect& rect) {
  //cerr << "render handler on popup size" << endl;
  CEF_REQUIRE_UI_THREAD();
  
}


void BrowserHandler::GetViewRect(CefRefPtr<CefBrowser> browser,
                                   CefRect& rect) 
{

  
  CEF_REQUIRE_UI_THREAD();
  if(view_content_height > 0 )
      rect.height = view_content_height;
  else
     rect.height = view_port_height;
    
  rect.width = view_port_width;
   
   
}


void BrowserHandler::OnPaint(CefRefPtr<CefBrowser> browser,
                               PaintElementType type,
                               const RectList& dirtyRects,
                               const void* buffer,
                               int width,
                               int height) {
  
  CEF_REQUIRE_UI_THREAD();
  if(type == PET_VIEW && requireCapture){
     srand(time(NULL));
     mkdir(std::string(work_path + "/screenshot/").c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
     string filename = work_path + "/screenshot/screenshot_"   + std::to_string(rand()) + std::string(".png");
     bool ret = write_png_for_image((unsigned char*)buffer,width,height,filename.c_str());
     if(ret){
       string msg = "{\"action\":\"getScreenshot\",\"result\":\"" + filename + "\"}";
       sendMessageToClient(msg.c_str());
     }
     else
       sendMessageToClient("{\"action\":\"getScreenshot\",\"result\":\"fail\"}");
     requireCapture = false;
     view_content_height = view_port_height;
     browser->GetHost()->NotifyScreenInfoChanged();
  } 
}

void BrowserHandler::OnAcceleratedPaint(
    CefRefPtr<CefBrowser> browser,
    CefRenderHandler::PaintElementType type,
    const CefRenderHandler::RectList& dirtyRects,
    void* share_handle) {
  //cerr << "accelerated render handler" << endl;
  CEF_REQUIRE_UI_THREAD();
  
}

/*void BrowserHandler::OnCursorChange(CefRefPtr<CefBrowser> browser,
                                      CefCursorHandle cursor,
                                      CursorType type,
                                      const CefCursorInfo& custom_cursor_info) {
  //cerr << "render handler on curor change" << endl;
  CEF_REQUIRE_UI_THREAD();
  
}*/

bool BrowserHandler::StartDragging(
    CefRefPtr<CefBrowser> browser,
    CefRefPtr<CefDragData> drag_data,
    CefRenderHandler::DragOperationsMask allowed_ops,
    int x,
    int y) {
  //cerr << "render handler start dragging" << endl;
  CEF_REQUIRE_UI_THREAD();
  return false;
  
}

void BrowserHandler::UpdateDragCursor(
    CefRefPtr<CefBrowser> browser,
    CefRenderHandler::DragOperation operation) {
  //cerr << "render handler update drage cursor" << endl;
  CEF_REQUIRE_UI_THREAD();
  
}

void BrowserHandler::OnImeCompositionRangeChanged(
    CefRefPtr<CefBrowser> browser,
    const CefRange& selection_range,
    const CefRenderHandler::RectList& character_bounds) {
  //cerr << "render handler ime OnImeCompositionRangeChanged" << endl;
  CEF_REQUIRE_UI_THREAD();
  
}