/*
 * ofxAndroidVideoPlayer.h
 *
 *  Created on: 25/04/2013
 *      Author: p
 */

#pragma once

#include "ofBaseTypes.h"
#include "ofPixels.h"
#include "ofEvents.h"
#include "ofTexture.h"
#include <jni.h>

class ofxAndroidVideoPlayer: public ofBaseVideoPlayer{

	public:

		ofxAndroidVideoPlayer();
		virtual ~ofxAndroidVideoPlayer();

		bool loadMovie(string fileName);
		void close(); // empty!
		void update();

		void play();
		void stop();

		bool isLoaded() const;
		bool isPlaying() const;
		bool isPaused() const;
		bool isFrameNew() const { return bIsFrameNew;};

		ofTexture *	getTexture();
		void reloadTexture();
		void unloadTexture();
		void removeTexture();

		float getWidth() const;
		float getHeight() const;

		unsigned char * getPixels(){ return NULL;}; // no pixels in town!
        ofPixels& getPixelsRef() {return pixels;}  // no pixels in town!
        const ofPixels& getPixelsRef() const {return pixels;}  // no pixels in town!

		bool setPixelFormat(ofPixelFormat pixelFormat){ return false;};  // no pixels in town!
		ofPixelFormat getPixelFormat() const { return OF_PIXELS_RGBA;};  // no pixels in town!

		//should implement!
		//float getSpeed();
		float getPosition() const;
		float getDuration() const;
		bool  getIsMovieDone() const;

		void setPosition(float pct);
		void setPaused(bool bPause);
		void setVolume(float volume); // 0..1
		void setLoopState(ofLoopType state);
		//void setSpeed(float speed);
		//void setFrame(int frame);  // frame 0 = first frame...

		ofLoopType getLoopState() const;
		//int	getCurrentFrame();
		//int	getTotalNumFrames();

		//void firstFrame();
		//void nextFrame();
		//void previousFrame();

	private:

		jobject javaVideoPlayer;
		jclass javaClass;

		int width;
		int height;
		ofTexture texture;

		ofPixels pixels;

		jfloatArray matrixJava;

		bool bIsFrameNew;

};
