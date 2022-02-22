#include <Windows.h>
#include <detours.h>

#include <fstream>
#include <map>
#include <string>

#include "loader.h"

namespace pkodev
{
	// 3D vector
	struct D3DXVECTOR3
	{
		float x;
		float y;
		float z;

		// Constructor
		D3DXVECTOR3() :
			x(1.0f),
			y(1.0f),
			z(1.0f)
		{

		}

		// Constructor
		D3DXVECTOR3(float x_, float y_, float z_) :
			x(x_),
			y(y_),
			z(z_)
		{

		}

		// Set dimensions
		void update(float x_ = 1.0f, float y_ = 1.0f, float z_ = 1.0f)
		{
			x = x_;
			y = y_;
			z = z_;
		}
	};

	// Character scale structure
	struct mob_scale
	{
		// Scale vector
		D3DXVECTOR3 scale_;

		// Is characterinfo data updated?
		bool updated;

		// Constructor
		mob_scale(float x_, float y_, float z_) :
			updated(false)
		{
			scale_.x = x_;
			scale_.y = y_;
			scale_.z = z_;
		}
	};

	namespace address
	{
		// Game.exe 0 (1222073761)
		namespace GAME_13X_0
		{
			// CCharacter* CGameScene::AddCharacter( int nScriptID )
			const unsigned int CGameScene__AddCharacter = 0x004D7580;

			// CCharacter* CGameScene::AddCharacter( int nScriptID )
			// variable D3DXVECTOR3 scale
			const unsigned int CGameScene__AddCharacter__scale = 0x004D7959;
		}

		// Game.exe 1 (1243412597)
		namespace GAME_13X_1
		{
			// CCharacter* CGameScene::AddCharacter( int nScriptID )
			const unsigned int CGameScene__AddCharacter = 0x004D7690;

			// CCharacter* CGameScene::AddCharacter( int nScriptID )
			// variable D3DXVECTOR3 scale
			const unsigned int CGameScene__AddCharacter__scale = 0x004D7A69;
		}

		// Game.exe 2 (1252912474)
		namespace GAME_13X_2
		{
			// CCharacter* CGameScene::AddCharacter( int nScriptID )
			const unsigned int CGameScene__AddCharacter = 0x004D7760;

			// CCharacter* CGameScene::AddCharacter( int nScriptID )
			// variable D3DXVECTOR3 scale
			const unsigned int CGameScene__AddCharacter__scale = 0x004D7B39;
		}

		// Game.exe 3 (1244511158)
		namespace GAME_13X_3
		{
			// CCharacter* CGameScene::AddCharacter( int nScriptID )
			const unsigned int CGameScene__AddCharacter = 0x004D7750;

			// CCharacter* CGameScene::AddCharacter( int nScriptID )
			// variable D3DXVECTOR3 scale
			const unsigned int CGameScene__AddCharacter__scale = 0x004D7B29;
		}

		// Game.exe 4 (1585009030)
		namespace GAME_13X_4
		{
			// CCharacter* CGameScene::AddCharacter( int nScriptID )
			const unsigned int CGameScene__AddCharacter = 0x004D7930;

			// CCharacter* CGameScene::AddCharacter( int nScriptID )
			// variable D3DXVECTOR3 scale
			const unsigned int CGameScene__AddCharacter__scale = 0x004D7D09;
		}

		// Game.exe 5 (1207214236)
		namespace GAME_13X_5
		{
			// CCharacter* CGameScene::AddCharacter( int nScriptID )
			const unsigned int CGameScene__AddCharacter = 0x004D73B0;

			// CCharacter* CGameScene::AddCharacter( int nScriptID )
			// variable D3DXVECTOR3 scale
			const unsigned int CGameScene__AddCharacter__scale = 0x004D7789;
		}
	}

	namespace pointer
	{
		// CCharacter* CGameScene::AddCharacter( int nScriptID )
		typedef void* (__thiscall* CGameScene__AddCharacter__Ptr)(void*, int);
		CGameScene__AddCharacter__Ptr CGameScene__AddCharacter = (CGameScene__AddCharacter__Ptr)(void*)(address::MOD_EXE_VERSION::CGameScene__AddCharacter);
	}

	namespace hook
	{
		// CCharacter* CGameScene::AddCharacter( int nScriptID )
		void* __fastcall CGameScene__AddCharacter(void* This, void* NotUsed, int nScriptID);
	}

	// Variable D3DXVECTOR3 scale for CCharacter* CGameScene::AddCharacter( int nScriptID )
	D3DXVECTOR3 scale{ 1.0f, 1.0f, 1.0f };

	// List of characters to be resized
	std::map<int, mob_scale> resize;
}

