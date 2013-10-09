

class GamePage: public Poco::Runnable {
public:

	Singleton* singleton;

	int thisPageId;
	int usingPoolId;
	PooledResource* gpuRes;

	int threshVal;

	int bufferedPageSizeInUnits;
	FIVector4 offsetInUnits;
	int iVolumeSize;
	

	uint* volData;
	uint* volDataLinear;
	

	int paramsPerEntry;
	int numEntries;
	int totParams;
	float* paramArr;

	int maxHeightInUnits;
	int totLenO2;
	int totLenVisO2;

	FIVector4 worldSeed;

	bool isDirty;

	bool threadRunning;

	float unitSizeInPixels;

	E_STATES curState;
	E_STATES nextState;

	FIVector4 worldMinVisInPixels;
	FIVector4 worldMaxVisInPixels;
	FIVector4 worldMinBufInPixels;
	FIVector4 worldMaxBufInPixels;


	FIVector4 worldUnitMin;
	FIVector4 worldUnitMax;


	E_FILL_STATE fillState;

	GamePage() {

	}



	void init(Singleton* _singleton, int _thisPageId, FIVector4* _offsetInUnits) {
		thisPageId = _thisPageId;
		singleton = _singleton;
		usingPoolId = -1;


		int i;

		
		threshVal = 140;
		threadRunning = false;

		paramsPerEntry = 18;
		numEntries = singleton->gameGeom.size();
		totParams = numEntries*paramsPerEntry;
		

		paramArr = new float[totParams];


		maxHeightInUnits = (singleton->maxHeightInUnits);

		isDirty = false;

		fillState = E_FILL_STATE_PARTIAL;

		curState = E_STATE_INIT_BEG;
		nextState = E_STATE_WAIT;





		bufferedPageSizeInUnits = (singleton->visPageSizeInUnits) * (singleton->bufferMult);
		offsetInUnits.copyFrom(_offsetInUnits);

		unitSizeInPixels = (float)(singleton->unitSizeInPixels);


		worldSeed.copyFrom(&(singleton->worldSeed));



		iVolumeSize = bufferedPageSizeInUnits*bufferedPageSizeInUnits*bufferedPageSizeInUnits;
		volData = new uint[iVolumeSize];
		for (i = 0; i < iVolumeSize; i++) {
			volData[i] = 0;
		}

		volDataLinear = new uint[iVolumeSize];
		for (i = 0; i < iVolumeSize; i++) {
			volDataLinear[i] = (255<<24)|(255<<16)|(255<<8)|(0);
		}


		totLenO2 = bufferedPageSizeInUnits/2;
		totLenVisO2 = bufferedPageSizeInUnits/(2*(singleton->bufferMult));



		worldMinVisInPixels.copyFrom(&offsetInUnits);
		worldMaxVisInPixels.copyFrom(&offsetInUnits);
		worldMinVisInPixels.addXYZ( -totLenVisO2 );
		worldMaxVisInPixels.addXYZ(  totLenVisO2 );
		worldMinVisInPixels.multXYZ((float)unitSizeInPixels);
		worldMaxVisInPixels.multXYZ((float)unitSizeInPixels);

		worldMinBufInPixels.copyFrom(&offsetInUnits);
		worldMaxBufInPixels.copyFrom(&offsetInUnits);
		worldMinBufInPixels.addXYZ( -totLenO2 );
		worldMaxBufInPixels.addXYZ(  totLenO2 );
		worldMinBufInPixels.multXYZ((float)unitSizeInPixels);
		worldMaxBufInPixels.multXYZ((float)unitSizeInPixels);



		worldUnitMin.copyFrom(&offsetInUnits);
		worldUnitMax.copyFrom(&offsetInUnits);
		worldUnitMin.addXYZ( -totLenO2 );
		worldUnitMax.addXYZ(  totLenO2 );

		

		curState = E_STATE_INIT_END;
	}

