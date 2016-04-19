#include "includes.h"

#ifndef __PERLIN_H_INCLUDED__
#define __PERLIN_H_INCLUDED__

class perlin_2d{
private:
	

public:

	float fade(float t) {                                   // Fade function as defined by Ken Perlin.  This eases coordinate values
         return t * t * t * (t * (t * 6 - 15) + 10);		// so that they will ease towards integral values.  This ends up smoothing
	}														// the final output.
															// 6t^5 - 15t^4 + 10t^3

	// NOT WORKING //

	float perlin(int posX, int posY){
		//possibly shouldn't use fade in this manner
		float temp_0_0 = fade(dotProduct(gradients[int(float(posX)/gradientChunkSize)][int(float(posY)/gradientChunkSize)], XMFLOAT2(float(posX)/gradientChunkSize, float(posY)/gradientChunkSize)));
		float temp_1_0 = fade(dotProduct(gradients[int(float(posX)/gradientChunkSize)+1][int(float(posY)/gradientChunkSize)], XMFLOAT2(1.0f-float(posX)/gradientChunkSize, float(posY)/gradientChunkSize)));
		float temp_1_1 = fade(dotProduct(gradients[int(float(posX)/gradientChunkSize)+1][int(float(posY)/gradientChunkSize)+1], XMFLOAT2(1.0f-float(posX)/gradientChunkSize, 1.0f-float(posY)/gradientChunkSize)));
		float temp_0_1 = fade(dotProduct(gradients[int(float(posX)/gradientChunkSize)][int(float(posY)/gradientChunkSize)+1], XMFLOAT2(float(posX)/gradientChunkSize, 1.0f-float(posY)/gradientChunkSize)));

		//float temp_0_0 = dotProduct(gradients[int(float(posX)/gradientChunkSize)][int(float(posY)/gradientChunkSize)], XMFLOAT2(float(posX)/gradientChunkSize, float(posY)/gradientChunkSize));
		//float temp_1_0 = dotProduct(gradients[int(float(posX)/gradientChunkSize)+1][int(float(posY)/gradientChunkSize)], XMFLOAT2(1.0f-float(posX)/gradientChunkSize, float(posY)/gradientChunkSize));
		//float temp_1_1 = dotProduct(gradients[int(float(posX)/gradientChunkSize)+1][int(float(posY)/gradientChunkSize)+1], XMFLOAT2(1.0f-float(posX)/gradientChunkSize, 1.0f-float(posY)/gradientChunkSize));
		//float temp_0_1 = dotProduct(gradients[int(float(posX)/gradientChunkSize)][int(float(posY)/gradientChunkSize)+1], XMFLOAT2(float(posX)/gradientChunkSize, 1.0f-float(posY)/gradientChunkSize));

		return temp_0_0*(1.0f-float(posX)/gradientChunkSize)*(1.0f-float(posY)/gradientChunkSize)
			+ temp_1_0*(float(posX)/gradientChunkSize)*(1.0f-float(posY)/gradientChunkSize)
			+ temp_1_1*(float(posX)/gradientChunkSize)*(float(posY)/gradientChunkSize)
			+ temp_0_1*(1.0f-float(posX)/gradientChunkSize)*(float(posY)/gradientChunkSize);
	}

};

#endif