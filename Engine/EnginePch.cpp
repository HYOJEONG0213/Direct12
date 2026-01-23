#include "pch.h"
#include "EnginePch.h"
#include "Engine.h"

//언제 어디서나 꺼내쓰기 : 싱글톤 OR 전역클래스로 만들기 
unique_ptr<Engine> GEngine = make_unique<Engine>();