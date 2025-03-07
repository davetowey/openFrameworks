#pragma once

#include "ofGstUtils.h"


class ofGstVideoPlayer: public ofBaseVideoPlayer, public ofGstAppSink{
public:

	ofGstVideoPlayer();
	~ofGstVideoPlayer();

	/// needs to be called before loadMovie
	bool 	setPixelFormat(ofPixelFormat pixelFormat);
	ofPixelFormat	getPixelFormat() const;
	
	bool 	loadMovie(string uri);

	void 	update();

	int		getCurrentFrame() const;
	int		getTotalNumFrames() const;

	void 	firstFrame();
	void 	nextFrame();
	void 	previousFrame();
	void 	setFrame(int frame);  // frame 0 = first frame...

	bool	isStream() const;

	void 	play();
	void 	stop();
	void 	setPaused(bool bPause);
	bool 	isPaused() const;
	bool 	isLoaded() const;
	bool 	isPlaying() const;

	float	getPosition() const;
	float 	getSpeed() const;
	float 	getDuration() const;
	bool  	getIsMovieDone() const;

	void 	setPosition(float pct);
	void 	setVolume(float volume);
	void 	setLoopState(ofLoopType state);
	ofLoopType 	getLoopState() const;
	void 	setSpeed(float speed);
	void 	close();

	bool 			isFrameNew() const;

	unsigned char * getPixels();
	ofPixels&		getPixelsRef();
	const ofPixels& getPixelsRef() const;

	float 			getHeight() const;
	float 			getWidth() const;

	void setFrameByFrame(bool frameByFrame);
	void setThreadAppSink(bool threaded);

	ofGstVideoUtils * getGstVideoUtils();

protected:
	bool	allocate();
	void	on_stream_prepared();

	// return true to set the message as attended so upstream doesn't try to process it
	virtual bool on_message(GstMessage* msg){return false;};

private:
	ofPixelFormat		internalPixelFormat;
	guint64				nFrames;
	int 				fps_n, fps_d;
	bool				bIsStream;
	bool				bIsAllocated;
	bool				threadAppSink;
	ofGstVideoUtils		videoUtils;
};
