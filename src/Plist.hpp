//
//   PlistCpp Property List (plist) serialization and parsing library.
//
//   https://github.com/animetrics/PlistCpp
//   
//   Copyright (c) 2011 Animetrics Inc. (marc@animetrics.com)
//   
//   Permission is hereby granted, free of charge, to any person obtaining a copy
//   of this software and associated documentation files (the "Software"), to deal
//   in the Software without restriction, including without limitation the rights
//   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//   copies of the Software, and to permit persons to whom the Software is
//   furnished to do so, subject to the following conditions:
//   
//   The above copyright notice and this permission notice shall be included in
//   all copies or substantial portions of the Software.
//   
//   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//   THE SOFTWARE.
   
#ifndef __PLIST_H__
#define __PLIST_H__

#include <boost/any.hpp>
#include <string>
#include <vector>
#include <list>
#include <map>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <math.h>
#include "PlistDate.hpp"
#include "pugixml.hpp"
#include "base64.hpp"

#if defined(_WIN32) || defined(_WIN64)
#include <stdint.h>
#endif



class Plist
{

	public:

		// Public read methods.  Plist type (binary or xml) automatically detected.
		
		template<typename T>
		static void readPlist(const char* byteArray, int64_t size, T& message);
		template<typename T>
		static void readPlist(std::istream& stream, T& message);
		template<typename T>
		static void readPlist(const std::string& filename, T& message);

		// Public binary write methods.
		
		static void writePlistBinary(std::ostream& stream, const boost::any& message);
		static void writePlistBinary(std::vector<char>& plist, const boost::any& message);
		static void writePlistBinary(const std::string& filename, const boost::any& message);

		// Public XML write methods.
		
		static void writePlistXML(std::ostream& stream, const boost::any& message);
		static void writePlistXML(std::vector<char>& plist, const boost::any& message);
		static void writePlistXML(const std::string& filename, const boost::any& message);

	private:

		struct PlistHelperData
		{
			public:

				// binary helper data
				std::vector<int32_t> _offsetTable;
				std::vector<unsigned char> _objectTable;
				int32_t _offsetByteSize;
				int64_t _offsetTableOffset;

				int32_t _objRefSize;
				int32_t _refCount;
		};

		static void writePlistBinary(
				PlistHelperData& d, 
				const boost::any& message);

		static void writePlistXML(
				pugi::xml_document& doc,
				const boost::any& message);

		// helper functions

		// msvc <= 2005 doesn't have std::vector::data() method

		template<typename T>
		static T* vecData(std::vector<T>& vec)
		{
			if(vec.size() > 0)
				return &vec[0];
			else
				throw std::runtime_error("Plist::vecData trying to get pointer to empty std::vector");
		}

		template<typename T>
		static const T* vecData(const std::vector<T>& vec)
		{
			if(vec.size() > 0)
				return &vec[0];
			else
				throw std::runtime_error("Plist::vecData trying to get pointer to empty std::vector");
		}

		// xml helper functions

		template<typename T>
			static std::string stringFromValue(const T& value);
		template<typename T>
			static void writeXMLSimpleNode(pugi::xml_node& node, const char* name, const boost::any& obj);

		// xml parsing
		static std::map<std::string, boost::any> parseDictionary(pugi::xml_node& node);
		static std::vector<boost::any> parseArray(pugi::xml_node& node);
		static std::vector<char> base64Decode(const char* data);
		static void base64Encode(std::string& dataEncoded, const std::vector<char>& data);
		static PlistDate parseDate(pugi::xml_node& node);
		static boost::any parse(pugi::xml_node& doc);

		// xml writing

		static void writeXMLArray(pugi::xml_node& node, const std::vector<boost::any>& array);
		static void writeXMLDictionary(pugi::xml_node& node, const std::map<std::string, boost::any>& message);
		static void writeXMLNode(pugi::xml_node& node, const boost::any& obj);


		// binary helper functions
		
		template <typename IntegerType>
			static IntegerType bytesToInt(const unsigned char* bytes, bool littleEndian = true );
		static double bytesToDouble(const unsigned char* bytes, bool littleEndian = true);
		static std::vector<unsigned char> doubleToBytes(double val, bool littleEndian = true);
		template<typename IntegerType>
			static std::vector<unsigned char> intToBytes(IntegerType val, bool littleEndian = true);
		static std::vector<unsigned char> getRange(const unsigned char* origBytes, int64_t index, int64_t size);
		static std::vector<unsigned char> getRange(const std::vector<unsigned char>& origBytes, int64_t index, int64_t size);
		static std::vector<char> getRange(const char* origBytes, int64_t index, int64_t size);

		// binary parsing
		
