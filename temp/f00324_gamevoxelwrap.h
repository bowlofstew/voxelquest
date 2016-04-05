// f00324_gamevoxelwrap.h
//

#include "f00324_gamevoxelwrap.e"
#define LZZ_INLINE inline
GameVoxelWrap::GameVoxelWrap ()
                        {
		lastFFSteps = 0;
		voxelBuffer = NULL;
		basePD = NULL;
		baseData = NULL;
		
		oneVec = vec3(1.0f,1.0f,1.0f);
		halfOff = vec3(0.5,0.5,0.5);
		crand0 = vec3(12.989, 78.233, 98.422);
		crand1 = vec3(93.989, 67.345, 54.256);
		crand2 = vec3(43.332, 93.532, 43.734);
	}
void GameVoxelWrap::init (Singleton * _singleton)
          {
		singleton = _singleton;
		//dimInVoxels = _dimInVoxels;
		// octInVoxels = dimInVoxels*2;
		
		// octOffsetInVoxels = ivec3(
		// 	(octInVoxels-dimInVoxels)/2,
		// 	(octInVoxels-dimInVoxels)/2,
		// 	(octInVoxels-dimInVoxels)/2	
		// );
		
		voxelsPerCell = singleton->voxelsPerCell;
		//fVoxelsPerCell = voxelsPerCell;
		cellsPerHolder = singleton->cellsPerHolder;
		cellsPerHolderPad = singleton->cellsPerHolderPad;
		paddingInCells = singleton->paddingInCells;
		paddingInVoxels = paddingInCells*voxelsPerCell;
		
		voxelsPerHolderPad = singleton->voxelsPerHolderPad;
		voxelsPerHolder = singleton->voxelsPerHolder;
		
		
		
	}
