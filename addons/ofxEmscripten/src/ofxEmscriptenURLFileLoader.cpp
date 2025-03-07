/*
 * ofxEmscriptenURLFileLoader.cpp
 *
 *  Created on: May 30, 2014
 *      Author: arturo
 */

#include "ofxEmscriptenURLFileLoader.h"
#include <emscripten/emscripten.h>


ofxEmscriptenURLFileLoader::ofxEmscriptenURLFileLoader() {
}

ofxEmscriptenURLFileLoader::~ofxEmscriptenURLFileLoader() {
}

ofHttpResponse ofxEmscriptenURLFileLoader::get(string url){
	getAsync(url,url);
	return ofHttpResponse();
}

int ofxEmscriptenURLFileLoader::getAsync(string url, string name){
	ofHttpRequest * req = new ofHttpRequest(url,name,false);
	emscripten_async_wget2_data(url.c_str(), "GET", "", req, true, &onload_cb, &onerror_cb, NULL);
	return req->getID();
}

ofHttpResponse ofxEmscriptenURLFileLoader::saveTo(string url, string path){
	saveAsync(url,path);
	return ofHttpResponse();
}

int ofxEmscriptenURLFileLoader::saveAsync(string url, string path){
	ofHttpRequest * req = new ofHttpRequest(url,url,true);
	emscripten_async_wget2(url.c_str(), path.c_str(), "GET", "", req, &onload_file_cb, &onerror_file_cb, NULL);
	return 0;
}

void ofxEmscriptenURLFileLoader::remove(int id){

}

void ofxEmscriptenURLFileLoader::clear(){

}

void ofxEmscriptenURLFileLoader::stop(){

}

void ofxEmscriptenURLFileLoader::onload_cb(void* request, void* data, unsigned int size){
	ofHttpResponse response;
	response.data.set((const char*)data,size);
	response.status = 200;
	response.request = *(ofHttpRequest*)request;
	ofNotifyEvent(ofURLResponseEvent(),response);
	delete (ofHttpRequest*)request;
}

void ofxEmscriptenURLFileLoader::onerror_cb(void* request, int status, const char* message){
	ofHttpResponse response;
	response.status = status;
	response.error = message;
	response.request = *(ofHttpRequest*)request;
	ofNotifyEvent(ofURLResponseEvent(),response);
	delete (ofHttpRequest*)request;
}

void ofxEmscriptenURLFileLoader::onload_file_cb(void* request, const char* file){
	ofHttpResponse response;
	response.status = 200;
	response.request = *(ofHttpRequest*)request;
	ofNotifyEvent(ofURLResponseEvent(),response);
	delete (ofHttpRequest*)request;
}

void ofxEmscriptenURLFileLoader::onerror_file_cb(void* request, int status){
	ofHttpResponse response;
	response.status = status;
	response.request = *(ofHttpRequest*)request;
	ofNotifyEvent(ofURLResponseEvent(),response);
	delete (ofHttpRequest*)request;
}