		static boost::any parseBinary(const PlistHelperData& d, int objRef);
		static std::map<std::string, boost::any>  parseBinaryDictionary(const PlistHelperData& d, int objRef);
		static std::vector<boost::any>  parseBinaryArray(const PlistHelperData& d, int objRef);
		static std::vector<int32_t> getRefsForContainers(const PlistHelperData& d, int objRef);
		static int64_t parseBinaryInt(const PlistHelperData& d, int headerPosition, int& intByteCount);
		static double parseBinaryReal(const PlistHelperData& d, int headerPosition);
		static PlistDate parseBinaryDate(const PlistHelperData& d, int headerPosition);
		static bool parseBinaryBool(const PlistHelperData& d, int headerPosition);
		static std::string parseBinaryString(const PlistHelperData& d, int objRef);
		static std::vector<char> parseBinaryByteArray(const PlistHelperData& d, int headerPosition);
		static inline std::vector<unsigned char> regulateNullBytes(const std::vector<unsigned char>& origBytes, unsigned int minBytes);
		static void parseTrailer(PlistHelperData& d, const std::vector<unsigned char>& trailer);
		static void parseOffsetTable(PlistHelperData& d, const std::vector<unsigned char>& offsetTableBytes);
		static int32_t getCount(const PlistHelperData& d, int bytePosition, unsigned char headerByte, int& startOffset);

		// binary writing
		
		static int countAny(const boost::any& object);
		static int countDictionary(const std::map<std::string, boost::any>& dictionary);
		static int countArray(const std::vector<boost::any>& array);
		static std::vector<unsigned char> writeBinaryDictionary(PlistHelperData& d, const std::map<std::string, boost::any>& dictionary);
		static std::vector<unsigned char> writeBinaryArray(PlistHelperData& d, const std::vector<boost::any>& array);
		static std::vector<unsigned char> writeBinaryByteArray(PlistHelperData& d, const std::vector<char>& byteArray);
		static std::vector<unsigned char> writeBinaryInteger(PlistHelperData& d, int64_t value, bool write);
		static std::vector<unsigned char> writeBinaryBool(PlistHelperData& d, bool value);
		static std::vector<unsigned char> writeBinaryDate(PlistHelperData& d, const PlistDate& date);
		static std::vector<unsigned char> writeBinaryDouble(PlistHelperData& d, double value);
		static std::vector<unsigned char> writeBinary(PlistHelperData& d, const boost::any& obj);
		static std::vector<unsigned char> writeBinaryString(PlistHelperData& d, const std::string& value, bool head);

};

template<typename T>
inline void Plist::writeXMLSimpleNode(pugi::xml_node& node, const char* name, const boost::any& obj)
{
	pugi::xml_node newNode;
	newNode = node.append_child(name);
	newNode.append_child(pugi::node_pcdata).set_value(stringFromValue(boost::any_cast<const T&>(obj)).c_str());
}

inline void Plist::writeXMLNode(pugi::xml_node& node, const boost::any& obj)
{
	using namespace std;

	// the following are used just for type checking
	
	static boost::any aString = string();
	static boost::any aDouble = double(0);
	static boost::any aFloat = float(0);
	static boost::any anInt32 = int32_t(0);
	static boost::any anInt64 = int64_t(0);
	static boost::any aLong = long(0);
	static boost::any aShort = short(0);
	static boost::any anArray = vector<boost::any>();
	static boost::any aMap = map<string, boost::any>();
	static boost::any aByteArray = vector<char>();
	static boost::any aPlistDate = PlistDate();
	static boost::any aPlistBool = bool(true);

	if(obj.type() == anInt32.type())
		writeXMLSimpleNode<int32_t>(node, "integer", obj);
	else if(obj.type() == anInt64.type())
		writeXMLSimpleNode<int64_t>(node, "integer", obj);
	else if(obj.type() == aLong.type())
		writeXMLSimpleNode<long>(node, "integer", obj);
	else if(obj.type() == aShort.type())
		writeXMLSimpleNode<short>(node, "integer", obj);
	else if(obj.type() == aMap.type())
		writeXMLDictionary(node, boost::any_cast<const map<string, boost::any>& >(obj));
	else if(obj.type() == aString.type())
		writeXMLSimpleNode<string>(node, "string", obj);
	else if(obj.type() == anArray.type())
		writeXMLArray(node, boost::any_cast<const vector<boost::any>& >(obj));
	else if(obj.type() == aByteArray.type())
	{
		string dataEncoded;
		base64Encode(dataEncoded, boost::any_cast<const vector<char>& >(obj));
		writeXMLSimpleNode<string>(node, "data", dataEncoded); 
	}
	else if(obj.type() == aDouble.type())
		writeXMLSimpleNode<double>(node, "real", obj);
	else if(obj.type() == aFloat.type())
		writeXMLSimpleNode<float>(node, "real", obj);
	else if(obj.type() == aPlistDate.type())
		writeXMLSimpleNode<string>(node, "date", boost::any_cast<const PlistDate&>(obj).timeAsXMLConvention());
	else if(obj.type() == aPlistBool.type())
	{
		bool value = boost::any_cast<const bool&>(obj);
		if(value)
			node.append_child("true");
		else
			node.append_child("false");
	}
	else
		throw runtime_error((string("Plist Error: Can't serialize type ") + obj.type().name()).c_str());

}

inline void Plist::writeXMLArray(
		pugi::xml_node& node,
		const std::vector<boost::any>& array)
{
	using namespace std;
	pugi::xml_node newNode = node.append_child("array");
	for(vector<boost::any>::const_iterator it = array.begin();
			it != array.end();
			++it)
		writeXMLNode(newNode, *it);
}