void GameVoxelWrap::fillVec (GamePageHolder * gph)
                                          {
		int totSize = voxelBuffer->voxelList.size();
		
		if (totSize <= 0) {
			return;
		}
		
		int q;
		int q2;
		int p;
		int p2;
		int m;
		float fVPC = voxelsPerCell;
		fVPC = 1.0f/fVPC;
		
		ivec3 voxOffset;
		ivec3 voxOffset2;
		ivec3 cellOffset;
		ivec3 localVoxOffset;
		
		//vec3 fLocalVoxOffset;
		vec3 fVO;
		
		
		int ii;
		int jj;
		int kk;
		
		int ii2;
		int jj2;
		int kk2;
		
		int ii3;
		int jj3;
		int kk3;
		
		int ind;
		
		int xx;
		int yy;
		int zz;
		
		uint curFlags;
		uint tempFlags;
		uint normFlags;
		
		float tempData[16];
		
		CubeWrap* curCW;
		
		int tempInd;
		int VLIndex;
		
		float normLength;
		
		vec3 curNorm;
		vec3 curNorm2;
		vec3 totNorm;
		vec3 tempPos;
		vec3 tempPos2;
		vec3 zeroVec = vec3(0.0f,0.0f,0.0f);
		
		float weight;
		float weight2;
		
		uint curNID;
		uint testNID;
		
		int voxelNormRad = singleton->iGetConst(E_CONST_VOXEL_NORM_RAD);
		int cellAORad = singleton->iGetConst(E_CONST_CELL_AO_RAD);
		float voxelAORad = voxelsPerCell*cellAORad;
		
		float frad = voxelNormRad;
		float maxRad = (frad*frad + frad*frad + frad*frad)*1.125f;
		
		int dataSize = 4;
		
		int passNum;
		
		int cellInd;
		int cellInd2;
		int curSize;
		
		float totWeight = 0.0f;
		float weightCount = 1.0f;
		
		int ww;
		int curMat = 0;
		
		if (DO_AO) {
			for (p = 0; p < totSize; p++) {
				q = voxelBuffer->voxelList[p].viIndex;
				kk = q/(voxelsPerHolderPad*voxelsPerHolderPad);
				jj = (q-kk*voxelsPerHolderPad*voxelsPerHolderPad)/voxelsPerHolderPad;
				ii = q-(kk*voxelsPerHolderPad*voxelsPerHolderPad + jj*voxelsPerHolderPad);
				
				if (voxelBuffer->getFlag(q,E_OCT_SURFACE)) {
					
					kk2 = kk/voxelsPerCell;
					jj2 = jj/voxelsPerCell;
					ii2 = ii/voxelsPerCell;
					
					cellInd = kk2*cellsPerHolderPad*cellsPerHolderPad + jj2*cellsPerHolderPad + ii2;
					
					voxelBuffer->cellLists[cellInd].indexArr[voxelBuffer->cellLists[cellInd].curSize] = p;
					voxelBuffer->cellLists[cellInd].curSize++;
					
					voxOffset.x = ii;
					voxOffset.y = jj;
					voxOffset.z = kk;
					//voxOffset -= paddingInVoxels;
					
					tempPos.x = voxOffset.x;
					tempPos.y = voxOffset.y;
					tempPos.z = voxOffset.z;
					
					voxelBuffer->voxelList[p].pos = tempPos;
					
				}
			}
		}
		
		
		int maxPass = 1;
		
		if (DO_AO) {
			maxPass = 2;
		}
		
		for (passNum = 0; passNum < maxPass; passNum++) {
			for (p = 0; p < totSize; p++) {
				q = voxelBuffer->voxelList[p].viIndex;
				kk = q/(voxelsPerHolderPad*voxelsPerHolderPad);
				jj = (q-kk*voxelsPerHolderPad*voxelsPerHolderPad)/voxelsPerHolderPad;
				ii = q-(kk*voxelsPerHolderPad*voxelsPerHolderPad + jj*voxelsPerHolderPad);
				
				if (voxelBuffer->getFlag(q,E_OCT_SURFACE)) {
					
					voxOffset.x = ii;
					voxOffset.y = jj;
					voxOffset.z = kk;
					//voxOffset -= paddingInVoxels;
					
					tempPos.x = voxOffset.x;
					tempPos.y = voxOffset.y;
					tempPos.z = voxOffset.z;
					
					
					
					
					if (inBounds(&voxOffset,paddingInVoxels,voxelsPerHolder+paddingInVoxels)) {
						
						
						curNID = voxelBuffer->voxelList[p].normId;
						curMat = voxelBuffer->voxelList[p].matId;
						
						
						
						curFlags = voxelBuffer->getFlags(q);
						
						switch (passNum) {
							case 0:
								
								totNorm.set(0.0f,0.0f,0.0f);
								
								for (zz = -voxelNormRad; zz <= voxelNormRad; zz++) {
									for (yy = -voxelNormRad; yy <= voxelNormRad; yy++) {
										for (xx = -voxelNormRad; xx <= voxelNormRad; xx++) {
											
											
											
											// if (
											// 	(zz == 0) && (yy == 0) && (xx == 0)	
											// ) {
												
											// }
											// else {
												
											// }
											
											tempInd = (zz+kk)*voxelsPerHolderPad*voxelsPerHolderPad + (yy+jj)*voxelsPerHolderPad + (xx + ii);
											tempFlags = voxelBuffer->getFlagsAtNode(tempInd);
											VLIndex = voxelBuffer->getIndAtNode(tempInd);
											
											if (VLIndex == -1) {
												testNID = 0;
											}
											else {
												testNID = voxelBuffer->voxelList[VLIndex].normId;
											}
											
											// if (p%1000 == 0) {
											// 	cout << "curNID " << curNID << "testNID" << testNID << "\n";
											// }
											
											
											
											// if (
											// 	((tempFlags&E_OCT_SURFACE) > 0)
											// 	&& (curNID == testNID)
											// ) {
											// 	normFlags = (tempFlags&63);
											// 	totNorm += BASE_NORMALS[normFlags];
											// }
											
											
											
											if ((tempFlags&E_OCT_SURFACE) > 0) {
												
												weight = maxRad-(xx*xx + yy*yy + zz*zz);
												
												normFlags = (tempFlags&63);
											
												if (curNID == testNID) {
													totNorm += BASE_NORMALS[normFlags]*weight;
												}
												else {
													totNorm -= BASE_NORMALS[normFlags]*0.75f*weight;
												}
											}
											
											// position(3)
											// normal(3)
											// material(1)
											// id(1)
											
											
											
										}
									}
								}
								
								
								if (totNorm.normalize()) {
									
								}
								else {
									totNorm = vec3(0.0f,0.0f,1.0f);
								}
								
								voxelBuffer->voxelList[p].normal = totNorm;

							break;
							
							case 1:
								
								if (DO_AO) {
									curNorm = voxelBuffer->voxelList[p].normal;
									
									kk2 = kk/voxelsPerCell;
									jj2 = jj/voxelsPerCell;
									ii2 = ii/voxelsPerCell;
									
									cellInd = kk2*cellsPerHolderPad*cellsPerHolderPad + jj2*cellsPerHolderPad + ii2;
									
									totWeight = 0.0f;
									weightCount = 0.01f;
									
									for (zz = -cellAORad; zz <= cellAORad; zz++) {
										for (yy = -cellAORad; yy <= cellAORad; yy++) {
											for (xx = -cellAORad; xx <= cellAORad; xx++) {
												cellInd2 =
													(kk2+zz)*cellsPerHolderPad*cellsPerHolderPad +
													(jj2+yy)*cellsPerHolderPad +
													(ii2+xx);
													
												curSize = voxelBuffer->cellLists[cellInd2].curSize;
												
												for (ww = 0; ww < curSize; ww++) {
													p2 = voxelBuffer->cellLists[cellInd2].indexArr[ww];
													q2 = voxelBuffer->voxelList[p2].viIndex;
													
													tempPos2 = voxelBuffer->voxelList[p2].pos;
													curNorm2 = voxelBuffer->voxelList[p2].normal;
													
													totNorm = tempPos2 - tempPos;
													
													normLength = totNorm.length();
													
													if (normLength == 0.0f) {
														
													}
													else {
														totNorm /= normLength;
														
														// weight = how much it occludes; 0: none, 1: all
														
														weight = clampfZO((voxelAORad-normLength)/voxelAORad);
														
														weight2 =
															weight * 
															clampfZO((curNorm.dot(totNorm)+1.0f)*0.5f) * 
															clampfZO(normLength*2.0f/voxelAORad) *
															clampfZO((curNorm.dot(-curNorm2)+1.0f)*0.5f)
														;
														
														totWeight += weight2;
														weightCount += weight;
														
													}
													
													
													
													
													
													// kk3 = q2/(voxelsPerHolderPad*voxelsPerHolderPad);
													// jj3 = (q2-kk*voxelsPerHolderPad*voxelsPerHolderPad)/voxelsPerHolderPad;
													// ii3 = q2-(kk*voxelsPerHolderPad*voxelsPerHolderPad + jj*voxelsPerHolderPad);
													
													// voxOffset2.x = ii3;
													// voxOffset2.y = jj3;
													// voxOffset2.z = kk3;
													//voxOffset2 -= paddingInVoxels;
													
													// voxelBuffer->voxelList[p2]
													/// 
													
													
												}
												
											}
										}
									}
								}
								
								
								
								
								
							
							
							
							break;
						}
						
						
						if (
							(
								DO_AO&&(passNum == 1)
							) ||
							(
								!DO_AO	
							)	
						) {
							
							
							//voxOffset += paddingInVoxels;
							voxOffset += offsetInVoxels;
							
							fVO.x = voxOffset.x;
							fVO.y = voxOffset.y;
							fVO.z = voxOffset.z;
							fVO *= fVPC;
							
							totNorm = voxelBuffer->voxelList[p].normal;
							
							tempData[0] = totNorm.x;
							tempData[1] = totNorm.y;
							tempData[2] = totNorm.z;
							tempData[3] = curMat;//totWeight/weightCount;
							
							if (DO_POINTS) {
								gph->vboWrapper.vertexVec.push_back(fVO.x);
								gph->vboWrapper.vertexVec.push_back(fVO.y);
								gph->vboWrapper.vertexVec.push_back(fVO.z);
								gph->vboWrapper.vertexVec.push_back(1.0f);
								
								for (m = 0; m < dataSize; m++) {
									gph->vboWrapper.vertexVec.push_back(tempData[m]);
								}
								
							}
							else {
								
								gph->vboWrapper.vboBox(
									fVO.x, fVO.y, fVO.z,
									0.0f,fVPC,
									curFlags,
									tempData,
									4
								);
							}
						}
						
						
						
						
						
					}
					else {
						// not in bounds
						
						
						
					}
					
					// if (inBounds(&voxOffset,0,voxelsPerHolder)) {
					// 	cellOffset = voxOffset/voxelsPerCell;
					// 	localVoxOffset = voxOffset-(cellOffset*voxelsPerCell);
					// 	//fLocalVoxOffset = toVEC(localVoxOffset);
					// 	//fLocalVoxOffset *= fVPC;
						
					// 	ind = cellOffset.z*cellsPerHolder*cellsPerHolder + cellOffset.y*cellsPerHolder + cellOffset.x;
						
					// 	if (gph->cubeData[ind] == CUBE_DATA_INVALID) {
					// 		gph->cubeWraps.push_back(CubeWrap());
					// 		gph->cubeData[ind] = (gph->cubeWraps.size()-1);
					// 		gph->cubeWraps[gph->cubeData[ind]].init();
					// 	}
						
					// 	curCW = &(gph->cubeWraps[gph->cubeData[ind]]);
						
					// 	curCW->insertValue(&localVoxOffset,voxelBuffer->getFlags(q));//, &fLocalVoxOffset);
					// }
					
					
					
					
					
					
					
					
					
					
					
					
					
				}
				
			}
		}
		
	}
