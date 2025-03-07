//TODO: Add edge flags?

#pragma once
#include "ofConstants.h"
#include "ofVec3f.h"
#include "ofColor.h"
#include "ofUtils.h"
#include "ofMesh.h"
#include "ofGLUtils.h"
#include <map>

class ofVbo {
public:
	
	ofVbo();
	ofVbo(const ofVbo & mom);
	ofVbo & operator=(const ofVbo& mom);
	~ofVbo();

	void setMesh(const ofMesh & mesh, int usage);
	void setMesh(const ofMesh & mesh, int usage, bool useColors, bool useTextures, bool useNormals);
	
	void setVertexData(const ofVec3f * verts, int total, int usage);
	void setVertexData(const ofVec2f * verts, int total, int usage);

	void setColorData(const ofFloatColor * colors, int total, int usage);
	void setNormalData(const ofVec3f * normals, int total, int usage);	
	void setTexCoordData(const ofVec2f * texCoords, int total, int usage);
	void setIndexData(const ofIndexType * indices, int total, int usage);

	void setVertexData(const float * vert0x, int numCoords, int total, int usage, int stride=0);
	void setColorData(const float * color0r, int total, int usage, int stride=0);
	void setNormalData(const float * normal0x, int total, int usage, int stride=0);
	void setTexCoordData(const float * texCoord0x, int total, int usage, int stride=0);
	
	void setAttributeData(int location, const float * vert0x, int numCoords, int total, int usage, int stride=0);

	void updateMesh(const ofMesh & mesh);

	void updateVertexData(const ofVec3f * verts, int total);
	void updateVertexData(const ofVec2f * verts, int total);
	void updateColorData(const ofFloatColor * colors, int total);
	void updateNormalData(const ofVec3f * normals, int total);	
	void updateTexCoordData(const ofVec2f * texCoords, int total);
	void updateIndexData(const ofIndexType * indices, int total);
	
	void updateVertexData(const float * ver0x, int total);
	void updateColorData(const float * color0r, int total);
	void updateNormalData(const float * normal0x, int total);
	void updateTexCoordData(const float * texCoord0x, int total);
	
	void updateAttributeData(int location, const float * vert0x, int total);

	void enableColors();
	void enableNormals();
	void enableTexCoords();
	void enableIndices();

	void disableColors();
	void disableNormals();
	void disableTexCoords();
	void disableIndices();

	GLuint getVertId() const;
	GLuint getColorId() const;
	GLuint getNormalId() const;
	GLuint getTexCoordId() const;
	GLuint getIndexId() const;
	
	bool getIsAllocated() const;
	bool getUsingVerts() const;
	bool getUsingColors() const;
	bool getUsingNormals() const;
	bool getUsingTexCoords() const;
	bool getUsingIndices() const;
	
	void draw(int drawMode, int first, int total) const;
	void drawElements(int drawMode, int amt) const;
	
	void drawInstanced(int drawMode, int first, int total, int primCount) const;
	void drawElementsInstanced(int drawMode, int amt, int primCount) const;
	
	void bind() const;
	void unbind() const;

	void clear();

	void clearVertices();
	void clearNormals();
	void clearColors();
	void clearTexCoords();
	void clearIndices();

	int getNumVertices() const;
	int getNumIndices() const;
	

	static void disableVAOs();
	static void enableVAOs();

private:
	GLuint vaoID;
	mutable bool vaoChanged;

	GLuint indexId;

	GLuint vertId;
	GLuint colorId;
	GLuint normalId;
	GLuint texCoordId;

	bool bAllocated;

	mutable bool bUsingVerts;		// need at least vertex data
	mutable bool bUsingTexCoords;
	mutable bool bUsingColors;
	mutable bool bUsingNormals;
	mutable bool bUsingIndices;

	GLsizei vertStride;
	GLsizei colorStride;
	GLsizei normalStride;
	GLsizei texCoordStride;

	int		vertSize;
	int		totalVerts;
	int		totalIndices;

	int vertUsage;
	int colorUsage;
	int normUsage;
	int texUsage;

	mutable bool bBound;

	map<int,GLuint> attributeIds;
	map<int,int> attributeSize;
	map<int,int> attributeStrides;
	map<int,int> attributeNumCoords;

	static bool vaoChecked;
	static bool supportVAOs;
};