inline void Plist::writeXMLDictionary(
		pugi::xml_node& node, 
		const std::map<std::string, boost::any>& message)
{
	using namespace std;
	pugi::xml_node newNode = node.append_child("dict");
	for(map<string, boost::any>::const_iterator it = message.begin();
			it != message.end();
			++it)
	{
		pugi::xml_node keyNode = newNode.append_child("key");
		keyNode.append_child(pugi::node_pcdata).set_value(it->first.c_str());
		writeXMLNode(newNode, it->second);
	}

}

inline void Plist::writePlistXML(
		pugi::xml_document& doc,
		const boost::any& message)
//		const std::map<std::string, boost::any>& message)
{

	// declaration node
	pugi::xml_node decNode = doc.append_child(pugi::node_declaration);
	decNode.append_attribute("version") = "1.0";
	decNode.append_attribute("encoding") = "UTF-8";

	// doctype node
	doc.append_child(pugi::node_doctype).set_value("plist PUBLIC \"-//Apple//DTD PLIST 1.0//EN\" \"http://www.apple.com/DTDs/PropertyList-1.0.dtd\"");

	// root node
	pugi::xml_node plistNode = doc.append_child("plist");
	plistNode.append_attribute("version") = "1.0";

	writeXMLNode(plistNode, message);
}

inline void Plist::writePlistBinary(
		PlistHelperData& d, 
		const boost::any& message)
{
	using namespace std;
	//int totalRefs =  countDictionary(message);
	int totalRefs =  countAny(message) - 1;
	d._refCount = totalRefs;

	d._objRefSize = regulateNullBytes(intToBytes<int32_t>(d._refCount), 1).size();

	//writeBinaryDictionary(d, message);
	writeBinary(d, message);
	writeBinaryString(d, "bplist00", false);
	d._offsetTableOffset = (int64_t) d._objectTable.size();
	d._offsetTable.push_back(d._objectTable.size() - 8);
	d._offsetByteSize = regulateNullBytes(intToBytes<int>(d._offsetTable.back()), 1).size();

	vector<unsigned char> offsetBytes;

	reverse(d._offsetTable.begin(), d._offsetTable.end());

	for(unsigned int i = 0; i < d._offsetTable.size(); ++i)
	{
		d._offsetTable[i] = d._objectTable.size() - d._offsetTable[i];
		vector<unsigned char> buffer = regulateNullBytes(intToBytes<int>(d._offsetTable[i]), d._offsetByteSize);
		//reverse(buffer.begin(), buffer.end());

		offsetBytes.insert(offsetBytes.end(), buffer.rbegin(), buffer.rend());
	}

	d._objectTable.insert(d._objectTable.end(), offsetBytes.begin(), offsetBytes.end());

	vector<unsigned char> dummy(6, 0);
	d._objectTable.insert(d._objectTable.end(), dummy.begin(), dummy.end());
	d._objectTable.push_back((unsigned char) (d._offsetByteSize));
	d._objectTable.push_back((unsigned char) (d._objRefSize));
	vector<unsigned char> temp = intToBytes<int64_t>((int64_t) totalRefs + 1);
	d._objectTable.insert(d._objectTable.end(), temp.rbegin(), temp.rend());
	temp = intToBytes<int64_t>(0);
	d._objectTable.insert(d._objectTable.end(), temp.begin(), temp.end());
	temp = intToBytes<int64_t>(d._offsetTableOffset);
	d._objectTable.insert(d._objectTable.end(), temp.rbegin(), temp.rend());
}

inline void Plist::writePlistBinary(std::vector<char>& plist, const boost::any& message)
{
	PlistHelperData d;
	writePlistBinary(d, message);
	plist.resize(d._objectTable.size());
	std::copy((const char*) vecData(d._objectTable), (const char*) vecData(d._objectTable) + d._objectTable.size(), plist.begin());
}

inline void Plist::writePlistBinary(
		std::ostream& stream,
		const boost::any& message)
{
	PlistHelperData d;
	writePlistBinary(d, message);
	stream.write((const char*) vecData(d._objectTable), d._objectTable.size());
}

inline void Plist::writePlistBinary(
				const std::string& filename, 
				const boost::any& message)
{
  std::ofstream stream(filename.c_str(), std::ios::binary);
  writePlistBinary(stream, message);
  stream.close();
}

inline void Plist::writePlistXML(std::vector<char>& plist, const boost::any& message)
{
	std::stringstream ss;
	writePlistXML(ss, message);

	std::istreambuf_iterator<char> beg(ss);
	std::istreambuf_iterator<char> end;
	plist.clear();
	plist.insert(plist.begin(), beg, end);
}

inline void Plist::writePlistXML(
		std::ostream& stream,
		const boost::any& message)
{
	pugi::xml_document doc;
	writePlistXML(doc, message);
	doc.save(stream);
}

inline void Plist::writePlistXML(
		const std::string& filename,
		const boost::any& message)
{

	std::ofstream stream(filename.c_str(), std::ios::binary);
	writePlistXML(stream, message);
	stream.close();
}

inline int Plist::countAny(const boost::any& object)
{
	using namespace std;
	static boost::any dict = map<string, boost::any>();
	static boost::any array = vector<boost::any>();

	int count = 0;
	if(object.type() == dict.type())
		count += countDictionary(boost::any_cast<map<string, boost::any> >(object)) + 1;
	else if (object.type() == array.type())
		count += countArray(boost::any_cast<vector<boost::any> >(object)) + 1;
	else
		++count;

	return count;
}

