#include "cache.h"

cache::cache()
{
	for (int i=0; i<L1_CACHE_SETS; i++)
		L1[i].valid = false; 
	for (int i=0; i<L2_CACHE_SETS; i++)
		for (int j=0; j<L2_CACHE_WAYS; j++)
			L2[i][j].valid = false; 

	// Do the same for Victim Cache ...
	for (int i=0; i<VICTIM_SIZE; i++)
		Vic[i].valid = false;

	this->myStat.missL1 =0;
	this->myStat.missL2 =0;
	this->myStat.accL1 =0;
	this->myStat.accL2 =0;

	// Add stat for Victim cache ... 
	this->myStat.missVic = 0;
	this->myStat.accVic = 0;

	this->myStat.hitL1 = 0;
	this->myStat.hitL2 = 0;
	this->myStat.hitVic = 0;
	
}
void cache::controller(bool MemR, bool MemW, int* data, int adr, int* myMem)
{
	// add your code here
	if(MemR)
		load_word(*data, adr, myMem);
	if(MemW)
		store_word(*data, adr, myMem);

}

// Get stats of the cache
// TODO: Add Victim Stats separately
long double cache::cacheStats(int requestStat)
{
	long double statVal;
	switch(requestStat)
	{
		case 0:
			// statVal = (((float)myStat.missL1 - (float)myStat.hitVic) / ((float)myStat.missL1 + (float)myStat.hitL1));
			// statVal = ((float)myStat.missL1)/((float)myStat.accL1);
			statVal = ((long double)myStat.missL1)/((long double)myStat.missL1 + (long double)myStat.hitL1);
			break;
		case 1:
			// statVal = ((float)myStat.missVic)/((float)myStat.accVic);
			statVal = ((long double)myStat.missVic)/((long double)myStat.missVic + (long double)myStat.hitVic);
			break;
		case 2:
			// statVal = ((float)myStat.missL2 / ((float)myStat.missL2 + (float)myStat.hitL2));
			// statVal = ((float)myStat.missL2)/((float)myStat.accL2);
			statVal = ((long double)myStat.missL2)/((long double)myStat.missL2 + (long double)myStat.hitL2);
			break;
		default:
			cerr << "Unsupported cache stat operation" << endl;
	}
	// cout << "L1 miss rate: "<< myStat.missL1 << endl;
	// cout << "Victim miss rate: "<< myStat.missVic << endl;
	// cout << "L2 miss rate: "<< myStat.missL2 << endl;
	// cout << "L1 hit rate: "<< myStat.hitL1 << endl;
	// cout << "Victim hit rate: "<< myStat.hitVic << endl;
	// cout << "L2 hit rate: "<< myStat.hitL2 << endl;
	// cout << endl;

	return statVal;
}

// L1 cache functions

bool cache::searchInL1(int currAdr)
{
	int currOffset, currIndex, currTag;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

    // Extract next 4 bits (shift right by 2 and mask with 0b1111)
    currIndex = (currAdr >> 2) & 0b1111;

    // Extract remaining bits after removing lower 6 bits
    currTag = currAdr >> 6;

	// cout << "L1 addressing: index=" <<currIndex<<", tag="<<currTag << endl;
	// cerr << "Debug statement" << endl;
	cout <<"";

	if((L1[currIndex].valid)&&(L1[currIndex].tag == currTag))
	{
		// cout << "searchInL1() returns True"<< endl;
		return true;
	}
	return false;
	
}

void cache::getBlockfromL1(int currAdr, struct cacheBlock &L1Block)
{
	int currOffset, currIndex, currTag;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

    // Extract next 4 bits (shift right by 2 and mask with 0b1111)
    currIndex = (currAdr >> 2) & 0b1111;
	// currIndex = currAdr & 0b1111;

    // Extract remaining bits after removing lower 6 bits
    currTag = currAdr >> 6;
	// currTag = currAdr >> 4;

	L1Block.data = L1[currIndex].data;
	L1Block.address = L1[currIndex].address;
	L1Block.valid = L1[currIndex].valid;

	// cout << "getBlockfromL1() executed"<< endl;

	// if(!L1Block.valid)
	// {
	// 	cout << "Invalid data requested from L1 cache" << endl;
	// }
}

