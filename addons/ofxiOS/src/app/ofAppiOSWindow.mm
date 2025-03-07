/***********************************************************************
 
 Copyright (c) 2008, 2009, Memo Akten, www.memo.tv
 *** The Mega Super Awesome Visuals Company ***
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of MSA Visuals nor the names of its contributors 
 *       may be used to endorse or promote products derived from this software
 *       without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS 
 * OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE. 
 *
 * ***********************************************************************/ 

#import "ofMain.h"
#import "ofGLProgrammableRenderer.h"
#import "ofAppiOSWindow.h"
#import "ofxiOSEAGLView.h"
#import "ofxiOSAppDelegate.h"
#import "ofxiOSViewController.h"
#import "ofxiOSExtras.h"

//----------------------------------------------------------------------------------- instance.
static ofAppiOSWindow * _instance = NULL;
ofAppiOSWindow * ofAppiOSWindow::getInstance() {
	return _instance;
}

//----------------------------------------------------------------------------------- constructor / destructor.
ofAppiOSWindow::ofAppiOSWindow() {
	if(_instance == NULL) {
        _instance = this;
    } else {
        ofLogError("ofAppiOSWindow") << "instanciated more than once";
    }

    windowMode = OF_FULLSCREEN;
    
	bEnableSetupScreen = true;
//    orientation = OF_ORIENTATION_DEFAULT;
    orientation = OF_ORIENTATION_UNKNOWN;
    
    bHardwareOrientation = false;
    bOrientationIsAnimated = false;

	bRetinaEnabled = false;
    bRetinaSupportedOnDevice = false;
    bRetinaSupportedOnDeviceChecked = false;
	bDepthEnabled = false;
	bAntiAliasingEnabled = false;
    antiAliasingSamples = 0;
}

ofAppiOSWindow::~ofAppiOSWindow() {
    //
}

//----------------------------------------------------------------------------------- opengl setup.
void ofAppiOSWindow::setupOpenGL(int w, int h, ofWindowMode screenMode) {
	windowMode = screenMode; // use this as flag for displaying status bar or not
}

void ofAppiOSWindow::initializeWindow() {
    //
}

void ofAppiOSWindow::runAppViaInfiniteLoop(ofBaseApp * appPtr) {
    startAppWithDelegate("ofxiOSAppDelegate");
}

void ofAppiOSWindow::startAppWithDelegate(string appDelegateClassName) {
    static bool bAppCreated = false;
    if(bAppCreated == true) {
        return;
    }
    bAppCreated = true;
    
    @autoreleasepool {
        cout << "trying to launch app delegate " << appDelegateClassName << endl;
        UIApplicationMain(nil, nil, nil, [NSString stringWithUTF8String:appDelegateClassName.c_str()]);
    }
}


//----------------------------------------------------------------------------------- cursor.
void ofAppiOSWindow::hideCursor() {
    // not supported on iOS.
}

void ofAppiOSWindow::showCursor() {
    // not supported on iOS.
}

//----------------------------------------------------------------------------------- window / screen properties.
void ofAppiOSWindow::setWindowPosition(int x, int y) {
	// not supported on iOS.
}

void ofAppiOSWindow::setWindowShape(int w, int h) {
	// not supported on iOS.
}

ofPoint	ofAppiOSWindow::getWindowPosition() {
	return *[[ofxiOSEAGLView getInstance] getWindowPosition];
}

ofPoint	ofAppiOSWindow::getWindowSize() {
	return *[[ofxiOSEAGLView getInstance] getWindowSize];
}

ofPoint	ofAppiOSWindow::getScreenSize() {
	return *[[ofxiOSEAGLView getInstance] getScreenSize];
}

int ofAppiOSWindow::getWidth(){
	if(bHardwareOrientation == true || orientation == OF_ORIENTATION_DEFAULT || orientation == OF_ORIENTATION_180){
		return (int)getWindowSize().x;
	}
	return (int)getWindowSize().y;
}

int ofAppiOSWindow::getHeight(){
	if(bHardwareOrientation == true || orientation == OF_ORIENTATION_DEFAULT || orientation == OF_ORIENTATION_180){
		return (int)getWindowSize().y;
	}
	return (int)getWindowSize().x;
}

ofWindowMode ofAppiOSWindow::getWindowMode() {
	return windowMode;
}

//----------------------------------------------------------------------------------- orientation.
void ofAppiOSWindow::setOrientation(ofOrientation toOrientation) {
    if(orientation == toOrientation) {
        return;
    }
    bool bOrientationPortraitOne = (orientation == OF_ORIENTATION_DEFAULT) || (orientation == OF_ORIENTATION_180);
    bool bOrientationPortraitTwo = (toOrientation == OF_ORIENTATION_DEFAULT) || (toOrientation == OF_ORIENTATION_180);
    bool bResized = bOrientationPortraitOne != bOrientationPortraitTwo;

    orientation = toOrientation;
    
    UIInterfaceOrientation interfaceOrientation = UIInterfaceOrientationPortrait;
    switch (orientation) {
        case OF_ORIENTATION_DEFAULT:
            interfaceOrientation = UIInterfaceOrientationPortrait;
            break;
        case OF_ORIENTATION_180:
            interfaceOrientation = UIInterfaceOrientationPortraitUpsideDown;
            break;
        case OF_ORIENTATION_90_RIGHT:
            interfaceOrientation = UIInterfaceOrientationLandscapeLeft;
            break;
        case OF_ORIENTATION_90_LEFT:
            interfaceOrientation = UIInterfaceOrientationLandscapeRight;
            break;
    }

    id<UIApplicationDelegate> appDelegate = [UIApplication sharedApplication].delegate;
    if([appDelegate respondsToSelector:@selector(glViewController)] == NO) {
        // check app delegate has glViewController,
        // otherwise calling glViewController will cause a crash.
        return;
    }
    ofxiOSViewController * glViewController = ((ofxiOSAppDelegate *)appDelegate).glViewController;
    ofxiOSEAGLView * glView = glViewController.glView;
    
    if(bHardwareOrientation == true) {
        [glViewController rotateToInterfaceOrientation:interfaceOrientation animated:bOrientationIsAnimated];
    } else {
        [[UIApplication sharedApplication] setStatusBarOrientation:interfaceOrientation animated:bOrientationIsAnimated];
        if(bResized == true) {
            [glView layoutSubviews]; // calling layoutSubviews so window resize notification is fired.
        }
    }
}

