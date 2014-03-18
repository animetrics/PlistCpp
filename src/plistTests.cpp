#include "Plist.hpp"
#include <UnitTest++/UnitTest++.h>
#include <iostream>
#include <fstream>
#include <iterator>

using namespace std;

static void createMessage(map<string, boost::any>& dict)
{
		Plist::Date date;
		date.setTimeFromAppleEpoch(338610664);

		dict["testDate"] = date;
		dict["testInt"] = int(-3455);
		dict["testInt32"] = int32_t(-3455);
		dict["testInt64"] = int64_t(-3455);
		dict["testShort"] = short(3455);
		dict["testLong"] = long(-3455);
		dict["testDouble"] = 1.34223;
		dict["testFloat"] = 1.34223f;
		dict["testBoolTrue"] = true;
		dict["testBoolFalse"] = false;
		std::ifstream stream("testImage.png", std::ios::binary);
		if(!stream)
			throw std::runtime_error("Can't open file: testImage.png");

		int start = stream.tellg();
		stream.seekg(0, std::ifstream::end);
		int size = ((int) stream.tellg()) - start;
		std::vector<char> actualData(size);
		if(size > 0)
		{
			stream.seekg(0, std::ifstream::beg);
			stream.read( (char *)&actualData[0], size );
		}
		else
		{
			throw std::runtime_error("Can't read zero length data");
		}

		dict["testImage"] = actualData;
		vector<boost::any> array(2);
		array[0] = 34;
		array[1] = string("string item in array");
		dict["testArray"] = array;
		dict["testString"] = string("hello there");
		dict["testEmptyString"] = string("");

		map<string, boost::any> innerDict;
		innerDict["test string"] = string("inner dict item");
		dict["testDict"] = innerDict;

		innerDict.clear();
		array.resize(256);

		for(int i = 0; i < 256; ++i)
		{
			stringstream ss;
			if(i < 10)
				ss<<"00";
			if((i >= 10) && (i < 100))
				ss<<"0";
			ss<<i;
			array[i] = i;
			innerDict[ss.str()] = i;
		}
		dict["testArrayLarge"] = array;
		dict["testDictLarge"] = innerDict;

}