void cache::putBlockinL1(int currAdr, struct cacheBlock dataBlock)
{
	int currOffset, currIndex, currTag;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

    // Extract next 4 bits (shift right by 2 and mask with 0b1111)
    currIndex = (currAdr >> 2) & 0b1111;
	// currIndex = currAdr & 0b1111;

    // Extract remaining bits after removing lower 6 bits
    currTag = currAdr >> 6;
	// currTag = currAdr >> 4;

	int L1Adr = dataBlock.address;
	int L1Offset = L1Adr & 0b11;
	int L1Index = (L1Adr >> 2) && 0b1111;
	int L1Tag = L1Adr >> 6;

	L1[currIndex].data = dataBlock.data;
	L1[currIndex].address = dataBlock.address;
	L1[currIndex].valid = dataBlock.valid;
	// L1[currIndex].tag = currTag;
	L1[currIndex].tag = L1Tag;

	// cout << "putBlockinL1() executed" << endl;
}

void cache::putByteinL1(int currAdr, struct cacheBlock dataBlock)
{
	int currOffset, currIndex, currTag;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

    // Extract next 4 bits (shift right by 2 and mask with 0b1111)
    currIndex = (currAdr >> 2) & 0b1111;

    // Extract remaining bits after removing lower 6 bits
    currTag = currAdr >> 6;

	L1[currIndex].address = dataBlock.address;
	L1[currIndex].valid = dataBlock.valid;
	L1[currIndex].tag = currTag;

	// Clear the corresponding bytes
	L1[currIndex].data &= ~(0xFF << (currOffset * 8));

	// Set the new data
	L1[currIndex].data |= (dataBlock.data & 0xFF) << (currOffset * 8);
}


// Victim cache functions

bool cache::searchInVic(int currAdr, int &retIndex)
{
	int currOffset, currIndex, currTag;
	
	currIndex = 0;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

 	// Extract remaining bits after removing lower 2 bits
    currTag = currAdr >> 2;
	// currTag = currAdr;

	// cout << "Vic addressing: index=" <<currIndex<<", tag="<<currTag << endl;

	for(int i=0; i<VICTIM_SIZE; i++)
	{
		if((Vic[i].valid) && (Vic[i].tag == currTag))
		{
			retIndex = i;
			// cout << "searchInVic() returns True" << endl;
			return true;
		}
	}
	// cout << "searchInVic() returns False" << endl;
	return false;
}

void cache::getBlockfromVic(int currAdr, int retIndex, struct cacheBlock &VicBlock)
{
	int currOffset, currIndex, currTag;
	
	currIndex = 0;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

 	// Extract remaining bits after removing lower 2 bits
    currTag = currAdr >> 2;
	// currTag = currAdr;

	VicBlock.data = Vic[retIndex].data;
	VicBlock.address = Vic[retIndex].address;
	VicBlock.valid = Vic[retIndex].valid;
	VicBlock.lru_position = Vic[retIndex].lru_position;

	// cout << "getBlockfromVic() executed" << endl;

	// if(!VicBlock.valid)
	// {
	// 	cout << "Invalid data requested from Victim cache" << endl;
	// }
}

void cache::putBlockinVic(int currAdr, int retIndex, struct cacheBlock dataBlock)
{
	int currOffset, currIndex, currTag;
	
	currIndex = 0;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

 	// Extract remaining bits after removing lower 2 bits
    currTag = currAdr >> 2;
	// currTag = currAdr;

	int VicAdr = dataBlock.address;
	int VicOffset = VicAdr & 0b11;
	int VicTag = VicAdr >> 2;

	Vic[retIndex].data = dataBlock.data;
	Vic[retIndex].address = dataBlock.address;
	Vic[retIndex].valid = dataBlock.valid;
	// Vic[retIndex].tag = currTag;
	Vic[retIndex].tag = VicTag;


	// cout << "putBlockinVic() executed" << endl;
}

void cache::updateVicLRU(int VicEvictLRU)
{
	for(int b = 0; b < VICTIM_SIZE; b++)
	{
		if((Vic[b].valid) && (Vic[b].lru_position > VicEvictLRU))
			Vic[b].lru_position--;
	}
	// cout << "Victim LRU Updated" << endl; 
}

