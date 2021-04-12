#include "../browser_handler.h"
#include "../global.h"

void BrowserHandler::OnTitleChange(CefRefPtr<CefBrowser> browser,
                                  const CefString& title) {
    CEF_REQUIRE_UI_THREAD();

}