static void checkDictionary(const map<string, boost::any>& dict)
{
		string actualString = "hello there";
		string actualEmptyString = "";
		double actualDouble = 1.34223;
		int actualInt = -3455;

		// checking byte array
		std::ifstream stream("testImage.png", std::ios::binary);
		if(!stream)
			throw std::runtime_error("Can't open file: testImage.png");

		int start = stream.tellg();
		stream.seekg(0, std::ifstream::end);
		int size = ((int) stream.tellg()) - start;
		std::vector<char> actualData(size);
		if(size > 0)
		{
			stream.seekg(0, std::ifstream::beg);
			stream.read( (char *)&actualData[0], size );
		}
		else
		{
			throw std::runtime_error("Can't read zero length data");
		}

#ifdef TEST_VERBOSE
		cout<<"   Checking data..."<<endl<<"     length: "<<endl;
#endif


		const vector<char>& plistData = boost::any_cast<const vector<char>& >(dict.find("testImage")->second);

		// length
		CHECK_EQUAL(actualData.size(), plistData.size());
#ifdef TEST_VERBOSE
		cout<<"                            done."<<endl<<"     Data values: "<<endl;
#endif


		// data
		CHECK_ARRAY_EQUAL(actualData.data(), plistData.data(), actualData.size());
#ifdef TEST_VERBOSE
		cout<<"                            done."<<endl;
#endif


		// checking double

#ifdef TEST_VERBOSE
		cout<<"   Checking double... "<<endl;
#endif


		CHECK_CLOSE(actualDouble,boost::any_cast<const double&>(dict.find("testDouble")->second), 1E-5); 
#ifdef TEST_VERBOSE
		cout<<"                            done."<<endl;
#endif


		// checking float 
		
#ifdef TEST_VERBOSE
		cout<<"   Checking float... "<<endl;
#endif


		CHECK_CLOSE(actualDouble,boost::any_cast<const double&>(dict.find("testFloat")->second), 1E-5); 
#ifdef TEST_VERBOSE
		cout<<"                            done."<<endl;
#endif


		// checking int
#ifdef TEST_VERBOSE
		cout<<"   Checking int... "<<endl;
#endif


		CHECK_EQUAL(actualInt, boost::any_cast<const int64_t&>(dict.find("testInt")->second));
#ifdef TEST_VERBOSE
		cout<<"                            done."<<endl;
#endif


		// checking string
#ifdef TEST_VERBOSE
		cout<<"   Checking string... "<<endl;
#endif


		CHECK_ARRAY_EQUAL(actualString.c_str(),  boost::any_cast<const string&>(dict.find("testString")->second).c_str(), actualString.size());
#ifdef TEST_VERBOSE
		cout<<"                            done."<<endl;
#endif

		// checking empty string
		
		CHECK_ARRAY_EQUAL(actualEmptyString.c_str(),  boost::any_cast<const string&>(dict.find("testEmptyString")->second).c_str(), actualEmptyString.size());
#ifdef TEST_VERBOSE
		cout<<"                            done."<<endl;
#endif

		// checking array
#ifdef TEST_VERBOSE
		cout<<"   Checking array... "<<endl;
#endif

		const vector<boost::any>& plistArray = boost::any_cast<const vector<boost::any>& >(dict.find("testArray")->second);
		int actualArrayItem0 = 34;
		string actualArrayItem1 = "string item in array";
		CHECK_EQUAL(actualArrayItem0, boost::any_cast<const int64_t&>(plistArray[0]));
		CHECK_ARRAY_EQUAL(actualArrayItem1.c_str(), boost::any_cast<const string&>(plistArray[1]).c_str(), actualArrayItem1.size());
#ifdef TEST_VERBOSE
		cout<<"                            done."<<endl;
#endif


		// checking long array (need to do this because there is different logic if
		// the length of the array is >= 15 elements
		
#ifdef TEST_VERBOSE
		cout<<"   Checking long array... "<<endl;
#endif

		const vector<boost::any>& plistArrayLarge = boost::any_cast<const vector<boost::any>& >(dict.find("testArrayLarge")->second);
		int i = 0;
		for(vector<boost::any>::const_iterator it = plistArrayLarge.begin(); 
				i < 256;
				++it, ++i)
			CHECK_EQUAL(i, boost::any_cast<const int64_t&>(*it));
#ifdef TEST_VERBOSE
		cout<<"                            done."<<endl;
#endif


		// checking long dict (need to do this because there is different logic if the length
		// of the dict is >= 15 elements
		
#ifdef TEST_VERBOSE
		cout<<"   Checking long dict... "<<endl;
#endif

		const map<string, boost::any>& plistDictLarge = boost::any_cast<const map<string, boost::any>& >(dict.find("testDictLarge")->second);
		i = 0;
		for(map<string, boost::any>::const_iterator it = plistDictLarge.begin();
				i < 256;
				++it, ++i)
			CHECK_EQUAL(i, boost::any_cast<const int64_t&>(it->second));
#ifdef TEST_VERBOSE
		cout<<"                            done."<<endl;
#endif


		// checking date

#ifdef TEST_VERBOSE
		cout<<"   Checking date... "<<endl;
#endif


		int actualDate = 338610664;
		CHECK_EQUAL((int) actualDate, (int) boost::any_cast<const Plist::Date&>(dict.find("testDate")->second).timeAsAppleEpoch());
//		cout<<"time as xml convention "<<boost::any_cast<const Plist::Date&>(dict.find("testDate")->second).timeAsXMLConvention()<<endl;
#ifdef TEST_VERBOSE
		cout<<"                            done."<<endl;
#endif



		// checking bools

#ifdef TEST_VERBOSE
		cout<<"   Checking bools... "<<endl;
#endif


		bool actualTrue = true;
		bool actualFalse = false;
		CHECK_EQUAL(actualTrue, boost::any_cast<const bool&>(dict.find("testBoolTrue")->second));
		CHECK_EQUAL(actualFalse, boost::any_cast<const bool&>(dict.find("testBoolFalse")->second));
#ifdef TEST_VERBOSE
		cout<<"                            done."<<endl;
#endif


//		cout<<"testString = "<<boost::any_cast<const string&>(dict.find("testString")->second)<<endl;
//		cout<<"testDouble = "<<boost::any_cast<const double&>(dict.find("testDouble")->second)<<endl;
//		cout<<"testInt = "<<boost::any_cast<const int&>(dict.find("testInt")->second)<<endl;
//		cout<<"testDate = "<<boost::any_cast<const Plist::Date&>(dict.find("testDate")->second).timeAsXMLConvention()<<endl;
		//cout<<"testDate = "<<boost::any_cast<const Plist::Date&>(dict.find("testDate")->second).timeAsAppleEpoch()<<endl;

}