void cache::putByteinVic(int currAdr, int retIndex, struct cacheBlock dataBlock)
{
	int currOffset, currIndex, currTag;
	
	currIndex = 0;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

 	// Extract remaining bits after removing lower 2 bits
    currTag = currAdr >> 2;
	// currTag = currAdr;

	// Vic[retIndex].data = dataBlock.data;
	Vic[retIndex].address = dataBlock.address;
	Vic[retIndex].valid = dataBlock.valid;
	Vic[retIndex].tag = currTag;

	// Clear the corresponding bytes
	Vic[retIndex].data &= ~(0xFF << (currOffset * 8));

	// Set the new data
	Vic[retIndex].data |= (dataBlock.data & 0xFF) << (currOffset * 8);
}

// L2 cache functions

bool cache::searchInL2(int currAdr, int &retIndex)
{
	int currOffset, currIndex, currTag;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

    // Extract next 4 bits (shift right by 2 and mask with 0b1111)
    currIndex = (currAdr >> 2) & 0b1111;
	// currIndex = currAdr & 0b1111;

	// Extract remaining bits after removing lower 6 bits
    currTag = currAdr >> 6;
	// currTag = currAdr >> 4;

	// cout << "L2 addressing: index=" <<currIndex<<", tag="<<currTag << endl;

	// for(int i=0; i < L2_CACHE_WAYS; i++)
	// {
	// 	if((L2[currIndex][i].valid) && (L2[currIndex][i].tag == currTag))
	// 	{
	// 		retIndex = i;
	// 		// cout << "searchInL2() returns True" << endl;
	// 		return true;
	// 	}
	// }
	// // cout << "searchInL2() returns False" << endl;
	// return false;

	for(int j=0; j<L2_CACHE_SETS; j++)
	{
		for(int i=0; i < L2_CACHE_WAYS; i++)
		{
			if((L2[j][i].valid) && (L2[j][i].tag == currTag))
			{
				retIndex = i;
				// cout << "searchInL2() returns True" << endl;
				return true;
			}
		}
	}
	// cout << "searchInL2() returns False" << endl;
	return false;
}

void cache::getBlockfromL2(int currAdr, int retIndex, struct cacheBlock &L2Block)
{
	int currOffset, currIndex, currTag;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

    // Extract next 4 bits (shift right by 2 and mask with 0b1111)
    currIndex = (currAdr >> 2) & 0b1111;
	// currIndex = currAdr & 0b1111;

	// Extract remaining bits after removing lower 6 bits
    currTag = currAdr >> 6;
	// currTag = currAdr >> 4;

	L2Block.data = L2[currIndex][retIndex].data;
	L2Block.address = L2[currIndex][retIndex].address;
	L2Block.valid = L2[currIndex][retIndex].valid;
	L2Block.lru_position = L2[currIndex][retIndex].lru_position;

	// cout << "getBlockfromL2() executed";

	// if(!L2Block.valid)
	// {
	// 	cout << "Invalid data requested from L2 cache"<< endl;
	// }
}

void cache::putBlockinL2(int currAdr, int retIndex, struct cacheBlock dataBlock)
{
	int currOffset, currIndex, currTag;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

    // Extract next 4 bits (shift right by 2 and mask with 0b1111)
    currIndex = (currAdr >> 2) & 0b1111;
	// currIndex = currAdr & 0b1111;

	// Extract remaining bits after removing lower 6 bits
    currTag = currAdr >> 6;
	// currTag = currAdr >> 4;

	int L2Adr = dataBlock.address;
	int L2Offset = L2Adr & 0b11;
	int L2Index = (L2Adr >> 2) & 0b1111;
	int L2Tag = L2Adr >> 6;

	L2[currIndex][retIndex].data = dataBlock.data;
	L2[currIndex][retIndex].address = dataBlock.address;
	L2[currIndex][retIndex].valid = dataBlock.valid;
	// L2[currIndex][retIndex].tag = currTag;
	L2[currIndex][retIndex].tag = L2Tag;
	
	// cout << "putBlockinL2() executed" << endl;

}

