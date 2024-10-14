#include "esp.h"
#include "../gui.h"
#include "../interfaces/QAngle.hpp"
#include <codecvt>
#include <locale>
#include <Windows.h>
void ESP::DrawBoxes(CEntity* player)
{
	ImDrawList* draw_list = gui::settings::dlist1;

	// Позиции игрока
	Vector3 top;
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) + Vector3{ 0.f, 0.f, 8.f }, top))
	{
		return;
	}

	Vector3 bottom;
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) - Vector3{ 0.f, 0.f, 60.f }, bottom))
	{
		return;
	}

	// Вычисление высоты и ширины коробки
	const float h = bottom.y - top.y;
	const float w = h * 0.3f;

	// Цвет основного прямоугольника и углов
	ImVec4 boxColor = ImVec4(gui::settings::esp::boxcolor.x, gui::settings::esp::boxcolor.y, gui::settings::esp::boxcolor.z, 1.0f);
	const ImU32 outlineColor = ImColor(0, 0, 0, 255); // Цвет для обводки (черный)

	if (gui::settings::esp::length <= 9)
	{
		// Corner ESP
		const int length = static_cast<int>(w * (gui::settings::esp::length / 10.0f));

		// Вычисление координат углов
		const auto lefttop = top.x - w;
		const auto righttop = top.x + w;
		const auto leftbottom = bottom.x - w;
		const auto rightbottom = bottom.x + w;

		const auto topYPlusLength = top.y + length;
		const auto bottomYMinusLength = bottom.y - length;

		// Рисование линий углов
		draw_list->AddLine(ImVec2(lefttop, top.y), ImVec2(lefttop + length, top.y), ImColor(boxColor));
		draw_list->AddLine(ImVec2(righttop, top.y), ImVec2(righttop - length, top.y), ImColor(boxColor));
		draw_list->AddLine(ImVec2(leftbottom, bottom.y), ImVec2(leftbottom + length, bottom.y), ImColor(boxColor));
		draw_list->AddLine(ImVec2(rightbottom, bottom.y), ImVec2(rightbottom - length, bottom.y), ImColor(boxColor));

		draw_list->AddLine(ImVec2(lefttop, top.y), ImVec2(lefttop, topYPlusLength), ImColor(boxColor));
		draw_list->AddLine(ImVec2(righttop, top.y), ImVec2(righttop, topYPlusLength), ImColor(boxColor));
		draw_list->AddLine(ImVec2(leftbottom, bottom.y), ImVec2(leftbottom, bottomYMinusLength), ImColor(boxColor));
		draw_list->AddLine(ImVec2(rightbottom, bottom.y), ImVec2(rightbottom, bottomYMinusLength), ImColor(boxColor));

		// Нарисовать обводку углов
		if (gui::settings::esp::outer)
		{
			const int outlineThickness = 1;
			draw_list->AddRect(ImVec2(lefttop - outlineThickness, top.y - outlineThickness),
				ImVec2(righttop + outlineThickness, bottom.y + outlineThickness), outlineColor);
		}
		if (gui::settings::esp::inner)
		{
			const int outlineThickness = 1;
			draw_list->AddRect(ImVec2(lefttop + outlineThickness, top.y + outlineThickness),
				ImVec2(righttop - outlineThickness, bottom.y - outlineThickness), outlineColor);
		}
	}
	else
	{
		// Box ESP (Прямоугольная рамка)
		const auto left = top.x - w;
		const auto right = top.x + w;

		// Рисуем основной прямоугольник
		draw_list->AddRect(ImVec2(left, top.y), ImVec2(right, bottom.y), ImColor(boxColor));

		// Нарисовать обводку вокруг основного прямоугольника
		if (gui::settings::esp::outer)
		{
			const int outlineThickness = 1;
			draw_list->AddRect(ImVec2(left - outlineThickness, top.y - outlineThickness),
				ImVec2(right + outlineThickness, bottom.y + outlineThickness), outlineColor);
		}
		if (gui::settings::esp::inner)
		{
			const int outlineThickness = 1;
			draw_list->AddRect(ImVec2(left + outlineThickness, top.y + outlineThickness),
				ImVec2(right - outlineThickness, bottom.y - outlineThickness), outlineColor);
		}
	}
}