	uint NumberOfSetBits(uint i)
	{
	    i = i - ((i >> 1) & 0x55555555);
	    i = (i & 0x33333333) + ((i >> 2) & 0x33333333);
	    return (((i + (i >> 4)) & 0x0F0F0F0F) * 0x01010101) >> 24;
	}

	uint clamp(uint val) {
		if (val > 255) {
			val = 255;
		}
		if (val < 0) {
			val = 0;
		}
		return val;
	}
	




	void createSimplexNoise() {

		threadRunning = true;

		// REMINDER: DO NOT ACCESS EXTERNAL POINTERS INSIDE THREAD

		bool isBlank = false;
		bool isFull = false;
		

		curState = E_STATE_CREATESIMPLEXNOISE_BEG;

		int i, j, k, m;

		int totLen = bufferedPageSizeInUnits;

		float fTotLen = (float)totLen;

		int ind = 0;

		uint tmp;

		float fx, fy, fz;

		uint randOff[3];
		float ijkVals[3];

		const float RAND_MOD[9] = {
			3456.0f, 5965.0f, 45684.0f,
			4564.0f, 1234.0f, 6789.0f,
			4567.0f, 67893.0f, 13245.0f
		};

		float totLenO4 = totLen/4;
		float totLen3O4 = (totLen*3)/4;
		float fSimp;
		float heightThresh;
		float testVal;

		bool mustNotBeFull = false;

		for (j = 0; j < totLen; j++) {

			ijkVals[1] = (float)j;

			fy = (j - totLenO2) + offsetInUnits.getFY();

			for (i = 0; i < totLen; i++) {

				ijkVals[0] = (float)i;

				fx = (i - totLenO2) + offsetInUnits.getFX();
				
				for (k = 0; k < totLen; k++) {

					ijkVals[2] = (float)k;

					fz = (k - totLenO2) + offsetInUnits.getFZ();

					ind = k*totLen*totLen + j*totLen + i;

					
					
					if (fx < 0.0f || fy < 0.0f || fz < 0.0f ) {
						volData[ind] = 0;
						volDataLinear[ind] = (255<<16)|(255<<8)|255;
					}
					else {

						heightThresh = (fz/ ((float)maxHeightInUnits) );
						if (heightThresh > 1.0f) {
							heightThresh = 1.0f;
						}
						if (heightThresh < 0.0f) {
							heightThresh = 0.0f;
						}



						if (k + offsetInUnits.getIZ() >= maxHeightInUnits) {
							tmp = 0;
							mustNotBeFull = true;
						}
						else {
							testVal = simplexScaledNoise(
												4.0f, //octaves
												0.5f, //persistence (amount added in each successive generation)
												1.0f/32.0f, //scale (frequency)
												0.0f,
												1.0f,
												fx+worldSeed.getFX(),
												fy+worldSeed.getFY(),
												fz+worldSeed.getFZ()
											);
							

							
							tmp = clamp(testVal*255.0f*(1.0f-heightThresh*heightThresh*heightThresh));
						}

						

						if ( i >= totLenO4 && i <= totLen3O4 ) {
							if ( j >= totLenO4 && j <= totLen3O4 ) {
								if ( k >= totLenO4 && k <= totLen3O4 ) {
									if (tmp > threshVal) {
										isBlank = false;
									}
									else {
										isFull = false;
									}
								}
							}
						}

						
						


						for (m = 0; m < 3; m++) {
							fSimp = simplexScaledNoise(
																		1.0f, //octaves
																		1.0f, //persistence (amount added in each successive generation)
																		1.0f/4.0, //scale (frequency)
																		0.0f,
																		1.0f,
																		fx+RAND_MOD[m*3+0],
																		fy+RAND_MOD[m*3+1],
																		fz+RAND_MOD[m*3+2]
																	);
							randOff[m] = clamp( ( fSimp + ijkVals[m])*255.0f/fTotLen);
							


						}

						if ( (tmp%16 > 5) && ( (i+j+k)%2 == 0) ) {

							/*if (randOff[0] == 0 && randOff[1] == 0 && randOff[2] == 0) {
								randOff[1] = 1;
							} */

							volData[ind] = (0)|(randOff[2]<<16)|(randOff[1]<<8)|randOff[0];
							volDataLinear[ind] = (tmp<<24)|(255<<16)|(255<<8)|255;
						}
						else {
							volData[ind] = (0);
							volDataLinear[ind] = (tmp<<24)|(255<<16)|(255<<8)|255;;
						}

						
					}
					
					

					
				}
			}
		}



		/*
		if (mustNotBeFull) {
			isFull = false;
		}

		if (isBlank || isFull ) {

			if (isBlank) {
				fillState = E_FILL_STATE_EMPTY;
			}
			if (isFull) {
				fillState = E_FILL_STATE_FULL;
			}

			curState = E_STATE_LENGTH;
		}
		else {*/

			fillState = E_FILL_STATE_PARTIAL;
			curState = E_STATE_CREATESIMPLEXNOISE_END;
		//}

		threadRunning = false;

	}