void GameVoxelWrap::process (GamePageHolder * gph)
                                          {
		ivec3 cellCoord;
		ivec3 voxResult;
		
		curPD = gph->curPD;
		
		offsetInCells.set(
			gph->gphMinInCells.getIX(),
			gph->gphMinInCells.getIY(),
			gph->gphMinInCells.getIZ()
		);
		
		offsetInVoxels = offsetInCells - paddingInCells;
		offsetInVoxels *= voxelsPerCell;
		

		basePD = (&singleton->pdPool[curPD]);
		baseData = singleton->pdPool[curPD].data;
		
		voxelBuffer = &(basePD->voxelBuffer);
		voxelBuffer->clearAllNodes();
		
		while( findNextCoord(&voxResult) ) {
			floodFill(voxResult);
			//goto DONE_WITH_IT;
		}
		
		DONE_WITH_IT:
		
		fillVec(gph);
		
	}
bool GameVoxelWrap::findNextCoord (ivec3 * voxResult)
                                             {
		int i;
		int j;
		int k;
		int q;
		int r;
		
		int ii;
		int jj;
		int kk;
		int ikk;
		
		ivec3 curVoxel;
		ivec3 localOffset;
		
		
		int minv = 0;// + paddingInCells;
		int maxv = cellsPerHolderPad; // -paddingInCells;
		
		int lastPtr;
		int cellData;
		int cellData2;
		int voxelsPerCellM1 = voxelsPerCell-1;
		
		bool foundCell = false;
		
		for (i = minv; i < maxv; i++) {
			for (j = minv; j < maxv; j++) {
				for (k = minv; k < maxv; k++) {
					
						
						if (getPadData(i,j,k)->visited) {
							
						}
						else {
							getPadData(i,j,k)->visited = true;						
							
							cellData = getPadData(i,j,k)->cellVal;
							
							if (cellData == E_CD_SOLID) {
								
								foundCell = false;
								
								for (q = 0; q < NUM_ORIENTATIONS; q++) {
									cellData2 = getPadData(
										i + DIR_VECS_I[q][0],
										j + DIR_VECS_I[q][1],
										k + DIR_VECS_I[q][2]
									)->cellVal;
									
									if (cellData2 != E_CD_SOLID) {
										foundCell = true;
										break;
									}
									
								}
							
								if (foundCell) {
									
									foundCell = true;
									
									localOffset.set(i,j,k);
									localOffset *= voxelsPerCell;
									
									for (kk = 0; kk < voxelsPerCell; kk++) {
										
										ikk = voxelsPerCellM1-kk;
										
										for (r = 0; r < 4; r++) {
											switch (r) {
												case 0:
													curVoxel.set(kk, kk, ikk);
												break;
												case 1:
													curVoxel.set(ikk, kk, ikk);
												break;
												case 2:
													curVoxel.set(kk, ikk, ikk);
												break;
												case 3:
													curVoxel.set(ikk, ikk, ikk);
												break;
											}
											
											//curVoxel += offsetInVoxels;
											curVoxel += localOffset;
											if (isSurfaceVoxel(&curVoxel,lastPtr, false)) {
												voxResult->set(
													curVoxel.x, curVoxel.y, curVoxel.z
												);
												return true;
											}
											
										}
									}
									
									for (jj = 0; jj < voxelsPerCell; jj++) {
										for (ii = 0; ii < voxelsPerCell; ii++) {
											for (r = 0; r < 6; r++) {
												switch (r) {
													case 0:
														curVoxel.set(0, ii, jj);
													break;
													case 1:
														curVoxel.set(voxelsPerCellM1, ii, jj);
													break;
													case 2:
														curVoxel.set( ii, 0, jj );
													break;
													case 3:
														curVoxel.set( ii, voxelsPerCellM1, jj );
													break;
													case 4:
														curVoxel.set(ii,jj,0);
													break;
													case 5:
														curVoxel.set(ii,jj,voxelsPerCellM1);
													break;
												}
												
												//curVoxel += offsetInVoxels;
												curVoxel += localOffset;
												if (isSurfaceVoxel(&curVoxel,lastPtr,false)) {
													voxResult->set(
														curVoxel.x, curVoxel.y, curVoxel.z
													);
													return true;
												}
												
										}
									}
									
									
									
								}
							
							}
						}
					}
						
				}
			}
		}
		
		return false;
	}
