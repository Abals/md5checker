#include "Base.Type.h"
#include "MoonLogger.h"
#include "MoonSRE.h"
#include "MoonSSE.h"
#include "MoonProc.h"
#include "MoonDP.h"
#include "MoonAudio.h"

#ifdef WIN32
#include <windows.h>
#include <direct.h>
#endif

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdio>
#include <thread>

#define OFF		0
#define ON		1

#define LANGUAGE		mzBasexTypexTBCH("KOK")

#define ISERROR(EXPRESSION)																					\
	if (EXPRESSION) {																						\
		std::ostringstream strDebug;																		\
		strDebug << "ERROR: LOCATION: " << __FILE__ << "::" << __FUNCTION__ << " " << __LINE__ << "L\n" ;	\
		OutputDebugString((LPCSTR)strDebug.str().c_str());													\
		goto EXIT;																							\
	}

#define MZ_TEST_MOON_SE_SYMBOL				mzBasexTypexTBCH("../LPTE/BASE/SE.SYMBOL.DAT")
#define MZ_TEST_MOONSRE_STT_ENV				mzBasexTypexTBCH("../LPTE/BASE/SE.STT.ENV.DAT")
#define MZ_TEST_MOONSSE_TTS_ENV				mzBasexTypexTBCH("../LPTE/BASE/SE.TTS.ENV.DAT")

#define MZ_TEST_MOONTIMA_ENV				mzBasexTypexTBCH("../LPTE/BASE/TIMA.ENV.DAT")
#define MZ_TEST_MOONTIMA_SYMBOL				mzBasexTypexTBCH("../LPTE/BASE/TIMA.SYMBOL.DAT")
#define MZ_TEST_MOONJINIE_SYMBOL			mzBasexTypexTBCH("../LPTE/BASE/JINIE.SYMBOL.DAT")
#define MZ_TEST_MOONCRF_CONFIG				mzBasexTypexTBCH("../LPTE/DATA/ENU/ENU.SEMANTIC.CONFIG")
#define MZ_TEST_MOONTIMA_VARIABLE			mzBasexTypexTBCH("../LPTE/BIN/TIMA_REF_PRE_DEF_VARIABLES.txt")
#define MZ_TEST_MOON_WAVEDROP_PATH			mzBasexTypexTBCH("../LPTE")
#define MZ_TEST_MOON_DEBUG_LOG_PATH			mzBasexTypexTBCH("../LPTE/LOG/DEBUG.LOG")
#define MZ_TEST_MOON_ERROR_LOG_PATH			mzBasexTypexTBCH("../LPTE/LOG/ERROR.LOG")

using namespace Moon;
bool g_Threadrun = false;
bool g_Interrupt = false;
bool g_Continue = true;

void ShowMenu(void) {
	system("cls");

	std::cout << "\n TIMA IS READY ! " << std::endl;
	std::cout << "  Say \"Hello Mediazen!\" \n\n";
	std::cout << "  Or Select $ 0.QUIT, 1.START (Select and Press Enter) \n";
	std::cout << "  > ";
}

void ThreadInterrupt(MoonProc *phProc) {
	g_Threadrun = true;

	while (1) {
		std::string strInput;			

		std::getline(std::cin, strInput);

		if (0 < strInput.length()) {
			if (atoi(strInput.c_str()) == 0) {
				g_Continue = false;
				phProc->interrupt(MoonProc::INTERRUPT_STOP, 0, "");

				break;
			}
			else if (atoi(strInput.c_str()) == 1) {
				g_Interrupt = true;
				phProc->interrupt(MoonProc::INTERRUPT_STOP, 0, "");
			}
		}
	}
}

void UserDebugLogger(char *str) {

	//std::cout << str << std::endl;
	return ;
}

int cbAudio(void *const pvInstance, unsigned long rEvent, signed long rValue, void *const pvBuffer) {
	std::ostringstream ostr;

	switch(rEvent) {
	case MZ_AUDIO_IN_CTRL_CALLBACK_EVENT_BUFFER:
		break;
	case MZ_AUDIO_IN_CTRL_CALLBACK_EVENT_STREAM:
		//std::cout << "AudioStreamLevel... " << std::dec << rValue << std::endl;
		break;
	}

	return 0;
}