void cache::hitInL2(int currAdr, int retIndex)
{

	int currOffset, currIndex, currTag;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

    // Extract next 4 bits (shift right by 2 and mask with 0b1111)
    currIndex = (currAdr >> 2) & 0b1111;
	// currIndex = currAdr & 0b1111;

	// Extract remaining bits after removing lower 6 bits
    currTag = currAdr >> 6;
	// currTag = currAdr >> 4;

	//Get hit data from L2
	struct cacheBlock L2Block;
	getBlockfromL2(currAdr, retIndex, L2Block);
	// L2[currIndex][retIndex].valid = false;

	//Get existing data from L1
	struct cacheBlock existL1Block;
	getBlockfromL1(currAdr, existL1Block);

	//move hit data from L2 to L1
	putBlockinL1(currAdr, L2Block);
	// putByteinL1(currAdr, L2Block);
	// cout << "Hit data moved from L2 to L1" << endl;

	int evictLRUL2;
	evictLRUL2 = L2Block.lru_position;

	// Move existing L1 data to Victim
	if(existL1Block.valid)
	{
		//Find empty spot in Victim
		bool emptyVic = false;

		for(int i=0; i<VICTIM_SIZE; i++)
		{
			if(!Vic[i].valid)
			{
				//Empty spot found in Victim
				// cout << "Empty spot found in victim" << endl;
				putBlockinVic(currAdr, i, existL1Block);
				// putByteinVic(currAdr, i, existL1Block);
				// cout << "Existing L1 data moved to Victim"<< endl; 
				emptyVic = true;
				updateVicLRU(0);
				Vic[i].lru_position = VICTIM_SIZE - 1;
				break;
			}
		}

		if(!emptyVic)
		{
			// No empty spot in Victim

			// find oldest victim data to evcict
			int oldVicLRU = VICTIM_SIZE - 1;
			int oldVicIndex;

			for (int i=0; i<VICTIM_SIZE; i++)
			{
				if((Vic[i].valid)&&(Vic[i].lru_position < oldVicLRU))
				{
					//oldest victim data
					oldVicIndex = i;
					oldVicLRU = Vic[i].lru_position;
				}
			}

			//Get oldest victim data
			struct cacheBlock existVicBlock;
			getBlockfromVic(currAdr, oldVicIndex, existVicBlock);

			//Move oldest victim data to L2
			putBlockinL2(currAdr, retIndex, existVicBlock);
			// cout << "Oldest Victim data evicted and moved to L2" << endl;
			updateL2LRU(currIndex, evictLRUL2);
			L2[currIndex][retIndex].valid = true;
			L2[currIndex][retIndex].lru_position = L2_CACHE_WAYS - 1;
			// cout << "LRU updated for oldest victim data put in L2" << endl;

			//Now victim has space
			//Put existing L1 data in victim
			putBlockinVic(currAdr, oldVicIndex, existL1Block);
			updateVicLRU(0);
			Vic[oldVicIndex].lru_position = VICTIM_SIZE - 1;
			// cout << "Moved data from existing L1 to Victim" << endl;
		}
	}
	// else
	// {
	// 	cout << "Existing L1 data was invalid, discarded"<<endl;
	// }

}