inline std::vector<unsigned char> Plist::writeBinary(PlistHelperData& d, const boost::any& obj)
{
	using namespace std;

	// the following are used just for type checking
	
	static boost::any aString = string();
	static boost::any aDouble = double(0);
	static boost::any aFloat = float(0);
	static boost::any anInt32 = int32_t(0);
	static boost::any anInt64 = int64_t(0);
	static boost::any aLong = long(0);
	static boost::any aShort = short(0);
	static boost::any anArray = vector<boost::any>();
	static boost::any aMap = map<string, boost::any>();
	static boost::any aByteArray = vector<char>();
	static boost::any aPlistDate = PlistDate();
	static boost::any aPlistBool = bool(true);

	std::vector<unsigned char> value;
	if(obj.type() == anInt32.type())
		value = writeBinaryInteger(d, boost::any_cast<const int32_t&>(obj), true);
	else if(obj.type() == anInt64.type())
		value = writeBinaryInteger(d, boost::any_cast<const int64_t&>(obj), true);
	else if(obj.type() == aLong.type())
		value = writeBinaryInteger(d, boost::any_cast<const long&>(obj), true);
	else if(obj.type() == aShort.type())
		value = writeBinaryInteger(d, boost::any_cast<const short&>(obj), true);
	else if(obj.type() == aMap.type())
		value = writeBinaryDictionary(d, boost::any_cast<const map<string, boost::any>& >(obj));
	else if(obj.type() == aString.type())
		value = writeBinaryString(d, boost::any_cast<const string&>(obj), true);
	else if(obj.type() == anArray.type())
		value = writeBinaryArray(d, boost::any_cast<const vector<boost::any>& >(obj));
	else if(obj.type() == aByteArray.type())
		value = writeBinaryByteArray(d, boost::any_cast<const vector<char>& >(obj));
	else if(obj.type() == aDouble.type())
		value = writeBinaryDouble(d, boost::any_cast<const double&>(obj));
	else if(obj.type() == aFloat.type())
		value = writeBinaryDouble(d, boost::any_cast<const float&>(obj));
	else if(obj.type() == aPlistDate.type())
		value = writeBinaryDate(d, boost::any_cast<const PlistDate&>(obj));
	else if(obj.type() == aPlistBool.type())
		value = writeBinaryBool(d, boost::any_cast<const bool&>(obj));
	else
		throw runtime_error((string("Plist Error: Can't serialize type ") + obj.type().name()).c_str());

	return value;
}

inline std::vector<unsigned char> Plist::writeBinaryByteArray(PlistHelperData& d, const std::vector<char>& byteArray)
{
	using namespace std;
	vector<unsigned char> header;
	if(byteArray.size() < 15)
		header.push_back(0x40 | ((unsigned char) byteArray.size()));
	else
	{
		header.push_back(0x40 | 0xf);
		vector<unsigned char> theSize = writeBinaryInteger(d, byteArray.size(), false);
		header.insert(header.end(), theSize.begin(), theSize.end());
	}

	vector<unsigned char> buffer(header);
	buffer.insert(buffer.end(), (unsigned char*) vecData(byteArray), (unsigned char*) vecData(byteArray) + byteArray.size());
	d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());

	return buffer;
}

inline std::vector<unsigned char> Plist::writeBinaryArray(PlistHelperData& d, const std::vector<boost::any>& array)
{
	using namespace std;

	vector<int32_t> refs;
	for(vector<boost::any>::const_reverse_iterator it = array.rbegin();
			it != array.rend();
			++it)
	{
		writeBinary(d, *it);
		d._offsetTable.push_back(d._objectTable.size());
		refs.push_back(d._refCount);
		d._refCount--;
	}

	vector<unsigned char> header;
	if (array.size() < 15)
	{
		header.push_back(0xA0 | ((unsigned char) array.size()));
	}
	else
	{
		header.push_back(0xA0 | 0xf);
		vector<unsigned char> theSize = writeBinaryInteger(d, array.size(), false);
		header.insert(header.end(), theSize.begin(), theSize.end());
	}

	// try to do this more efficiently.  Not good to insert at the begining of buffer.

	vector<unsigned char> buffer;
	for(vector<int32_t>::const_iterator it = refs.begin();
			it != refs.end();
			++it)
	{
		vector<unsigned char> refBuffer = regulateNullBytes(intToBytes<int32_t>(*it), d._objRefSize);
//		reverse(refBuffer.begin(), refBuffer.end());
		buffer.insert(buffer.begin(), refBuffer.rbegin(), refBuffer.rend());
	}

	buffer.insert(buffer.begin(), header.begin(), header.end());

	d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());

	return buffer;
}

