#include <android/log.h>

#include <android_native_app_glue.h>

#define AndroidLogInfo(...) ((void)__android_log_print(ANDROID_LOG_INFO, "demo-app", __VA_ARGS__))
#define AndroidLogWarn(...) ((void)__android_log_print(ANDROID_LOG_WARN, "demo-app", __VA_ARGS__))

void
android_main(struct android_app *AppState) {
    // Check if app-glue is linked with the app. (MUST be, bc .a-linked)
    app_dummy();

    while(1) {
        int Ident;

    }
}