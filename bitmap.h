#ifndef bitmap_h
#define bitmap_h

#include "colorObj.h"

class bitmap {

	public :
				bitmap(void);
				bitmap(int width,int height,bool alpha=false);
				bitmap(bitmap* aBitmap);
	virtual	~bitmap(void);

				bool			setSize(int width,int height,bool alpha=false);
				void			clearMap(void);
				bool			getHasMap(void);
				int			getWidth(void);
				int			getHeight(void);
				void			setColor(int x,int y,colorObj* aColor);
				void			setColor(int x,int y,RGBpack* aColor);
				void			setAlpha(int x,int y,byte alpha);
				colorObj		getColor(int x,int y);
				RGBpack		getColorPack(int x,int y);
				byte			getAlpha(int x,int y);
				RGBpack*		getBitmapAddr(void);
				byte*			getAlphaAddr(void);
				
  protected:
      
				int			mWidth;
				int			mHeight;
				bool			mHaveMap;
				RGBpack*		mBitmap;
				byte*			mAlpha;
};


#endif
