//! Include project headers
#include "stdafx.h"
#include "NeuroNetwok.h"
#include "Conversion.h"

//! Вспомогательная ф-ция для инициализации структуры информации об ошибке fann
void fann_init_error_data_x(struct fann_error *errdat)
{
	errdat->errstr = NULL;
	errdat->errno_f = FANN_E_NO_ERROR;
	errdat->error_log = (FILE *)-1;
}

//! Инициализируем статические данные
const char * const CNeuroNetwok::NETWORK_FILE_NAME = "network.net";


CNeuroNetwok::CNeuroNetwok(void) :
	m_bIsNetworkTeached(false),
	m_bIsInited(false)
{ 
}

CNeuroNetwok::~CNeuroNetwok(void)
{
	if (m_bIsInited)
		fann_destroy(m_pANN);
}

const bool CNeuroNetwok::IsNetworkteached() const
{
	return m_bIsNetworkTeached;
}

void CNeuroNetwok::Init(const int nNeirons, const int nResults, const int nEpochs)
{
	const unsigned int nLayersCount = 3;
	const unsigned int nHiddenNeironsCount = 3;
	m_nEpochsCount = nEpochs;
	
	// Создаем нейросеть
	// Количество входных нейронов столько же, сколько и входных параметров
	// Выходных нейронов столько же, сколько и результатов.
	m_pANN = fann_create_standard(nLayersCount, nNeirons, nHiddenNeironsCount, nResults);

	if (!m_pANN)
		throw std::runtime_error("Failed to init fann!");

	// Выполняем очень важные инициализации :)
	fann_set_activation_steepness_hidden(m_pANN, 1);
	fann_set_activation_steepness_output(m_pANN, 1);

	fann_set_activation_function_hidden(m_pANN, FANN_SIGMOID_SYMMETRIC);
	fann_set_activation_function_output(m_pANN, FANN_SIGMOID_SYMMETRIC);

	fann_set_train_stop_function(m_pANN, FANN_STOPFUNC_BIT);
	fann_set_bit_fail_limit(m_pANN, 0.01f);
	m_bIsInited = true;
}


const _tstring CNeuroNetwok::Teach(const std::vector< std::pair < _tstring, bool > >& InputData)
{
	// На входе в Data карта путей к файлам и результатам, которые должны получится при распознавании
	// Подгружаем данные для каждого файла
	std::vector< std::pair < _tstring, bool > >::const_iterator it = InputData.begin();
	const std::vector< std::pair < _tstring, bool > >::const_iterator itEnd = InputData.end();
	for (; it != itEnd; ++it)
	{
		// Путь
		const _tstring& sPath = it->first;

		// Результат
		const bool bResult = it->second;

		// Данные
		std::list< float > BmpData;

		// Получаем данные для смещения 0 градусов
		AnalizeBMP(sPath, BmpData);

		// Добавляем данные
		m_TrainData.push_back(std::pair< std::list< float >, bool > (BmpData, bResult));

		/*
		// Получаем данные для смещения 90 градусов
		AnalizeBMP(sPath, BmpData, 90);

		// Добавляем данные
		m_TrainData.push_back(std::pair< std::list< float >, bool > (BmpData, bResult));

		// Получаем данные для смещения 180 градусов
		AnalizeBMP(sPath, BmpData, 180);

		// Добавляем данные
		m_TrainData.push_back(std::pair< std::list< float >, bool > (BmpData, bResult));

		// Получаем данные для смещения 270 градусов
		AnalizeBMP(sPath, BmpData, 270);

		// Добавляем данные
		m_TrainData.push_back(std::pair< std::list< float >, bool > (BmpData, bResult));*/
	}

	// Получили структуру данных, необходимую для тренировки нейросети
	// преобразуем ее к виду, необходимую для библиотеки fann
	boost::scoped_ptr< fann_train_data > pTrainData(MakeTrainData(m_TrainData));

	if (!pTrainData)
		throw std::runtime_error("Failed to make train data!");
	
#ifdef _DEBUG
	// Для дебага
	fann_save_train(pTrainData.get(), "debug_data.dat");
#endif

	// Инициализируем веса связей нейронов
	fann_init_weights(m_pANN, pTrainData.get());

	const float fDesiredError = (const float) 0;
	const unsigned int nEpochsBetweenReports = 10;

	// Тренируем нейросеть
	fann_train_on_data(m_pANN, pTrainData.get(), m_nEpochsCount, nEpochsBetweenReports, fDesiredError);	

	// Сохраняем нейросеть
	fann_save(m_pANN, NETWORK_FILE_NAME);

	// Тестируем сеть и возвращаем результат
	m_bIsNetworkTeached = true;
	return boost::lexical_cast< _tstring > (fann_test_data(m_pANN, pTrainData.get()));
}