void cache::missInL2(int currAdr)
{
	int currOffset, currIndex, currTag;

	// Extract lower 2 bits (mask with 0b11)
    currOffset = currAdr & 0b11;

    // Extract next 4 bits (shift right by 2 and mask with 0b1111)
    currIndex = (currAdr >> 2) & 0b1111;
	// currIndex = currAdr & 0b1111;

	// Extract remaining bits after removing lower 6 bits
    currTag = currAdr >> 6;
	// currTag = currAdr >> 4;

	//Get existing data from L1
	struct cacheBlock existL1Block;
	getBlockfromL1(currAdr, existL1Block);

	//Move existing L1 data to victim
	if(existL1Block.valid)
	{
		//Find empty spot in Victim
		bool emptyVic = false;

		for(int i=0; i<VICTIM_SIZE; i++)
		{
			if(!Vic[i].valid)
			{
				//Empty spot found in Victim
				// cout << "Empty spot found in victim" << endl;
				putBlockinVic(currAdr, i, existL1Block);
				// putByteinVic(currAdr, i, existL1Block);
				// cout << "Existing L1 data moved to Victim"<< endl; 
				emptyVic = true;
				updateVicLRU(0);
				Vic[i].lru_position = VICTIM_SIZE - 1;
				// int validCount=0;
				// for (int j=0; j<VICTIM_SIZE; j++)
				// {
				// 	if(Vic[j].valid)
				// 	{
				// 		validCount++;
				// 	}
				// }
				// Vic[i].lru_position = validCount - 1;
				break;
			}
		}

		if(!emptyVic)
		{
			// No empty spot in victim

			// find oldest victim data to evcict
			int oldVicLRU;
			int oldVicIndex;

			for (int i=0; i<VICTIM_SIZE; i++)
			{
				if((Vic[i].valid)&&(Vic[i].lru_position < oldVicLRU))
				{
					//oldest victim data
					oldVicIndex = i;
					oldVicLRU = Vic[i].lru_position;
				}
			}

			//Get oldest victim data
			struct cacheBlock existVicBlock;
			getBlockfromVic(currAdr, oldVicIndex, existVicBlock);

			//Move oldest victim data to L2
			int emptyIndexL2 = findEmptySpotL2(currIndex);
			putBlockinL2(currAdr, emptyIndexL2, existVicBlock);
			// cout << "Oldest victim data evicted and moved to L2" << endl;
			updateL2LRU(currIndex, 0);
			L2[currIndex][emptyIndexL2].valid = true;
			L2[currIndex][emptyIndexL2].lru_position = L2_CACHE_WAYS - 1;
			// cout << "LRU updated for oldest victim data put in L2" << endl;

			//Move existing L1 data to victim
			putBlockinVic(currAdr, oldVicIndex, existL1Block);
			updateVicLRU(0);
			Vic[oldVicIndex].lru_position = VICTIM_SIZE - 1;
			// cout << "Moved data from existing L1 to Victim" << endl;

		}
	}
	// else
	// {
	// 	cout << "Existing L1 data was invalid, discarded"<<endl;
	// }
}

//Update LRU in L2
void cache::updateL2LRU(int currIndex, int L2EvictLRU)
{
	// int currOffset, currIndex, currTag;

	// Extract lower 2 bits (mask with 0b11)
    // currOffset = currAdr & 0b11;

    // Extract next 4 bits (shift right by 2 and mask with 0b1111)
    // currIndex = (currAdr >> 2) & 0b1111;

	// Extract remaining bits after removing lower 6 bits
    // currTag = currAdr >> 6;

	for (int w=0; w<L2_CACHE_WAYS; w++)
	{
		if((L2[currIndex][w].valid) && (L2[currIndex][w].lru_position > L2EvictLRU))
			L2[currIndex][w].lru_position--;
	}
	// cout << "LRU updated in L2 Cache" << endl;
}

//find empty spot in L2
int cache::findEmptySpotL2(int currIndex)
{
	int w;
	bool emptySlotL2 = false;
	int posEvictL2 = 0;

	for(w=0; w<L2_CACHE_WAYS; w++)
	{
		if(!(L2[currIndex][w].valid))
		{
			// Empty spot in L2 found
			updateL2LRU(currIndex, 0);
			L2[currIndex][w].lru_position = L2_CACHE_WAYS - 1;
			L2[currIndex][w].valid = true;
			emptySlotL2 = true;
			posEvictL2 = w;
			break;
		}
	}
	if(!emptySlotL2)
	{
		// No empty slot in L2 found
			// Evict oldest and place there
		int LRUEvictL2 = L2_CACHE_WAYS - 1;
		for(w=0; w<L2_CACHE_WAYS; w++)
		{
			if(L2[currIndex][w].lru_position < LRUEvictL2)
			{
				LRUEvictL2 = L2[currIndex][w].lru_position;
				posEvictL2 = w;
			}
		}
		updateL2LRU(currIndex, LRUEvictL2);
		L2[currIndex][posEvictL2].lru_position = L2_CACHE_WAYS - 1;
		emptySlotL2 = true;
	}
	return posEvictL2;
}


