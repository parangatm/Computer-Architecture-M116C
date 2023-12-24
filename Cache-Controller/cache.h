#include <iostream>
#include <bitset>
#include <stdio.h>
#include<stdlib.h>
#include <string>
#include <iomanip>

using namespace std;

#define L1_CACHE_SETS 16
#define L2_CACHE_SETS 16
#define VICTIM_SIZE 4
#define L2_CACHE_WAYS 8
#define MEM_SIZE 4096
#define BLOCK_SIZE 4 // bytes per block
#define DM 0
#define SA 1

struct cacheBlock
{
	int tag; // you need to compute offset and index to find the tag.
	int lru_position; // for SA only
	int data; // the actual data stored in the cache/memory
	bool valid;
	// add more things here if needed
	int index;
	int address;
};

struct Stat
{
	int missL1; 
	int missL2; 
	int accL1;
	int accL2;
	int accVic;
	int missVic;
	// add more stat if needed. Don't forget to initialize!
	int hitL1;
	int hitL2;
	int hitVic;
};

class cache {
private:
	cacheBlock L1[L1_CACHE_SETS]; // 1 set per row.
	cacheBlock L2[L2_CACHE_SETS][L2_CACHE_WAYS]; // x ways per row 
	// Add your Victim cache here ...
	cacheBlock Vic[VICTIM_SIZE];	
	
	Stat myStat;
	// add more things here
	void load_word(int &currData, int currAdr, int *myMem);
	void store_word(int &currData, int currAdr, int *myMem);

public:
	cache();
	void controller(bool MemR, bool MemW, int* data, int adr, int* myMem);
	// add more functions here ...	
	long double cacheStats(int requestStat);
	void Dump_L1();
	void Dump_L2();
	void Dump_Vic();

	//L1 Cache Functions
	bool searchInL1(int currAdr);
	void getBlockfromL1(int currAdr, struct cacheBlock &L1Block);
	void putBlockinL1(int currAdr, struct cacheBlock dataBlock);
	void putByteinL1(int currAdr, struct cacheBlock dataBlock);

	//Victim Cache Functions
	// bool searchInVic(int currIndex, int currTag, struct cacheBlock &VicInfo, int &VicWay);
	bool searchInVic(int currAdr, int &retIndex);
	void getBlockfromVic(int currAdr, int retIndex, struct cacheBlock &VicBlock);
	void putBlockinVic(int currAdr, int retIndex, struct cacheBlock dataBlock);
	void updateVicLRU(int VicEvictLRU);
	void putByteinVic(int currAdr, int retIndex, struct cacheBlock dataBlock);

	//L2 Cache Functions
	bool searchInL2(int currAdr, int &retIndex);
	void hitInL2(int currAdr, int retIndex);
	void getBlockfromL2(int currAdr, int retIndex, struct cacheBlock &L2Block);
	void putBlockinL2(int currAdr, int retIndex, struct cacheBlock dataBlock);
	void updateL2LRU(int currIndex, int L2EvictLRU);
	void missInL2(int currAdr);
	int findEmptySpotL2(int currIndex);
	void putByteinL2(int currAdr, int retIndex, struct cacheBlock dataBlock);
};
