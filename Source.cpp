#include "ParseConvert.h"


int main() {
	ParseConvert *obj = new ParseConvert();
	//if (obj->LoadOBJ(L"C:\\Users\\user\\Desktop\\������\\Article8\\aquafish01.obj")) {
	if (obj->LoadOBJ(L"Data.obj")) {
		obj->Out();
	}
	else {
		std::cerr << "������";
	}
	return 0;
}