#include <cmath>

#define M_PI       3.14159265358979323846   // pi

// Преобразование градусов в радианы

void ESP::DrawOffscreenIndicator(CEntity* player)
{
	auto drawList = gui::settings::dlist1;
	int screenWidth, screenHeight;
	engine->GetScreenSize(screenWidth, screenHeight);

	Vector3 screenPos;

	if (!debugOverlay->ScreenPosition(player->m_vecOrigin, screenPos))
	{
		return;
	}

	if (screenPos.x >= 0 && screenPos.x <= screenWidth && screenPos.y >= 0 && screenPos.y <= screenHeight)
	{
		return; // Игрок на экране, не нужно рисовать индикатор
	}

	

	// Рассчитываем центр экрана
	Vector3 screenCenter = { screenWidth / 2.0f, screenHeight / 2.0f, 0.0f };
	Vector3 dir = screenPos - screenCenter;

	// Нормализуем направление
	float length = std::sqrt(dir.x * dir.x + dir.y * dir.y);
	dir.x /= length;
	dir.y /= length;

	// Устанавливаем длину индикатора
	float indicatorRadius = gui::settings::esp::offscreenradius;

	// Позиция индикатора на краю экрана
	Vector3 indicatorPos = screenCenter + dir * indicatorRadius;

	// Ограничиваем индикатор, чтобы он оставался на экране
	if (indicatorPos.x < 0) indicatorPos.x = 0;
	if (indicatorPos.x > screenWidth) indicatorPos.x = screenWidth;
	if (indicatorPos.y < 0) indicatorPos.y = 0;
	if (indicatorPos.y > screenHeight) indicatorPos.y = screenHeight;

	Color oofcolor = player->IsVisible(8) ?
		Color(gui::settings::esp::outoffovicolor.x * 255, gui::settings::esp::outoffovicolor.y * 255, gui::settings::esp::outoffovicolor.z * 255, 255) :
		Color(gui::settings::esp::outoffovcolor.x * 255, gui::settings::esp::outoffovcolor.y * 255, gui::settings::esp::outoffovcolor.z * 255, 255);

	// Определяем углы треугольника для индикатора
	float triangleSize = 10.0f;
	Vector3 point1 = indicatorPos + Vector3(-dir.y, dir.x, 0) * triangleSize; // Левая точка
	Vector3 point2 = indicatorPos + Vector3(dir.y, -dir.x, 0) * triangleSize; // Правая точка
	Vector3 point3 = indicatorPos + dir * (triangleSize * 1.5f); // Верхняя точка (острие)

	// Рисуем треугольник с помощью ImGui
	drawList->AddTriangleFilled(ImVec2(point1.x, point1.y), ImVec2(point2.x, point2.y), ImVec2(point3.x, point3.y), ImColor(oofcolor.iR, oofcolor.iG, oofcolor.iB, oofcolor.iA));
}

void DrawDropShadowText( ImVec2 pos, ImU32 color, const char* text) {
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();
	int alpha = (color >> IM_COL32_A_SHIFT) & 0xFF;
	ImU32 shadowColor = ImColor(0, 0, 0, alpha / 2);
	ImVec2 newPos = ImVec2(pos.x + 2, pos.y + 2);
	drawList->AddText(newPos, shadowColor, text);
	drawList->AddText(pos, color, text);
}