	void unbindGPUResources() {
		usingPoolId = -1;
		curState = E_STATE_CREATESIMPLEXNOISE_END;
	}

	void copyToTexture() {

		curState = E_STATE_COPYTOTEXTURE_BEG;


		if (usingPoolId == -1) {
			usingPoolId = singleton->requestPoolId(thisPageId);
			gpuRes = singleton->pagePoolItems[usingPoolId];
		}

		

		glBindTexture(GL_TEXTURE_3D,gpuRes->volID);
			glTexSubImage3D(
				GL_TEXTURE_3D,
				0,
				
				0,
				0,
				0,

				bufferedPageSizeInUnits,
				bufferedPageSizeInUnits,
				bufferedPageSizeInUnits,

				GL_RGBA,
				GL_UNSIGNED_BYTE,

				volData
			);

		glBindTexture(GL_TEXTURE_3D,0);
		glBindTexture(GL_TEXTURE_3D,gpuRes->volIDLinear);
			glTexSubImage3D(
				GL_TEXTURE_3D,
				0,
				
				0,
				0,
				0,

				bufferedPageSizeInUnits,
				bufferedPageSizeInUnits,
				bufferedPageSizeInUnits,

				GL_RGBA,
				GL_UNSIGNED_BYTE,

				volDataLinear
			);
		glBindTexture(GL_TEXTURE_3D,0);

		

		curState = E_STATE_COPYTOTEXTURE_END;

	}