void cache::Dump_L1()
{
	int set;
	cout << endl;
	cout << "L1 Cache Dump" << endl;
	for(set=0; set<L1_CACHE_SETS; set++)
	{
		if(L1[set].valid)
		{
			cout << L1[set].valid <<", L1[" <<set<<"].tag = "<<L1[set].tag << ", L1[" <<set<<"].data = "<<L1[set].data << endl;
		// cout<<"(" << set << "," << L1[set].valid << "," << (L1[set].tag << 4 | set) << ")"<<endl;
		}
	}
	cout << endl;
}

void cache::Dump_L2()
{
	int set;
	cout << endl;
	cout << "L2 Cache Dump" << endl;
	for (set=0; set<L2_CACHE_SETS; set++)
	{
		// cout<<"Row " << set << endl;
		for (int w=0; w<L2_CACHE_WAYS; w++)
		{
			if(L2[set][w].valid)
			{
				cout<<L2[set][w].valid << ", L2[" << set << "][" << w<< "].tag = "<<L2[set][w].tag<< ", L2[" << set << "][" << w<< "].data = "<<L2[set][w].data << ", L2[" << set << "][" << w<< "].lru_position = "<<L2[set][w].lru_position << endl;
			}
		}
		// cout << endl;
	}
	cout << endl;
}

void cache::Dump_Vic()
{
	int set;
	cout << endl;
	cout << "Victim Cache Dump" << endl;
	for(int i=0; i<VICTIM_SIZE; i++)
	{
		if(Vic[i].valid)
		{
			cout << Vic[i].valid <<", Vic["<<i<<"].tag = "<<Vic[i].tag<<",Vic["<<i<<"].data = "<<Vic[i].data<<",Vic["<<i<<"].LRU = "<<Vic[i].lru_position<<endl; 
		// cout<<"(" << i << "," << Vic[i].valid << "," << (Vic[i].tag << 4 | Vic[i].index) << ")"<<endl;
		}
	}
	cout << endl;
}