bool GameVoxelWrap::inBounds (ivec3 * pos, int minB, int maxB)
                                                      {
		
		// ivec3 minB = offsetInVoxels - octOffsetInVoxels;
		// ivec3 maxB = offsetInVoxels + octOffsetInVoxels;
		
		// minB += (0);
		// maxB += (dimInVoxels - 0);
		
		
		
		return (
			(pos->x >= minB) && (pos->x < maxB)  &&
			(pos->y >= minB) && (pos->y < maxB)  &&
			(pos->z >= minB) && (pos->z < maxB)
		);
		
	}
int GameVoxelWrap::getNode (ivec3 * pos)
                                {
		return pos->x + pos->y*voxelsPerHolderPad + pos->z*voxelsPerHolderPad*voxelsPerHolderPad;
	}
void GameVoxelWrap::floodFill (ivec3 startVox)
                                       {
		basePD->fillStack.clear();
		basePD->fillStack.push_back(startVox);
		
		ivec3 curVox;
		ivec3 tempVox;
		int curNode;
		int tempNode;
		
		int q;
		int lastPtr;
		
		bool foundNext;
		
		lastFFSteps = 0;
		
		curNode = getNode(&startVox);
		voxelBuffer->setFlag(curNode,E_OCT_VISITED);
		
		while (basePD->fillStack.size() > 0) {
			
			lastFFSteps++;
			
			curVox = basePD->fillStack.back();
			basePD->fillStack.pop_back();
			
			curNode = getNode(&curVox);
			
			
			//foundNext = false;
			
			for (q = 0; q < NUM_ORIENTATIONS; q++) {
				tempVox = curVox + DIR_VECS_IV[q];
				
				
				if (isSurfaceVoxel(&tempVox,lastPtr,true)) {
					basePD->fillStack.push_back(tempVox);
					voxelBuffer->setFlag(lastPtr,E_OCT_VISITED);
					// foundNext = true;
					// goto NEXT_FF_ITERATION;
				}
				
			}
			
			for (q = 0; q < NUM_ORIENTATIONS; q++) {
				tempVox = curVox + DIR_VECS_IV[q];
				
				
				
				if (isInvSurfaceVoxel(&tempVox,curNode,lastPtr,true)) {
					basePD->fillStack.push_back(tempVox);
					voxelBuffer->setFlag(lastPtr,E_OCT_VISITED);
					// foundNext = true;
					// goto NEXT_FF_ITERATION;
				}
				
				
			}
			
// NEXT_FF_ITERATION:
			
// 			if (foundNext) {
// 				basePD->fillStack.push_back(tempVox);
// 			}
			
		}
		
	}
