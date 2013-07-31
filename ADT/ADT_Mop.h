#pragma once

#include "Structs_MOP.h"

struct FileChunk
{
	uint32 signature, size;
};

class ADT_Mop
{
private:
	std::ofstream mOutFile;
	std::ifstream mFile;
	std::stringstream mTexStream;
	std::stringstream mObjStream;

	std::wstring mObjBase;
	std::wstring mTexBase;
	uint sizeObj0;

	MHDR outHeader;

	void copyOut(std::istream& strm, uint numBytes) {
		std::vector<char> content(numBytes);
		strm.read(&content[0], numBytes);
		mOutFile.write(&content[0], numBytes);
	}

	template<typename T>
	void read(T& value) {
		mFile.read((char*)&value, sizeof(T));
	}

	template<typename T>
	T read() {
		T ret;
		read(ret);
		return ret;
	}

	template<typename T>
	void read(T& value, std::istream& strm) {
		strm.read((char*)&value, sizeof(T));
	}

	template<typename T>
	T read(std::istream& strm) {
		T ret;
		read(ret, strm);
		return ret;
	}

	template<typename T>
	void readObj(T& value) {
		mObjStream.read((char*)&value, sizeof(T));
	}

	template<typename T>
	T readObj() {
		T ret;
		readObj(ret);
		return ret;
	}

	template<typename T>
	void write(const T& value) {
		mOutFile.write((const char*)&value, sizeof(T));
	}

	template<typename T, uint size>
	void write(const T(& value)[size]) {
		mOutFile.write((const char*)value, size * sizeof(T));
	}

	template<typename T, uint size>
	void writeAt(std::ofstream::pos_type pos, const T(& value)[size]) {
		auto oldPos = mOutFile.tellp();
		mOutFile.seekp(pos);
		mOutFile.write((const char*)value, size * sizeof(T));
		mOutFile.seekp(oldPos);
	}

	template<typename T>
	void writeAt(const T& value, std::ofstream::pos_type pos) {
		auto oldPos = mOutFile.tellp();
		mOutFile.seekp(pos);
		write(value);
		mOutFile.seekp(oldPos);
	}

	bool SeekChunk(std::istream& strm, uint id)
    {
		strm.seekg(0, std::ios::beg);
		uint curId = 0;

        while (strm.read((char*)&curId, 4) && curId != id)
        {
            uint size = 0;
			strm.read((char*)&size, 4);
			strm.seekg(size, std::ios::cur);
        }

		if(curId != id) {
			strm.clear(std::ios::goodbit);
			strm.seekg(0, std::ios::beg);

			return false;
		}

        strm.seekg(-4, std::ios::cur);
		return true;
    }

	void loadStreams();
	void copyMainChunk(std::istream& input, uint id, uint& headerOfs);
	void copySubChunk(std::istream& input, uint inputOffset, uint& offset, uint baseInput, uint baseOutput);
	void copySubChunk(std::istream& input, uint inputOffset, uint& offset, uint& outsize, uint base, uint baseOutput);
	uint createEmptyChunk(uint signature);
	void checkOptionalChunk(std::istream& input, uint& offset, uint& size, uint signature, uint base, uint baseOutput);
	uint getNextChunk(std::istream& input);

	void convertDoodadMain();
	void convertHeaders();
	void convertLiquid();
	void convertVariousMain();
	void convertChunks(MCIN (&chunks)[256]);
	void convertOptionalSubchunks(uint base, MCNK& chunk, uint baseOutput);
	void convertTexSubchunks(uint texSize, MCNK& outHeader, uint baseOutput, uint texBase);
	void convertObjSubchunks(uint objSize, MCNK& outHeader, uint baseOutput, uint objBase);

	ADT_Mop(const ADT_Mop& ) { }
	void operator = (const ADT_Mop& ) { }

public:
	ADT_Mop(LPCWSTR discFileName);
	~ADT_Mop();

	void convert();
};