#include "../neuro/NeuroNetwork.h"
#include "../neuro/NetworkTeacher.h"
#include "Player.h"
#include "Config.h"

#include "gtest/gtest.h"

#include <iostream>

#include <boost/assign.hpp>
#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>
#include <boost/random/random_device.hpp>
#include <boost/lexical_cast.hpp>

using namespace pcmn;
using testing::Range;
using testing::Combine;

void CompareVectors(const std::vector<float>& in1, const std::vector<float>& in2)
{
	ASSERT_EQ(in1.size(), in2.size());

	for (std::size_t i = 0 ; i < in1.size(); ++i)
	{
		ASSERT_TRUE(fabs(in1[i] - in2[i]) < 0.01);
	}
}

TEST(Neuro, Simple)
{
	std::vector<float> in1 = boost::assign::list_of(0.1f)(0.2f)(0.3f)(0.4f)(0.5f)(0.6f)(0.7f)(0.8f)(0.9f);
	std::vector<float> out1 = boost::assign::list_of(0.0f)(0.5f)(1.0f);

	std::vector<float> in2 = boost::assign::list_of(0.4f)(0.2f)(0.1f)(0.9f)(0.2f)(1.0f)(0.4f)(0.5f)(0.2f);
	std::vector<float> out2 = boost::assign::list_of(0.1f)(0.5f)(0.0f);

	std::vector<float> in3 = boost::assign::list_of(0.2f)(0.8f)(0.1f)(0.0f)(0.9f)(0.6f)(0.5f)(0.1f)(0.8f);
	std::vector<float> out3 = boost::assign::list_of(0.8f)(0.1f)(0.2f);

	std::vector<float> in4 = boost::assign::list_of(0.6f)(0.2f)(0.6f)(0.1f)(0.9f)(0.5f)(0.1f)(0.9f)(0.2f);
	std::vector<float> out4 = boost::assign::list_of(0.0f)(0.0f)(1.0f);

	{
		neuro::NetworkTeacher teacher("test.txt");

		for (std::size_t i = 0; i < cfg::TEACH_REPETITIONS_COUNT; ++i)
		{
			teacher.Process(in1, out1);
			teacher.Process(in2, out2);
			teacher.Process(in3, out3);
			teacher.Process(in4, out4);
		}
	}

	neuro::Network network("test.txt");

	std::vector<float> output;
	
	network.Process(in1, output);
	CompareVectors(out1, output);

	network.Process(in2, output);
	CompareVectors(out2, output);

	network.Process(in3, output);
	CompareVectors(out3, output);

	network.Process(in4, output);
	CompareVectors(out4, output);
}


