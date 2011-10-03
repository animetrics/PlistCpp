#include "Plist.hpp"
#include <UnitTest++/UnitTest++.h>
#include <iostream>
#include <fstream>
#include <iterator>

using namespace std;

static void createMessage(map<string, boost::any>& message)
{

//		vector<boost::any> array(1);
//		array[0] = 34;
//		array[1] = string("string item in array");
//		message["testArray"] = array;
		//message["testString"] = string("hello there");
		
		PlistDate date;
		date.setTimeFromAppleEpoch(338610664);

		message["testDate"] = date;
		message["testInt32"] = int32_t(-3455);
		message["testInt64"] = int64_t(-3455);
		message["testShort"] = short(-3455);
		message["testDouble"] = 1.34223;
		message["testBoolTrue"] = true;
		message["testBoolFalse"] = false;
		std::ifstream stream("testImage.jpg", std::ios::binary);
		if(!stream)
			throw std::runtime_error("Can't open file: testImage.jpg");

		int start = stream.tellg();
		stream.seekg(0, std::ifstream::end);
		int size = ((int) stream.tellg()) - start;
		std::vector<unsigned char> actualData(size);
		if(size > 0)
		{
			stream.seekg(0, std::ifstream::beg);
			stream.read( (char *)&actualData[0], size );
		}
		else
		{
			throw std::runtime_error("Can't read zero length data");
		}

		message["testImage"] = actualData;
		vector<boost::any> array(2);
		array[0] = 34;
		array[1] = string("string item in array");
		message["testArray"] = array;
		message["testString"] = string("hello there");
		

}

static void checkDictionary(const map<string, boost::any>& dict)
{
//	for(map<string, boost::any>::const_iterator it = dict.begin();
//			it != dict.end();
//			++it)
//	{
//		cout<<"key: "<<it->first<<" value type: "<<(it->second).type().name()<<endl;
//	}
		string actualString = "hello there";
		double actualDouble = 1.34223;
		int actualInt = -3455;

		// checking byte array
		std::ifstream stream("testImage.jpg", std::ios::binary);
		if(!stream)
			throw std::runtime_error("Can't open file: testImage.jpg");

		int start = stream.tellg();
		stream.seekg(0, std::ifstream::end);
		int size = ((int) stream.tellg()) - start;
		std::vector<unsigned char> actualData(size);
		if(size > 0)
		{
			stream.seekg(0, std::ifstream::beg);
			stream.read( (char *)&actualData[0], size );
		}
		else
		{
			throw std::runtime_error("Can't read zero length data");
		}

		cout<<"Checking data..."<<endl<<"  length: ";
		cout.flush();
		const vector<unsigned char>& plistData = boost::any_cast<const vector<unsigned char>& >(dict.find("testImage")->second);

		// length
		CHECK_EQUAL(actualData.size(), plistData.size());
		cout<<"done."<<endl<<"  Data values: ";

		// data
		CHECK_ARRAY_EQUAL(actualData.data(), plistData.data(), actualData.size());
		cout<<"done."<<endl;

		// checking double

		cout<<"Checking double... ";
		cout.flush();
		CHECK_CLOSE(actualDouble,boost::any_cast<const double&>(dict.find("testDouble")->second), 1E-5); 
		cout<<"done."<<endl;

		// checking int
		cout<<"Checking int... ";
		cout.flush();
		CHECK_EQUAL(actualInt, boost::any_cast<const int&>(dict.find("testInt")->second));
		cout<<"done."<<endl;

		// checking string
		cout<<"Checking string... ";
		cout.flush();
		CHECK_ARRAY_EQUAL(actualString.c_str(),  boost::any_cast<const string&>(dict.find("testString")->second).c_str(), actualString.size());
		cout<<"done."<<endl;

		// checking array
		cout<<"Checking array... ";
		cout.flush();
		const vector<boost::any>& plistArray = boost::any_cast<const vector<boost::any>& >(dict.find("testArray")->second);
		int actualArrayItem0 = 34;
		string actualArrayItem1 = "string item in array";
		CHECK_EQUAL(actualArrayItem0, boost::any_cast<const int&>(plistArray[0]));
		CHECK_ARRAY_EQUAL(actualArrayItem1.c_str(), boost::any_cast<const string&>(plistArray[1]).c_str(), actualArrayItem1.size());
		cout<<"done."<<endl;

		// checking date

		cout<<"Checking date... ";
		cout.flush();
		int actualDate = 338610664;
		CHECK_EQUAL((int) actualDate, (int) boost::any_cast<const PlistDate&>(dict.find("testDate")->second).timeAsAppleEpoch());
//		cout<<"time as xml convention "<<boost::any_cast<const PlistDate&>(dict.find("testDate")->second).timeAsXMLConvention()<<endl;
		cout<<"done."<<endl;


		// checking bools

		cout<<"Checking bools... ";
		cout.flush();
		bool actualTrue = true;
		bool actualFalse = false;
		CHECK_EQUAL(actualTrue, boost::any_cast<const bool&>(dict.find("testBoolTrue")->second));
		CHECK_EQUAL(actualFalse, boost::any_cast<const bool&>(dict.find("testBoolFalse")->second));
		cout<<"done."<<endl;

//		cout<<"testString = "<<boost::any_cast<const string&>(dict.find("testString")->second)<<endl;
//		cout<<"testDouble = "<<boost::any_cast<const double&>(dict.find("testDouble")->second)<<endl;
//		cout<<"testInt = "<<boost::any_cast<const int&>(dict.find("testInt")->second)<<endl;
//		cout<<"testDate = "<<boost::any_cast<const PlistDate&>(dict.find("testDate")->second).timeAsXMLConvention()<<endl;
		//cout<<"testDate = "<<boost::any_cast<const PlistDate&>(dict.find("testDate")->second).timeAsAppleEpoch()<<endl;

}