void ESP::DrawNames( CEntity* player)
{
	// Получаем размеры экрана
	static int screenWidth = hooks::screenwidth;
	static int screenHeight = hooks::screenheight;
	ImDrawList* draw_list = gui::settings::dlist1;

	// Проверка позиции игрока на экране
	Vector3 screenPos;
	if (!debugOverlay->ScreenPosition(player->m_vecOrigin, screenPos))
	{
		if (screenPos.x < 0 || screenPos.x > screenWidth || screenPos.y < 0 || screenPos.y > screenHeight)
		{
			return;
		}
	}
	int index = player->GetIndex();
	// Получение информации об игроке
	player_info_t info;
	if (!engine->GetPlayerInfo(index, &info))
	{
		return;
	}

	// Позиция верха игрока (например, голова)
	Vector3 top;
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) + Vector3{ 0.f, 0.f, 13.f }, top))
	{
		return;
	}

	// Позиция низа игрока (например, ноги)
	Vector3 bottom;
	if (debugOverlay->ScreenPosition(player->m_vecOrigin - Vector3{ 0.f, 0.f, 6.f }, bottom))
	{
		return;
	}

	// Вычисляем высоту игрока
	const float h = bottom.y - top.y;

	// Преобразование имени игрока в Unicode
	std::wstring name = util::ansi2unicode(info.name);
	std::string playerName(name.begin(), name.end()); // Конвертируем в std::string для ImGui

	// Вычисление размера текста
	ImVec2 textSize = ImGui::CalcTextSize(playerName.c_str());

	// Центрирование текста по горизонтали
	const float centerX = top.x - (textSize.x / 2);
	const float centerY = top.y - textSize.y - 5;  // Немного выше головы

	// Устанавливаем цвет текста (например, белый)
	ImVec4 textColor = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);

	// Отрисовка текста с центрированием
	DrawDropShadowText(ImVec2(centerX, centerY), ImColor(textColor), playerName.c_str());
}



void ESP::DrawHealthbars(CEntity* player)
{
	// Получаем размеры экрана
	static int screenWidth = hooks::screenwidth;
	static int screenHeight = hooks::screenheight;
	ImDrawList* draw_list = gui::settings::dlist1;

	// Проверяем, находится ли игрок на экране
	Vector3 screenPos;
	if (!debugOverlay->ScreenPosition(player->m_vecOrigin, screenPos))
	{
		if (screenPos.x < 0 || screenPos.x > screenWidth || screenPos.y < 0 || screenPos.y > screenHeight)
		{
			return;
		}
	}

	// Позиции костей игрока для верхней и нижней точки
	Vector3 top, bottom;
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) + Vector3{ 0.f, 0.f, 8.f }, top))
	{
		return;
	}
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) - Vector3{ 0.f, 0.f, 75.f }, bottom))
	{
		return;
	}

	// Вычисляем высоту игрока и ширину полосы здоровья
	const float h = bottom.y - top.y;
	const float w = h * 0.3f;
	const float left = top.x - w;
	const float right = top.x + w;

	// Получаем текущее здоровье игрока и вычисляем пропорцию для высоты полосы здоровья
	float health = static_cast<float>(player->m_iHealth);
	float healthFrac = health / 100.f;
	float barHeight = h * healthFrac;

	// Отрисовка черной подложки для полосы здоровья
	draw_list->AddRectFilled(ImVec2(left - 5, top.y), ImVec2(left - 2, bottom.y), ImColor(0, 0, 0, 255));

	// Устанавливаем цвет полосы здоровья в зависимости от текущего здоровья
	ImColor healthColor;
	if (!(gui::settings::esp::healthbarcolorcustom))
	{
		if (health > 50)
			healthColor = ImColor(0.2f, 1.0f, 0.2f);  // Зеленый
		else if (health > 20)
			healthColor = ImColor(1.0f, 1.0f, 0.2f);  // Желтый
		else
			healthColor = ImColor(1.0f, 0.2f, 0.2f);  // Красный
	}
	else
	{
		healthColor = ImColor(gui::settings::esp::healthbarcolor.x, gui::settings::esp::healthbarcolor.y, gui::settings::esp::healthbarcolor.z , 1.0f);
	}

	// Отрисовка полосы здоровья
	draw_list->AddRectFilled(ImVec2(left - 4, top.y + h - barHeight), ImVec2(left - 3, bottom.y - 1), healthColor);

	// Отображение значения здоровья, если оно меньше 100
	if (health < 100)
	{
		std::string healthText = std::to_string(static_cast<int>(health));
		ImVec2 textSize = ImGui::CalcTextSize(healthText.c_str());
		draw_list->AddText(ImVec2(left - 10 - (textSize.x / 2), top.y + h - barHeight - 5), ImColor(255, 255, 255, 205), healthText.c_str());
	}
}


