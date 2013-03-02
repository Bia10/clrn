#include "NetworkTeacher.h"
#include "Exception.h"
#include "Config.h"

#include <fann/floatfann.h>

namespace neuro
{
	NetworkTeacher::NetworkTeacher(const std::string& file) : m_File(file)
	{
		CHECK(m_Net = fann_create_standard(cfg::LAYERS_COUNT, cfg::INPUT_COUNT, cfg::HIDDEN_NEIRONS_COUNT, cfg::OUTPUT_COUNT), "Failed to create network");

		fann_set_activation_steepness_hidden(m_Net, 1);
		fann_set_activation_steepness_output(m_Net, 1);

		fann_set_activation_function_hidden(m_Net, FANN_SIGMOID_SYMMETRIC);
		fann_set_activation_function_output(m_Net, FANN_SIGMOID_SYMMETRIC);

		fann_set_train_stop_function(m_Net, FANN_STOPFUNC_BIT);
		fann_set_bit_fail_limit(m_Net, 0.01f);

		fann_print_parameters(m_Net);
		fann_print_connections(m_Net);
	}

	NetworkTeacher::~NetworkTeacher()
	{
		fann_print_connections(m_Net);
		fann_save(m_Net, m_File.c_str());
		fann_destroy(m_Net);
	}

	void NetworkTeacher::Process(const std::vector<float>& input, std::vector<float>& output)
	{
		CHECK(m_Net);
		CHECK(input.size() == cfg::INPUT_COUNT);
		CHECK(output.size() == cfg::OUTPUT_COUNT);

		float* inputPtr = const_cast<float*>(&input.front());
		float* outputPtr = const_cast<float*>(&output.front());
		fann_train(m_Net, inputPtr, outputPtr);
	}

} // namespace neuro


