#include <Windows.h>
#include <iostream>
#include <thread>
#include <fstream>

std::string outputPath = "text.txt";

// Put {a} where you want the artist name to be
// Put {s} where you want the song name to be
std::string outputFormat = "{a} - {s}";

bool getSpotifyInfo(std::string &artist, std::string &song)
{
	HWND hwSpotify = nullptr;

	hwSpotify = FindWindowExA(0, 0, "Chrome_WidgetWin_0", NULL);
	if (!hwSpotify)
		return false;

	char windowTextBuf[255];

	while (hwSpotify)
	{
		GetWindowTextA(hwSpotify, windowTextBuf, 255);
		
		if (strnlen_s(windowTextBuf, 255) > 0)
			break;

		hwSpotify = FindWindowExA(0, 0, "Chrome_WidgetWin_0", NULL);
	}

	if (windowTextBuf == "Spotify" || windowTextBuf == "Spotify Free" || windowTextBuf == "Advertisement")
		return false;

	std::string windowText = windowTextBuf;

	size_t found = windowText.find(" - ");

	artist = windowText.substr(0, found);

	song = windowText.substr(found + 3, windowText.begin() - windowText.end());

	return true;
}

std::string replaceString(std::string subject, const std::string& search,
	const std::string& replace) {
	size_t pos = 0;
	while ((pos = subject.find(search, pos)) != std::string::npos) {
		subject.replace(pos, search.length(), replace);
		pos += replace.length();
	}
	return subject;
}

bool saveToFile(std::string artist, std::string song)
{
	std::string out = replaceString(replaceString(outputFormat, "{a}", artist), "{s}", song);

	std::ofstream file(outputPath, std::ios::trunc);

	if (file.good())
		file << out;
	else
		return false;

	file.close();

	return true;
}

int main(int argc, char** argv, char** envp)
{
	std::string artist = "";
	std::string song = "";

	std::string lastArtist = "";
	std::string lastSong = "";

	while (true)
	{
		if (getSpotifyInfo(artist, song))
		{
			if (artist != lastArtist || song != lastSong)
			{
				std::cout << replaceString(replaceString(outputFormat, "{a}", artist), "{s}", song) << std::endl;

				// Only save details to file if the artist or song changes
				if (!saveToFile(artist, song))
					std::cout << "Failed to save file\n";
			}

			lastArtist = artist;
			lastSong = song;
		}

		if (GetAsyncKeyState(VK_END))
			break;

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	return 0;
}