inline std::vector<unsigned char> Plist::writeBinaryDictionary(PlistHelperData& d, const std::map<std::string, boost::any>& dictionary)
{
	using namespace std;

	vector<int32_t> refs;
	for(map<string, boost::any>::const_reverse_iterator it = dictionary.rbegin();
			it != dictionary.rend();
			++it)
	{
		writeBinary(d, it->second);
		d._offsetTable.push_back(d._objectTable.size());
		refs.push_back(d._refCount);
		d._refCount--;
	}

	for(map<string, boost::any>::const_reverse_iterator it = dictionary.rbegin();
			it != dictionary.rend();
			++it)
	{
		writeBinary(d, it->first);
		d._offsetTable.push_back(d._objectTable.size());
		refs.push_back(d._refCount);
		d._refCount--;
	}

	vector<unsigned char> header;
	if (dictionary.size() < 15)
	{
		header.push_back(0xD0 | ((unsigned char) dictionary.size()));
	}
	else
	{
		header.push_back(0xD0 | 0xf);
		vector<unsigned char> theSize = writeBinaryInteger(d, dictionary.size(), false);
		header.insert(header.end(), theSize.begin(), theSize.end());
	}

	// try to do this more efficiently.  Not good to insert at the begining of buffer.

	vector<unsigned char> buffer;
	for(vector<int32_t>::const_iterator it = refs.begin();
			it != refs.end();
			++it)
	{
		vector<unsigned char> refBuffer = regulateNullBytes(intToBytes<int32_t>(*it), d._objRefSize);
//		reverse(refBuffer.begin(), refBuffer.end());
		buffer.insert(buffer.begin(), refBuffer.rbegin(), refBuffer.rend());
	}

	buffer.insert(buffer.begin(), header.begin(), header.end());

	d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());

	return buffer;
}

inline std::vector<unsigned char> Plist::writeBinaryDouble(PlistHelperData& d, double value)
{
	using namespace std;
	vector<unsigned char> buffer = regulateNullBytes(doubleToBytes(value), 4);
	while(buffer.size() != pow(2., log((double) buffer.size()) / log(2.)))
		buffer.push_back(0);

	unsigned char header = 0x20 | (int)(log((double) buffer.size()) / log(2.));
	buffer.push_back(header);
	reverse(buffer.begin(), buffer.end());

	d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());

	return buffer;
}

inline std::vector<unsigned char> Plist::writeBinaryBool(PlistHelperData& d, bool value)
{
	std::vector<unsigned char> buffer;
	if(value)
		buffer.push_back(0x09);
	else
		buffer.push_back(0x08);

	d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());
	return buffer;
}

inline std::vector<unsigned char> Plist::writeBinaryDate(PlistHelperData& d, const PlistDate& date)
{
	std::vector<unsigned char> buffer;

	// need to serialize as Apple epoch.
	
	double macTime = date.timeAsAppleEpoch();

	buffer = doubleToBytes(macTime, false);
	buffer.insert(buffer.begin(), 0x33); 

	d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());
	return buffer;
}

inline std::vector<unsigned char> Plist::writeBinaryInteger(PlistHelperData& d, int64_t value, bool write)
{
	using namespace std;

	// The integer is initially forced to be 64 bit because it must be serialized
	// as 8 bytes if it is negative.   If it is not negative, the
	// regulateNullBytes step will reduce the representation down to the min
	// power base 2 bytes needed to store it.
	
	vector<unsigned char> buffer = intToBytes<int64_t>(value);
	buffer = regulateNullBytes(intToBytes<int64_t>(value), 1);
	while(buffer.size() != pow(2., log((double) buffer.size()) / log(2.)))
		buffer.push_back(0);

	unsigned char header = 0x10 | (int)(log((double) buffer.size()) / log(2.));
	buffer.push_back(header);
	reverse(buffer.begin(), buffer.end());

	if(write)
		d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());

	return buffer;
}

inline std::vector<unsigned char> Plist::writeBinaryString(PlistHelperData& d, const std::string& value, bool head)
{
	using namespace std;
	vector<unsigned char> buffer;
	buffer.reserve(value.size());

	for(std::string::const_iterator it = value.begin();
			it != value.end();
			++it)
		buffer.push_back((unsigned char) *it);

	if(head)
	{
		vector<unsigned char> header;
		if (value.length() < 15)
			header.push_back(0x50 | ((unsigned char) value.length()));
		else
		{
			header.push_back(0x50 | 0xf);
			vector<unsigned char> theSize = writeBinaryInteger(d, buffer.size(), false);
			header.insert(header.end(), theSize.begin(), theSize.end());
		}
		buffer.insert(buffer.begin(), header.begin(), header.end());
	}

	d._objectTable.insert(d._objectTable.begin(), buffer.begin(), buffer.end());

	return buffer;
}	

inline int Plist::countDictionary(const std::map<std::string, boost::any>& dictionary)
{
	using namespace std;

	int count = 0;
	for(std::map<std::string, boost::any>::const_iterator it = dictionary.begin();
			it != dictionary.end();
			++it)
	{
		++count;
		count += countAny(it->second);
	}

	return count;
}

inline int Plist::countArray(const std::vector<boost::any>& array)
{
	using namespace std;
	int count = 0;
	for(std::vector<boost::any>::const_iterator it = array.begin();
			it != array.end();
			++it)
		count += countAny(*it);

	return count;
}

template <typename T>
inline void Plist::readPlist( std::istream& stream, T& message)
{
	int start = stream.tellg();
	stream.seekg(0, std::ifstream::end);
	int size = ((int) stream.tellg()) - start;
	if(size > 0)
	{
		stream.seekg(0, std::ifstream::beg);
		std::vector<char> buffer(size);
		stream.read( (char *)&buffer[0], size );

		readPlist(&buffer[0], size, message);
	}
	else
	{
		throw std::runtime_error("Can't read zero length data");
	}

}

