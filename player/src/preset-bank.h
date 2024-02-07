#ifndef __PLAYER_PRESET_H
#define __PLAYER_PRESET_H

#include "base/stream.h"
#include "collection/map.h"
//#include "player/src/input.h"


NS_FW_BASE_USE

namespace PLAYER {

	#define PresetBankNameLength 16

	class PresetBank : public Map {
	private: PROP_R(int, bankSize);
	public:
		PresetBank();
		PresetBank(int bankSize, byte* data = NULL);
		~PresetBank();
		void initialize(int bankSize, byte* data = NULL);
		void data(byte* data);
		void add(char* name, byte* prg);
		//void remove(int ix);
		void copy(PresetBank* bytes, int start, int end);
		byte* getPreset(int ix);
		int indexOf(char* name);


		//Stream* toStream();
		void initializeFromStream(byte*&);
		void writeToStream(Stream*);
		//void remove(int ix);
	};
}

#endif