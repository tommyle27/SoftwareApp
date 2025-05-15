#pragma once
#ifndef FONTHANDLER_H
#define FONTHANDLER_H

#include <imgui.h>
#include <GLFW/glfw3.h>


class Font {
public:
	static ImFont ChooseFont(float& scaleFactor) {

		if (scaleFactor < 0.8f) scaleFactor = 0.8f;
		if (scaleFactor > 2.0f) scaleFactor = 2.0f;

		return scaleFont(scaleFactor);
	}

private:
	static ImFont scaleFont(float& scale) {
		ImGuiIO& io = ImGui::GetIO();
		ImFont* current_font = io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 12.0f);
		
		if (scale < 1.0f) {
			current_font = io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 16.0f);
		}
		else if (scale > 1.5f) {
			current_font = io.Fonts->AddFontFromFileTTF("fonts/DroidSans.ttf", 20.0f);
		}



		return *current_font;
	}

};

#endif // !FONTHANDLER_H
