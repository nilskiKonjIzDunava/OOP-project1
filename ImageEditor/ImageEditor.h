#ifndef _IMAGE_EDITOR_H_
#define _IMAGE_EDITOR_H_

#include<string>
#include <math.h>
#include<iostream>


using namespace std;



struct Pixel {
	unsigned char b;
	unsigned char g;
	unsigned char r;
	int noColor; // 0 stavljamo kada piksel nije prazan,
	             // pri pravljenju lejera stavljamo na 1

	Pixel() :b(0), g(0), r(0), noColor(1) {};
	~Pixel() 
	{
		b = g = r =noColor= 0;

	};

};

struct Layer {
	int w;
	int h;
	unsigned int opacity;
	Pixel **image;
	Layer *next;
	Layer(int width, int height, Pixel** pix = nullptr, Layer* nxt = nullptr)
	{
		w = width;
		h = height;
		image = pix;
		next = nxt;
		opacity = 100;
	}
};

struct outImagePixel {
	unsigned char b;
	unsigned char g;
	unsigned char r;
	int leftPercentage;
};


class ImageEditor {
public:
	//ImageEditor():name("") , width(0), height(0), leap(0), skip(0), numOfLayers(0), activeBlue(0), activeGreen(0), activeRed(0) {};
	ImageEditor();
	~ImageEditor();

	bool loadImage(unsigned char* image);
	Pixel** makeEmptyMatrix(int width, int height);
	Layer* reverse_iterative(Layer* head);
	void deallocateMatrix(Pixel** pixi);                   //uzuma dimenzije pre kropovanja, nemoj da se zeznes

	void addLayer();
	void deleteLayer();
	void selectLayer(int i);
	void setLayerOpacity(int i);
	void invertColors();
	void toGrayScale();
	void blur(int size);
	void flipHorizontal();
	void flipVertical();
	void crop(int x, int y, int w, int h);
	void setActiveColor(string hex);
	void fillRect(int x, int y, int w, int h);
	void eraseRect(int x, int y, int w, int h);
	unsigned char* saveImage();

	int getWidth() { return this->width; };
	int getHeight() { return this->height; };
	string getNAme() { return this->name; };
private:
	
	int extractDim(unsigned char* niska, int pos);
	//int skipSpaces(unsigned char* niska, int i);

	// lista lejera koji dodatno imaju pokazivac na akrivni lejer
	//v2 je da first i active budu pokazivaci na lejer
	Layer* first;
	Layer* active;
	// ne racuna i bazni sloj
	int numOfLayers;
	unsigned char activeRed;
	unsigned char activeGreen;
	unsigned char activeBlue;

	string name;
	int width;
	int height;
	int leap;
	// ovo sluzi da znamo koliko mesta da ostavimo posle naziva, do prve lokacije deljive sa 4
	// sluzi za izlaznu nisku
	int skip;
	
	
	
};
#endif // !_IMAGE_EDITOR_H_

