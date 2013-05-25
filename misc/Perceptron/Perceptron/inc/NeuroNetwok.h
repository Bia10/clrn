#pragma once

//! Include project headers
#include "stdafx.h"
#include "floatfann.h"
#include "Conversion.h"

//! Include STL library headers
#include <map>

//! Include boost library headers
#include <boost/lexical_cast.hpp>
#include <boost/scoped_ptr.hpp>


class CNeuroNetwok
{
public:

	//! ��� ������ ��� ��������
	typedef std::vector< std::pair< std::list< float >, bool > > TTrainData;
	
	//! �����������
	CNeuroNetwok(void);

	//! ����������
	~CNeuroNetwok(void);

	//! ������������� ��������� ����
	void								Init(
											const int nNeirons, 
											const int nResults, 
											const int nEpochs
											);

	//! �������� ��������� ����
	const _tstring						Teach(const std::vector< std::pair < _tstring, bool > >& Data);

	//! ���������� �����������
	const float							Recognize(const _tstring& sPath);

	//! ������� �� ����
	const bool							IsNetworkteached() const;

private:

	//! �������� ������ �� bmp �����
	void								AnalizeBMP(const _tstring &sPath, std::list< float >& Data, const int nDegree = 0);

	//! ������������� ������� ������ � ���������, ����������� ��� fann
	struct fann_train_data *			Array2FannTrainData(
														float * pin, 
														float * pout, 
														unsigned int num_data,
														unsigned int num_input, 
														unsigned int num_output);

	//! ������������� ��������� ����������� fann, �� bmp ������
	fann_train_data *					MakeTrainData(const TTrainData& Data);

	//! ������ ���������� fann
	struct fann *						m_pANN;

	//! ������ ��� ���������� ����
	TTrainData							m_TrainData;

	//! ���-�� ���� ��� ���������� ���������
	int									m_nEpochsCount;

	//! ������� �� ����
	bool								m_bIsNetworkTeached;

	//! ���������������� �� ���������� fann
	bool								m_bIsInited;

	//! ��� ��������� ��� ���������� � �����
	static const char * const			NETWORK_FILE_NAME;


};
