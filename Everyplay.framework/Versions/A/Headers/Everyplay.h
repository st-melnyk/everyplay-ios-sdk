/*
 * Copyright 2012 Applifier
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#import "EveryplayFaceCam.h"
#import "EveryplayCapture.h"
#import "EveryplayAccount.h"
#import "EveryplayRequest.h"
#import "EveryplaySoundEngine.h"

#pragma mark - Developer metadata
static NSString * const kEveryplayMetadataScoreInteger = @"score";
static NSString * const kEveryplayMetadataLevelInteger = @"level";
static NSString * const kEveryplayMetadataLevelNameString = @"level_name";

#pragma mark - View controller flow settings

typedef enum  {
    EveryplayFlowReturnsToGame         = 1 << 0,    // Default
    EveryplayFlowReturnsToVideoPlayer  = 1 << 1
} EveryplayFlowDefs;

#pragma mark - Error codes
static NSString * const kEveryplayErrorDomain = @"com.everyplay";

static const int kEveryplayLoginCanceledError = 100;
static const int kEveryplayMovieExportCanceledError = 101;
static const int kEveryplayFileUploadError = 102;

#pragma mark - Notifications

extern NSString * const EveryplayAccountDidChangeNotification;
extern NSString * const EveryplayDidFailToRequestAccessNotification;

#pragma mark - Handler

typedef void(^EveryplayAccessRequestCompletionHandler)(NSError *error);
typedef void(^EveryplayPreparedAuthorizationURLHandler)(NSURL *preparedURL);
typedef void(^EveryplayDataLoadingHandler)(NSError *error, id data);

#pragma mark - Compile-time options

@interface EveryplayFeatures : NSObject
/*
 * Is running on iOS 5 or later?
 * Useful for disabling functionality on older devices.
 */
+ (BOOL) isSupported;

/*
 * Returns the number of CPU cores on device.
 * Useful for disabling functionality on older devices.
 */
+ (NSUInteger) numCores;

/*
 * To disable Everyplay OpenAL implementation, override this class
 * method to return NO.
 */
+ (BOOL) supportsOpenAL;

/*
 * CocosDenshion background music support currently lacks hardware
 * decoder support. To disable recording support for background music,
 * override this class method to return NO.
 */
+ (BOOL) supportsCocosDenshion;

/*
 * AVFoundation AVAudioPlayer support currently lacks hardware
 * decoder support. To disable recording support for background music,
 * override this class method to return NO.
 */
+ (BOOL) supportsAVFoundation;
@end

#pragma mark -

@class EveryplayAccount;

@class EveryplayVideoPlayerViewController;

@protocol EveryplayDelegate <NSObject>
- (void)everyplayShown;
- (void)everyplayHidden;
@optional
- (void)everyplayReadyForRecording:(NSNumber *)enabled;
- (void)everyplayRecordingStarted;
- (void)everyplayRecordingStopped;

- (void)everyplayFaceCamSessionStarted;
- (void)everyplayFaceCamSessionStopped;

- (void)everyplayUploadDidStart:(NSNumber *)videoId;
- (void)everyplayUploadDidProgress:(NSNumber *)videoId progress:(NSNumber *)progress;
- (void)everyplayUploadDidComplete:(NSNumber *)videoId;

- (void)everyplayThumbnailReadyAtFilePath:(NSString *)thumbnailFilePath;
- (void)everyplayThumbnailReadyAtURL:(NSURL *)thumbnailUrl;

- (void)everyplayThumbnailReadyAtTextureId:(NSNumber *)textureId portraitMode:(NSNumber *)portrait;
@end

@interface Everyplay : NSObject

#pragma mark - Properties
@property (nonatomic, unsafe_unretained) EveryplayCapture *capture;
@property (nonatomic, strong) EveryplayFaceCam *faceCam;
@property (nonatomic, strong) UIViewController *parentViewController;
@property (nonatomic, strong) id <EveryplayDelegate> everyplayDelegate;
@property (nonatomic, assign) EveryplayFlowDefs flowControl;

#pragma mark - Singleton
+ (Everyplay *)sharedInstance;

+ (BOOL)isSupported;

+ (Everyplay *)initWithDelegate:(id <EveryplayDelegate>)everyplayDelegate;
+ (Everyplay *)initWithDelegate:(id <EveryplayDelegate>)everyplayDelegate andParentViewController:(UIViewController *)viewController;
+ (Everyplay *)initWithDelegate:(id <EveryplayDelegate>)everyplayDelegate andAddRootViewControllerForView:(UIView *)view;

#pragma mark - Public Methods
- (void)showEveryplay;
- (void)showEveryplayWithPath:(NSString *)path;
- (void)showEveryplaySharingModal;
- (void)hideEveryplay;
- (void)playLastRecording;

- (void)mergeSessionDeveloperData:(NSDictionary *)dictionary;

#pragma mark - Video playback
- (void)playVideoWithURL:(NSURL *)videoURL;
- (void)playVideoWithDictionary:(NSDictionary *)videoDictionary;

#pragma mark - Manage Accounts
+ (EveryplayAccount *)account;

+ (void)requestAccessWithCompletionHandler:(EveryplayAccessRequestCompletionHandler)aCompletionHandler;
+ (void)requestAccessforScopes:(NSString *)scopes
         withCompletionHandler:(EveryplayAccessRequestCompletionHandler)aCompletionHandler;
+ (void)removeAccess;

#pragma mark - Configuration
+ (void)setClientId:(NSString *)client
       clientSecret:(NSString *)secret
        redirectURI:(NSString *)url;

#pragma mark - OAuth2 Flow
+ (BOOL)handleRedirectURL:(NSURL *)URL;

@end


#pragma mark - Macros

#define EVERYPLAY_CANCELED(error) ([error.domain isEqualToString:(NSString *)kEveryplayErrorDomain] && error.code == kEveryplayLoginCanceledError)