void ESP::DrawFlags(vgui::HFont font, CEntity* player)
{
	int screenWidth, screenHeight;
	engine->GetScreenSize(screenWidth, screenHeight);
	Vector3 screenPos;
	if (!debugOverlay->ScreenPosition(player->m_vecOrigin, screenPos))
	{
		if (screenPos.x < 0 || screenPos.x > screenWidth || screenPos.y < 0 || screenPos.y > screenHeight)
		{
			return;

		}
	}
	Vector3 bonePos = player->GetBonePos(8);

	Vector3 top;
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) + Vector3{ 0.f,0.f, 10.f }, top))
	{
		return;
	}

	Vector3 bottom;
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) - Vector3{ 0.f, 0.f, 78.f }, bottom))
	{
		return;
	}

	std::vector<std::pair<std::string, Color>> flags;
	
	if (player->m_ArmorValue > 0)
	{
		if (player->m_bHasHelmet)
			flags.push_back(std::pair<std::string, Color>(("HK"), Color(255,255,255,255)));
		else
			flags.push_back(std::pair<std::string, Color>(("K"), Color(255, 255, 255, 255)));
	}

	if (player->m_bHasDefuser)
		flags.push_back(std::pair<std::string, Color>(("KIT"), Color(46, 112, 255, 255)));

	if (player->m_bIsDefusing)
		flags.push_back(std::pair<std::string, Color>(("DEFUSING"), Color(255, 56, 56, 255)));

	if (player->m_bIsScoped)
		flags.push_back(std::pair<std::string, Color>(("SCOPED"), Color(255, 255, 255, 255)));

	if (player->m_flFlashDuration > 0.f)
		flags.push_back(std::pair<std::string, Color>(("FLASHED"), Color(255, 211, 66, 255)));

	if (player->m_bImmunity)
		flags.push_back(std::pair<std::string, Color>(("IMMUNE"), Color(255, 211, 66, 255)));

	if (globals::playerResource->m_iPlayerC4)
	{
		if (player->GetIndex() == globals::playerResource->m_iPlayerC4)
		{
			flags.push_back(std::pair<std::string, Color>(("C4"), Color(255, 56, 56, 255)));
			if (globals::C4 && globals::C4->m_bStartedArming)
			{
				flags.push_back(std::pair<std::string, Color>(("PLANTING"), Color(255, 56, 56, 255)));
			}
		}	
	}
		


	for (int i = 0; i < flags.size(); ++i)
	{
		auto text = flags[i];
		const float h = bottom.y - top.y;



		const float w = h * 0.3f;


		const auto left = (top.x - w);
		const auto right = (top.x + w);

		static int tW, tH;
		std::wstring s = util::ansi2unicode(text.first);
		const wchar_t* texto = s.c_str();
		surface->GetTextSize(font, texto, tW, tH);

		static int txtH = tH / 1.5;
		surface->DrawTextSurface(font, Vector3{ right , top.y + (txtH * i)}, text.first.c_str(), text.second);
	}



	
}