template <typename T>
inline void Plist::readPlist(const std::string& filename, T& message)
{
  std::ifstream stream(filename.c_str(), std::ios::binary);
  if(!stream)
		throw std::runtime_error((std::string("Can't open file: ") + filename).c_str());
  readPlist(stream, message);
  stream.close();
}


template<typename T>
inline void Plist::readPlist(const char* byteArrayTemp, int64_t size, T& message)
{
	using namespace std;
	const unsigned char* byteArray = (const unsigned char*) byteArrayTemp;
	if (!byteArray || (size == 0))
		throw std::runtime_error("Plist: Empty plist data");

	// infer plist type from header.  If it has the bplist00 header as first 8
	// bytes, then it's a binary plist.  Otherwise, assume it's XML
	
	std::string magicHeader((const char*) byteArray, 8);
	if(magicHeader == "bplist00")
	{
		PlistHelperData d;
		parseTrailer(d, getRange(byteArray, size - 32, 32));

		d._objectTable = getRange(byteArray, 0, d._offsetTableOffset);
		std::vector<unsigned char> offsetTableBytes = getRange(byteArray, d._offsetTableOffset, size - d._offsetTableOffset - 32);

		parseOffsetTable(d, offsetTableBytes);

		message = boost::any_cast<T>(parseBinary(d, 0));
	}
	else
	{
		pugi::xml_document doc;
		pugi::xml_parse_result result = doc.load_buffer(byteArray, size);
		if(!result)
			throw std::runtime_error((string("Plist: XML parsed with error ") + result.description()).c_str());

		pugi::xml_node rootNode = doc.child("plist").first_child();
		message = boost::any_cast<T>(parse(rootNode));
	}

}

inline std::map<std::string, boost::any> Plist::parseDictionary(pugi::xml_node& node)
{
	using namespace std;

	map<string, boost::any> dict;
	for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
	{
		if(string("key") != it->name())
			throw runtime_error("Plist: XML dictionary key expected but not found");

		string key(it->first_child().value());
		++it;

		if(it == node.end())
			throw runtime_error("Plist: XML dictionary value expected for key " + key + "but not found");
		else if(string("key") == it->name())
			throw runtime_error("Plist: XML dictionary value expected for key " + key + "but found another key node");

		dict[key] = parse(*it); 
	}

	return dict;
}

inline std::vector<boost::any> Plist::parseArray(pugi::xml_node& node)
{
	using namespace std;

	vector<boost::any> array;
	for(pugi::xml_node_iterator it = node.begin(); it != node.end(); ++it)
		array.push_back(parse(*it));

	return array;
}

inline PlistDate Plist::parseDate(pugi::xml_node& node)
{

	PlistDate date;
	date.setTimeFromXMLConvention(node.first_child().value());

	return date;
}

inline std::vector<char> Plist::base64Decode(const char* encodedData)
{
	using namespace std;

	vector<char> data;
	insert_iterator<vector<char> > ii(data, data.begin());
	base64<char> b64;
	int state = 0;
	b64.get(encodedData, encodedData + strlen(encodedData), ii, state); 

	return data;
}

inline void Plist::base64Encode(std::string& dataEncoded, const std::vector<char>& data)
{
	using namespace std;
	dataEncoded.clear();
	insert_iterator<string> ii(dataEncoded, dataEncoded.begin());
	base64<char> b64;
	int state = 0;

#if defined(_WIN32) || defined(_WIN64)
	b64.put(data.begin(), data.end(), ii, state , base64<>::crlf());
#else
	b64.put(data.begin(), data.end(), ii, state , base64<>::lf());
#endif

}

inline boost::any Plist::parse(pugi::xml_node& node)
{
	using namespace std;

	string nodeName = node.name();
	
	boost::any result;
	if("dict" == nodeName)
		result = parseDictionary(node);
	else if("array" == nodeName)
		result = parseArray(node);
	else if("string" == nodeName)
		result = string(node.first_child().value());
	else if("integer" == nodeName)
		result = (int64_t) atoi(node.first_child().value());
	else if("real" == nodeName)
		result = atof(node.first_child().value());
	else if("false" == nodeName)
		result = bool(false);
	else if("true" == nodeName)
		result = bool(true);
	else if("data" == nodeName)
		result = base64Decode(node.first_child().value());
	else if("date" == nodeName)
		result = parseDate(node);
	else
		throw runtime_error(string("Plist: XML unknown node type " + nodeName));
		
	return result;
}

inline void Plist::parseOffsetTable(PlistHelperData& d, const std::vector<unsigned char>& offsetTableBytes)
{
	for (unsigned int i = 0; i < offsetTableBytes.size(); i += d._offsetByteSize)
	{
		std::vector<unsigned char> temp = getRange(offsetTableBytes, i, d._offsetByteSize);
		std::reverse(temp.begin(), temp.end());
		d._offsetTable.push_back(
				bytesToInt<int32_t>(
					vecData(regulateNullBytes(temp, 4))));
	}
}

