#pragma once

#include "ofConstants.h"
#ifndef TARGET_ANDROID
#include "ofConstants.h"
#include "ofBaseTypes.h"
#include "ofPixels.h"
#include "ofTypes.h"
#include "ofEvents.h"
#include "ofThread.h"

#define GST_DISABLE_DEPRECATED
#include <gst/gst.h>
#include <gst/gstpad.h>
#include <gst/video/video.h>
#include "Poco/Condition.h"

class ofGstAppSink;
typedef struct _GstElement GstElement;
typedef struct _GstBuffer GstBuffer;
typedef struct _GstMessage GstMessage;

//-------------------------------------------------
//----------------------------------------- ofGstUtils
//-------------------------------------------------

class ofGstUtils{
public:
	ofGstUtils();
	virtual ~ofGstUtils();

	bool 	setPipelineWithSink(string pipeline, string sinkname="sink", bool isStream=false);
	bool 	setPipelineWithSink(GstElement * pipeline, GstElement * sink, bool isStream=false);
	bool	startPipeline();

	void 	play();
	void 	stop();
	void 	setPaused(bool bPause);
	bool 	isPaused() const {return bPaused;}
	bool 	isLoaded() const {return bLoaded;}
	bool 	isPlaying() const {return bPlaying;}

	float	getPosition() const;
	float 	getSpeed() const;
	float 	getDuration() const;
	int64_t  getDurationNanos() const;
	bool  	getIsMovieDone() const;

	void 	setPosition(float pct);
	void 	setVolume(float volume);
	void 	setLoopState(ofLoopType state);
	ofLoopType	getLoopState() const {return loopMode;}
	void 	setSpeed(float speed);

	void 	setFrameByFrame(bool bFrameByFrame);
	bool	isFrameByFrame() const;

	GstElement 	* getPipeline() const;
	GstElement 	* getSink() const;
	GstElement 	* getGstElementByName(const string & name) const;
	uint64_t getMinLatencyNanos() const;
	uint64_t getMaxLatencyNanos() const;

	virtual void close();

	void setSinkListener(ofGstAppSink * appsink);

	// callbacks to get called from gstreamer
#if GST_VERSION_MAJOR==0
	virtual GstFlowReturn preroll_cb(GstBuffer * buffer);
	virtual GstFlowReturn buffer_cb(GstBuffer * buffer);
#else
	virtual GstFlowReturn preroll_cb(GstSample * buffer);
	virtual GstFlowReturn buffer_cb(GstSample * buffer);
#endif
	virtual void 		  eos_cb();

	static void startGstMainLoop();
	static GMainLoop * getGstMainLoop();
protected:
	ofGstAppSink * 		appsink;
	bool				isStream;
	bool				closing;

private:
	static bool			busFunction(GstBus * bus, GstMessage * message, ofGstUtils * app);
	bool				gstHandleMessage(GstBus * bus, GstMessage * message);

	bool 				bPlaying;
	bool 				bPaused;
	bool				bIsMovieDone;
	bool 				bLoaded;
	bool 				bFrameByFrame;
	ofLoopType			loopMode;

	GstElement  *		gstSink;
	GstElement 	*		gstPipeline;

	float				speed;
	mutable int64_t		durationNanos;
	bool				isAppSink;
	Poco::Condition		eosCondition;
	ofMutex				eosMutex;
	guint				busWatchID;

	class ofGstMainLoopThread: public ofThread{
	public:
		ofGstMainLoopThread()
		:main_loop(NULL)
		{

		}

		void start(){
			main_loop = g_main_loop_new (NULL, FALSE);
			startThread();
		}
		void threadedFunction(){
			g_main_loop_run (main_loop);
		}

		GMainLoop * getMainLoop(){
			return main_loop;
		}
	private:
		GMainLoop *main_loop;
	};

	static ofGstMainLoopThread * mainLoop;
};





//-------------------------------------------------
//----------------------------------------- videoUtils
//-------------------------------------------------

class ofGstVideoUtils: public ofBaseVideo, public ofGstUtils{
public:

	ofGstVideoUtils();
	virtual ~ofGstVideoUtils();

	bool 			setPipeline(string pipeline, ofPixelFormat pixelFormat=OF_PIXELS_RGB, bool isStream=false, int w=-1, int h=-1);

	bool 			setPixelFormat(ofPixelFormat pixelFormat);
	ofPixelFormat 	getPixelFormat() const;
	bool 			allocate(int w, int h, ofPixelFormat pixelFormat);

	bool 			isFrameNew() const;
	unsigned char * getPixels();
	ofPixels&		getPixelsRef();
	const ofPixels& getPixelsRef() const;
	void 			update();

	float 			getHeight() const;
	float 			getWidth() const;

	void 			close();

#if GST_VERSION_MAJOR>0
	static string			getGstFormatName(ofPixelFormat format);
	static GstVideoFormat	getGstFormat(ofPixelFormat format);
	static ofPixelFormat	getOFFormat(GstVideoFormat format);
#endif

	bool			isInitialized() const;

	// this events happen in a different thread
	// do not use them for opengl stuff
	ofEvent<ofPixels> prerollEvent;
	ofEvent<ofPixels> bufferEvent;
	ofEvent<ofEventArgs> eosEvent;

protected:
#if GST_VERSION_MAJOR==0
	GstFlowReturn process_buffer(GstBuffer * buffer);
	GstFlowReturn preroll_cb(GstBuffer * buffer);
	GstFlowReturn buffer_cb(GstBuffer * buffer);
#else
	GstFlowReturn process_sample(GstSample * sample);
	GstFlowReturn preroll_cb(GstSample * buffer);
	GstFlowReturn buffer_cb(GstSample * buffer);
#endif
	void			eos_cb();


	ofPixels		pixels;				// 24 bit: rgb
	ofPixels		backPixels;
	ofPixels		eventPixels;
private:
	bool			bIsFrameNew;			// if we are new
	bool			bHavePixelsChanged;
	bool			bBackPixelsChanged;
	ofMutex			mutex;
#if GST_VERSION_MAJOR==0
	GstBuffer * 	buffer, *prevBuffer;
#else
	GstSample * 	buffer, *prevBuffer;
	GstMapInfo mapinfo;
#endif
	ofPixelFormat	internalPixelFormat;
};


//-------------------------------------------------
//----------------------------------------- appsink listener
//-------------------------------------------------

class ofGstAppSink{
public:
	virtual ~ofGstAppSink(){}
#if GST_VERSION_MAJOR==0
	virtual GstFlowReturn on_preroll(GstBuffer * buffer){
		return GST_FLOW_OK;
	}
	virtual GstFlowReturn on_buffer(GstBuffer * buffer){
		return GST_FLOW_OK;
	}
#else
	virtual GstFlowReturn on_preroll(GstSample * buffer){
		return GST_FLOW_OK;
	}
	virtual GstFlowReturn on_buffer(GstSample * buffer){
		return GST_FLOW_OK;
	}
#endif
	virtual void			on_eos(){}

	// return true to set the message as attended so upstream doesn't try to process it
	virtual bool on_message(GstMessage* msg){return false;};

	// pings when enough data has arrived to be able to get sink properties
	virtual void on_stream_prepared(){};
};

#endif