void ESP::DrawC4(vgui::HFont font)
{
	if (!globals::C4 || globals::C4 != (CC4*)entityList->GetEntityFromClass(ClassID::CC4))
		globals::C4 = (CC4*)entityList->GetEntityFromClass(ClassID::CC4);

	if (!globals::plantedC4 || globals::plantedC4 != (CPlantedC4*)entityList->GetEntityFromClass(ClassID::CPlantedC4))
		globals::plantedC4 = (CPlantedC4*)entityList->GetEntityFromClass(ClassID::CPlantedC4);

	Vector3 pos;
	int tW, tH;
	std::wstring s;
	const wchar_t* text;

	if (globals::playerResource->m_iPlayerC4)
		return;

	if (globals::C4)
	{
		
		if (debugOverlay->ScreenPosition(globals::C4->m_vecOrigin, pos))
		{
			return;
		}


		
		s = util::ansi2unicode("C4");
		text = s.c_str();
		surface->GetTextSize(font, text, tW, tH);

		surface->DrawTextSurface(font, Vector3{ pos.x - (tW / 2), pos.y, 0.f }, "C4", Color(255, 0, 0, 255));
	}

	if (!globals::plantedC4 || !globals::plantedC4->m_bBombTicking)
		return;

	if (debugOverlay->ScreenPosition(globals::playerResource->m_vecC4, pos))
	{
		return;
	}

	s = util::ansi2unicode(std::to_string((int)(globals::plantedC4->m_flC4Blow - globalVars->currentTime)));
	text = s.c_str();
	surface->GetTextSize(font, text, tW, tH);

	surface->DrawTextSurface(font, Vector3{ pos.x - (tW / 2), pos.y, 0.f }, std::to_string((int)(globals::plantedC4->m_flC4Blow - globalVars->currentTime)).c_str(), Color(255, 0, 0, 255));

	if (globals::plantedC4->m_hBombDefuser != 0xffffffff)
	{
		s = util::ansi2unicode(std::to_string((int)(globals::plantedC4->m_flDefuseCountDown - globalVars->currentTime)));
		text = s.c_str();
		surface->GetTextSize(font, text, tW, tH);

		surface->DrawTextSurface(font, Vector3{ pos.x - (tW / 2), pos.y + 10, 0.f }, std::to_string((int)(globals::plantedC4->m_flDefuseCountDown - globalVars->currentTime)).c_str(), Color(84, 190, 255, 255));
	}


}

void ESP::DrawWeapons(CEntity* player, vgui::HFont font)
{
	int screenWidth, screenHeight;
	engine->GetScreenSize(screenWidth, screenHeight);
	Vector3 screenPos;
	if (!debugOverlay->ScreenPosition(player->m_vecOrigin, screenPos))
	{
		if (screenPos.x < 0 || screenPos.x > screenWidth || screenPos.y < 0 || screenPos.y > screenHeight)
		{
			return;

		}
	}
	Vector3 top;
	if (debugOverlay->ScreenPosition(player->GetBonePos(8) + Vector3{ 0.f,0.f, 8.f }, top))
	{
		return;
	}

	Vector3 bottom;
	if (debugOverlay->ScreenPosition(player->m_vecOrigin - Vector3{ 0.f,0.f,6.f }, bottom))
	{
		return;
	}

	int tW, tH;

	CBaseCombatWeapon* weapon = player->GetActiveWeapon();
	const char* weapName = weapon->GetWeaponName();
	std::wstring s = util::ansi2unicode(weapName);
	const wchar_t* text = s.c_str();
	surface->GetTextSize(font, text, tW, tH);

	const float h = bottom.y - top.y;

	const float w = h * 0.3f;

	const auto left = (top.x - w);
	const auto right = (top.x + w);
	const float center = left - (tW / 2);

	surface->DrawTextSurface(font, Vector3{ top.x - (tW / 2), bottom.y, 0.f }, weapName, weapon->IsKnife() ? Color(255, 56, 56, 255) : Color(255, 255, 255, 255));

}
static float hitmarkerStartTime = 0.0f;
static bool showHitmarker = false;