ofOrientation ofAppiOSWindow::getOrientation() {
	return orientation;
}

bool ofAppiOSWindow::doesHWOrientation() {
    return bHardwareOrientation;
}

//-----------------------------------------------------------------------------------
void ofAppiOSWindow::setWindowTitle(string title) {
    // not supported on iOS.
}

void ofAppiOSWindow::setFullscreen(bool fullscreen) {
    [[UIApplication sharedApplication] setStatusBarHidden:fullscreen withAnimation:UIStatusBarAnimationSlide];
	if(fullscreen) {
        windowMode = OF_FULLSCREEN;
    } else {
        windowMode = OF_WINDOW;
    }
}

void ofAppiOSWindow::toggleFullscreen() {
	if(windowMode == OF_FULLSCREEN) {
        setFullscreen(false);
    } else {
        setFullscreen(true);
    }
}

//-----------------------------------------------------------------------------------
bool ofAppiOSWindow::enableHardwareOrientation() {
    return (bHardwareOrientation = true);
}

bool ofAppiOSWindow::disableHardwareOrientation() {
    return (bHardwareOrientation = false);
}

bool ofAppiOSWindow::enableOrientationAnimation() {
    return (bOrientationIsAnimated = true);
}

bool ofAppiOSWindow::disableOrientationAnimation() {
    return (bOrientationIsAnimated = false);
}

//-----------------------------------------------------------------------------------
bool ofAppiOSWindow::enableRendererES2() {
    if(isRendererES2() == true) {
        return false;
    }
    shared_ptr<ofBaseRenderer> renderer(new ofGLProgrammableRenderer(false));
    ofSetCurrentRenderer(renderer);
    return true;
}

bool ofAppiOSWindow::enableRendererES1() {
    if(isRendererES1() == true) {
        return false;
    }
    shared_ptr<ofBaseRenderer> renderer(new ofGLRenderer(false));
    ofSetCurrentRenderer(renderer);
    return true;
}

bool ofAppiOSWindow::isRendererES2() {
    return (ofGetCurrentRenderer() && ofGetCurrentRenderer()->getType()==ofGLProgrammableRenderer::TYPE);
}

bool ofAppiOSWindow::isRendererES1() {
    return (ofGetCurrentRenderer() && ofGetCurrentRenderer()->getType()==ofGLRenderer::TYPE);
}

//-----------------------------------------------------------------------------------
void ofAppiOSWindow::enableSetupScreen() {
	bEnableSetupScreen = true;
};

void ofAppiOSWindow::disableSetupScreen() {
	bEnableSetupScreen = false;
};

bool ofAppiOSWindow::isSetupScreenEnabled() {
    return bEnableSetupScreen;
}

void ofAppiOSWindow::setVerticalSync(bool enabled) {
    // not supported on iOS.
}

//----------------------------------------------------------------------------------- retina.
bool ofAppiOSWindow::enableRetina() {
    if(isRetinaSupportedOnDevice()) {
        bRetinaEnabled = true;
    }
    return bRetinaEnabled;
}

bool ofAppiOSWindow::disableRetina() {
    return (bRetinaEnabled = false);
}

bool ofAppiOSWindow::isRetinaEnabled() {
    return bRetinaEnabled;
}

bool ofAppiOSWindow::isRetinaSupportedOnDevice() {
    if(bRetinaSupportedOnDeviceChecked) {
        return bRetinaSupportedOnDevice;
    }
    
    bRetinaSupportedOnDeviceChecked = true;
    
    @autoreleasepool {
        if([[UIScreen mainScreen] respondsToSelector:@selector(scale)]){
            if ([[UIScreen mainScreen] scale] > 1){
                bRetinaSupportedOnDevice = true;
            }
        }
    }
    
    return bRetinaSupportedOnDevice;
}

//----------------------------------------------------------------------------------- depth buffer.
bool ofAppiOSWindow::enableDepthBuffer() {
    return (bDepthEnabled = true);
}

bool ofAppiOSWindow::disableDepthBuffer() {
    return (bDepthEnabled = false);
}

bool ofAppiOSWindow::isDepthBufferEnabled() {
    return bDepthEnabled;
}

//----------------------------------------------------------------------------------- anti aliasing.
bool ofAppiOSWindow::enableAntiAliasing(int samples) {
	antiAliasingSamples = samples;
    return (bAntiAliasingEnabled = true);
}

bool ofAppiOSWindow::disableAntiAliasing() {
    return (bAntiAliasingEnabled = false);
}

bool ofAppiOSWindow::isAntiAliasingEnabled() {
    return bAntiAliasingEnabled;
}

int	ofAppiOSWindow::getAntiAliasingSampleCount() {
    return antiAliasingSamples;
}
