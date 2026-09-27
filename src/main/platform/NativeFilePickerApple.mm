#ifdef __APPLE__
#include "NativeFilePicker.hpp"
#include <disk/ImageFileDevice.hpp>
#import <Foundation/Foundation.h>
#import <TargetConditionals.h>
#if TARGET_OS_IPHONE
#import <UIKit/UIKit.h>
#else
#import <AppKit/AppKit.h>
#endif
using namespace mpc::platform;

namespace
{
    FileSelection selection(NSURL *url)
    {
        const BOOL scoped = [url startAccessingSecurityScopedResource];
        NSError *error = nil;
#if TARGET_OS_IPHONE
        const auto options = NSURLBookmarkCreationOptions(0);
#else
        const auto options = NSURLBookmarkCreationWithSecurityScope;
#endif
        NSData *bookmark = [url bookmarkDataWithOptions:options
                         includingResourceValuesForKeys:nil
                                          relativeToURL:nil
                                                  error:&error];
        if (scoped)
        {
            [url stopAccessingSecurityScopedResource];
        }
#if TARGET_OS_IPHONE
        if (!bookmark)
        {
            return {{}, "Unable to retain image access"};
        }
#endif
        return {selectedImage(std::string([[url path] UTF8String]), {},
                              bookmark ? std::string([[bookmark
                                             base64EncodedStringWithOptions:0]
                                             UTF8String])
                                       : ""),
                {}};
    }
} // namespace
#if TARGET_OS_IPHONE
@interface MpcImagePickerDelegate : NSObject <UIDocumentPickerDelegate>
{
@public
    std::shared_ptr<PickerRequest> request;
}
@end
@implementation MpcImagePickerDelegate
- (void)documentPicker:(UIDocumentPickerViewController *)controller
    didPickDocumentsAtURLs:(NSArray<NSURL *> *)urls
{
    request->finish(urls.count ? selection(urls.firstObject) : FileSelection{});
}
- (void)documentPickerWasCancelled:(UIDocumentPickerViewController *)controller
{
    request->finish({});
}
@end
#endif
void mpc::platform::showNativeFilePicker(std::shared_ptr<PickerRequest> request,
                                         void *parent, const std::string &)
{
    dispatch_async(dispatch_get_main_queue(), ^{
      if (request->cancelled)
      {
          return;
      }
#if TARGET_OS_IPHONE
      if (!parent)
      {
          request->finish({{}, "Image picker needs a presenting view"});
          return;
      }
      UIResponder *responder = (UIView *)parent;
      while (responder && ![responder isKindOfClass:[UIViewController class]])
          responder = responder.nextResponder;
      UIViewController *presenter = (UIViewController *)responder;
      if (!presenter || !presenter.view.window)
      {
          request->finish({{}, "Image picker has no active view"});
          return;
      }
      while (presenter.presentedViewController)
          presenter = presenter.presentedViewController;
      UIDocumentPickerViewController *panel =
          [[UIDocumentPickerViewController alloc]
              initWithDocumentTypes:@[ @"public.data" ]
                             inMode:UIDocumentPickerModeOpen];
      MpcImagePickerDelegate *delegate = [[MpcImagePickerDelegate alloc] init];
      delegate->request = request;
      panel.delegate = delegate;
      auto keepPanel = std::shared_ptr<void>((void *)panel,
                                             [](void *p)
                                             {
                                                 [(id)p release];
                                             });
      auto keepDelegate = std::shared_ptr<void>((void *)delegate,
                                                [](void *p)
                                                {
                                                    [(id)p release];
                                                });
      request->setDismiss(
          [keepPanel, keepDelegate]
          {
              dispatch_async(dispatch_get_main_queue(), ^{
                [(UIViewController *)keepPanel.get()
                    dismissViewControllerAnimated:YES
                                       completion:nil];
              });
          });
      [presenter presentViewController:panel animated:YES completion:nil];
#else
        NSOpenPanel *panel = [NSOpenPanel openPanel];
        panel.canChooseDirectories = NO;
        panel.canChooseFiles = YES;
        panel.allowsMultipleSelection = NO;
        panel.title = @"Select a FAT16 disk image";
        [panel retain];
        auto keepPanel = std::shared_ptr<void>((void *)panel, [](void *p){ [(id)p release]; });
        request->setDismiss([keepPanel] {
            dispatch_async(dispatch_get_main_queue(), ^{ [(NSOpenPanel *)keepPanel.get() cancel:nil]; });
        });
        auto completion = ^(NSModalResponse result) {
            if (result == NSModalResponseOK && panel.URL) request->finish(selection(panel.URL));
            else request->finish({});
        };
        if (parent) [panel beginSheetModalForWindow:(NSWindow *)parent completionHandler:completion];
        else [panel beginWithCompletionHandler:completion];
#endif
    });
}
std::shared_ptr<akaifat::BlockDevice>
mpc::platform::openNativeImage(const disk::Volume &v, bool ro)
{
    @autoreleasepool
    {
        if (v.diskImageAccessToken.empty())
        {
            return disk::ImageFileDevice::open(v.diskImagePath, ro);
        }
        NSData *data = [[NSData alloc]
            initWithBase64EncodedString:
                [NSString stringWithUTF8String:v.diskImageAccessToken.c_str()]
                                options:0];
        NSError *error = nil;
        BOOL stale = NO;
#if TARGET_OS_IPHONE
        const auto options = NSURLBookmarkResolutionOptions(0);
#else
        const auto options = NSURLBookmarkResolutionWithSecurityScope |
                             NSURLBookmarkResolutionWithoutUI;
#endif
        NSURL *url = data ? [NSURL URLByResolvingBookmarkData:data
                                                      options:options
                                                relativeToURL:nil
                                          bookmarkDataIsStale:&stale
                                                        error:&error]
                          : nil;
        [data release];
        if (!url || error)
        {
            throw std::runtime_error(
                "Image access expired; replace the binding");
        }
        const BOOL scoped = [url startAccessingSecurityScopedResource];
        [url retain];
        auto lease = std::shared_ptr<void>(
            (void *)url,
            [scoped](void *p)
            {
                NSURL *resource = (NSURL *)p;
                if (scoped)
                {
                    [resource stopAccessingSecurityScopedResource];
                }
                [resource release];
            });
        return disk::ImageFileDevice::open(std::string([[url path] UTF8String]),
                                           ro, std::move(lease));
    }
}
#endif