int main(int argc, char **argv) {
	#ifdef WIN32
	SetConsoleOutputCP(CP_UTF8);
	#endif

	int iError  = MoonError::MOON_ERROR_NONE;
	std::string strInput;
	bool bIsReady = false;

	std::cout << "Tima Test !" << std::endl;
		
	MoonSRE *phSRE = MoonSRE::getInstance();
	MoonSSE *phSSE = MoonSSE::getInstance();
	MoonDP *phDP = MoonDP::getInstance();
	MoonProc *phProc = MoonProc::getInstance();

	/* For Logger */
	MoonLogger::getInstance()->init(MZ_TEST_MOON_ERROR_LOG_PATH, MZ_TEST_MOON_DEBUG_LOG_PATH);

	/* For Audio Channel */
	MoonAudioIn::getInstance()->init();
	MoonAudioOut::getInstance()->init();

	// Register Channel Callback
	iError = MoonAudioIn::getInstance()->registerCallback(::cbAudio, nullptr);
	ISERROR(iError != MoonError::MOON_ERROR_NONE);

	/* For STT */
	// 인식기의 환경설정을 합니다.
	iError = phSRE->setLanguage(LANGUAGE);
	ISERROR(iError != MoonError::MOON_ERROR_NONE);

	iError = phSRE->setEnvironment(MZ_TEST_MOONSRE_STT_ENV, MZ_TEST_MOON_SE_SYMBOL);
	ISERROR(iError != MoonError::MOON_ERROR_NONE);
	
	/* 인식에서 사용한 record된 audio low data를 지정한 위치에 저장합니다.
	  오디오 데이터 디버깅용으로 사용하지 않을 때는 꺼두거나, 호출하지 않아도 됩니다.
	*/
	#if OFF
	iError = phSRE->waveDrop(true, "./input.pcm");
	ISERROR(iError != MoonError::MOON_ERROR_NONE);
	#endif

	/* For TTS */
	// 합성기의 환경설정을 합니다.
	iError = phSSE->setLanguage(LANGUAGE);
	ISERROR(iError != MoonError::MOON_ERROR_NONE);

	iError = phSSE->setEnvironment(MZ_TEST_MOONSSE_TTS_ENV, MZ_TEST_MOON_SE_SYMBOL);
	ISERROR(iError != MoonError::MOON_ERROR_NONE);

	/* For Tima Proc */
	iError = phProc->setEnvironment(LANGUAGE, MZ_TEST_MOONTIMA_ENV, MZ_TEST_MOONTIMA_SYMBOL, MZ_TEST_MOONJINIE_SYMBOL, MZ_TEST_MOONTIMA_VARIABLE);
	ISERROR(iError != MoonError::MOON_ERROR_NONE);
		
	iError = phProc->init();
	ISERROR(iError != MoonError::MOON_ERROR_NONE);

	if(iError == MoonError::MOON_ERROR_NONE)
		bIsReady = true;
	else
		std::cout << "\nTIMA IS FAIL ! " << std::endl;

	if (bIsReady) {
		std::thread thread(ThreadInterrupt, phProc);
		thread.detach();

		while (!g_Threadrun)
			std::this_thread::sleep_for(std::chrono::milliseconds(100));
			
		do {
			try {
				ShowMenu();
				phProc->startProc(2000, true);
			}
			catch (...) {
				std::cout << "\nProcess Wakeup Error! " << std::endl;
			}

			if (g_Interrupt) {
				g_Interrupt = false;
				try {
					iError = phProc->startProc(56, true);

					if (iError == MoonError::MOON_ERROR_NONE) {
						std::cout << "\nEND SCENARIO." << std::endl;
						g_Continue = true;
					}
					else {
						std::cout << "\nERROR SCENARIO." << std::endl;
						g_Continue = false;
					}
				}
				catch (...) {
					std::cout << "\nProcess Main Dialog Error!"  << std::endl;
				}
			}

			std::this_thread::sleep_for(std::chrono::milliseconds(1000));
		} while (g_Continue);
	}
	else {
		std::cout << "\nTIMA IS NOT READY. PLEASE REEXCUTE PROGRAM. " << std::endl;
	}

	iError = phProc->terminate();
	ISERROR(iError != MoonError::MOON_ERROR_NONE);

	return 0;

EXIT:

	return -1;
}