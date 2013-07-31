#include "stdafx.h"
#include "ADT_Mop.h"
#include "ConsoleProgress.h"
#include "Structs_MOP.h"
#include <cassert>

#include <Shlwapi.h>

#pragma comment(lib, "shlwapi.lib")

ADT_Mop::ADT_Mop(LPCWSTR fileName) {
	mFile.open(fileName, std::ios::in | std::ios::binary);
	LPWSTR tmpFile = _wcsdup(fileName);
	LPWSTR filePath = _wcsdup(fileName);
	PathStripPath(tmpFile);
	std::wstring adtName = tmpFile;
	std::wstring outPath = L".\\Output\\";
	outPath += adtName;

	CreateDirectory(L"Output", nullptr);

	mOutFile.open(outPath, std::ios::out | std::ios::binary);

	*PathFindFileName(filePath) = L'\0';
	*PathFindExtension(tmpFile) = L'\0';

	std::wstring baseName = filePath;
	baseName += tmpFile;
	std::wstring objName = baseName;
	objName += L"_obj";
	mObjBase = objName;
	mTexBase = baseName;
	mTexBase += L"_tex";

	free(tmpFile);
	free(filePath);

	loadStreams();
}

ADT_Mop::~ADT_Mop() {
	mFile.close();
	mOutFile.close();
}

void ADT_Mop::loadStreams() {
	std::ifstream obj0, obj1, tex0, tex1;
	obj0.open(mObjBase + L"0.adt", std::ios::in | std::ios::binary);
	obj1.open(mObjBase + L"1.adt", std::ios::in | std::ios::binary);
	tex0.open(mTexBase + L"0.adt", std::ios::in | std::ios::binary);
	tex1.open(mTexBase + L"1.adt", std::ios::in | std::ios::binary);

	if(obj0.is_open()) {
		obj0.seekg(0, std::ios::end);
		uint size = (uint)obj0.tellg();
		obj0.seekg(0);
		std::vector<char> content(size);
		obj0.read(&content[0], size);
		mObjStream.write(&content[0], size);
		sizeObj0 = size;
	}

	if(obj1.is_open()) {
		obj1.seekg(0, std::ios::end);
		uint size = (uint)obj1.tellg();
		obj1.seekg(0);
		std::vector<char> content(size);
		obj1.read(&content[0], size);
		mObjStream.write(&content[0], size);
	}

	if(tex0.is_open()) {
		tex0.seekg(0, std::ios::end);
		uint size = (uint)tex0.tellg();
		tex0.seekg(0);
		std::vector<char> content(size);
		tex0.read(&content[0], size);
		mTexStream.write(&content[0], size);
	}

	if(tex1.is_open()) {
		tex1.seekg(0, std::ios::end);
		uint size = (uint)tex1.tellg();
		tex1.seekg(0);
		std::vector<char> content(size);
		tex1.read(&content[0], size);
		mTexStream.write(&content[0], size);
	}

	obj0.close();
	obj1.close();
	tex0.close();
	tex1.close();

	mObjStream.seekg(0);
	mTexStream.seekg(0);
}

void ADT_Mop::convert() {
	std::cout << "Reading header...." << std::endl;
	ConsoleProgress::printProgress(0, 2);
	convertHeaders();

	ConsoleProgress::printProgress(2, 2);

	std::cout << std::endl << std::endl;
	std::cout << "Creating MCIN...." << std::endl;

	MCIN mcin[256];

	FileChunk cnk;
	cnk.signature = 'MCIN';
	cnk.size = sizeof(mcin);
	write(cnk);
	write(mcin);

	ConsoleProgress::printProgress(1, 1);
	std::cout << std::endl << std::endl;
	convertDoodadMain();
	
	convertLiquid();
	convertVariousMain();

	writeAt(outHeader, 20);

	convertChunks(mcin);
}

void ADT_Mop::convertDoodadMain() {
	std::cout << "Parsing doodads..." << std::endl;
	ConsoleProgress::printProgress(0, 6);
	copyMainChunk(mObjStream, 'MMDX', outHeader.ofsMmdx);
	ConsoleProgress::printProgress(1, 6);
	copyMainChunk(mObjStream, 'MMID', outHeader.ofsMmid);
	ConsoleProgress::printProgress(2, 6);
	copyMainChunk(mObjStream, 'MDDF', outHeader.ofsMddf);
	ConsoleProgress::printProgress(3, 6);
	copyMainChunk(mObjStream, 'MWMO', outHeader.ofsMwmo);
	ConsoleProgress::printProgress(4, 6);
	copyMainChunk(mObjStream, 'MWID', outHeader.ofsMwid);
	ConsoleProgress::printProgress(5, 6);
	copyMainChunk(mObjStream, 'MODF', outHeader.ofsModf);
	ConsoleProgress::printProgress(6, 6);

	writeAt(outHeader, 20);
	std::cout << std::endl << std::endl;
}