const float CNeuroNetwok::Recognize(const _tstring& sPath)
{
	if (!m_bIsNetworkTeached)
		throw std::runtime_error("You should teach network first!");

	// Восстанавливаем нейросеть из файла
	m_pANN = fann_create_from_file(NETWORK_FILE_NAME);
	if (!m_pANN)
	{
		std::string sError = "Failed to load data from: ";
		sError += NETWORK_FILE_NAME;
		throw std::runtime_error(sError);
	}

	// Подгружаем данные указанного файла
	std::list< float > BmpData;
	AnalizeBMP(sPath, BmpData);

	// Преобразуем
	TTrainData TestData;
	TestData.push_back(std::pair< std::list< float >, bool > (BmpData, false));
	boost::scoped_ptr< fann_train_data > pTestData(MakeTrainData(TestData));

#ifdef _DEBUG
	// Для дебага
	fann_save_train(pTestData.get(), "debug_data.dat");
#endif

	// Получаем результат
	fann_reset_MSE(m_pANN);

	fann_type * pResult = fann_test(m_pANN, pTestData->input[0], pTestData->output[0]);

	return *pResult;
}

void CNeuroNetwok::AnalizeBMP(const _tstring &sPath, std::list< float >& Data, const int nDegree)
{
	CBitmap Bmp;
	HBITMAP hBmp = (HBITMAP)LoadImage(0, sPath.c_str(), IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LP_MONOCHROME);
	if (!hBmp)
	{
		_tstring sError = L"Can't load bmp from: ";
		sError += sPath;
		throw std::runtime_error(vr::str::narrow(sPath, std::locale()));
	}

	BOOL bMapLoaded = Bmp.Attach(hBmp);
	CDC dc;
	dc.CreateCompatibleDC(NULL);
	CBitmap* hbmSrcT = (CBitmap*)dc.SelectObject(Bmp);

	BITMAP bm;
	Bmp.GetBitmap(&bm);

	// размер одного анализируемого квадрата
	const int nSizeX = bm.bmWidth / 10;
	const int nSizeY = bm.bmHeight / 10;

	int nCount = 0;

	int nStartSquareX = 0;
	int nStartSquareY = 0;
	int nEndSquareX = 10;
	int nEndSquareY = 10;
	int nStepSquareX = 1;
	int nStepSquareY = 1;

	if (nDegree == 90)
	{
		nStartSquareX = 10;
		nStartSquareY = 10;
		nEndSquareX = 0;
		nEndSquareY = 0;
		nStepSquareX = -1;
		nStepSquareY = -1;
	}
	else if (nDegree == 180)
	{
		nStartSquareY = 10;
		nEndSquareY = 0;
		nStepSquareY = -1;
	}
	else if (nDegree == 270)
	{
		nStartSquareX = 10;
		nEndSquareX = 0;
		nStepSquareX = -1;
	}

	Data.clear();

	// Делаем проход по всем квадратам
	for (int nXSquare = nStartSquareX; (nStepSquareX > 0 ? nXSquare < nEndSquareX : nXSquare > nEndSquareX); nXSquare += nStepSquareX)
	{
		for (int nYSquare = nStartSquareY; (nStepSquareY > 0 ? nYSquare < nEndSquareY : nYSquare > nEndSquareY); nYSquare += nStepSquareY)
		{
			// Координаты квадрата
			const int nStartX = nXSquare * nSizeX;
			const int nStartY = nYSquare * nSizeY;

			// Проходим по квадрату
			int nBlackPointsCount = 0;
			for (int x = nStartX; x < nStartX + nSizeX; ++x)
			{
				for (int y = nStartY; y < nStartY + nSizeY; ++y)
				{
					COLORREF color = dc.GetPixel(x ,y);
					const int Red   = GetRValue( color );
					const int Green = GetGValue( color );
					const int Blue  = GetBValue( color );
					if (!Red && !Green && !Blue)
					{
						// Нашли черный пиксель
						++nBlackPointsCount;					
					}
				}
			}

			// определяем коэффициент заполенения квадрата
			const float nKoeff = static_cast< float > (nBlackPointsCount) / static_cast< float > (nSizeX * nSizeY);
			Data.push_back(nKoeff);
		}
	}	
}

