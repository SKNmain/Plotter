#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

using namespace std;

Uint32 getpixel(SDL_Surface *surface, int x, int y);
bool fileExists(string filename);

struct cmd {
	bool	pen = false;
	int		x	= 0;
	int		y	= 0;
};

int main(int argc, char** argv) {
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
		cout << "SDL_Init doesn't work!" << endl;
		return 1;
	}
	if (argc != 2) {
		cout	<< "No input file!" << endl
				<< "Usage: " << argv[0] << " <image>" << endl
				<< "Image formats: PNG, JPEG, JPG, BMP, GIF (one frame), " << endl
				<< "LBM, PNM, PCX, TIF, XPM, XCF" << endl;
		return 2;
	}

	SDL_Surface*	image = IMG_Load(argv[1]);

	vector<cmd>		cmdList;
	cmd				tmpCMD;
	tmpCMD.pen		= false;
	tmpCMD.x		= 0;
	tmpCMD.y		= 0;
	cmdList.push_back(tmpCMD);

	SDL_Color	clr;
	SDL_Color	lastClr;
	clr.a		= 0x0;

	for(int y = 0; y < image->h; ++y) {
		for(int x = 0; x < image->w; ++x) {
			SDL_GetRGB(
				getpixel(image, x, y),
				image->format,
				&clr.r, &clr.g, &clr.b
			);
			if (x == 0 && y == 0) {
				SDL_memcpy(&lastClr, &clr, sizeof(SDL_Color));
			}

			if (SDL_memcmp(&lastClr, &clr, sizeof(SDL_Color)) != 0) {
				if (clr.r == 0 && clr.g == 0 && clr.b == 0) {
					tmpCMD.pen	= false;
				} else {
					tmpCMD.pen	= true;
				}
				tmpCMD.x		= x;
				tmpCMD.y		= y;

				if (cmdList.back().pen == false && tmpCMD.pen == false) {
					cmdList.pop_back();
				}
				if (cmdList.back().pen == true && tmpCMD.pen == true) {
					cmdList.pop_back();
				}
				cmdList.push_back(tmpCMD);
				cout << "P: " << tmpCMD.pen << " => " << x << ";" << y << endl;
			}
			if (x == (image->w - 1)) {
				tmpCMD.pen	= false;
				tmpCMD.x	= 0;
				tmpCMD.y	= y + 1;
				if (cmdList.back().pen == false) {
					cmdList.pop_back();
				}
				cmdList.push_back(tmpCMD);
			}

			SDL_memcpy(&lastClr, &clr, sizeof(SDL_Color));
		}
	}

	if (cmdList.back().pen == false) {
		cmdList.pop_back();
	}

	cout << "======================= FINISH" << endl;
	cout << "CMD Amount: " << cmdList.size() << endl << endl;

	if (fileExists("./out.txt")) {
		cout	<< "Are you sure you want overwrite file \"out.txt\"?" << endl
				<< "[ENTER] to proceed, to cancel close this window." << endl;
	}

	ofstream hFile("./out.txt", ios::trunc);

	hFile << "unsigned short cmdsAmount = " << cmdList.size() << ";\n";
	hFile << "unsigned short cmds[] = {\n";
	for(int i = 0; i < cmdList.size(); ++i) {
		hFile << "\t";
		hFile << (cmdList[i].pen? "1": "0") << ", ";
		hFile << cmdList[i].x << ", " << cmdList[i].y;
		if (i < (cmdList.size() - 1)) {
			hFile	<< ",";
		}
		hFile << "\n";
	}
	hFile << "};\n";
	hFile.flush();
	hFile.close();

	SDL_FreeSurface(image);
	system("pause>nul");
	return 0;
}

Uint32 getpixel(SDL_Surface *surface, int x, int y)
{
    int bpp = surface->format->BytesPerPixel;
    /* Here p is the address to the pixel we want to retrieve */
    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        return *p;
        break;

    case 2:
        return *(Uint16 *)p;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN)
            return p[0] << 16 | p[1] << 8 | p[2];
        else
            return p[0] | p[1] << 8 | p[2] << 16;
        break;

    case 4:
        return *(Uint32 *)p;
        break;

    default:
        return 0;       /* shouldn't happen, but avoids warnings */
    }
}

bool fileExists(string filename) {
	FILE*	file = fopen(filename.c_str(), "r");
	if (file == NULL) {
		return false;
	}
	fclose(file);
	return true;
}