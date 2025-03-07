#include "ofFileUtils.h"
#ifndef TARGET_WIN32
 #include <pwd.h>
#endif

#include "ofUtils.h"


#ifdef TARGET_OSX
 #include <mach-o/dyld.h>       /* _NSGetExecutablePath */
 #include <limits.h>        /* PATH_MAX */
#endif


//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- ofBuffer
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------


size_t ofBuffer::ioSize = 1024;

//--------------------------------------------------
ofBuffer::ofBuffer()
:currentLine(begin(),end()){
	buffer.resize(1);
}

//--------------------------------------------------
ofBuffer::ofBuffer(const char * _buffer, unsigned int size)
:buffer(_buffer,_buffer+size)
,currentLine(begin(),end()){
	buffer.resize(buffer.size()+1,0);
}

//--------------------------------------------------
ofBuffer::ofBuffer(const string & text)
:buffer(text.begin(),text.end())
,currentLine(begin(),end()){
	buffer.resize(buffer.size()+1,0);
}

//--------------------------------------------------
ofBuffer::ofBuffer(istream & stream)
:currentLine(begin(),end()){
	set(stream);
}

//--------------------------------------------------
bool ofBuffer::set(istream & stream){
	if(stream.bad()){
		clear();
		return false;
	}else{
		buffer.clear();
	}

	vector<char> aux_buffer(ioSize);
	while(stream.good()){
		stream.read(&aux_buffer[0], ioSize);
		buffer.insert(buffer.end(),aux_buffer.begin(),aux_buffer.begin()+stream.gcount());
	}
	buffer.push_back(0);
	currentLine = getLines().begin();
	return true;
}

//--------------------------------------------------
bool ofBuffer::writeTo(ostream & stream) const {
	if(stream.bad()){
		return false;
	}
	stream.write(&(buffer[0]), buffer.size() - 1);
	return true;
}

//--------------------------------------------------
void ofBuffer::set(const char * _buffer, unsigned int _size){
	buffer.assign(_buffer,_buffer+_size);
	buffer.resize(buffer.size()+1,0);
	currentLine = getLines().begin();
}

//--------------------------------------------------
void ofBuffer::set(const string & text){
	set(text.c_str(),text.size());
	currentLine = getLines().begin();
}

//--------------------------------------------------
void ofBuffer::append(const string& _buffer){
	append(_buffer.c_str(), _buffer.size());
	currentLine = getLines().begin();
}

//--------------------------------------------------
void ofBuffer::append(const char * _buffer, unsigned int _size){
	buffer.insert(buffer.end()-1,_buffer,_buffer+_size);
	buffer.back() = 0;
	currentLine = getLines().begin();
}

//--------------------------------------------------
void ofBuffer::clear(){
	buffer.resize(1,0);
	currentLine = getLines().begin();
}

//--------------------------------------------------
void ofBuffer::allocate(long _size){
	clear();
	buffer.resize(_size);
}

//--------------------------------------------------
char *ofBuffer::getBinaryBuffer(){
	if(buffer.empty()){
		return NULL;
	}
	return &buffer[0];
}

//--------------------------------------------------
const char *ofBuffer::getBinaryBuffer() const {
	if(buffer.empty()){
		return "";
	}
	return &buffer[0];
}

//--------------------------------------------------
string ofBuffer::getText() const {
	if(buffer.empty()){
		return "";
	}
	return &buffer[0];
}

//--------------------------------------------------
ofBuffer::operator string() const {
	return getText();
}

//--------------------------------------------------
ofBuffer & ofBuffer::operator=(const string & text){
	set(text);
	return *this;
}

//--------------------------------------------------
long ofBuffer::size() const {
	if(buffer.empty()){
		return 0;
	}
	//we always add a 0 at the end to avoid problems with strings
	return buffer.size() - 1;
}

//--------------------------------------------------
void ofBuffer::setIOBufferSize(size_t _ioSize){
	ioSize = _ioSize;
}

//--------------------------------------------------
string ofBuffer::getNextLine(){
	++currentLine;
	return currentLine.asString();
}

//--------------------------------------------------
string ofBuffer::getFirstLine(){
	currentLine = getLines().begin();
	return currentLine.asString();
}

//--------------------------------------------------
bool ofBuffer::isLastLine(){
	return currentLine == getLines().end();
}

//--------------------------------------------------
void ofBuffer::resetLineReader(){
	currentLine = getLines().begin();
}

//--------------------------------------------------
vector<char>::iterator ofBuffer::begin(){
	return buffer.begin();
}

//--------------------------------------------------
vector<char>::iterator ofBuffer::end(){
	return buffer.end();
}

//--------------------------------------------------
vector<char>::const_iterator ofBuffer::begin() const{
	return buffer.begin();
}

//--------------------------------------------------
vector<char>::const_iterator ofBuffer::end() const{
	return buffer.end();
}

//--------------------------------------------------
vector<char>::reverse_iterator ofBuffer::rbegin(){
	return buffer.rbegin();
}

//--------------------------------------------------
vector<char>::reverse_iterator ofBuffer::rend(){
	return buffer.rend();
}

//--------------------------------------------------
vector<char>::const_reverse_iterator ofBuffer::rbegin() const{
	return buffer.rbegin();
}