fann_train_data * CNeuroNetwok::MakeTrainData(const TTrainData& Data)
{
	if (Data.empty())
		throw std::runtime_error("Can't make train data on empty source data!");

	// Определяем кол-во нейронов по размеру листа входных данных
	const int nNeironsCount = Data[0].first.size();

	// Резервируем место под массив с входными/выходными данными
	const int nInputDataSize = Data.size() * nNeironsCount;
	const int nOutputDataSize = Data.size();

	// Выделяем память под массивы 
	float * pInputData = new float[nInputDataSize];
	float * pOutputData= new float[nOutputDataSize];

	// Заполняем массивы с данными
	int nCurrentInputIndex = 0;
	int nCurrentOutputIndex = 0;
	TTrainData::const_iterator itVector = Data.begin();
	const TTrainData::const_iterator itVectorEnd = Data.end();
	for (; itVector != itVectorEnd; ++itVector)
	{
		// Входные
		std::list< float >::const_iterator it = itVector->first.begin();
		const std::list< float >::const_iterator itEnd = itVector->first.end();
		for (; it != itEnd; ++it)
			pInputData[nCurrentInputIndex++] = *it;

		// Результат, который необходимо получить
		const bool nResult = itVector->second;

		// Выходные
		pOutputData[nCurrentOutputIndex++] = nResult ? 1 : 0;
	}

	// Получаем данные в формате fann
	return Array2FannTrainData(pInputData, pOutputData, Data.size(), nNeironsCount, 1);
}

struct fann_train_data * CNeuroNetwok::Array2FannTrainData(
	float *din, 
	float *dout, 
	unsigned int num_data,
	unsigned int num_input, 
	unsigned int num_output) 
{
		unsigned int i, j;
		fann_type *data_input, *data_output;
		struct fann_train_data *data =
			(struct fann_train_data *) malloc(sizeof(struct fann_train_data));
		if(data == NULL) 
			throw std::runtime_error("Can't allocate memory!");

		fann_init_error_data_x((struct fann_error *) data);

		data->num_data = num_data;
		data->num_input = num_input;
		data->num_output = num_output;
		data->input = (float **) calloc(num_data, sizeof(float *));
		if(data->input == NULL)
		{
			fann_destroy_train(data);
			throw std::runtime_error("Can't allocate memory!");		
			return NULL;
		}

		data->output = (float **) calloc(num_data, sizeof(float *));
		if(data->output == NULL) 
		{
			fann_destroy_train(data);
			throw std::runtime_error("Can't allocate memory!");	
		}

		data_input = (float *) calloc(num_input * num_data, sizeof(float));
		if(data_input == NULL) {
			throw std::runtime_error("Can't allocate memory!");
			return NULL;
		}

		data_output = (float *) calloc(num_output * num_data, sizeof(float));
		if(data_output == NULL) 
		{
			fann_destroy_train(data);
			throw std::runtime_error("Can't allocate memory!");	
		}

		for(i = 0; i != num_data; i++) 
		{
			data->input[i] = data_input;
			data_input += num_input;

			for(j = 0; j != num_input; j++) 
			{
				data->input[i][j] = din[i*num_input+j];
			}

			data->output[i] = data_output;
			data_output += num_output;

			for(j = 0; j != num_output; j++) 
			{
				data->output[i][j] = dout[i*num_output+j];
			}
		}
		return data;
}