//LW instruction
void cache::load_word(int &currData, int currAdr, int *myMem)
{
	// int currTag = currAdr >> 4;
	// int currIndex = currAdr & 0xF;

	// cout << "LW instr: addr = "<< currAdr << ", data = " << currData << endl;

	// cout << "Begin LW" << endl;
	// Dump_L1();
	// Dump_Vic();
	// Dump_L2();

	// Search L1
	bool foundInL1 = searchInL1(currAdr);

	if(foundInL1)
	{
		//Hit in L1
		myStat.hitL1++;
		myStat.accL1++;
		// cout << "Hit in L1" << endl;
		
		//Load hit data from L1 to LW
		struct cacheBlock L1Block;
		getBlockfromL1(currAdr, L1Block);
		currData = L1Block.data;
		// cout << "--- SUCCESS: L1 cache accessed to load data ---" << endl;
	}
	else
	{
		//Miss in L1
		myStat.missL1++;
		myStat.accL1++;
		// cout << "Miss in L1" << endl;

		//Search Vic
		int retIndexVic;
		bool foundinVic = searchInVic(currAdr, retIndexVic);

		if(foundinVic)
		{
			//Hit in Vic
			myStat.hitVic++;
			myStat.accVic++;
			// cout << "Hit in Victim" << endl;

			struct cacheBlock VicBlock;
			getBlockfromVic(currAdr, retIndexVic, VicBlock);

			//Load hit data from Victim to LW
			currData = VicBlock.data;
			// cout << "--- SUCCESS: Victim cache accessed to load data ---" << endl;
			Vic[retIndexVic].valid = false;

			//Get existing data from L1
			struct cacheBlock existL1Block;
			getBlockfromL1(currAdr, existL1Block);

			//Move hit data found in Victim to L1
			putBlockinL1(currAdr, VicBlock);
			// putByteinL1(currAdr, VicBlock);
			// cout << "Hit data moved from Victim to L1" << endl;

			if(existL1Block.valid)
			{
				//Move existing L1 data to victim
				putBlockinVic(currAdr, retIndexVic, existL1Block);
				// putByteinVic(currAdr, retIndexVic, existL1Block);
				Vic[retIndexVic].valid = true;
				// cout << "existing L1 data moved to Victim" << endl;
			}
			// else
			// {
			// 	cout << "Existing L1 data was invalid, discarded" << endl;
			// }

			int evictLRUVic = VicBlock.lru_position;
			updateVicLRU(evictLRUVic);
			Vic[retIndexVic].lru_position = VICTIM_SIZE - 1;
			// cout << "LRU updated for existing L1 data put in Victim" << endl;
		}
		else
		{
			//Miss in Vic
			myStat.missVic++;
			myStat.accVic++;
			// cout << "Miss in Vic" << endl;

			// Search L2
			int retIndexL2;
			bool foundinL2 = searchInL2(currAdr, retIndexL2);

			if(foundinL2)
			{
				//Hit in L2
				myStat.hitL2++;
				myStat.accL2++;
				// cout << "Hit in L2" << endl;
				hitInL2(currAdr, retIndexL2);

				//Load hit data from L2 to LW
				struct cacheBlock L2Block;
				getBlockfromL2(currAdr, retIndexL2, L2Block);
				currData = L2Block.data;
				// cout << "--- SUCCESS: L2 cache accessed to load data ---" << endl;
			}
			else
			{
				//Miss in L2
				myStat.missL2++;
				myStat.accL2++;
				// cout << "Miss in L2" << endl;

				currData = myMem[currAdr];
				// cout << "--- EHH: Main Memory accessed to load data ---" << endl;
				// cout << "currData="<<currData<<endl;
				missInL2(currAdr);

				struct cacheBlock mainBlock;
				mainBlock.address = currAdr;
				mainBlock.data = currData;
				mainBlock.valid = true;
				putBlockinL1(currAdr, mainBlock);
				// putByteinL1(currAdr, mainBlock);
				// cout << "Accessed data stored in L1" << endl;

			}

		}

	}

	// cout << "End LW" << endl;
	// Dump_L1();
	// Dump_Vic();
	// Dump_L2();
	
	// cout << "---------------------------------------------------------------------------------" << endl;
	
}


//SW instruction
void cache::store_word(int &currData, int currAdr, int *myMem)
{
	// int currTag = currAdr >> 4;
	// int currIndex = currAdr & 0xF;

	// cout << "SW instr: addr = "<< currAdr << ", data = " << currData << endl;

	// cout << "Begin SW" << endl;
	
	bool foundInL1 = searchInL1(currAdr);
	
	if(foundInL1)
	{
		//Hit in L1
		// myStat.hitL1++;
		// myStat.accL1++;
		// cout << "SW: Hit in L1" << endl;
		
		struct cacheBlock goInL1;
		// goInL1.valid = true;
		// goInL1.address = currAdr;
		// goInL1.data = currData;
		// putBlockinL1(currAdr, goInL1);
		
		// cout << "SW: Data moved to L1" << endl;
		
	}
	else
	{
		//Miss in L1
		// myStat.missL1++;
		// myStat.accL1++;
		// cout << "SW: Miss in L1" << endl;

		int retIndexVic;
		bool foundinVic = searchInVic(currAdr, retIndexVic);

		if(foundinVic)
		{
			//Hit in Vic
			// myStat.hitVic++;
			// myStat.accVic++;
			// cout << "SW: Hit in Victim" << endl;

			struct cacheBlock goInVic;

		}
		else
		{
			//Miss in Vic
			// myStat.missVic++;
			// myStat.accVic++;
			// cout << "SW: Miss in Victim" << endl;

			int retIndexL2;
			bool foundinL2 = searchInL2(currAdr, retIndexL2);

			if(foundinL2)
			{
				//Hit in L2
				// myStat.hitL2++;
				// myStat.accL2++;
				// cout << "SW: Hit in L2" << endl;
				struct cacheBlock goInL2;
			}
			else
			{
				//Miss in L2
				// myStat.missL2++;
				// myStat.accL2++;
				myMem[currAdr] = currData;
				// cout << "SW: Miss in L2" << endl;
			}

		}


	}

	// cout << "End SW" << endl << endl;
	// Dump();
}
