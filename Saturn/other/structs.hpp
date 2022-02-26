#include <locale>
#include <list>
#include <d3d9.h>
#include "utils.hpp"
#include "custom_winapi.hpp"
#define m_pi 3.14159265358979323846264338327950288419716939937510
#define bit_check(a,b) (!!((a) & (1ULL<<(b))))
#define read_byte(base, offset) (*(((PBYTE)base + offset)))


namespace structs
{
	uintptr_t get_name_by_index;
	uintptr_t free_name;
	uintptr_t bone_matrix;
	float fov_angle;

	struct FMatrix
	{
		float M[4][4];
	};
	static FMatrix* player_matrix = new FMatrix();

	struct Vector2D
	{
		float x, y;
		Vector2D() { x = y = 0.0; }
		Vector2D(float xx, float yy) { x = xx; y = yy; }
	};

	struct Vector3D
	{
		float x, y, z;
		Vector3D() { x = y = z = 0.0; }
		Vector3D(float xx, float yy, float zz) { x = xx; y = yy; z = zz; }
	};

	struct Vector4D
	{
		float x, y, z, w;
		Vector4D() { x = y = z = w = 0.0; }
		Vector4D(float xx, float yy, float zz, float ww) { x = xx; y = yy; z = zz; w = ww; }
	};

	class vector3
	{
	public:
		vector3() : x(0.f), y(0.f), z(0.f)
		{}

		vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z)
		{}

		vector3(float _v) : x(_v), y(_v), z(_v)
		{}

		~vector3()
		{}

		float x;
		float y;
		float z;

		inline bool zero() const
		{
			return (x > -0.1f && x < 0.1f && y > -0.1f && y < 0.1f && z > -0.1f && z < 0.1f);
		}

		inline float dot(vector3 v)
		{
			return x * v.x + y * v.y + z * v.z;
		}

		inline float distance(vector3 v)
		{
			return spoof_call(custom::sqrtf, custom::powf(v.x - x, 2.0) + custom::powf(v.y - y, 2.0) + custom::powf(v.z - z, 2.0));
		}

		inline double length() {
			return ((double)(spoof_call(custom::sqrtf, x * x + y * y + z * z)));
		}

		vector3 operator+(vector3 v)
		{
			return vector3(x + v.x, y + v.y, z + v.z);
		}

		vector3 operator-(vector3 v)
		{
			return vector3(x - v.x, y - v.y, z - v.z);
		}

		vector3 operator*(float number) const {
			return vector3(x * number, y * number, z * number);
		}