bool GameVoxelWrap::isInvSurfaceVoxel (ivec3 * pos, int ignorePtr, int & curPtr, bool checkVisited)
                                                                                          {
		int q;
		ivec3 tempVox;
		
		curPtr = getVoxelAtCoord(pos);
		if (curPtr < 0) {
			return false;
		}
		
		
		int tempPtr;
		
		if (checkVisited) {
			if (voxelBuffer->getFlag(curPtr,E_OCT_VISITED)) {
				return false;
			}
		}
		
		if ( !(voxelBuffer->getFlag(curPtr,E_OCT_SOLID)) ) {
			
			for (q = 0; q < NUM_ORIENTATIONS; q++) {
				
				tempVox = (*pos) + DIR_VECS_IV[q];
				tempPtr = getVoxelAtCoord(&tempVox);
				if (tempPtr >= 0) {
					if (tempPtr == ignorePtr) {
						// ignore the voxel we came from
					}
					else {
						if (voxelBuffer->getFlag(tempPtr,E_OCT_SOLID)) {
							
							return true;
						}
					}
				}
				
				
				
			}
		}
		
		return false;
	}
bool GameVoxelWrap::isSurfaceVoxel (ivec3 * pos, int & curPtr, bool checkVisited)
                                                                        {
		int q;
		ivec3 tempVox;
		
		curPtr = getVoxelAtCoord(pos);
		if (curPtr < 0) {
			return false;
		}
		
		if (checkVisited) {
			if (voxelBuffer->getFlag(curPtr,E_OCT_VISITED)) {
				return false;
			}
		}
		
		int tempPtr;
		
		bool isSurface = false;
		
		uint curSide = E_OCT_XP;
		
		if ( voxelBuffer->getFlag(curPtr,E_OCT_SOLID) ) {
			
			for (q = 0; q < NUM_ORIENTATIONS; q++) {
				
				tempVox = (*pos) + DIR_VECS_IV[q];
				tempPtr = getVoxelAtCoord(&tempVox);
				
				if (tempPtr >= 0) {
					if (voxelBuffer->getFlag(tempPtr,E_OCT_SOLID)) {
						
					}
					else {
						voxelBuffer->setFlag(curPtr, curSide);
						isSurface = true;
					}
				}
				
				curSide *= 2;
				
			}
		}
		
		if (isSurface) {
			voxelBuffer->setFlag(curPtr, E_OCT_SURFACE);
		}
		
		return isSurface;
	}