SUITE(PLIST_TESTS)
{

	TEST(READ_XML)
	{
		map<string, boost::any> dict; 
		Plist::readPlist("XMLExample1.plist", dict);

#ifdef TEST_VERBOSE
		cout<<"READ_XML test"<<endl<<endl;
#endif

		checkDictionary(dict);
#ifdef TEST_VERBOSE
		cout<<endl<<"READ_XML test done"<<endl<<endl;
#endif

	}

	TEST(READ_BINARY)
	{
		map<string, boost::any> dict; 
		Plist::readPlist("binaryExample1.plist", dict);

#ifdef TEST_VERBOSE
		cout<<"READ_BINARY test"<<endl<<endl;
#endif

		checkDictionary(dict);
#ifdef TEST_VERBOSE
		cout<<endl<<"READ_BINARY test done"<<endl<<endl;
#endif

	}

	TEST(WRITE_BINARY)
	{
		map<string, boost::any> dict;
		createMessage(dict);
#ifdef TEST_VERBOSE
		cout<<"WRITE_BINARY test"<<endl<<endl;
#endif

		Plist::writePlistBinary("binaryExampleWritten.plist", dict);
		dict.clear();
		Plist::readPlist("binaryExampleWritten.plist", dict);
		checkDictionary(dict);
#ifdef TEST_VERBOSE
		cout<<endl<<"WRITE_BINARY test done"<<endl<<endl;
#endif

	}

	TEST(WRITE_XML)
	{
		map<string, boost::any> dict;
		createMessage(dict);
#ifdef TEST_VERBOSE
		cout<<"WRITE_XML test"<<endl<<endl;
#endif

		Plist::writePlistXML("xmlExampleWritten.plist", dict);
		dict.clear();
		Plist::readPlist("xmlExampleWritten.plist", dict);
		checkDictionary(dict);
#ifdef TEST_VERBOSE
		cout<<endl<<"WRITE_XML test done"<<endl<<endl;
#endif

	}

	TEST(WRITE_BINARY_TO_BYTE_ARRAY)
	{
#ifdef TEST_VERBOSE
		cout<<"WRITE_BINARY_TO_BYTE_ARRAY test"<<endl<<endl;
#endif

		vector<char> data;
		map<string, boost::any> dict;
		createMessage(dict);
		Plist::writePlistBinary(data, dict);
		map<string, boost::any> dictCheck;
		Plist::readPlist(&data[0], data.size(), dictCheck);
		checkDictionary(dictCheck);
#ifdef TEST_VERBOSE
		cout<<endl<<"WRITE_BINARY_TO_BYTE_ARRAY test done"<<endl<<endl;
#endif

	}

	TEST(WRITE_XML_TO_BYTE_ARRAY)
	{
#ifdef TEST_VERBOSE
		cout<<"WRITE_XML_TO_BYTE_ARRAY test"<<endl<<endl;
#endif

		vector<char> data;
		map<string, boost::any> dict;
		createMessage(dict);
		Plist::writePlistXML(data, dict);
		map<string, boost::any> dictCheck;
		Plist::readPlist(&data[0], data.size(), dictCheck);
		checkDictionary(dictCheck);
#ifdef TEST_VERBOSE
		cout<<endl<<"WRITE_XML_TO_BYTE_ARRAY test done"<<endl<<endl;
#endif

	}

	TEST(DATE)
	{
		Plist::Date date;

		// check comparisons.

		double objectTime = date.timeAsAppleEpoch();

		Plist::Date dateGreater(date);
		dateGreater.setTimeFromAppleEpoch(objectTime + 1);
		Plist::Date dateLess(date);
		dateLess.setTimeFromAppleEpoch(objectTime - 1);

		CHECK_EQUAL(1, Plist::Date::compare(dateGreater, dateLess));
		CHECK_EQUAL(-1, Plist::Date::compare(dateLess, dateGreater));
		CHECK_EQUAL(0, Plist::Date::compare(date, date));

		CHECK(dateGreater > dateLess);
		CHECK(dateLess < dateGreater);
		CHECK(date == date);

		dateGreater.setTimeFromAppleEpoch(objectTime + 100);

		time_t seconds = dateGreater.secondsSinceDate(date);

		CHECK_EQUAL(100, seconds);
	}

//	TEST(UNICODE)
//	{
//		map<string, boost::any> dict;
//		Plist::readPlist("binaryExampleUnicode.plist", dict);
//	}

}