void ADT_Mop::convertChunks(MCIN (&chunks)[256]) {
	std::cout << "Converting chunks..." << std::endl;
	ConsoleProgress::printProgress(0, 256);

	SeekChunk(mFile, 'MCNK');
	SeekChunk(mTexStream, 'MCNK');
	SeekChunk(mObjStream, 'MCNK');

	for(int i = 0; i < 256; ++i) {
		uint fileBase = mFile.tellg();
		uint texBase = mTexStream.tellg();
		uint objBase = mObjStream.tellg();

		uint nextChunk = getNextChunk(mFile);
		uint nextTexChunk = getNextChunk(mTexStream);
		uint nextObjChunk = getNextChunk(mObjStream);
		mTexStream.seekg(4, std::ios::cur);
		mObjStream.seekg(4, std::ios::cur);
		mFile.seekg(8, std::ios::cur);

		MCNK baseHeader = read<MCNK>(mFile);
		uint texSize = read<uint>(mTexStream);
		uint objSize = read<uint>(mObjStream);

		chunks[i].ofsMcnk = mOutFile.tellp();
		write((uint)'MCNK');
		write(sizeof(MCNK));
		write(baseHeader);

		copySubChunk(mFile, baseHeader.ofsHeight, baseHeader.ofsHeight, fileBase, chunks[i].ofsMcnk);
		copySubChunk(mFile, baseHeader.ofsNormal, baseHeader.ofsNormal, fileBase, chunks[i].ofsMcnk);

		convertOptionalSubchunks(fileBase, baseHeader, chunks[i].ofsMcnk);
		convertTexSubchunks(texSize, baseHeader, chunks[i].ofsMcnk, texBase);
		convertObjSubchunks(objSize, baseHeader, chunks[i].ofsMcnk, objBase);

		baseHeader.ofsMCLV = 0;
		baseHeader.unused = 0;
		baseHeader.ofsShadow = createEmptyChunk('MCSH') - chunks[i].ofsMcnk;
		baseHeader.sizeShadow = 8;

		chunks[i].size = (uint)mOutFile.tellp() - (uint)chunks[i].ofsMcnk;
		mOutFile.seekp(chunks[i].ofsMcnk + 4, std::ios::beg);
		write(chunks[i].size - 8);
		write(baseHeader);
		mOutFile.seekp(chunks[i].ofsMcnk + chunks[i].size);

		chunks[i].flags = chunks[i].asyncId = 0;
		mFile.seekg(nextChunk);

		mTexStream.seekg(nextTexChunk);
		mObjStream.seekg(nextObjChunk);

		ConsoleProgress::printProgress(i + 1, 256);
	}

	writeAt(92, chunks);
	std::cout << std::endl << std::endl;
}

uint ADT_Mop::getNextChunk(std::istream& strm) {
	strm.seekg(4, std::ios::cur);
	uint size = 0;
	if(!strm.read((char*)&size, 4))
		assert(false);

	uint nextChunk = (uint)strm.tellg() + size;
	if(!strm.seekg(-8, std::ios::cur))
		assert(false);

	return nextChunk;
}

void ADT_Mop::convertOptionalSubchunks(uint base, MCNK& header, uint baseOutput) {
	checkOptionalChunk(mFile, header.ofsLiquid, header.sizeLiqud, 'MCLQ', base, baseOutput);
	// convert to the actual size of the chunk for checkOptionalChunk
	header.nSndEmitters *= 0x1C;
	checkOptionalChunk(mFile, header.ofsSndEmitter, header.nSndEmitters, 'MCSE', base, baseOutput);
	// convert back to a count indicator.
	header.nSndEmitters /= 0x1C;

	if(header.ofsMCCV != 0) {
		copySubChunk(mFile, header.ofsMCCV, header.ofsMCCV, base, baseOutput);
	}
}