int GameVoxelWrap::getVoxelAtCoord (ivec3 * pos)
                                        {
		
		int VLIndex;
		
		//int minB = 0;
		//int maxB = voxelsPerHolderPad;
		if (inBounds(pos,0,voxelsPerHolderPad)) {
			bool wasNew = false;
			int result = voxelBuffer->addNode(pos,wasNew);
			
			if (wasNew) {
				voxelBuffer->setFlag(result, E_OCT_NOTNEW);
				VLIndex = voxelBuffer->addIndex(result);
				calcVoxel(pos,result,VLIndex);
			}
			
			return result;
		}
		else {
			return -1;
		}
		
		
	}
float GameVoxelWrap::sampLinear (ivec3 * pos, ivec3 offset)
                                                   {
		int q;
		int i;
		int j;
		int k;
		
		float res[8];
		
		ivec3 newPos = ((*pos) + offset);
		
		int xv = newPos.x/voxelsPerCell;
		int yv = newPos.y/voxelsPerCell;
		int zv = newPos.z/voxelsPerCell;
		
		float fx = newPos.x-xv*voxelsPerCell;
		float fy = newPos.y-yv*voxelsPerCell;
		float fz = newPos.z-zv*voxelsPerCell;
		
		float fVPC = voxelsPerCell;
		
		
		
		fx /= fVPC;
		fy /= fVPC;
		fz /= fVPC;
		
		getPadData(xv,yv,zv)->visited = true;
		
		for (q = 0; q < 8; q++) {
			k = q/(2*2);
			j = (q-k*2*2)/2;
			i = q-(k*2*2 + j*2);
			
			res[q] = getPadData(xv+i,yv+j,zv+k)->terVal;
		}
		
		res[0] = res[0]*(1.0f-fz) + res[4]*fz;
		res[1] = res[1]*(1.0f-fz) + res[5]*fz;
		res[2] = res[2]*(1.0f-fz) + res[6]*fz;
		res[3] = res[3]*(1.0f-fz) + res[7]*fz;
		
		res[0] = res[0]*(1.0f-fy) + res[2]*fy;
		res[1] = res[1]*(1.0f-fy) + res[3]*fy;
		
		return res[0]*(1.0f-fx) + res[1]*fx;
		
	}