inline void Plist::parseTrailer(PlistHelperData& d, const std::vector<unsigned char>& trailer)
{
	d._offsetByteSize = bytesToInt<int32_t>(vecData(regulateNullBytes(getRange(trailer, 6, 1), 4)));
	d._objRefSize = bytesToInt<int32_t>(vecData(regulateNullBytes(getRange(trailer, 7, 1), 4)));

	std::vector<unsigned char> refCountBytes = getRange(trailer, 12, 4);
//	std::reverse(refCountBytes.begin(), refCountBytes.end());
	d._refCount = bytesToInt<int32_t>(vecData(refCountBytes), false); 
	
	std::vector<unsigned char> offsetTableOffsetBytes = getRange(trailer, 24, 8);
//	std::reverse(offsetTableOffsetBytes.begin(), offsetTableOffsetBytes.end());
	d._offsetTableOffset = bytesToInt<int64_t>(vecData(offsetTableOffsetBytes), false);
}


inline std::vector<unsigned char> Plist::regulateNullBytes(const std::vector<unsigned char>& origBytes, unsigned int minBytes)
{

	std::vector<unsigned char> bytes(origBytes);
	while((bytes.back() == 0) && (bytes.size() > minBytes))
		bytes.pop_back();

	while(bytes.size() < minBytes)
		bytes.push_back(0);

	return bytes;
}

inline boost::any Plist::parseBinary(const PlistHelperData& d, int objRef)
{
	unsigned char header = d._objectTable[d._offsetTable[objRef]];
	switch (header & 0xF0)
	{
		case 0x00:
			{
				return parseBinaryBool(d, d._offsetTable[objRef]);
			}
		case 0x10:
			{
				int intByteCount;
				return parseBinaryInt(d, d._offsetTable[objRef], intByteCount);
			}
		case 0x20:
			{
				return parseBinaryReal(d, d._offsetTable[objRef]);
			}
		case 0x30:
			{
				return parseBinaryDate(d, d._offsetTable[objRef]);
			}
		case 0x40:
			{
				return parseBinaryByteArray(d, d._offsetTable[objRef]);
			}
		case 0x50:
			{
				return parseBinaryString(d, d._offsetTable[objRef]);
			}
		case 0xD0:
			{
				return parseBinaryDictionary(d, objRef);
			}
		case 0xA0:
			{
				return parseBinaryArray(d, objRef);
			}
	}
	throw std::runtime_error("This type is not supported");
}

inline std::vector<int32_t> Plist::getRefsForContainers(const PlistHelperData& d, int objRef)
{
	using namespace std;
	int32_t refCount = 0;
	int refStartPosition;
	refCount = getCount(d, d._offsetTable[objRef], d._objectTable[d._offsetTable[objRef]], refStartPosition);
	refStartPosition += d._offsetTable[objRef];

	vector<int32_t> refs;
	int mult = 1;
	if((((unsigned char) d._objectTable[d._offsetTable[objRef]]) & 0xF0) == 0xD0)
		mult = 2;
	for (int i = refStartPosition; i < refStartPosition + refCount * mult * d._objRefSize; i += d._objRefSize)
	{
		std::vector<unsigned char> refBuffer = getRange(d._objectTable, i, d._objRefSize);
		reverse(refBuffer.begin(), refBuffer.end());
		refs.push_back(bytesToInt<int32_t>(vecData(regulateNullBytes(refBuffer, 4))));
	}

	return refs;
}

inline std::vector<boost::any>  Plist::parseBinaryArray(const PlistHelperData& d, int objRef)
{
	using namespace std;
	vector<int32_t> refs = getRefsForContainers(d, objRef);
	int32_t refCount = refs.size();

	vector<boost::any> array;
	for(int i = 0; i < refCount; ++i)
		array.push_back(parseBinary(d, refs[i]));

	return array;
}

inline std::map<std::string, boost::any> Plist::parseBinaryDictionary(const PlistHelperData& d, int objRef)
{
	using namespace std;
	vector<int32_t> refs = getRefsForContainers(d, objRef);
	int32_t refCount = refs.size() / 2;

	map<string, boost::any> dict;
	for (int i = 0; i < refCount; i++)
	{
		boost::any keyAny = parseBinary(d, refs[i]);

		try
		{
			std::string& key = boost::any_cast<std::string&>(keyAny);
			dict[key] =  parseBinary(d, refs[i + refCount]);
		}
		catch(boost::bad_any_cast& )
		{
			throw std::runtime_error("Error parsing dictionary.  Key can't be parsed as a string");
		}
	}

	return dict;
}

inline std::string Plist::parseBinaryString(const PlistHelperData& d, int headerPosition)
{
	unsigned char headerByte = d._objectTable[headerPosition];
	int charStartPosition;
	int32_t charCount = getCount(d, headerPosition, headerByte, charStartPosition);
	charStartPosition += headerPosition;

	std::vector<unsigned char> characterBytes = getRange(d._objectTable, charStartPosition, charCount);
	std::string buffer = std::string((char*) vecData(characterBytes), characterBytes.size()); 
	return buffer;
}

inline int64_t Plist::parseBinaryInt(const PlistHelperData& d, int headerPosition, int& intByteCount)
{
	unsigned char header = d._objectTable[headerPosition];
	intByteCount = pow(2., header & 0xf);
	std::vector<unsigned char> buffer = getRange(d._objectTable, headerPosition + 1, intByteCount);
	reverse(buffer.begin(), buffer.end());

	return bytesToInt<int64_t>(vecData(regulateNullBytes(buffer, 4)));
}