// Функция для отображения и анимации хитмаркера
void ESP::DrawHitmarker(CEntity* player) {
	static int oldHits = 0;
	float hitmarkerDuration = gui::settings::misc::hitmarkertime; // Длительность анимации в секундах
	static Vector3 pos;
	Vector3 playerPos = player->m_vecOrigin; // Позиция другого игрока
	Vector3 localPlayerPos = globals::localPlayer->m_vecOrigin; // Позиция локального игрока

	// Рассчитываем разницу по осям X и Y
	float deltaX = playerPos.x - localPlayerPos.x;
	float deltaY = playerPos.y - localPlayerPos.y;

	// Рассчитываем дистанцию по X и Y
	float distance = sqrt(deltaX * deltaX + deltaY * deltaY);

	// Если дистанция меньше 5, показываем хитмаркер
	if (distance < 5.0f)
	{
		showHitmarker = true;
	}

	if (showHitmarker) {
		// Получаем текущее время и рассчитываем альфа-канал
		float currentTime = ImGui::GetTime();
		float elapsedTime = currentTime - hitmarkerStartTime;
		float alpha = 0.0f;

		if (elapsedTime < hitmarkerDuration) {
			// Альфа-канал плавно меняется от 1.0 до 0.0
			alpha = 1.0f - (elapsedTime / hitmarkerDuration);
		}
		else {
			showHitmarker = false;
		}

		if (alpha > 0.0f) {
			// Отрисовка хитмаркера в виде крестика
			ImDrawList* drawList = gui::settings::dlist1;
			ImVec2 pos = ImVec2(ImGui::GetIO().DisplaySize.x / 2.0f, ImGui::GetIO().DisplaySize.y / 2.0f);
			float halfSize = gui::settings::misc::hitmarkersize / 2.0f;
			ImU32 color = IM_COL32((gui::settings::misc::hitmarkercolor.x * 255), (gui::settings::misc::hitmarkercolor.y * 255), (gui::settings::misc::hitmarkercolor.z * 255), static_cast<int>(255 * alpha));

			// Отрисовка диагональных линий с учетом промежутка
			drawList->AddLine(
				ImVec2(pos.x - halfSize, pos.y - halfSize),
				ImVec2(pos.x + halfSize, pos.y + halfSize),
				color, gui::settings::misc::hitmarkerthickness
			);
			drawList->AddLine(
				ImVec2(pos.x - halfSize, pos.y + halfSize),
				ImVec2(pos.x + halfSize, pos.y - halfSize),
				color, gui::settings::misc::hitmarkerthickness
			);
		}
	}
}
void ESP::HandleESP()
{
	if (!gui::settings::esp::enable || globals::localPlayer == nullptr)
		return;

	static int screenWidth = hooks::screenwidth;
	static int screenHeight = hooks::screenheight;

	// Перебираем игроков из playerList
	for (CEntity* player : hooks::playerList)
	{
		if (!player)
			continue;

		if (player == globals::localPlayer)
			continue;

		// Проверяем, жив ли игрок и не находится ли он в режиме "дорманта"
		if (!player->IsAlive() || player->IsDormant())
			continue;

		// Проверяем дистанцию до игрока
		if ((player->m_vecOrigin - globals::localPlayer->m_vecOrigin).Length2D() > gui::settings::esp::espdistance)
			continue;

		// Проверяем, находится ли игрок на экране
		Vector3 screenPos;
		if (player->m_iTeamNum == globals::localPlayer->m_iTeamNum || !player->m_iTeamNum)
			continue;

		if (gui::settings::esp::offscreen)
			DrawOffscreenIndicator(player); // Убираем индекс i, так как он больше не нужен

		if (debugOverlay->ScreenPosition(player->m_vecOrigin, screenPos) != 0)
		{
			if (screenPos.x < 0 || screenPos.x > screenWidth || screenPos.y < 0 || screenPos.y > screenHeight)
				continue; // Игрок за пределами границ экрана
		}

		// Рисуем ESP
		if (gui::settings::esp::boxes)
			DrawBoxes(player);

		if (gui::settings::esp::name)
			DrawNames(player);

		if (gui::settings::esp::health)
			DrawHealthbars(player);
	}
}