void ADT_Mop::convertObjSubchunks(uint objSize, MCNK& outHeader, uint outBase, uint objBase) {
	uint totalRead = 0;
	std::vector<char> refDoodad, refWmo;

	while(totalRead < objSize) {
		FileChunk c = read<FileChunk>(mObjStream);
		switch(c.signature) {
		case 'MCRD':
			{
				outHeader.nDoodadRefs = c.size / 4;
				refDoodad.resize(c.size);
				mObjStream.read(&refDoodad[0], c.size);
				totalRead += c.size + 8;
			}
			break;

		case 'MCRW':
			{
				outHeader.nMapObjRefs = 0;//c.size / 4;
				refWmo.resize(c.size);
				mObjStream.read(&refWmo[0], c.size);
				totalRead += c.size + 8;
			}
			break;

		default:
			{
				mObjStream.seekg(c.size, std::ios::cur);
				totalRead += c.size + 8;
				break;
			}
		}
	}

	outHeader.ofsRefs = (uint)mOutFile.tellp() - outBase;
	write('MCRF');
	write(refDoodad.size() + refWmo.size());
	if(refDoodad.size() > 0)
		mOutFile.write(&refDoodad[0], refDoodad.size());

	if(refWmo.size() > 0)
		mOutFile.write(&refWmo[0], refWmo.size());
}

void ADT_Mop::convertTexSubchunks(uint texSize, MCNK& outHeader, uint outBase, uint texBase) {
	uint totalRead = 0;
	while(totalRead < texSize) {
		FileChunk c = read<FileChunk>(mTexStream);
		switch(c.signature) {
		case 'MCLY':
			{
				outHeader.nLayers = c.size / 0x10;
				outHeader.ofsLayer = (uint)mOutFile.tellp() - outBase;
				write(c.signature);
				write(c.size);

				copyOut(mTexStream, c.size);
				totalRead += c.size + 8;
			}
			break;

		case 'MCAL':
			{
				outHeader.ofsAlpha = (uint)mOutFile.tellp() - outBase;
				outHeader.sizeAlpha = c.size + 8;
				write(c.signature);
				write(c.size);
				copyOut(mTexStream, c.size);
				totalRead += c.size + 8;
			}
			break;

		default:
			{
				mTexStream.seekg(c.size, std::ios::cur);
				totalRead += c.size + 8;
				break;
			}
		}
	}
}

void ADT_Mop::checkOptionalChunk(std::istream& input, uint& offset, uint& size, uint signature, uint base, uint baseOutput) {
	if(size <= 8 || offset == 0) {
		offset = createEmptyChunk(signature) - baseOutput;
		size = 8;
	} else {
		copySubChunk(input, offset, offset, size, base, baseOutput);
	}
}

uint ADT_Mop::createEmptyChunk(uint signature) {
	uint ret = mOutFile.tellp();
	write(signature);
	write((uint)0);

	return ret;
}

void ADT_Mop::copySubChunk(std::istream& stream, uint offset, uint& newOffset, uint base, uint baseOutput) {
	uint size = 0;
	copySubChunk(stream, offset, newOffset, size, base, baseOutput);
}

void ADT_Mop::copySubChunk(std::istream& stream, uint offset, uint& newOffset, uint& outSize, uint base, uint baseOutput) {
	stream.seekg(offset + base, std::ios::beg);
	newOffset = (uint)mOutFile.tellp() - baseOutput;
	uint signature = 0, size = 0;
	stream.read((char*)&signature, 4);
	stream.read((char*)&size, 4);

	write(signature);
	write(size);

	outSize = size;

	std::vector<char> content(size);
	stream.read(&content[0], size);
	mOutFile.write(&content[0], size);
}

void ADT_Mop::convertHeaders() {
	auto cnk = read<FileChunk>();
	assert(cnk.signature == 'MVER');

	MVER mver = read<MVER>();

	write(cnk);
	write(mver);

	ConsoleProgress::printProgress(1, 2);

	cnk = read<FileChunk>();
	assert(cnk.signature == 'MHDR');

	MHDR header = read<MHDR>();
	memset(&outHeader, 0, sizeof(MHDR));

	outHeader.flags = header.flags;
	outHeader.ofsMcin = 64;

	write(cnk);
	write(outHeader);
}

void ADT_Mop::convertLiquid() {
	copyMainChunk(mFile, 'MH2O', outHeader.ofsMh2o);
}

void ADT_Mop::convertVariousMain() {
	copyMainChunk(mFile, 'MFBO', outHeader.ofsMfbo);
	copyMainChunk(mFile, 'MTFX', outHeader.ofsMtfx);
	copyMainChunk(mTexStream, 'MTEX', outHeader.ofsMtex);
}

void ADT_Mop::copyMainChunk(std::istream& strm, uint id, uint& ofs) {
	if(!SeekChunk(strm, id)) {
		ofs = 0;
		return;
	}

	ofs = (uint)mOutFile.tellp() - 20;

	FileChunk cnk;
	strm.read((char*)&cnk.signature, 4);
	strm.read((char*)&cnk.size, 4);
	write(cnk);
	copyOut(strm, cnk.size);
}