inline double Plist::parseBinaryReal(const PlistHelperData& d, int headerPosition)
{
	unsigned char header = d._objectTable[headerPosition];
	int byteCount = pow(2., header & 0xf);
	std::vector<unsigned char> buffer = getRange(d._objectTable, headerPosition + 1, byteCount);
	reverse(buffer.begin(), buffer.end());

	return bytesToDouble(vecData(regulateNullBytes(buffer, 8)));
}

inline bool Plist::parseBinaryBool(const PlistHelperData& d, int headerPosition)
{
	unsigned char header = d._objectTable[headerPosition];
	bool value;
	if(header == 0x09)
		value = true;
	else if (header == 0x08)
		value = false;
	else if (header == 0x00)
	{
		// null byte, not sure yet what to do with this.  It's in the spec but we
		// have never encountered it. 

		throw std::runtime_error("Plist: null byte encountered, unsure how to parse");
	}
	else if (header == 0x0F)
	{
		// fill byte, not sure yet what to do with this.  It's in the spec but we
		// have never encountered it.

		throw std::runtime_error("Plist: fill byte encountered, unsure how to parse");
	}
	else
	{
		std::stringstream ss;
		ss<<"Plist: unknown header "<<header;
		throw std::runtime_error(ss.str().c_str());
	}

	return value;
}

inline PlistDate Plist::parseBinaryDate(const PlistHelperData& d, int headerPosition)
{
	// date always an 8 byte float starting after full byte header
	std::vector<unsigned char> buffer = getRange(d._objectTable, headerPosition + 1, 8);

	PlistDate date;

	// Date is stored as Apple Epoch and big endian. 
	date.setTimeFromAppleEpoch(bytesToDouble(vecData(buffer), false));

	return date;
}

inline std::vector<char> Plist::parseBinaryByteArray(const PlistHelperData& d, int headerPosition)
{
	unsigned char headerByte = d._objectTable[headerPosition];
	int byteStartPosition;
	int32_t byteCount = getCount(d, headerPosition, headerByte, byteStartPosition);
	byteStartPosition += headerPosition;

	return getRange((const char*) vecData(d._objectTable), byteStartPosition, byteCount);
}

inline int32_t Plist::getCount(const PlistHelperData& d, int bytePosition, unsigned char headerByte, int& startOffset)
{
	unsigned char headerByteTrail = headerByte & 0xf;
	if (headerByteTrail < 15)
	{
		startOffset = 1;
		return headerByteTrail;
	}
	else
	{
		int32_t count = parseBinaryInt(d, bytePosition + 1, startOffset);
		startOffset += 2;
		return count;
	}
}

template<typename T>
inline std::string Plist::stringFromValue(const T& value)
{
	std::stringstream ss;
	ss<<value;
	return ss.str();
}

template <typename IntegerType>
inline IntegerType Plist::bytesToInt(const unsigned char* bytes, bool littleEndian)
{
	IntegerType result = 0;
	if (littleEndian)
		for (int n = sizeof( result ); n >= 0; n--)
			result = (result << 8) + bytes[n];
	else
		for (unsigned n = 0; n < sizeof( result ); n++)
			result = (result << 8) + bytes[n];
	return result;
}

inline double Plist::bytesToDouble(const unsigned char* bytes, bool littleEndian)
{
	double result;
	int numBytes = sizeof(double);
	if(littleEndian)
		memcpy( &result, bytes, numBytes);
	else
	{
		std::vector<unsigned char> bytesReverse(numBytes);
		std::reverse_copy(bytes, bytes + numBytes, bytesReverse.begin());
		memcpy( &result, vecData(bytesReverse), numBytes);
	}
	return result;
}

inline std::vector<unsigned char> Plist::doubleToBytes(double val, bool littleEndian)
{
	std::vector<unsigned char> result(sizeof(double));
	memcpy(vecData(result), &val, sizeof(double));
	if(!littleEndian)
		std::reverse(result.begin(), result.end());

	return result;
}

template<typename IntegerType>
inline std::vector<unsigned char> Plist::intToBytes(IntegerType val, bool littleEndian)
{
	unsigned int numBytes = sizeof(val);
	std::vector<unsigned char> bytes(numBytes);

	for(unsigned n = 0; n < numBytes; ++n)
		if(littleEndian)
			bytes[n] = (val >> 8 * n) & 0xff;
		else
			bytes[numBytes - 1 - n] = (val >> 8 * n) & 0xff;

	return bytes;
}

inline std::vector<unsigned char> Plist::getRange(const unsigned char* origBytes, int64_t index, int64_t size)
{
	std::vector<unsigned char> result(size);
	std::copy(origBytes + index, origBytes + index + size, result.begin());
	return result;
}

inline std::vector<char> Plist::getRange(const char* origBytes, int64_t index, int64_t size)
{
	std::vector<char> result(size);
	std::copy(origBytes + index, origBytes + index + size, result.begin());
	return result;
}

inline std::vector<unsigned char> Plist::getRange(const std::vector<unsigned char>& origBytes, int64_t index, int64_t size)
{
	if((index + size) > (int64_t) origBytes.size())
		throw std::runtime_error("Out of bounds getRange");
	return getRange(vecData(origBytes), index, size);
}


#endif
