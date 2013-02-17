#include "NeuroNetwork.h"
#include "Exception.h"

#include <fann/floatfann.h>

#include <boost/scoped_array.hpp>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>

namespace neuro
{
	Network::Network(const std::string& file)
	{
		CHECK(m_Net = fann_create_from_file(file.c_str()), "Failed to create network");
	}

	Network::~Network()
	{
		fann_destroy(m_Net);
	}

	void Network::Process(const std::vector<float>& input, std::vector<float>& output)
	{
		CHECK(m_Net);
		CHECK(input.size() == INPUT_COUNT);
		output.clear();

		fann_reset_MSE(m_Net);
		const float* results = fann_run(m_Net, const_cast<float*>(&input.front()));
		std::copy(results, results + OUTPUT_COUNT, std::back_inserter(output));
	}

} // namespace neuro