//--------------------------------------------------
vector<char>::const_reverse_iterator ofBuffer::rend() const{
	return buffer.rend();
}

//--------------------------------------------------
ofBuffer::Line::Line(vector<char>::iterator _begin, vector<char>::iterator _end)
	:_current(_begin)
	,_begin(_begin)
	,_end(_end){
	if(_begin == _end){
		line =  "";
		return;
	}

	bool lineEndWasCR = false;
	while(_current != _end && *_current != '\n'){
		if(*_current != '\r'){
			_current++;
		}else{
			lineEndWasCR = true;
			break;
		}
	}
	line = string(_begin, _current);
	if(_current != _end){
		_current++;
	}
	// if lineEndWasCR check for CRLF
	if(lineEndWasCR && _current != _end && *_current == '\n'){
		_current++;
	}
}

//--------------------------------------------------
const string & ofBuffer::Line::operator*() const{
	return line;
}

//--------------------------------------------------
const string * ofBuffer::Line::operator->() const{
	return &line;
}

//--------------------------------------------------
const string & ofBuffer::Line::asString() const{
	return line;
}

//--------------------------------------------------
ofBuffer::Line & ofBuffer::Line::operator++(){
	*this = Line(_current,_end);
	return *this;
}

//--------------------------------------------------
ofBuffer::Line ofBuffer::Line::operator++(int) {
	Line tmp(*this);
	operator++();
	return tmp;
}

//--------------------------------------------------
bool ofBuffer::Line::operator!=(Line const& rhs) const{
	return rhs._begin != _begin || rhs._end != _end;
}

//--------------------------------------------------
bool ofBuffer::Line::operator==(Line const& rhs) const{
	return rhs._begin == _begin && rhs._end == _end;
}

//--------------------------------------------------
ofBuffer::Lines::Lines(vector<char> & buffer)
:_begin(buffer.begin())
,_end(buffer.end()){}

//--------------------------------------------------
ofBuffer::Line ofBuffer::Lines::begin(){
	return Line(_begin,_end);
}

//--------------------------------------------------
ofBuffer::Line ofBuffer::Lines::end(){
	return Line(_end,_end);
}

//--------------------------------------------------
ofBuffer::Lines ofBuffer::getLines(){
	return ofBuffer::Lines(buffer);
}

//--------------------------------------------------
ostream & operator<<(ostream & ostr, const ofBuffer & buf){
	buf.writeTo(ostr);
	return ostr;
}

//--------------------------------------------------
istream & operator>>(istream & istr, ofBuffer & buf){
	buf.set(istr);
	return istr;
}

//--------------------------------------------------
ofBuffer ofBufferFromFile(const string & path, bool binary){
	ios_base::openmode mode = binary ? ifstream::binary : ios_base::in;
	ifstream istr(ofToDataPath(path, true).c_str(), mode);
	ofBuffer buffer(istr);
	istr.close();
	return buffer;
}

