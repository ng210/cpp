#ifndef __SOUNDBANK_H
#define __SOUNDBANK_H

#include "base/memory.h"
#include "base/stream.h"
#include "collection/map.h"

NS_FW_BASE_USE
namespace SYNTH {

#define SoundbankNameLength 16

	class Soundbank : public Map {
	private: PROP_R(int, bankSize);
	//private: PROP_R(byte*, data);
	//private: char* names_;
	//private: PROP_R(byte*, programs);
	public:
		Soundbank();
		Soundbank(int bankSize, int count, byte* data = NULL);
		~Soundbank();
		void initialize(int bankSize, byte* data = NULL);
		void data(byte* data);
		void add(char* name, byte* prg);
		//void remove(int ix);
		void copy(Soundbank* bytes, int start, int end);
		byte* getProgram(int ix);
		int indexOf(char* name);


		//Stream* toStream();
		void initializeFromStream(byte*&);
		void writeToStream(Stream*);
		//void remove(int ix);
	};
}

#endif