#include "other/structs.hpp"
#include "spoofer/lazy_importer.hpp"
#include "main/aimbot.hpp"
#include "main/menu.hpp"


structs::vector3 head2, neck, pelvis, chest, left_shoulder, right_shoulder, left_elbow, right_elbow, left_hand, right_hand, left_leg, right_leg, left_thigh, right_thigh, left_foot, right_foot, left_feet, right_feet, left_feet_finger, right_feet_finger;

float x, y;
bool first_check = true;

ImColor skel_color = color::white;


bool actor_loop() {
	auto width69 = spoof_call(safe_call(GetSystemMetrics), SM_CXSCREEN);
	auto height69 = spoof_call(safe_call(GetSystemMetrics), SM_CYSCREEN);

	float fov_max = 9999.f;
	float closest_pawn_dist = FLT_MAX;
	PVOID closest_pawn = NULL;

	auto gworld = structs::read<std::uintptr_t>(utils::uworld); if (!gworld) return false;
	auto game_instance = structs::read<std::uint64_t>(gworld + offsets::game_instance); if (!game_instance) return false;
	auto local_players = structs::read<std::uint64_t>(game_instance + offsets::local_players); if (!local_players) return false;
	auto local_player = structs::read<std::uint64_t>(local_players); if (!local_player) return false;
	utils::player_controller = structs::read<std::uint64_t>(local_player + offsets::player_controller); if (!utils::player_controller) return false;
	utils::player_camera_manager = structs::read<std::uintptr_t>(utils::player_controller + offsets::player_camera_manager); if (!utils::player_camera_manager) return false;
	utils::local_pawn = structs::read<std::uint64_t>(utils::player_controller + offsets::acknowledged_pawn);
	auto levels = structs::read<std::uintptr_t>(gworld + offsets::persistent_level); if (!levels) return false;
	auto actors_array = structs::read<std::uintptr_t>(levels + offsets::aactors); if (!actors_array) return false;
	int actors_count = structs::read<int>(levels + offsets::actor_count); if (!actors_count) return false;
	auto camera_manager = structs::read<std::uint64_t>(utils::player_controller + offsets::player_camera_manager); if (!camera_manager) return false;
	structs::fov_angle = structs::get_fov_angle(camera_manager); 

	if (utils::local_pawn && utils::player_controller) {
		utils::local_root_component = structs::read<std::uintptr_t>(utils::local_pawn + offsets::root_component);
		local_relative_location = structs::read<structs::vector3>(utils::local_root_component + offsets::relative_loc);
	}

	if (spoof_call(safe_call(GetAsyncKeyState), VK_INSERT) & 1)
		enabled = !enabled;

	if (settings::draw_circle)
		ImGui::GetOverlayDrawList()->AddCircle(ImVec2(width / 2, height / 2), (settings::fov_size * structs::fov_angle) / 2, color::white, 50, 1);

	if (settings::crosshair)
	{
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(width / 2 - 7, height / 2), ImVec2(width / 2 + 1, height / 2), ImColor(255, 0, 0, 255), 1.0);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(width / 2 + 8, height / 2), ImVec2(width / 2 + 1, height / 2), ImColor(255, 0, 0, 255), 1.0);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(width / 2, height / 2 - 7), ImVec2(width / 2, height / 2), ImColor(255, 0, 0, 255), 1.0);
		ImGui::GetOverlayDrawList()->AddLine(ImVec2(width / 2, height / 2 + 8), ImVec2(width / 2, height / 2), ImColor(255, 0, 0, 255), 1.0);
	}


	for (int i = 0; i < actors_count; i++)
	{
		auto current_pawn = structs::read<std::uintptr_t>(actors_array + i * sizeof(std::uintptr_t));

		auto object_name = structs::getobjectname(current_pawn);

	
		if (custom::strstr(object_name, _("PlayerPawn")))
		{
			structs::vector3 head, root, head1, root1, boxheadposbox;
			structs::vector3 view_point;
			structs::vector3 chesti, chestatright;

			structs::get_bone_loc(current_pawn, 98, &head);
			structs::get_bone_loc(current_pawn, 0, &root);

			structs::world_to_screen(structs::vector3(head.x, head.y, head.z), &head);
			structs::world_to_screen(structs::vector3(root.x, root.y, root.z), &root);

			structs::get_bone_loc(current_pawn, 98, &head);
			structs::world_to_screen(structs::vector3(head.x, head.y, head.z + 15), &boxheadposbox);

			auto is_visible = structs::line_of_sight((PVOID)utils::player_controller, (PVOID)current_pawn, &view_point);

			menu();

			if (settings::aimbot && utils::local_pawn && utils::player_controller && spoof_call(safe_call(GetAsyncKeyState), VK_RBUTTON))
			{
				if (structs::check_in_screen(current_pawn, width69, height69) && structs::check_in_fov(current_pawn, fov_max)) {

					if (is_visible) {
						structs::vector3 head;
						structs::vector3 headw2s;
						structs::vector3 headw2spredicted;
						structs::get_bone_loc(current_pawn, 98, &head);
						structs::world_to_screen(head, &headw2s);

						aimbot(headw2s.x, headw2s.y, width69, height69, settings::Xsmooth, settings::Ysmooth, is_visible, current_pawn);
					}
				}
			}

			float box_height = (float)(boxheadposbox.y - root.y);

			float corner_height = custom::fabsf(boxheadposbox.y - root.y);

			float corner_width = box_height * 0.6;

			if (current_pawn == utils::local_pawn) continue;

			ImColor box_color, corner_color, line_color;

			if (is_visible) { box_color = { color::green }; }
			else { box_color = { color::red }; }
			if (is_visible) { corner_color = { color::green }; }
			else { corner_color = { color::red }; }
			if (is_visible) { line_color = { color::green }; }
			else { line_color = { color::red }; }

			if (settings::draw_distance && utils::local_pawn && utils::player_controller && structs::check_in_screen(current_pawn, width69, height69)) {
				structs::vector3 a;
				structs::get_bone_loc(current_pawn, 98, &a);
				float distance = local_relative_location.distance(a) / 100.f;

				std::string null = _("");
				std::string finalstring = null + _("[") + custom::to_string(distance) + _("m]");

				ImVec2 distance_text_size = ImGui::CalcTextSize(finalstring.c_str());

				ImColor col = { 255, 255, 255,255 };


				ImGui::GetOverlayDrawList()->AddText(ImVec2(root.x - distance_text_size.x / 2, root.y + distance_text_size.y / 2), col, finalstring.c_str());
			}


			if (settings::corner_box && utils::local_pawn && utils::player_controller && structs::check_in_screen(current_pawn, width69, height69))
				draw_cornered_box(boxheadposbox.x - (corner_width / 2), boxheadposbox.y, corner_width, corner_height, corner_color, 2.0);


			if (settings::lines && utils::local_pawn && utils::player_controller && structs::check_in_screen(current_pawn, width69, height69))
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(960, 1100), ImVec2(root.x, root.y), line_color, 1);


			if (settings::skeleton && utils::local_pawn && utils::player_controller && structs::check_in_screen(current_pawn, width69, height69))
			{
				structs::vector3 chesti, chestatright;

				structs::get_bone_loc(current_pawn, 98, &head2);
				structs::get_bone_loc(current_pawn, 66, &neck);
				structs::get_bone_loc(current_pawn, 2, &pelvis);
				structs::get_bone_loc(current_pawn, 37, &chesti);
				structs::get_bone_loc(current_pawn, 8, &chestatright);
				structs::get_bone_loc(current_pawn, 93, &left_shoulder);
				structs::get_bone_loc(current_pawn, 9, &right_shoulder);
				structs::get_bone_loc(current_pawn, 94, &left_elbow);
				structs::get_bone_loc(current_pawn, 10, &right_elbow);
				structs::get_bone_loc(current_pawn, 100, &left_hand);
				structs::get_bone_loc(current_pawn, 99, &right_hand);
				structs::get_bone_loc(current_pawn, 76, &left_leg);
				structs::get_bone_loc(current_pawn, 69, &right_leg);
				structs::get_bone_loc(current_pawn, 82, &left_thigh);
				structs::get_bone_loc(current_pawn, 75, &right_thigh);
				structs::get_bone_loc(current_pawn, 78, &left_foot);
				structs::get_bone_loc(current_pawn, 71, &right_foot);
				structs::get_bone_loc(current_pawn, 85, &left_feet);
				structs::get_bone_loc(current_pawn, 84, &right_feet);
				structs::get_bone_loc(current_pawn, 81, &left_feet_finger);
				structs::get_bone_loc(current_pawn, 74, &right_feet_finger);

				structs::world_to_screen(head2, &head2);
				structs::world_to_screen(neck, &neck);
				structs::world_to_screen(pelvis, &pelvis);
				structs::world_to_screen(chesti, &chesti);
				structs::world_to_screen(chestatright, &chestatright);
				structs::world_to_screen(left_shoulder, &left_shoulder);
				structs::world_to_screen(right_shoulder, &right_shoulder);
				structs::world_to_screen(left_elbow, &left_elbow);
				structs::world_to_screen(right_elbow, &right_elbow);
				structs::world_to_screen(left_hand, &left_hand);
				structs::world_to_screen(right_hand, &right_hand);
				structs::world_to_screen(left_leg, &left_leg);
				structs::world_to_screen(right_leg, &right_leg);
				structs::world_to_screen(left_thigh, &left_thigh);
				structs::world_to_screen(right_thigh, &right_thigh);
				structs::world_to_screen(left_foot, &left_foot);
				structs::world_to_screen(right_foot, &right_foot);
				structs::world_to_screen(left_feet, &left_feet);
				structs::world_to_screen(right_feet, &right_feet);
				structs::world_to_screen(left_feet_finger, &left_feet_finger);
				structs::world_to_screen(right_feet_finger, &right_feet_finger);

				chest.x = chesti.x + ((chestatright.x - chesti.x) / 2);
				chest.y = chesti.y;

				if (!structs::valid_vector3(head2))continue;
				if (!structs::valid_vector3(neck))continue;
				if (!structs::valid_vector3(pelvis))continue;
				if (!structs::valid_vector3(left_shoulder))continue;
				if (!structs::valid_vector3(right_shoulder))continue;
				if (!structs::valid_vector3(left_elbow))continue;
				if (!structs::valid_vector3(right_elbow))continue;
				if (!structs::valid_vector3(left_hand))continue;
				if (!structs::valid_vector3(right_hand))continue;
				if (!structs::valid_vector3(left_leg))continue;
				if (!structs::valid_vector3(right_leg))continue;
				if (!structs::valid_vector3(left_thigh))continue;
				if (!structs::valid_vector3(right_thigh))continue;
				if (!structs::valid_vector3(left_foot))continue;
				if (!structs::valid_vector3(right_foot))continue;
				if (!structs::valid_vector3(left_feet))continue;
				if (!structs::valid_vector3(right_feet))continue;
				if (!structs::valid_vector3(left_feet_finger))continue;
				if (!structs::valid_vector3(right_feet_finger))continue;

				ImGui::GetOverlayDrawList()->AddLine(ImVec2(head2.x, head2.y), ImVec2(neck.x, neck.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(neck.x, neck.y), ImVec2(chest.x, chest.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(chest.x, chest.y), ImVec2(pelvis.x, pelvis.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(chest.x, chest.y), ImVec2(left_shoulder.x, left_shoulder.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(chest.x, chest.y), ImVec2(right_shoulder.x, right_shoulder.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(left_shoulder.x, left_shoulder.y), ImVec2(left_elbow.x, left_elbow.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(right_shoulder.x, right_shoulder.y), ImVec2(right_elbow.x, right_elbow.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(left_elbow.x, left_elbow.y), ImVec2(left_hand.x, left_hand.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(right_elbow.x, right_elbow.y), ImVec2(right_hand.x, right_hand.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(left_leg.x, left_leg.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(pelvis.x, pelvis.y), ImVec2(right_leg.x, right_leg.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(left_leg.x, left_leg.y), ImVec2(left_thigh.x, left_thigh.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(right_leg.x, right_leg.y), ImVec2(right_thigh.x, right_thigh.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(left_thigh.x, left_thigh.y), ImVec2(left_foot.x, left_foot.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(right_thigh.x, right_thigh.y), ImVec2(right_foot.x, right_foot.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(left_foot.x, left_foot.y), ImVec2(left_feet.x, left_feet.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(right_foot.x, right_foot.y), ImVec2(right_feet.x, right_feet.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(left_feet.x, left_feet.y), ImVec2(left_feet_finger.x, left_feet_finger.y), skel_color, 0.75f);
				ImGui::GetOverlayDrawList()->AddLine(ImVec2(right_feet.x, right_feet.y), ImVec2(right_feet_finger.x, right_feet_finger.y), skel_color, 0.75f);
			}	

		}
	}
	return TRUE;
}
	
bool bind_d3d_info(IDXGISwapChain* swap_chain)
{
	if (SUCCEEDED(swap_chain->GetDevice(__uuidof(ID3D11Device), (void**)&device)))
	{
		device->GetImmediateContext(&context);

		DXGI_SWAP_CHAIN_DESC sd;
		swap_chain->GetDesc(&sd);

		window = sd.OutputWindow;
		ID3D11Texture2D* back_buffer;

		swap_chain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&back_buffer);
		device->CreateRenderTargetView(back_buffer, 0, &render_target_view);
		back_buffer->Release();

		ImGui::GetIO().Fonts->AddFontFromFileTTF(_("C:\\Windows\\Fonts\\arial.ttf"), 14.0f);

		ImGui_ImplDX11_Init(window, device, context);

		first_check = false;
		return true;
	}
	return false;
}

HRESULT hook_present(IDXGISwapChain* swap_chain, UINT sync_interval, UINT flags)
{
	if (first_check)
	{
		if (!bind_d3d_info(swap_chain))
			return spoof_call(present_og, swap_chain, sync_interval, flags);
	}

	ImGui_ImplDX11_CreateDeviceObjects();
	
	context->OMSetRenderTargets(1, &render_target_view, nullptr);
	auto& present_scene = utils::begin_scene();
	
	actor_loop();

	utils::end_scene(present_scene);
	return spoof_call(present_og, swap_chain, sync_interval, flags);
}

ImGuiWindow& utils::begin_scene()
{
	ImGui_ImplDX11_NewFrame();
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	ImGui::Begin(_("##Saturn"), 0, ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoTitleBar);

	auto& io = ImGui::GetIO();
	ImGui::SetWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetWindowSize(ImVec2(io.DisplaySize.x, io.DisplaySize.y), ImGuiCond_Always);

	return *ImGui::GetCurrentWindow();
}

void utils::end_scene(ImGuiWindow& window)
{
	window.DrawList->PushClipRectFullScreen();
	ImGui::End();
	ImGui::PopStyleColor();
	ImGui::PopStyleVar(2);
	ImGui::Render();
}

void scan_sigs() {

	utils::uworld = scanners::pattern_scan(_("48 89 05 ? ? ? ? 48 8B 4B 78")); 
	utils::uworld = rva(utils::uworld, 7);

	structs::get_name_by_index = scanners::pattern_scan(_("48 89 5C 24 ? 48 89 6C 24 ? 56 57 41 56 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 84 24 ? ? ? ? 48 8B F2 4C 8B F1 E8 ? ? ? ? 45 8B 06 33 ED 41 0F B7 16 41 C1 E8 10 89 54 24 24 44 89 44 24 ? 48 8B 4C 24 ? 48 C1 E9 20 8D 3C 09 4A 03 7C C0 ? 0F B7 17 C1 EA 06 41 39 6E 04"));
	structs::free_name = scanners::pattern_scan(_("48 85 C9 0F 84 ? ? ? ? 53 48 83 EC 20 48 89 7C 24 30 48 8B D9 48 8B 3D ? ? ? ? 48 85 FF 0F 84 ? ? ? ? 48 8B 07 4C 8B 40 30 48 8D 05 ? ? ? ? 4C 3B C0"));
	
	structs::bone_matrix = scanners::pattern_scan(_("E8 ? ? ? ? 48 8B 47 30 F3 0F 10 45"));
	structs::bone_matrix = rva(structs::bone_matrix, 5);

	utils::worldtoscreen = scanners::pattern_scan(_("E8 ? ? ? ? 48 8B 5C 24 ? 41 88 07 48 83 C4 30"));
	utils::worldtoscreen = rva(utils::worldtoscreen, 5);

	structs::los = scanners::pattern_scan(_("48 8B C4 48 89 58 20 55 56 57 41 54 41 55 41 56 41 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 44 0F 29 40 ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 20 45 8A E9"));
}

void hook() {
	//detected
	std::uint64_t discord_hook = reinterpret_cast<std::uint64_t>((HMODULE)get_base_address(_(L"DiscordHook64.dll"))); 
	present_og = *reinterpret_cast<present_scene_t*>(discord_hook + 0x1B3080); 
	*reinterpret_cast<present_scene_t*>(discord_hook + 0x1B3080) = hook_present;
}