SUITE(PLIST_TESTS)
{

	TEST(READ_XML)
	{
//		boost::any plist;
//		Plist::readPlist("XMLExample1.plist", plist);
//		map<string, boost::any>& dict = boost::any_cast<map<string, boost::any>& >(plist);
		map<string, boost::any> dict; 
		Plist::readPlist("XMLExample1.plist", dict);

		cout<<"READ_XML test"<<endl<<endl;
		checkDictionary(dict);
		cout<<endl<<"READ_XML test done"<<endl<<endl;
	}

	// currently broken
//	TEST(READ_BINARY)
//	{
//		map<string, boost::any> dict; 
//		Plist::readPlist("binaryExample1.plist", dict);
//
//		cout<<"READ_BINARY test"<<endl<<endl;
//		checkDictionary(dict);
//		cout<<"READ_BINARY test done"<<endl<<endl;
//	}

	TEST(WRITE_BINARY)
	{
		map<string, boost::any> message;
		createMessage(message);
		Plist::writePlistBinary("binaryExampleWritten.plist", message);
	}

	TEST(WRITE_XML)
	{
		map<string, boost::any> message;
		createMessage(message);
		Plist::writePlistXML("xmlExampleWritten.plist", message);
		//Plist::writePlistXML("xmlExampleWritten.plist", message["testString"]);
	}

	TEST(DATE)
	{
		PlistDate date;

		// check comparisons.

		double objectTime = date.timeAsAppleEpoch();

		PlistDate dateGreater(date);
		dateGreater.setTimeFromAppleEpoch(objectTime + 1);
		PlistDate dateLess(date);
		dateLess.setTimeFromAppleEpoch(objectTime - 1);

//		cout<<"dateGreater.timeAsEpoch() "<<dateGreater.timeAsEpoch()<<endl;
//		cout<<"dateLess.timeAsEpoch() "<<dateLess.timeAsEpoch()<<endl;

		CHECK_EQUAL(1, PlistDate::compare(dateGreater, dateLess));
		CHECK_EQUAL(-1, PlistDate::compare(dateLess, dateGreater));
		CHECK_EQUAL(0, PlistDate::compare(date, date));

		CHECK(dateGreater > dateLess);
		CHECK(dateLess < dateGreater);
		CHECK(date == date);

		dateGreater.setTimeFromAppleEpoch(objectTime + 100);

		time_t seconds = dateGreater.secondsSinceDate(date);

		CHECK_EQUAL(100, seconds);
	}

}