//--------------------------------------------------
bool ofBufferToFile(const string & path, ofBuffer & buffer, bool binary){
	ios_base::openmode mode = binary ? ofstream::binary : ios_base::out;
	ofstream ostr(ofToDataPath(path, true).c_str(), mode);
	bool ret = buffer.writeTo(ostr);
	ostr.close();
	return ret;
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- ofFile
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------


#include "Poco/Util/FilesystemConfiguration.h"
#include "Poco/File.h"
#include "Poco/Path.h"
#include "Poco/DirectoryIterator.h"
#include "Poco/StringTokenizer.h"
#include "Poco/Exception.h"
#include "Poco/FileStream.h"
#include "Poco/String.h"

using namespace Poco;

//------------------------------------------------------------------------------------------------------------
ofFile::ofFile(){
	mode = Reference;
	binary = false;
}

//------------------------------------------------------------------------------------------------------------
ofFile::ofFile(string path, Mode mode, bool binary){
	open(path, mode, binary);
}

//-------------------------------------------------------------------------------------------------------------
ofFile::~ofFile(){
	//close();
}

//-------------------------------------------------------------------------------------------------------------
ofFile::ofFile(const ofFile & mom){
	copyFrom(mom);
}

//-------------------------------------------------------------------------------------------------------------
ofFile & ofFile::operator=(const ofFile & mom){
	copyFrom(mom);
	return *this;
}

//-------------------------------------------------------------------------------------------------------------
void ofFile::copyFrom(const ofFile & mom){
	if(&mom != this){
		Mode new_mode = mom.mode;
		if(new_mode != Reference && new_mode != ReadOnly){
			new_mode = ReadOnly;
			ofLogWarning("ofFile") << "copyFrom(): copying a writable file, opening new copy as read only";
		}
		open(mom.myFile.path(), new_mode, mom.binary);
	}
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::openStream(Mode _mode, bool _binary){
	mode = _mode;
	binary = _binary;
	ios_base::openmode binary_mode = binary ? ios::binary : (ios_base::openmode)0;
	switch(_mode) {
		case WriteOnly:
		case ReadWrite:
		case Append:
			ofFilePath::createEnclosingDirectory(path());
			break;
		case Reference:
		case ReadOnly:
			break;
	}
	switch(_mode){
	 case Reference:
		 return true;
		 break;

	 case ReadOnly:
		 if(exists()){
			 fstream::open(path().c_str(), ios::in | binary_mode);
		 }
		 break;

	 case WriteOnly:
		 fstream::open(path().c_str(), ios::out | binary_mode);
		 break;

		case ReadWrite:
		 fstream::open(path().c_str(), ios_base::in | ios_base::out | binary_mode);
		 break;

		case Append:
		 fstream::open(path().c_str(), ios::out | ios::app | binary_mode);
		 break;
	}
	return fstream::good();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::open(string _path, Mode _mode, bool binary){
	close();
	myFile = File(ofToDataPath(_path));
	return openStream(_mode, binary);
}

//-------------------------------------------------------------------------------------------------------------
bool ofFile::changeMode(Mode _mode, bool binary){
	if(_mode != mode){
		string _path = path();
		close();
		myFile = File(_path);
		return openStream(_mode, binary);
	}
	else{
		return true;
	}
}

//-------------------------------------------------------------------------------------------------------------
bool ofFile::isWriteMode(){
	return mode != ReadOnly;
}

//-------------------------------------------------------------------------------------------------------------
void ofFile::close(){
	myFile = File();
	if(mode!=Reference) fstream::close();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::create(){
	bool success = false;

	if(myFile.path() != ""){
		try{
			success = myFile.createFile();
		}
		catch(Poco::Exception & except){
			ofLogError("ofFile") << "create(): " << except.displayText();
			return false;
		}
	}

	return success;
}

//------------------------------------------------------------------------------------------------------------
ofBuffer ofFile::readToBuffer(){
	if(myFile.path() == "" || myFile.exists() == false){
		return ofBuffer();
	}

	return ofBuffer(*this);
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::writeFromBuffer(const ofBuffer & buffer){
	if(myFile.path() == ""){
		return false;
	}
	if(!isWriteMode()){
		ofLogError("ofFile") << "writeFromBuffer(): trying to write to read only file \"" << myFile.path() << "\"";
	}
	return buffer.writeTo(*this);
}

//------------------------------------------------------------------------------------------------------------
filebuf *ofFile::getFileBuffer() const {
	return rdbuf();
}


//-- poco wrappers
//------------------------------------------------------------------------------------------------------------
bool ofFile::exists() const {
	if(path().empty()){
		return false;
	}
	try{
		return myFile.exists();
	}
	catch(...){
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
string ofFile::path() const {
	return myFile.path();
}

//------------------------------------------------------------------------------------------------------------
string ofFile::getExtension() const {
	return ofFilePath::getFileExt(path());
}

//------------------------------------------------------------------------------------------------------------
string ofFile::getFileName() const {
	return ofFilePath::getFileName(path());
}

//------------------------------------------------------------------------------------------------------------
string ofFile::getBaseName() const {
	return ofFilePath::removeExt(ofFilePath::getFileName(path()));
}

//------------------------------------------------------------------------------------------------------------
string ofFile::getEnclosingDirectory() const {
	return ofFilePath::getEnclosingDirectory(path());
}

//------------------------------------------------------------------------------------------------------------
string ofFile::getAbsolutePath() const {
	return ofFilePath::getAbsolutePath(path());
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::canRead() const {
	try{
		return myFile.canRead();
	}catch(Poco::Exception & e){
		ofLogWarning("ofFile") << "Couldn't check canRead" << e.what();
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::canWrite() const {
	try{
		return myFile.canWrite();
	}catch(Poco::Exception & e){
		ofLogWarning("ofFile") << "Couldn't check canWrite" << e.what();
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::canExecute() const {
	try{
		return myFile.canExecute();
	}catch(Poco::Exception & e){
		ofLogWarning("ofFile") << "Couldn't check canExecute" << e.what();
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::isFile() const {
	try{
		return myFile.isFile();
	}catch(Poco::Exception & e){
		ofLogWarning("ofFile") << "Couldn't check isFile" << e.what();
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::isLink() const {
	try{
		return myFile.isLink();
	}catch(Poco::Exception & e){
		ofLogWarning("ofFile") << "Couldn't check isLink" << e.what();
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::isDirectory() const {
	try{
		return myFile.isDirectory();
	}catch(Poco::Exception & e){
		ofLogWarning("ofFile") << "Couldn't check isDirectory" << e.what();
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::isDevice() const {
	try{
		return myFile.isDevice();
	}catch(Poco::Exception & e){
		ofLogWarning("ofFile") << "Couldn't check isDevice" << e.what();
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::isHidden() const {
	try{
		return myFile.isHidden();
	}catch(Poco::Exception & e){
		ofLogWarning("ofFile") << "Couldn't check isHidden" << e.what();
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
void ofFile::setWriteable(bool flag){
	try{
		myFile.setWriteable(flag);
	}catch(Poco::Exception & e){
		ofLogWarning("ofFile") << "Couldn't setWriteable" << e.what();
	}
}

//------------------------------------------------------------------------------------------------------------
void ofFile::setReadOnly(bool flag){
	try{
		myFile.setReadOnly(flag);
	}catch(Poco::Exception & e){
		ofLogWarning("ofFile") << "Couldn't setReadOnly" << e.what();
	}
}

//------------------------------------------------------------------------------------------------------------
void ofFile::setExecutable(bool flag){
	try{
		myFile.setExecutable(flag);
	}catch(Poco::Exception & e){
		ofLogWarning("ofFile") << "Couldn't check setExecutable" << e.what();
	}
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::copyTo(string path, bool bRelativeToData, bool overwrite){
	if(path.empty()){
		ofLogError("ofFile") << "copyTo(): destination path is empty";
		return false;
	}
	if(!myFile.exists()){
		ofLogError("ofFile") << "copyTo(): source file does not exist";
		return false;
	}

	if(bRelativeToData){
		path = ofToDataPath(path);
	}
	if(ofFile::doesFileExist(path, bRelativeToData)){
		if(overwrite){
			ofFile::removeFile(path, bRelativeToData);
		}
		else{
			ofLogWarning("ofFile") << "copyTo(): destination file \"" << path << "\" already exists, set bool overwrite to true if you want to overwrite it";
		}
	}

	try{
		ofFilePath::createEnclosingDirectory(path, bRelativeToData);
		myFile.copyTo(path);
	}
	catch(Poco::Exception & except){
		ofLogError("ofFile") <<  "copyTo(): unable to copy \"" << path << "\":" << except.displayText();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::moveTo(string path, bool bRelativeToData, bool overwrite){
	if(path.empty()){
		ofLogError("ofFile") << "moveTo(): destination path is empty";
		return false;
	}
	if(!myFile.exists()){
		ofLogError("ofFile") << "moveTo(): source file does not exist";
		return false;
	}

	if(bRelativeToData){
		path = ofToDataPath(path);
	}
	if(ofFile::doesFileExist(path, bRelativeToData)){
		if(overwrite){
			ofFile::removeFile(path, bRelativeToData);
		}
		else{
			ofLogWarning("ofFile") << "moveTo(): destination file \"" << path << "\" already exists, set bool overwrite to true if you want to overwrite it";
		}
	}

	try{
		ofFilePath::createEnclosingDirectory(path, bRelativeToData);
		myFile.moveTo(path);
	}
	catch(Poco::Exception & except){
		ofLogError("ofFile") << "moveTo(): unable to move \"" << path << "\":" << except.displayText();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::renameTo(string path, bool bRelativeToData, bool overwrite){
	if(path.empty()){
		ofLogError("ofFile") << "renameTo(): destination path is empty";
		return false;
	}
	if(!myFile.exists()){
		ofLogError("ofFile") << "renameTo(): source file does not exist";
		return false;
	}

	if(bRelativeToData){
		path = ofToDataPath(path);
	}
	if(ofFile::doesFileExist(path, bRelativeToData)){
		if(overwrite){
			ofFile::removeFile(path, bRelativeToData);
		}
		else{
			ofLogWarning("ofFile") << "renameTo(): destination file \"" << path << "\" already exists, set bool overwrite to true if you want to overwrite it";
			return false;
		}
	}

	try{
		ofFilePath::createEnclosingDirectory(path, bRelativeToData);
		myFile.renameTo(path);
	}
	catch(Poco::Exception & except){
		ofLogError("ofFile") << "renameTo(): unable to rename \"" << path << "\":" << except.displayText();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::remove(bool recursive){
	if(myFile.path().empty()){
		ofLogError("ofFile") << "remove(): file path is empty";
		return false;
	}
	if(!myFile.exists()){
		ofLogError("ofFile") << "copyTo(): file does not exist";
		return false;
	}

	try{
		myFile.remove(recursive);
	}
	catch(Poco::Exception & except){
		ofLogError("ofFile") << "remove(): unable to remove \"" << myFile.path() << "\": " << except.displayText();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
uint64_t ofFile::getSize() const {
	return myFile.getSize();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::operator==(const ofFile & file) const {
	return getAbsolutePath() == file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::operator!=(const ofFile & file) const {
	return getAbsolutePath() != file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::operator<(const ofFile & file) const {
	return getAbsolutePath() < file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::operator<=(const ofFile & file) const {
	return getAbsolutePath() <= file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::operator>(const ofFile & file) const {
	return getAbsolutePath() > file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::operator>=(const ofFile & file) const {
	return getAbsolutePath() >= file.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
// ofFile Static Methods
//------------------------------------------------------------------------------------------------------------

bool ofFile::copyFromTo(string pathSrc, string pathDst, bool bRelativeToData,  bool overwrite){
	if(bRelativeToData){
		pathSrc = ofToDataPath(pathSrc);
	}
	if(bRelativeToData){
		pathDst = ofToDataPath(pathDst);
	}

	if(!ofFile::doesFileExist(pathSrc, bRelativeToData)){
		ofLogError("ofFile") << "copyFromTo(): source file/directory doesn't exist: \"" << pathSrc << "\"";
		return false;
	}

	if(ofFile::doesFileExist(pathDst, bRelativeToData)){
		if(overwrite){
			ofFile::removeFile(pathDst, bRelativeToData);
		}
		else{
			ofLogWarning("ofFile") << "copyFromTo(): destination file/directory \"" << pathSrc << "\"exists,"
			<< " set bool overwrite to true if you want to overwrite it";
		}
	}

	File fileSrc(pathSrc);
	try{
		fileSrc.copyTo(pathDst);
	}
	catch(Poco::Exception & except){
		ofLogError("ofFile") << "copyFromTo(): unable to copy \"" << pathSrc << "\": " <<  except.displayText();
		return false;
	}
	return true;
}

//be careful with slashes here - appending a slash when moving a folder will causes mad headaches
//------------------------------------------------------------------------------------------------------------
bool ofFile::moveFromTo(string pathSrc, string pathDst, bool bRelativeToData, bool overwrite){
	if(bRelativeToData){
		pathSrc = ofToDataPath(pathSrc);
	}
	if(bRelativeToData){
		pathDst = ofToDataPath(pathDst);
	}

	if(!ofFile::doesFileExist(pathSrc, bRelativeToData)){
		ofLogError("ofFile") << "moveFromTo(): source file/directory doesn't exist: \"" << pathSrc << "\"";
		return false;
	}

	if(ofFile::doesFileExist(pathDst, bRelativeToData)){
		if(overwrite){
			ofFile::removeFile(pathDst, bRelativeToData);
		}
		else{
			ofLogWarning("ofFile") << "moveFromTo(): destination file/folder \"" << pathDst << "\" exists,"
			<< " set bool overwrite to true if you want to overwrite it";
		}
	}

	File fileSrc(pathSrc);
	try{
		fileSrc.moveTo(pathDst);
	}
	catch(Poco::Exception & except){
		ofLogError("ofFile") << "moveFromTo(): unable to move \"" << pathSrc << "\" to \"" << pathDst << "\": " << except.displayText();
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::doesFileExist(string fPath,  bool bRelativeToData){
	if(bRelativeToData){
		fPath = ofToDataPath(fPath);
	}
	File file(fPath);
	return !fPath.empty() && file.exists();
}

//------------------------------------------------------------------------------------------------------------
bool ofFile::removeFile(string path, bool bRelativeToData){
	if(bRelativeToData){
		path = ofToDataPath(path);
	}
	File file(path);
	try{
		file.remove();
	}
	catch(Poco::Exception & except){
		ofLogError("ofFile") << "removeFile(): unable to remove file \"" << path << "\": "<< except.displayText();
		return false;
	}
	return true;
}

Poco::File & ofFile::getPocoFile(){
	return myFile;
}

//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- ofDirectory
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------


//----------------------------------------
// Simple example of a boolean function that can be used
// with ofRemove to remove items from vectors.
bool hiddenFile(ofFile file){
	return file.isHidden();
}

//----------------------------------------
// Advanced example of a class that has a boolean function
// that can be used with ofRemove to remove items from vectors.
class ExtensionComparator : public unary_function<ofFile, bool> {
	public:
		vector<string> * extensions;
		inline bool operator()(const ofFile & file){
			Path curPath(file.path());
			string curExtension = toLower(curPath.getExtension());
			return !ofContains(*extensions, curExtension);
		}
};

//------------------------------------------------------------------------------------------------------------
ofDirectory::ofDirectory(){
	showHidden = false;
}

//------------------------------------------------------------------------------------------------------------
ofDirectory::ofDirectory(string path){
	showHidden = false;
	open(path);
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::open(string path){
	path = ofFilePath::getPathForDirectory(path);
	originalDirectory = path;
	files.clear();
	myDir = File(ofToDataPath(path));
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::close(){
	myDir = File();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::create(bool recursive){

	if(myDir.path() != ""){
		try{
			if(recursive){
				myDir.createDirectories();
			}
			else{myDir.createDirectory();
			}
		}
		catch(Poco::Exception & except){
			ofLogError("ofDirectory") << "create(): " << except.displayText();
			return false;
		}
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::exists() const {
	return myDir.exists();
}

//------------------------------------------------------------------------------------------------------------
string ofDirectory::path() const {
	return myDir.path();
}

//------------------------------------------------------------------------------------------------------------
string ofDirectory::getAbsolutePath() const {
	return ofFilePath::getAbsolutePath(path());
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::canRead() const {
	return myDir.canRead();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::canWrite() const {
	return myDir.canWrite();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::canExecute() const {
	return myDir.canExecute();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::isHidden() const {
	try{
		return myDir.isHidden();
	}catch(Poco::Exception & e){
		ofLogWarning("ofDirectory") << "Couldn't check isHidden" << e.what();
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::setWriteable(bool flag){
	try{
		myDir.setWriteable(flag);
	}catch(Poco::Exception & e){
		ofLogWarning("ofDirectory") << "Couldn't setWriteable" << e.what();
	}
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::setReadOnly(bool flag){
	try{
		myDir.setReadOnly(flag);
	}catch(Poco::Exception & e){
		ofLogWarning("ofDirectory") << "Couldn't check isHidden" << e.what();
	}
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::setExecutable(bool flag){
	try{
		myDir.setExecutable(flag);
	}catch(Poco::Exception & e){
		ofLogWarning("ofDirectory") << "Couldn't setExecutable" << e.what();
	}
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::setShowHidden(bool showHidden){
	this->showHidden = showHidden;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::isDirectory() const {
	try{
		return myDir.isDirectory();
	}catch(Poco::Exception & e){
		ofLogWarning("ofDirectory") << "Couldn't check isDirectory" << e.what();
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::copyTo(string path, bool bRelativeToData, bool overwrite){
	if(myDir.path().empty()){
		ofLogError("ofDirectory") << "copyTo(): destination path is empty";
		return false;
	}
	if(!myDir.exists()){
		ofLogError("ofDirectory") << "copyTo(): source directory does not exist";
		return false;
	}

	if(bRelativeToData){
		path = ofToDataPath(path, bRelativeToData);
	}
	if(ofDirectory::doesDirectoryExist(path, bRelativeToData)){
		if(overwrite){
			ofDirectory::removeDirectory(path, true, bRelativeToData);
		}
		else{
			ofLogWarning("ofDirectory") << "copyTo(): dest \"" << path << "\" already exists, set bool overwrite to true to overwrite it";
		}
	}

	try{
		myDir.copyTo(path);
	}
	catch(Poco::Exception & except){
		ofLogError("ofDirectory") << "copyTo(): unable to copy \"" << path << "\": " << except.displayText();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::moveTo(string path, bool bRelativeToData, bool overwrite){
	if(myDir.path().empty()){
		ofLogError("ofDirectory") << "moveTo(): destination path is empty";
		return false;
	}
	if(!myDir.exists()){
		ofLogError("ofDirectory") << "moveTo(): source directory does not exist";
		return false;
	}

	if(bRelativeToData){
		path = ofToDataPath(path, bRelativeToData);
	}
	if(ofDirectory::doesDirectoryExist(path, bRelativeToData)){
		if(overwrite){
			ofDirectory::removeDirectory(path, true, bRelativeToData);
		}
		else{
			ofLogWarning("ofDirectory") << "moveTo(): destination folder already exists, set bool overwrite to true to overwrite it";
		}
	}

	try{
		myDir.moveTo(path);
	}
	catch(Poco::Exception & except){
		ofLogError("ofDirectory") << "moveTo(): unable to move \"" << path << "\": " << except.displayText();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::renameTo(string path, bool bRelativeToData, bool overwrite){
	if(myDir.path().empty()){
		ofLogError("ofDirectory") << "renameTo(): destination path is empty";
		return false;
	}
	if(!myDir.exists()){
		ofLogError("ofDirectory") << "renameTo(): source directory does not exist";
		return false;
	}

	if(bRelativeToData){
		path = ofToDataPath(path);
	}
	if(ofDirectory::doesDirectoryExist(path, bRelativeToData)){
		if(overwrite){
			ofDirectory::removeDirectory(path, true, bRelativeToData);
		}
		else{
			ofLogWarning("ofDirectory") << "renameTo(): destination directory \"" << path << "\" already exists,"
			<< " set bool overwrite to true to overwrite it";
			return false;
		}
	}

	try{
		myDir.renameTo(path);
	}
	catch(Poco::Exception & except){
		ofLogError("ofDirectory") << "renameTo(): unable to rename \"" << myDir.path() << "\" to \"" << path << "\": " << except.displayText();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::remove(bool recursive){
	if(path().empty() || !myDir.exists()){
		return false;
	}

	try{
		myDir.remove(recursive);
	}
	catch(Poco::Exception & except){
		ofLogError("ofDirectory") << "remove(): unable to remove file/directory: " << except.displayText();
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::allowExt(string extension){
	if(extension == "*"){
		ofLogWarning("ofDirectory") << "allowExt(): wildcard extension * is deprecated";
	}
	extensions.push_back(toLower(extension));
}

//------------------------------------------------------------------------------------------------------------
int ofDirectory::listDir(string directory){
	open(directory);
	return listDir();
}

//------------------------------------------------------------------------------------------------------------
int ofDirectory::listDir(){
	Path base(path());
	if(path().empty()){
		ofLogError("ofDirectory") << "listDir(): directory path is empty";
		return 0;
	}
	if(!myDir.exists()){
		ofLogError("ofDirectory") << "listDir:() source directory does not exist: \"" << myDir.path() << "\"";
		return 0;
	}
	
	// File::list(vector<File>) is broken on windows as of march 23, 2011...
	// so we need to use File::list(vector<string>) and build a vector<File>
	// in the future the following can be replaced width: cur.list(files);
	vector<string>fileStrings;
	myDir.list(fileStrings);
	for(int i = 0; i < (int)fileStrings.size(); i++){
		Path curPath(originalDirectory);
		curPath.setFileName(fileStrings[i]);
		try{
			files.push_back(ofFile(curPath.toString(), ofFile::Reference));
		}catch(Poco::Exception & e){
			ofLogWarning() << "couldn't add file " << e.what();
		}
	}

	if(!showHidden){
		ofRemove(files, hiddenFile);
	}

	if(!extensions.empty() && !ofContains(extensions, (string)"*")){
		ExtensionComparator extensionFilter;
		extensionFilter.extensions = &extensions;
		ofRemove(files, extensionFilter);
	}

	if(ofGetLogLevel() == OF_LOG_VERBOSE){
		for(int i = 0; i < (int)size(); i++){
			ofLogVerbose() << "\t" << getName(i);
		}
		ofLogVerbose() << "listed " << size() << " files in \"" << originalDirectory << "\"";
	}

	return size();
}

//------------------------------------------------------------------------------------------------------------
string ofDirectory::getOriginalDirectory(){
	return originalDirectory;
}

//------------------------------------------------------------------------------------------------------------
string ofDirectory::getName(unsigned int position){
	Path cur(files.at(position).path());
	return cur.getFileName();
}

//------------------------------------------------------------------------------------------------------------
string ofDirectory::getPath(unsigned int position){
	return originalDirectory + getName(position);
}

//------------------------------------------------------------------------------------------------------------
ofFile ofDirectory::getFile(unsigned int position, ofFile::Mode mode, bool binary){
	ofFile file = files[position];
	file.changeMode(mode, binary);
	return file;
}

ofFile ofDirectory::operator[](unsigned int position){
	return getFile(position);
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>ofDirectory::getFiles(){
	return files;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::getShowHidden(){
	return showHidden;
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::reset(){
	close();
}

//------------------------------------------------------------------------------------------------------------
bool natural(const ofFile& a, const ofFile& b) {
	string aname = a.getBaseName(), bname = b.getBaseName();
	int aint = ofToInt(aname), bint = ofToInt(bname);
	if(ofToString(aint) == aname && ofToString(bint) == bname) {
		return aint < bint;
	} else {
		return a < b;
	}
}

//------------------------------------------------------------------------------------------------------------
void ofDirectory::sort(){
	ofSort(files, natural);
}

//------------------------------------------------------------------------------------------------------------
unsigned int ofDirectory::size(){
	return files.size();
}

//------------------------------------------------------------------------------------------------------------
int ofDirectory::numFiles(){
	return size();
}

//------------------------------------------------------------------------------------------------------------
// ofDirectory Static Methods
//------------------------------------------------------------------------------------------------------------

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::removeDirectory(string path, bool deleteIfNotEmpty, bool bRelativeToData){
	if(bRelativeToData){
		path = ofToDataPath(path);
	}
	File file(path);
	try{
		file.remove(deleteIfNotEmpty);
	}
	catch(Poco::Exception & except){
		ofLogError("ofDirectory") << "deleteDirectory(): unable to delete directory \"" << path << "\": " << except.displayText();
		return false;
	}
	return true;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::createDirectory(string dirPath, bool bRelativeToData, bool recursive){
	if(bRelativeToData){
		dirPath = ofToDataPath(dirPath);
	}

	File file(dirPath);
	bool success = false;
	try{
		if(!recursive){
			success = file.createDirectory();
		}
		else{
			file.createDirectories();
			success = true;
		}
	}
	catch(Poco::Exception & except){
		ofLogError("ofDirectory") << "createDirectory(): couldn't create directory \"" << dirPath << "\": " << except.displayText();
		return false;
	}

	if(!success){
		ofLogWarning("ofDirectory") << "createDirectory(): directory already exists: \"" << dirPath << "\"";
		success = true;
	}

	return success;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::doesDirectoryExist(string dirPath, bool bRelativeToData){
	if(bRelativeToData){
		dirPath = ofToDataPath(dirPath);
	}
	File file(dirPath);
	return file.exists();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::isDirectoryEmpty(string dirPath, bool bRelativeToData){
	if(bRelativeToData){
		dirPath = ofToDataPath(dirPath);
	}
	File file(dirPath);
	if(!dirPath.empty() && file.exists() && file.isDirectory()){
		vector<string>contents;
		file.list(contents);
		if(contents.size() == 0){
			return true;
		}
	}
	return false;
}

//------------------------------------------------------------------------------------------------------------
Poco::File & ofDirectory::getPocoFile(){
	return myDir;
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::operator==(const ofDirectory & dir){
	return getAbsolutePath() == dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::operator!=(const ofDirectory & dir){
	return getAbsolutePath() != dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::operator<(const ofDirectory & dir){
	return getAbsolutePath() < dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::operator<=(const ofDirectory & dir){
	return getAbsolutePath() <= dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::operator>(const ofDirectory & dir){
	return getAbsolutePath() > dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
bool ofDirectory::operator>=(const ofDirectory & dir){
	return getAbsolutePath() >= dir.getAbsolutePath();
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>::iterator ofDirectory::begin(){
	return files.begin();
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>::iterator ofDirectory::end(){
	return files.end();
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>::const_iterator ofDirectory::begin() const{
	return files.begin();
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>::const_iterator ofDirectory::end() const{
	return files.end();
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>::reverse_iterator ofDirectory::rbegin(){
	return files.rbegin();
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>::reverse_iterator ofDirectory::rend(){
	return files.rend();
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>::const_reverse_iterator ofDirectory::rbegin() const{
	return files.rbegin();
}

//------------------------------------------------------------------------------------------------------------
vector<ofFile>::const_reverse_iterator ofDirectory::rend() const{
	return files.rend();
}


//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
// -- ofFilePath
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------


//------------------------------------------------------------------------------------------------------------
string ofFilePath::addLeadingSlash(string path){
	if(path.length() > 0){
		if(path[0] != '/'){
			path = "/" + path;
		}
	}
	return path;
}

//------------------------------------------------------------------------------------------------------------
string ofFilePath::addTrailingSlash(string path){
	if(path.length() > 0){
		if(path[path.length() - 1] != '/'){
			path += "/";
		}
	}
	return path;
}


//------------------------------------------------------------------------------------------------------------
string ofFilePath::getFileExt(string filename){
	std::string::size_type idx;
	idx = filename.rfind('.');

	if(idx != std::string::npos){
		return filename.substr(idx + 1);
	}
	else{
		return "";
	}
}

//------------------------------------------------------------------------------------------------------------
string ofFilePath::removeExt(string filename){
	std::string::size_type idx;
	idx = filename.rfind('.');

	if(idx != std::string::npos){
		return filename.substr(0, idx);
	}
	else{
		return filename;
	}
}


//------------------------------------------------------------------------------------------------------------
string ofFilePath::getPathForDirectory(string path){
	// if a trailing slash is missing from a path, this will clean it up
	// if it's a windows-style "\" path it will add a "\"
	// if it's a unix-style "/" path it will add a "/"
	return Path::forDirectory(path).toString();
}

//------------------------------------------------------------------------------------------------------------
string ofFilePath::removeTrailingSlash(string path){
	if(path.length() > 0 && (path[path.length() - 1] == '/' || path[path.length() - 1] == '\\')){
		path = path.substr(0, path.length() - 1);
	}
	return path;
}


//------------------------------------------------------------------------------------------------------------
string ofFilePath::getFileName(string filePath, bool bRelativeToData){
	if(bRelativeToData){
		filePath = ofToDataPath(filePath);
	}

	string fileName;

	Path myPath(filePath);
	try{
		fileName = myPath.getFileName();
	}
	catch(Poco::Exception & except){
		return "";
	}

	return fileName;
}

//------------------------------------------------------------------------------------------------------------
string ofFilePath::getBaseName(string filePath){
	return removeExt(getFileName(filePath));
}

//------------------------------------------------------------------------------------------------------------
string ofFilePath::getEnclosingDirectory(string filePath, bool bRelativeToData){
	if(bRelativeToData){
		filePath = ofToDataPath(filePath);
	}

	Path myPath(filePath);

	return myPath.parent().toString();
}

//------------------------------------------------------------------------------------------------------------
bool ofFilePath::createEnclosingDirectory(string filePath, bool bRelativeToData, bool bRecursive) {
	return ofDirectory::createDirectory(ofFilePath::getEnclosingDirectory(filePath), bRelativeToData, bRecursive);
}


//------------------------------------------------------------------------------------------------------------
string ofFilePath::getAbsolutePath(string path, bool bRelativeToData){
	if(bRelativeToData){
		path = ofToDataPath(path);
	}

	Path myPath(path);

	return myPath.makeAbsolute().toString();
}


//------------------------------------------------------------------------------------------------------------
bool ofFilePath::isAbsolute(string path){
	Path p(path);
	return p.isAbsolute();
}

//------------------------------------------------------------------------------------------------------------
string ofFilePath::getCurrentWorkingDirectory(){
	#ifdef TARGET_OSX
		char pathOSX[FILENAME_MAX];
		uint32_t size = sizeof(pathOSX);
		if(_NSGetExecutablePath(pathOSX, &size) != 0){
			ofLogError("ofFilePath") << "getCurrentWorkingDirectory(): path buffer too small, need size " <<  size;
		}
		string pathOSXStr = string(pathOSX);
		string pathWithoutApp;
		size_t found = pathOSXStr.find_last_of("/");
		pathWithoutApp = pathOSXStr.substr(0, found);
		return pathWithoutApp;
	#else
		return Path::current();
	#endif
}

string ofFilePath::join(string path1, string path2){
	return removeTrailingSlash(path1) + addLeadingSlash(path2);
}

string ofFilePath::getCurrentExePath(){
	#if defined(TARGET_LINUX) || defined(TARGET_ANDROID)
		char buff[FILENAME_MAX];
		ssize_t size = readlink("/proc/self/exe", buff, sizeof(buff) - 1);
		if (size == -1){
			ofLogError("ofFilePath") << "getCurrentExePath(): readlink failed with error " << errno;
		}
		else{
			buff[size] = '\0';
			return buff;
		}
	#elif defined(TARGET_OSX)
		char path[FILENAME_MAX];
		uint32_t size = sizeof(path);
		if(_NSGetExecutablePath(path, &size) != 0){
			ofLogError("ofFilePath") << "getCurrentExePath(): path buffer too small, need size " <<  size;
		}
		return path;
	#elif defined(TARGET_WIN32)
		vector<char> executablePath(MAX_PATH);
		DWORD result = ::GetModuleFileNameA(NULL, &executablePath[0], static_cast<DWORD>(executablePath.size()));
		if(result == 0) {
			ofLogError("ofFilePath") << "getCurrentExePath(): couldn't get path, GetModuleFileNameA failed";
		}else{
			return string(executablePath.begin(), executablePath.begin() + result);
		}
	#endif
	return "";
}

string ofFilePath::getCurrentExeDir(){
	return getEnclosingDirectory(getCurrentExePath(), false);
}

string ofFilePath::getUserHomeDir(){
	#ifdef TARGET_WIN32
		// getenv will return any Environent Variable on Windows
		// USERPROFILE is the key on Windows 7 but it might be HOME
		// in other flavours of windows...need to check XP and NT...
		return string(getenv("USERPROFILE"));
	#elif !defined(TARGET_EMSCRIPTEN)
		struct passwd * pw = getpwuid(getuid());
		return pw->pw_dir;
	#else
		return "";
	#endif
}
