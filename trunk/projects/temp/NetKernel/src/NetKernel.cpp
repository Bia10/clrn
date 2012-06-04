//============================================================================
// Name        : NetKernel.cpp
// Author      : CLRN
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <vector>
#include <algorithm>
#include <stdexcept>

#include <message.pb.h>

#include "XmlTools.h"

int main()
{
	try
	{
		xml::Foo();

		net::Batch Batch;

		for (int i = 0; i < 10; ++i)
		{
			net::Packet* Packet = Batch.add_packets();
			Packet->set_id(i);
			Packet->set_next(i + 1);

			Packet->set_type(net::Packet_PacketType_DATA);
			Packet->set_data("Some very usefull data.");
		}

		std::cout << Batch.ShortDebugString() << std::endl;

		std::vector< char > vecData(Batch.ByteSize());
		Batch.SerializeToArray(&vecData.front(), vecData.size());

		std::copy(vecData.begin(), vecData.end(), std::ostream_iterator< char >(std::cout));


		net::Batch Recieved;
		if (!Recieved.ParseFromArray(&vecData.front(), vecData.size()))
			throw std::runtime_error("Failed to parse.");

		std::cout << Recieved.ShortDebugString() << std::endl;
	}
	catch (std::exception& e)
	{
		std::cout << e.what() << std::endl;
	}

	return 0;
}
