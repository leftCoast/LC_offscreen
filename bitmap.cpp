#include "bitmap.h"
#include "resizeBuff.h"
#include "colorObj.h"
#include "debug.h"

// Give back an initialzed but empty bitmap.
bitmap::bitmap(void) {

  mWidth    = 0;
  mHeight   = 0;
  mBitmap   = NULL;
  mAlpha		= NULL;
  mHaveMap  = false;
}


// Give back a fully functonal bitmap. If it can get the RAM that is.
bitmap::bitmap(int width,int height,bool alpha) {
  
  mBitmap   = NULL;
  mAlpha		= NULL;
  mHaveMap  = false;
  setSize(width,height,alpha);   
}


// Create ourselves as a clone of another bitmap. Ju like all bitmaps, if it doesn't
// work? We ill have all the check methods working. So you can tell if it didn't make it.
bitmap::bitmap(bitmap* aBitmap) {

	byte* theirMap;
	byte* ourMap;
	int	numBytes;
	int	tWidth;														
	int	tHeight;
	bool	tAlpha;
	
	mWidth	= 0;															// art off with the andard defaults.
	mHeight	= 0;
	mAlpha	= NULL;
	mBitmap	= NULL;
	mHaveMap	= false;
	if (aBitmap) {															// Sanity, did they even pass in a bitmap?
		if (aBitmap->getHasMap()) {									// Cool, lets see if it has a bitmap allocated?
			tWidth	= aBitmap->getWidth();							// Save off their width.
			tHeight	= aBitmap->getHeight();							// Save off their height.
			tAlpha	= aBitmap->getAlphaAddr()!=NULL;				// Save off.. Do they have an alpha channel?
			if (setSize(tWidth,tHeight,tAlpha)) {					// Ok, lets see if we can allocate one for us?
				ourMap = (byte*)mBitmap;								// Everything seems fine, point at our map as if its ju as array of bytes.
				theirMap = (byte*)aBitmap->getBitmapAddr();		// Point at their map as if its ju an array of bytes. Because really, they are.
				numBytes  = mWidth * mHeight * sizeof(RGBpack);	// Calculate the number of bytes in our map buffer.
				for(int i=0;i<numBytes;i++) {							// For every byte..
					ourMap[i] = theirMap[i];							// Copy theirs into ours.
				}
				if (tAlpha) {												// If they had an alpha channel..
					ourMap = (byte*)mAlpha;								// We have already allocated ours, point at it.
					theirMap = (byte*)aBitmap->getAlphaAddr();	// Get the address of theirs.
					numBytes  = mWidth * mHeight * sizeof(byte);	// Calculate the number of bytes in our alpha buffer.
					for(int i=0;i<numBytes;i++) {						// For every byte..
						ourMap[i] = theirMap[i];						// Copy theirs into ours.
					}
				}
			}
		}
	}
}

		
bitmap::~bitmap(void) {

	resizeBuff(0,(byte**)&mBitmap);
	resizeBuff(0,(byte**)&mAlpha);
}


// Copy down the bitmap size then allocate the buffer, if possible.
// This can also be used to reset to different sizes, But it causes
// all previous data to be lo when called.
bool bitmap::setSize(int width,int height,bool alpha) {

	int numBytes;
  
	mWidth    = 0;														// Not yet.
	mHeight   = 0;														// Allocate before assuming.
	numBytes  = width * height * sizeof(RGBpack);			// Calculate buffer size.
	mHaveMap  = resizeBuff(numBytes,(byte**)&mBitmap);		// Make a grab for the RAM.
	if (mHaveMap && alpha) {										// If we got the RAM and we need an alpha channel..
  		numBytes  = width * height * sizeof(byte);			// Calculate the buffer for alpha channel.
  		if (resizeBuff(numBytes,(byte**)&mAlpha)) {			// If we can also grab the RAM for the alpha channel..
  			for (int i=0;i<numBytes;i++) {						// Ju to be nice..
  				mAlpha[i] = 1;											// Default the alpa channel to opaque. (Good default)
  			}
  			mHaveMap = true;											// We are good to go!
  		} else {															// Else, we could NOT get the RAM for the alpha channel..
  			resizeBuff(0,(byte**)&mBitmap);						// Sadly, we dump the bitmap buffer.
  			mHaveMap = false;											// We have failed.
  		}
	}
	if (mHaveMap) {													// If we have been a success..
		mWidth    = width;											// Its this wide.
		mHeight   = height;											// Ans this tall.
	}
	return mHaveMap;													// Return our success. Or failure..
}


// Just dump everything. Probably need the memory elsewhere.
void bitmap::clearMap(void) {

	mWidth    = 0;
	mHeight   = 0;
	resizeBuff(0,(byte**)&mBitmap);
	resizeBuff(0,(byte**)&mAlpha);
	mHaveMap  = false;
}


// Caller wants to know if we have a map allocated.
bool bitmap::getHasMap(void) { return mHaveMap; }


// These two are for reading back our size..
int bitmap::getWidth(void) { return mWidth; }
int bitmap::getHeight(void) { return mHeight; }


// Set this location to a color. Actually ju a pass
// through to the color pack version.
void bitmap::setColor(int x,int y,colorObj* aColor) {

  RGBpack aCPack;

  if (aColor) {
    aCPack = aColor->packColor();
  }
  setColor(x,y,&aCPack);
}


// Set this location to a color. This is the method that actually
// writes to RAM. Hence, everything is sanity checked fir.
void  bitmap::setColor(int x,int y,RGBpack* aColor) {

  int index;

  if (x>=0 && x<mWidth && y>=0 && y<mHeight && mHaveMap && aColor) {
    index = y * mWidth + x;
    mBitmap[index] = *aColor;
  }
}


// Set this location's alpha channel. This is the method that actually
// writes to RAM. Hence, everything is sanity checked fir.
void  bitmap::setAlpha(int x,int y,byte alpha) {

  int index;

  if (x>=0 && x<mWidth && y>=0 && y<mHeight && mHaveMap && mAlpha) {
    index = y * mWidth + x;
    mAlpha[index] = alpha;
  }
}


// Grab a color object out of RAM. We need this fa so, no sanity checking.
// Give bad input? Get undefined output.
colorObj bitmap::getColor(int x,int y) {

  colorObj  aColor;
  RGBpack   aCPack;

  aCPack = getColorPack(x,y);
  aColor.setColor(&aCPack);
  return aColor;
}


// Grab a color pack out of RAM. Again, we need this fa so, no sanity checking.
// Give bad input? Get bad output.
RGBpack bitmap::getColorPack(int x,int y) {

  RGBpack aCPack;
  int     index;
  
  index = y * mWidth + x;
  aCPack = mBitmap[index];
  return aCPack;
}


byte bitmap::getAlpha(int x,int y) {

	int     index;
  	
  	if (mAlpha) {
		index = y * mWidth + x;
		return mAlpha[index];
	}
	return 1;
}


// This one is kinda' scary. We hand off the address to your bitmap buffer. I mean, I
// don't even like valet parking.
RGBpack* bitmap::getBitmapAddr(void) { return mBitmap; }


// And this one too. Here's the address to our alpha channel buffer.
byte*  bitmap::getAlphaAddr(void) { return mAlpha; }