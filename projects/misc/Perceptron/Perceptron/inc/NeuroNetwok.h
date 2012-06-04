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

	//! Тип данных для обучения
	typedef std::vector< std::pair< std::list< float >, bool > > TTrainData;
	
	//! Конструктор
	CNeuroNetwok(void);

	//! Деструктор
	~CNeuroNetwok(void);

	//! Инициализация нейронной сети
	void								Init(
											const int nNeirons, 
											const int nResults, 
											const int nEpochs
											);

	//! Обучение нейронной сети
	const _tstring						Teach(const std::vector< std::pair < _tstring, bool > >& Data);

	//! Распознать изображение
	const float							Recognize(const _tstring& sPath);

	//! Обучена ли сеть
	const bool							IsNetworkteached() const;

private:

	//! Получить данные из bmp файла
	void								AnalizeBMP(const _tstring &sPath, std::list< float >& Data, const int nDegree = 0);

	//! Преобразовать массивы данных в структуры, необходимые для fann
	struct fann_train_data *			Array2FannTrainData(
														float * pin, 
														float * pout, 
														unsigned int num_data,
														unsigned int num_input, 
														unsigned int num_output);

	//! Сгенерировать структуру необходимую fann, из bmp данных
	fann_train_data *					MakeTrainData(const TTrainData& Data);

	//! Объект библиотеки fann
	struct fann *						m_pANN;

	//! Данные для тренировки сети
	TTrainData							m_TrainData;

	//! Кол-во эпох для тренировок нейросети
	int									m_nEpochsCount;

	//! Обучена ли сеть
	bool								m_bIsNetworkTeached;

	//! Инициализирована ли библиотека fann
	bool								m_bIsInited;

	//! Имя нейросети для сохранения в файле
	static const char * const			NETWORK_FILE_NAME;


};