// Entry point
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    // Nothing to do . . .
    return TRUE;
}

// Get information about the mod
void GetModInformation(mod_info& info)
{
	strcpy_s(info.name, TOSTRING(MOD_NAME));
	strcpy_s(info.version, TOSTRING(MOD_VERSION));
	strcpy_s(info.author, TOSTRING(MOD_AUTHOR));
	info.exe_version = MOD_EXE_VERSION;
}

// Start the mod
void Start(const char* path)
{
	// Build path to the config file
	char buf[MAX_PATH]{ 0x00 };
	sprintf_s(buf, sizeof(buf), "%s\\%s.cfg", path, TOSTRING(MOD_NAME));

	// Load the list of characters to be resized
	{
		// Open the config file
		std::ifstream fp(buf, std::ifstream::in);

		// Monster ID
		int id = 0;

		// Monster scale factor
		float x, y, z;
		x = y = z = 0.0f;

		// Check that the file is open
		if (fp.is_open() == true)
		{
			// Current line read from the file
			std::string line{ "" };

			// Read lines from file
			while (getline(fp, line))
			{
				// Remove spaces from the line
				line.erase(std::remove_if(line.begin(), line.end(), std::isspace), line.end());

				// Check that the line is empty
				if (line.empty() == true)
				{
					// Skip the line
					continue;
				}

				// Parse the line
				int ret = sscanf_s(
					line.c_str(), "%d{%f;%f;%f}", &id, &x, &y, &z
				);

				// Check the result
				if (ret == 4)
				{
					// Search the monster in the list
					auto it = pkodev::resize.find(id);

					// Check the result
					if (it == pkodev::resize.end())
					{
						// Insert a new record
						pkodev::resize.insert( { id, { x, y, z } } );
					}
					else
					{
						// Update the old record
						it->second.scale_.update(x, y, z);
					}
				}
			}

			// Close the file
			fp.close();
		}
	}

	// Set patch for the method CCharacter* CGameScene::AddCharacter( int nScriptID )
	// Change the variable D3DXVECTOR3 scale
	{
		// Access type
		DWORD access = PAGE_EXECUTE_READWRITE;

		// Change the access protection of the proccess
		VirtualProtect(reinterpret_cast<LPVOID>(pkodev::address::MOD_EXE_VERSION::CGameScene__AddCharacter__scale),
			5, access, &access);

		// Write patch bytes
		*reinterpret_cast<char*>(pkodev::address::MOD_EXE_VERSION::CGameScene__AddCharacter__scale) = static_cast<char>(0x68);
		*reinterpret_cast<unsigned int*>(pkodev::address::MOD_EXE_VERSION::CGameScene__AddCharacter__scale + 1) = reinterpret_cast<unsigned int>(&pkodev::scale);

		// Rollback the access protection of the proccess
		VirtualProtect(reinterpret_cast<LPVOID>(pkodev::address::MOD_EXE_VERSION::CGameScene__AddCharacter__scale),
			5, access, &access);
	}

	// Enable hooks
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourAttach(&(PVOID&)pkodev::pointer::CGameScene__AddCharacter, pkodev::hook::CGameScene__AddCharacter);
	DetourTransactionCommit();
}

// Stop the mod
void Stop()
{
	// Disable hooks
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)pkodev::pointer::CGameScene__AddCharacter, pkodev::hook::CGameScene__AddCharacter);
	DetourTransactionCommit();
}

// CCharacter* CGameScene::AddCharacter( int nScriptID )
void* __fastcall pkodev::hook::CGameScene__AddCharacter(void* This, 
	void* NotUsed, int nScriptID)
{
	// Search the character in the list
	const auto it = pkodev::resize.find(nScriptID);

	// Check that monster is found
	if (it != pkodev::resize.end())
	{
		// Set scale factor
		pkodev::scale.update(
			it->second.scale_.x, it->second.scale_.y, it->second.scale_.z
		);
	}
	else
	{
		// Set scale factor by default
		pkodev::scale.update();
	}

	// Call the original method CCharacter* CGameScene::AddCharacter( int nScriptID ) 
	void *ret =  pkodev::pointer::CGameScene__AddCharacter(This, nScriptID);

	// Check that monster is found
	if ( it != pkodev::resize.end() )
	{
		// Check that characerinfo is not updated
		if (it->second.updated == false)
		{
			// Update monster height
			*reinterpret_cast<float*>(
				*reinterpret_cast<unsigned int*>(
					reinterpret_cast<unsigned int>(ret) + 0x0414
				) + 0x010C
			) *= it->second.scale_.z;

			// characerinfo is updated
			it->second.updated = true;
		}
	}

	return ret;
}