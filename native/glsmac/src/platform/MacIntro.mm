#import <Cocoa/Cocoa.h>
#import <AVKit/AVKit.h>
#import <AVFoundation/AVFoundation.h>
#include "MacIntro.h"
#include <cstdio>

@interface GLSMACMovieDelegate : NSObject <NSWindowDelegate>
@end
@implementation GLSMACMovieDelegate
- (BOOL)windowShouldClose:(NSWindow*)sender {
    [NSApp stopModal];
    return NO;
}
@end

namespace platform {
bool PlayMacIntro( const std::string& path ) {
    @autoreleasepool {
        NSString* filename = [NSString stringWithUTF8String:path.c_str()];
        if ( ![[NSFileManager defaultManager] fileExistsAtPath:filename] ) {
            std::fprintf( stderr, "Intro movie missing: %s\n", path.c_str() );
            return false;
        }
        NSWindow* previous = NSApp.keyWindow;
        NSWindow* window = [[NSWindow alloc]
            initWithContentRect:NSMakeRect( 0, 0, 1000, 560 )
            styleMask:NSWindowStyleMaskTitled | NSWindowStyleMaskClosable | NSWindowStyleMaskResizable
            backing:NSBackingStoreBuffered defer:NO];
        window.title = @"Alien Crossfire — Opening Movie";
        window.releasedWhenClosed = NO;
        GLSMACMovieDelegate* delegate = [GLSMACMovieDelegate new];
        window.delegate = delegate;
        AVPlayer* player = [AVPlayer playerWithURL:[NSURL fileURLWithPath:filename]];
        AVPlayerView* view = [[AVPlayerView alloc] initWithFrame:window.contentView.bounds];
        view.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
        view.controlsStyle = AVPlayerViewControlsStyleFloating;
        view.videoGravity = AVLayerVideoGravityResizeAspect;
        view.player = player;
        window.contentView = view;
        __block bool failed = false;
        __block bool started = false;
        __block bool finished = false;
        NSNotificationCenter* center = NSNotificationCenter.defaultCenter;
        id end = [center addObserverForName:AVPlayerItemDidPlayToEndTimeNotification
            object:player.currentItem queue:NSOperationQueue.mainQueue usingBlock:^(NSNotification*) {
                finished = true;
                [NSApp stopModal];
            }];
        id error = [center addObserverForName:AVPlayerItemFailedToPlayToEndTimeNotification
            object:player.currentItem queue:NSOperationQueue.mainQueue usingBlock:^(NSNotification*) {
                failed = true;
                [NSApp stopModal];
            }];
        id keys = [NSEvent addLocalMonitorForEventsMatchingMask:NSEventMaskKeyDown handler:^NSEvent*(NSEvent* event) {
            if ( event.keyCode == 53 ) {
                [NSApp stopModal];
                return nil;
            }
            return event;
        }];
        NSDate* began = NSDate.date;
        NSTimer* timer = [NSTimer timerWithTimeInterval:0.25 repeats:YES block:^(NSTimer*) {
            double seconds = CMTimeGetSeconds( player.currentTime );
            if ( !started && seconds > 0 ) {
                started = true;
                std::fprintf( stderr, "Native intro playback started\n" );
            }
            if ( player.currentItem.status == AVPlayerItemStatusFailed
                || ( !started && -began.timeIntervalSinceNow > 20 ) ) {
                failed = true;
                [NSApp stopModal];
            }
        }];
        [NSRunLoop.mainRunLoop addTimer:timer forMode:NSModalPanelRunLoopMode];
        [window center];
        [window makeKeyAndOrderFront:nil];
        [NSApp activateIgnoringOtherApps:YES];
        [player play];
        [NSApp runModalForWindow:window];
        [timer invalidate];
        [player pause];
        [center removeObserver:end];
        [center removeObserver:error];
        [NSEvent removeMonitor:keys];
        [window orderOut:nil];
        window.delegate = nil;
        [window close];
        [previous makeKeyAndOrderFront:nil];
        std::fprintf( stderr, "Native intro playback %s\n", failed ? "failed" : finished ? "completed" : "skipped" );
        return !failed;
    }
}
}