		vector3 operator/(float number) const {
			return vector3(x / number, y / number, z / number);
		}
	};

	class vector2
	{
	public:
		vector2() : x(0.f), y(0.f)
		{}

		vector2(float _x, float _y) : x(_x), y(_y)
		{}

		vector2(float _v) : x(_v), y(_v)
		{}

		~vector2()
		{}

		float x;
		float y;

		inline float distance(vector2 v)
		{
			return spoof_call(custom::sqrtf, custom::powf((v.x - x), 2.0f) + custom::powf((v.y - y), 2.0f));
		}


		vector2 operator+(vector2 v)
		{
			return vector2(x + v.x, y + v.y);
		}

		vector2 operator-(vector2 v)
		{
			return vector2(x - v.x, y - v.y);
		}

		vector2 operator*(float number) const {
			return vector2(x * number, y * number);
		}

		vector2 operator/(float number) const {
			return vector2(x / number, y / number);
		}
	};


	template<class T>
	struct TArray
	{
		friend struct FString;

	public:
		inline TArray()
		{
			Data = nullptr;
			Count = Max = 0;
		};

		inline int Num() const
		{
			return Count;
		};

		inline T& operator[](int i)
		{
			return Data[i];
		};

		inline const T& operator[](int i) const
		{
			return Data[i];
		};

		inline bool IsValidIndex(int i) const
		{
			return i < Num();
		}

	private:
		T* Data;
		int32_t Count;
		int32_t Max;
	};

	struct FString : private TArray<wchar_t>
	{
		inline FString()
		{
		};

		FString(const wchar_t* other)
		{
			Max = Count = *other ? custom::wcslen(other) + 1 : 0;

			if (Count)
			{
				Data = const_cast<wchar_t*>(other);
			}
		};

		inline bool IsValid() const
		{
			return Data != nullptr;
		}

		inline const wchar_t* c_str() const
		{
			return Data;
		}


		std::string ToString() const
		{
			auto length = custom::wcslen(Data);

			std::string str(length, '\0');

			std::use_facet<std::ctype<wchar_t>>(std::locale()).narrow(Data, Data + length, '?', &str[0]);

			return str;
		}
	};

	class FRotator
	{
	public:
		FRotator() : fPitch(0), fYaw(0), fRoll(0)
		{

		}

		FRotator(float _pitch, float _yaw, float _roll) : fPitch(_pitch), fYaw(_yaw), fRoll(_roll)
		{

		}

		float fPitch;
		float fYaw;
		float fRoll;
	};

	struct FBox
	{
		vector3  Min;
		vector3  Max;
		unsigned char IsValid;
		unsigned char UnknownData00[0x3];
	};

	class FMinimalViewInfo
	{
	public:
		vector3 Location;
		FRotator Rotation;
		float FOV;
		float OrthoWidth;
		float OrthoNearClipPlane;
		float OrthoFarClipPlane;
		float AspectRatio;
	};

	struct FQuat
	{
		float x;
		float y;
		float z;
		float w;
	};


	vector3 cam_loc;
	vector3 cam_rot;


	static void free_names(__int64 amongus)
	{
		auto function = reinterpret_cast<__int64(__fastcall*)(__int64 sussyfortnite)>(free_name);
		spoof_call(function, amongus);
	}

	static const char* getobjectname(uintptr_t object)
	{
		if (object == NULL)
			return _("");
	
		auto fGetObjName = reinterpret_cast<FString * (__fastcall*)(int* index, FString * res)>(get_name_by_index);
		
		int index = *(int*)(object + 0x18);

		FString result;
		spoof_call(fGetObjName, &index, &result);
		
		if (result.c_str() == NULL)
			return _("");
		
		auto result_str = result.ToString();
	
		if (result.c_str() != NULL)
			free_names((__int64)result.c_str());
		

		return result_str.c_str();
	}

	static bool get_bone_loc(uintptr_t pawn, int id, vector3* out)
	{
		uintptr_t mesh = *(uintptr_t*)(pawn + offsets::mesh);
		if (!mesh) return false;
		auto get_bone_matrix = ((FMatrix * (__fastcall*)(uintptr_t, FMatrix*, int))(bone_matrix));
		spoof_call(get_bone_matrix, mesh, player_matrix, id);
		out->x = player_matrix->M[3][0];
		out->y = player_matrix->M[3][1];
		out->z = player_matrix->M[3][2];
		return true;
	}

	static bool world_to_screen(vector3 world_location, vector3* out)
	{
		auto world_to_screen = reinterpret_cast<bool(__fastcall*)(uintptr_t pplayer_controller, vector3 vworld_pos, vector3 * v_screen_pos_out, char)>(utils::worldtoscreen);
		spoof_call(world_to_screen, (uintptr_t)utils::player_controller, world_location, out, (char)0);
		return true;
	}

	double cross_dist(double x1, double y1, double x2, double y2)
	{
		return spoof_call(custom::sqrtf, custom::powf((float)(x1 - x2), (float)2) + custom::powf((float)(y1 - y2), (float)2));
	}

	bool check_in_fov(uintptr_t current_pawn, float& max)
	{
		structs::vector3 Pos;
		structs::get_bone_loc(current_pawn, 98, &Pos);
		structs::world_to_screen(Pos, &Pos);

		if (current_pawn)
		{
			float Dist = structs::cross_dist(Pos.x, Pos.y, (width / 2), (height / 2));

			if (Dist < max)
			{
				float Radius = (settings::fov_size * width / structs::fov_angle) / 2;

				if (Pos.x <= ((width / 2) + Radius) &&
					Pos.x >= ((width / 2) - Radius) &&
					Pos.y <= ((height / 2) + Radius) &&
					Pos.y >= ((height / 2) - Radius))
				{
					max = Dist;
					return true;
				}

				return false;
			}
		}

		return false;
	}

	bool valid_pointer(uintptr_t address)
	{
		if (!custom::is_bad_write_ptr((LPVOID)address, (UINT_PTR)8)) return TRUE;
		else return FALSE;
	}


	bool valid_vector3(structs::vector3 vec3)
	{
		return !(vec3.x == 0 && vec3.y == 0 && vec3.z == 0);
	}

	template<typename ReadT>
	ReadT read(DWORD_PTR address, const ReadT& def = ReadT())
	{
		if (valid_pointer(address)) {
			return *(ReadT*)(address);
		}
	}

	
	template<typename WriteT>
	bool write(DWORD_PTR address, WriteT value, const WriteT& def = WriteT())
	{
		if (valid_pointer(address)) {
			*(WriteT*)(address) = value;
			return true;
		}
		return false;
	}	
	

	bool check_in_screen(uintptr_t current_pawn, int Width, int Height) {
		structs::vector3 Pos;
		structs::get_bone_loc(current_pawn, 98, &Pos);
		structs::world_to_screen(Pos, &Pos);
		if (current_pawn)
		{
			if (((Pos.x <= 0 or Pos.x > Width) and (Pos.y <= 0 or Pos.y > Height)) or ((Pos.x <= 0 or Pos.x > Width) or (Pos.y <= 0 or Pos.y > Height))) {
				return false;
			}
			else {
				return true;
			}

		}
	}

	static float get_fov_angle(uintptr_t player_camera_manager)
	{
		auto fn = reinterpret_cast<float(*)(UINT64, char*)>(*(ULONG_PTR*)(*(ULONG_PTR*)player_camera_manager + 0x738));
		return spoof_call(fn, (ULONG_PTR)player_camera_manager, (char*)0);
	}

	
	uintptr_t los;
	static BOOLEAN line_of_sight(PVOID player_controller, PVOID actor, vector3* view_point) {

		auto fn = reinterpret_cast<bool(__fastcall*)(PVOID player_controller, PVOID actor, vector3 * view_point)>(los);
		return spoof_call(fn, player_controller, actor, view_point);
	}


}


structs::vector3 local_relative_location;