PaddedDataEntry * GameVoxelWrap::getPadData (int ii, int jj, int kk)
                                                            {
		
		
		int i = ii;
		int j = jj;
		int k = kk;
		int cphM1 = cellsPerHolderPad-1;
		
		if (i < 0) {
			i = 0;
		}
		if (i > cphM1) {
			i = cphM1;
		}
		
		if (j < 0) {
			j = 0;
		}
		if (j > cphM1) {
			j = cphM1;
		}
		
		if (k < 0) {
			k = 0;
		}
		if (k > cphM1) {
			k = cphM1;
		}
		
		
		return &(
			baseData[
				i + j*cellsPerHolderPad + k*cellsPerHolderPad*cellsPerHolderPad
			]	
		);
	}
float GameVoxelWrap::rand2D (vec3 co)
                              {
		vec3 myres = co;
		myres.z = 0.1725f;
		return fract(sin(myres.dot(crand0))*43758.8563f);
	}
vec3 GameVoxelWrap::randPN (vec3 co)
                             {
			
			vec3 myres = vec3(
					co.dot(crand0),
					co.dot(crand1),
					co.dot(crand2)
			);
			
			myres.doSin();
			myres *= 43758.8563f;
			myres.doFract();
			
			return myres*2.0f - oneVec;
	}
