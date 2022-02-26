#pragma once


int current = 1;

//terrible code yes I know

void menu() {
	if (enabled) {
		ImGui::SetNextWindowPos(ImVec2(5, 5));
		ImGui::SetNextWindowSize(ImVec2(260, 430));
		ImGui::Begin(_("Saturn"), nullptr,
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoScrollbar |
			ImGuiWindowFlags_NoScrollWithMouse |
			ImGuiWindowFlags_NoCollapse |
			ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoInputs);

		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(5, 5), ImVec2(260, 30), IM_COL32(255, 0, 0, 255));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 8), IM_COL32(255, 255, 255, 255), _("                          Saturn"));
		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(5, 30), ImVec2(260, 400), IM_COL32(10, 0, 0, 255));;
		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 38), IM_COL32(255, 255, 255, 255), _("Aimbot"));

		ImGui::GetOverlayDrawList()->AddRectFilled(ImVec2(6, 40 + current * 17), ImVec2(10, 56 + current * 17), IM_COL32(255, 255, 255, 255));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 38 + 18), IM_COL32(255, 255, 255, 255), _("  Softaim"));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(260 - 80, 38 + 18), IM_COL32(255, 255, 255, 255), settings::aimbot ? _(" Enabled ") : _(" Disabled "));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 38 + 18 * 2), IM_COL32(255, 255, 255, 255), _("  X Smooth"));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 38 + 18 * 3), IM_COL32(255, 255, 255, 255), _("  Y Smooth"));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 38 + 18 * 4), IM_COL32(255, 255, 255, 255), _("  Fov"));

		std::string dupeczka = custom::to_string(settings::Xsmooth);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(260 - 80, 38 + 18 * 2), IM_COL32(255, 255, 255, 255), dupeczka.c_str());
		std::string dupeczka2 = custom::to_string(settings::Ysmooth);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(260 - 80, 38 + 18 * 3), IM_COL32(255, 255, 255, 255), dupeczka2.c_str());
		std::string dupeczka3 = custom::to_string(settings::fov_size);
		ImGui::GetOverlayDrawList()->AddText(ImVec2(260 - 80, 38 + 18 * 4), IM_COL32(255, 255, 255, 255), dupeczka3.c_str());

		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 110 + 18), IM_COL32(255, 255, 255, 255), _("Visuals"));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 110 + 18 * 2), IM_COL32(255, 255, 255, 255), _("  Corner Box"));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(260 - 80, 110 + 18 * 2), IM_COL32(255, 255, 255, 255), settings::corner_box ? _(" Enabled ") : _(" Disabled "));

		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 110 + 18 * 3), IM_COL32(255, 255, 255, 255), _("  Skeleton"));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(260 - 80, 110 + 18 * 3), IM_COL32(255, 255, 255, 255), settings::skeleton ? _(" Enabled ") : _(" Disabled "));

		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 110 + 18 * 4), IM_COL32(255, 255, 255, 255), _("  Snaplines"));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(260 - 80, 110 + 18 * 4), IM_COL32(255, 255, 255, 255), settings::lines ? _(" Enabled ") : _(" Disabled "));

		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 110 + 18 * 5), IM_COL32(255, 255, 255, 255), _("  Distance"));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(260 - 80, 110 + 18 * 5), IM_COL32(255, 255, 255, 255), settings::draw_distance ? _(" Enabled ") : _(" Disabled "));

		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 110 + 18 * 6), IM_COL32(255, 255, 255, 255), _("Mods"));

		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 110 + 18 * 7), IM_COL32(255, 255, 255, 255), _("  Instant Revive"));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(260 - 80, 110 + 18 * 7), IM_COL32(255, 255, 255, 255), 1 ? _(" Enabled ") : _(" Disabled "));

		ImGui::GetOverlayDrawList()->AddText(ImVec2(13, 110 + 18 * 8), IM_COL32(255, 255, 255, 255), _("  Fly"));
		ImGui::GetOverlayDrawList()->AddText(ImVec2(260 - 80, 110 + 18 * 8), IM_COL32(255, 255, 255, 255), 1 ? _(" Enabled ") : _(" Disabled "));


		ImGui::End();


		if (spoof_call(safe_call(GetAsyncKeyState), VK_DOWN) & 1) {
			current += 1;
			if (current == 5) {
				current = 6;
			}
			if (current == 14) {
				current = 1;
			}
			if (current == 11) {
				current = 12;
			}
		}
		if (spoof_call(safe_call(GetAsyncKeyState), VK_UP) & 1) {
			current -= 1;
			if (current == 0) {
				current = 12;
			}
			if (current == 5) {
				current = 4;
			}
			if (current == 12) {
				current = 11;
			}
		}

		if (spoof_call(safe_call(GetAsyncKeyState), VK_RETURN) & 1) {
			if (current == 1) {
				settings::aimbot = !settings::aimbot;
			}
			if (current == 6) {
				settings::corner_box = !settings::corner_box;
			}
			if (current == 7) {
				settings::skeleton = !settings::skeleton;
			}
			if (current == 8) {
				settings::lines = !settings::lines;
			}
			if (current == 9) {
				settings::draw_distance = !settings::draw_distance;
			}
		}

		if (spoof_call(safe_call(GetAsyncKeyState), VK_LEFT) & 1) {
			if (current == 2) {
				if (settings::Xsmooth == 1) {

				}
				else {
					settings::Xsmooth -= 1;
				}
			}

			if (current == 3) {
				if (settings::Ysmooth == 1) {

				}
				else {
					settings::Ysmooth -= 1;
				}
			}

			if (current == 4) {
				if (settings::fov_size == 0) {

				}
				else {
					settings::fov_size -= 1;
				}
			}
		}

		if (spoof_call(safe_call(GetAsyncKeyState), VK_RIGHT) & 1) {
			if (current == 2) {
				if (settings::Xsmooth == 50) {

				}
				else {
					settings::Xsmooth += 1;
				}
			}

			if (current == 3) {
				if (settings::Ysmooth == 50) {

				}
				else {
					settings::Ysmooth += 1;
				}
			}

			if (current == 4) {
				if (settings::fov_size == 500) {

				}
				else {
					settings::fov_size += 1;
				}
			}
		}
	}
} 