	void generateVolume() {

		int i;
		int baseInd;

		curState = E_STATE_GENERATEVOLUME_BEG;
		
		
		if (usingPoolId == -1) {

		}
		else {

			// TODO: one shader, set flag

			if (singleton->isBare) {
				singleton->bindShader("GenerateVolumeBare");
			}
			else {
				singleton->bindShader("GenerateVolume");
			}



			for (i = 0; i < numEntries; i++) {
				baseInd = i*paramsPerEntry;

				paramArr[baseInd + 0] = singleton->gameGeom[i]->boundsMinInPixels.getFX();
				paramArr[baseInd + 1] = singleton->gameGeom[i]->boundsMinInPixels.getFY();
				paramArr[baseInd + 2] = singleton->gameGeom[i]->boundsMinInPixels.getFZ();

				paramArr[baseInd + 3] = singleton->gameGeom[i]->boundsMaxInPixels.getFX();
				paramArr[baseInd + 4] = singleton->gameGeom[i]->boundsMaxInPixels.getFY();
				paramArr[baseInd + 5] = singleton->gameGeom[i]->boundsMaxInPixels.getFZ();

				paramArr[baseInd + 6] = singleton->gameGeom[i]->originInPixels.getFX();
				paramArr[baseInd + 7] = singleton->gameGeom[i]->originInPixels.getFY();
				paramArr[baseInd + 8] = singleton->gameGeom[i]->originInPixels.getFZ();

				paramArr[baseInd + 9] = singleton->gameGeom[i]->powerVals.getFX();
				paramArr[baseInd + 10] = singleton->gameGeom[i]->powerVals.getFY();
				paramArr[baseInd + 11] = singleton->gameGeom[i]->powerVals.getFZ();

				paramArr[baseInd + 12] = singleton->gameGeom[i]->coefficients.getFX();
				paramArr[baseInd + 13] = singleton->gameGeom[i]->coefficients.getFY();
				paramArr[baseInd + 14] = singleton->gameGeom[i]->coefficients.getFZ();

				paramArr[baseInd + 15] = singleton->gameGeom[i]->minMaxMat.getFX();
				paramArr[baseInd + 16] = singleton->gameGeom[i]->minMaxMat.getFY();
				paramArr[baseInd + 17] = singleton->gameGeom[i]->minMaxMat.getFZ();
				
			}


			singleton->bindFBO("volGenFBO");
			singleton->setShaderTexture3D(gpuRes->volID, 0);
			singleton->setShaderTexture3D(gpuRes->volIDLinear, 1);
			singleton->setShaderTexture(singleton->lookup2to3ID, 2);

			
			singleton->setShaderFloat("bufferedPageSizeInUnits", bufferedPageSizeInUnits);
			singleton->setShaderFloat("threshVal", (float)threshVal);

			singleton->setShaderFloat("bufferMult", (float)(singleton->bufferMult));
			singleton->setShaderfVec3("worldMinVisInPixels", &(worldMinVisInPixels));
			singleton->setShaderfVec3("worldMaxVisInPixels", &(worldMaxVisInPixels));
			singleton->setShaderfVec3("worldMinBufInPixels", &(worldMinBufInPixels));
			singleton->setShaderfVec3("worldMaxBufInPixels", &(worldMaxBufInPixels));

			singleton->setShaderFloat("paramsPerEntry", (float)(paramsPerEntry/3) );
			singleton->setShaderFloat("numEntries", (float)numEntries);
			singleton->setShaderArrayfVec3("paramArr", paramArr, totParams/3);

			singleton->drawFSQuad(1.0f);

			singleton->setShaderTexture3D(0, 0);
			singleton->setShaderTexture3D(0, 1);
			singleton->setShaderTexture(0, 2);

			singleton->unbindFBO();
			singleton->unbindShader();

			//ray trace new texture, generate normals, AO, depth, etc
			singleton->bindShader("RenderVolume");
			singleton->bindFBODirect(gpuRes->fboSet);
			singleton->sampleFBO("volGenFBO");
			glClearColor(0.0f,0.0f,0.0f,0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			singleton->setShaderFloat("bufferMult", (float)(singleton->bufferMult));
			singleton->setShaderfVec3("worldMinVisInPixels", &(worldMinVisInPixels));
			singleton->setShaderfVec3("worldMaxVisInPixels", &(worldMaxVisInPixels));
			singleton->setShaderfVec3("worldMinBufInPixels", &(worldMinBufInPixels));
			singleton->setShaderfVec3("worldMaxBufInPixels", &(worldMaxBufInPixels));


			
			

			glCallList(singleton->volTris);
			singleton->unsampleFBO("volGenFBO");
			singleton->unbindFBO();
			singleton->unbindShader();
		}

		curState = E_STATE_GENERATEVOLUME_END;
	}

	~GamePage() {

		if (volData) {
			delete[] volData;
		}
		if (volDataLinear) {
			delete[] volDataLinear;
		}
	}




	void run() {
		switch (nextState) {
			case E_STATE_CREATESIMPLEXNOISE_LAUNCH:
				createSimplexNoise();
			break;

			default:

			break;
		}
	}

};