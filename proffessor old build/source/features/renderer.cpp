#include "renderer.h"
#include <urlmon.h>
#include <filesystem>
#pragma comment(lib, "urlmon.lib")



//Vector2D render::get_text_size(unsigned long font, std::string text) {
//	std::wstring a = MultiByteToWide(text);
//	const wchar_t* wstr = a.c_str();
//	static int w, h;
//
//	surface->get_text_size(font, wstr, w, h);
//	return { static_cast<float>(w), static_cast<float>(h) };
//}