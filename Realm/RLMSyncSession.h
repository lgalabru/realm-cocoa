////////////////////////////////////////////////////////////////////////////
//
// Copyright 2016 Realm Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
////////////////////////////////////////////////////////////////////////////

#import <Foundation/Foundation.h>

/**
 The current state of a sync session object.
 */
typedef NS_ENUM(NSUInteger, RLMSyncSessionState) {
    /// The sync session is bound to the Realm Object Server and communicating with it.
    RLMSyncSessionStateActive,
    /// The sync session is not currently communicating with the Realm Object Server.
    RLMSyncSessionStateInactive,
    /// The sync session encountered an error and is invalid; it should be discarded.
    RLMSyncSessionStateInvalid
};

/**
 The type of data transfer that a particular sync session notifier block will track.
 */
typedef NS_ENUM(NSUInteger, RLMSyncNotifierDirection) {
    /// For monitoring upload progress.
    RLMSyncNotifierDirectionUpload,
    /// For monitoring download progress.
    RLMSyncNotifierDirectionDownload,
};

/**
 The desired behavior of a particular sync session notifier block.
 */
typedef NS_ENUM(NSUInteger, RLMSyncNotifierMode) {
    /// The block will be called forever, or until the user unregisters it.
    /// It will always report the latest number of transferred bytes, and the
    /// latest number of total bytes to be transferred.
    RLMSyncNotifierModeAlwaysReportLatest,
    /// The block will, upon registration, store the total number of bytes
    /// to be transferred. When invoked, it will always report the latest number
    /// of transferred bytes out of that original number of bytes to be
    /// transferred. When the number of transferred bytes reaches or exceeds the
    /// number of bytes to be transferred, the block will be unregistered.
    RLMSyncNotifierModeProgressIndicator,
};

@class RLMSyncUser, RLMSyncConfiguration;

/**
 The type of a block intended for reporting a session's network activity to the user.

 `transferred_bytes` refers to the number of bytes that have been uploaded or downloaded.

 `transferrable_bytes` refers to the number of total bytes to be uploaded or downloaded.
 */
typedef void(^RLMProgressNotificationBlock)(NSUInteger transferred_bytes, NSUInteger transferrable_bytes);

NS_ASSUME_NONNULL_BEGIN

/**
 A token object corresponding to a progress notifier block on a `RLMSyncSession`. To stop notifications manually,
 destroy the token or call `-stop` on it.
 */
@interface RLMProgressNotificationToken : NSObject

/// Stop the notifier associated with this token. If the notifier is no longer active, this method does nothing.
- (void)stop;

@end

/**
 An object encapsulating a Realm Object Server "session". Sessions represent the communication between the client (and a
 local Realm file on disk), and the server (and a remote Realm at a given URL stored on a Realm Object Server).
 
 Sessions are always created by the SDK and vended out through various APIs. The lifespans of sessions associated with
 Realms are managed automatically.
 */
@interface RLMSyncSession : NSObject

/// The session's current state.
@property (nonatomic, readonly) RLMSyncSessionState state;

/// The Realm Object Server URL of the remote Realm this session corresponds to.
@property (nullable, nonatomic, readonly) NSURL *realmURL;

/// The user that owns this session.
- (nullable RLMSyncUser *)parentUser;

/// If the session is valid, return a sync configuration that can be used to open the Realm associated with this
/// session.
- (nullable RLMSyncConfiguration *)configuration;

/**
 Register a progress notification block. Multiple blocks can be registered on the same session at once. All blocks
 will be dispatched to the main queue.

 The token returned by this method must be retained as long as progress notifications are desired, and the `-stop`
 method should be called on it when notifications are no longer needed.

 If no token is returned, the session was not in a state where it could accept progress notifiers, or the notifier was
 not a streaming notifier, was called immediately, and will not be called again since there is no additional progress
 to report.

 @see: `RLMSyncNotifierDirection`, `RLMSyncNotifierMode`
 */
- (nullable RLMProgressNotificationToken *)addProgressNotificationBlock:(RLMProgressNotificationBlock)block
                                                              direction:(RLMSyncNotifierDirection)direction
                                                                   mode:(RLMSyncNotifierMode)mode NS_REFINED_FOR_SWIFT;

/**
 Register a progress notification block, and specify the queue upon which the block should be dispatched.

 @see: `addProgressNotificationBlock:direction:mode:`
 */
- (nullable RLMProgressNotificationToken *)addProgressNotificationBlock:(RLMProgressNotificationBlock)block
                                                              direction:(RLMSyncNotifierDirection)direction
                                                                   mode:(RLMSyncNotifierMode)mode
                                                                  queue:(dispatch_queue_t)queue NS_REFINED_FOR_SWIFT;
@end

NS_ASSUME_NONNULL_END
