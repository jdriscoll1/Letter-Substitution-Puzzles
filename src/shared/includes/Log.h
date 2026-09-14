#ifndef FLWG_LOG_H
#define FLWG_LOG_H

/* Where the engine talks.
 *
 * It talks through printf, which is right where it was written: the standalone
 * build runs in a terminal and that is where a terminal puts things. On
 * Android stdout goes to /dev/null, so on the one platform the game ships on,
 * everything the engine said went nowhere - a board failing to deal, a search
 * giving up, a hint finding nothing. The only native output anybody ever saw
 * was a tombstone.
 *
 * FLWG_LOG is printf on a desktop and a logcat line on a phone, under the tag
 * FLWGNative. So `adb logcat -s ReactNativeJS:I FLWGNative:I` is the whole
 * game, both sides of the bridge, in the order it happened.
 *
 * The terminal programs - main.c and the debug console - keep printf. They are
 * a user interface made of text, not an engine reporting on itself, and they
 * do not run on a phone at all.
 */

#ifdef __ANDROID__

#include <android/log.h>
#define FLWG_LOG(...) __android_log_print(ANDROID_LOG_INFO, "FLWGNative", __VA_ARGS__)

#else

#include <stdio.h>
#define FLWG_LOG(...) printf(__VA_ARGS__)

#endif

#endif