void GameVoxelWrap::getVoro (ivec3 * worldPos, ivec3 * worldClosestCenter, int iSpacing)
                                                                               {
		
		vec3 fWorldPos = vec3(
			worldPos->x,
			worldPos->y,
			worldPos->z
		);
		
		float fSpacing = iSpacing;
		
		fWorldPos *= 1.0f/fSpacing;
		
		vec3 fWorldCellPos = vec3(
			worldPos->x/iSpacing,
			worldPos->y/iSpacing,
			worldPos->z/iSpacing
		);
		
		fWorldPos -= vec3(
			fWorldCellPos.x,
			fWorldCellPos.y,
			fWorldCellPos.z
		);
		
		int i;
		
		vec3 testPos;
		float testDis;
		float variance = 0.4f;
		
		vec3 bestPos = VORO_OFFSETS[0] + randPN(fWorldCellPos+VORO_OFFSETS[0])*variance;
		float bestDis = fWorldPos.distance(bestPos);
		
		for (i = 1; i < 27; i++) {
			testPos = VORO_OFFSETS[i] + randPN(fWorldCellPos+VORO_OFFSETS[i])*variance;
			testDis = fWorldPos.distance(testPos);
			
			if (testDis < bestDis) {
				bestDis = testDis;
				bestPos = testPos;
			}
		}
		
		worldClosestCenter->set(
			(bestPos.x + fWorldCellPos.x)*fSpacing,
			(bestPos.y + fWorldCellPos.y)*fSpacing,
			(bestPos.z + fWorldCellPos.z)*fSpacing
		);
		
	}
void GameVoxelWrap::calcVoxel (ivec3 * pos, int octPtr, int VLIndex)
                                                            {
		
		ivec3 worldPos = (*pos) + offsetInVoxels;
		vec3 fWorldPos = vec3(
			worldPos.x,
			worldPos.y,
			worldPos.z
		);
		//worldPos -= paddingInVoxels;
		
		ivec3 worldClosestCenter;// = worldPos;
		ivec3 localClosestCenter;
		
		getVoro(&worldPos,&worldClosestCenter, voxelsPerCell);
		
		voxelBuffer->voxelList[VLIndex].normId = worldClosestCenter.x*3 + worldClosestCenter.y*7 + worldClosestCenter.z*11;
		
		localClosestCenter = worldClosestCenter - offsetInVoxels;
		//localClosestCenter += paddingInVoxels;
		
		
		int vOff = 8;
		
		float terSamp = sampLinear(&localClosestCenter, ivec3(0,0,0));
		
		float terSampOrig =  sampLinear(pos, ivec3(0,0,0));
		float terSampOrigX = sampLinear(pos, ivec3(vOff,0,0));
		float terSampOrigY = sampLinear(pos, ivec3(0,vOff,0));
		float terSampOrigZ = sampLinear(pos, ivec3(0,0,vOff));
		
		vec3 terNorm = vec3(
			terSampOrigX-terSampOrig,
			terSampOrigY-terSampOrig,
			terSampOrigZ-terSampOrig
		);
		
		terNorm *= -1.0f;
		
		bool isTer = (mixf(terSamp,terSampOrig,0.0f) >= 0.5f);
		
		bool isGrass = false;
		
		
		uint finalMat = TEX_NULL;
		
		
		if (isTer) {
			finalMat = TEX_EARTH;
		}
		else {
			if (terNorm.normalize()) {
				if (terNorm.z > 0.5f) {
					int grassOff = rand2D(fWorldPos)*8.0f;
					float terSampGrass = sampLinear(pos, ivec3(0,0,-grassOff));
					
					if (terSampGrass > 0.5f) {
						isGrass = true;
						finalMat = TEX_GRASS;
					}
				}
			}
			
		}
		
		
		
		bool isSolid = isTer||isGrass;
		
		//clampfZO(terNorm.z)*0.5f + 0.5f
		
		
		//bool isSolid = (terSamp >= 0.5f);
		
		
		if (isSolid) {
			voxelBuffer->setFlag(octPtr, E_OCT_SOLID);
			
			// x = base tex, y = variant
			//floor(curTex.x*256.0*255.0) + floor(curTex.y*255.0);
			
			voxelBuffer->voxelList[VLIndex].matId = finalMat*256;
		}
		
		
		
	}
#undef LZZ_INLINE
 
