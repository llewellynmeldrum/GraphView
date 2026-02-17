#pragma once

#include <cassert>
#include <sstream>
#include <string>
#include <ostream>
#include <cstdlib>
#include <iomanip>
#include <array>
namespace ansi {

// Styles
static constexpr const char *bold         =			 "\e[1m";
static constexpr const char *dim          =			 "\e[2m";
static constexpr const char *italic       =			 "\e[3m";
static constexpr const char *underline    =			 "\e[4m";
static constexpr const char *blink        =			 "\e[5m";
static constexpr const char *inverse      =			 "\e[7m";
static constexpr const char *hidden       =			 "\e[8m";
static constexpr const char *strikethrough =			 "\e[9m";

//constexpr const char *black        =			 "\e[30m";

// Background colors




#define DEFINE_COLOR(name, code)\
constexpr fmt name("\e[" #code "m", #name)


#define DEFINE_BGCOLOR(name, code)\
DEFINE_COLOR(name,48;5;code)

#define DEFINE_FGCOLOR(name, code)\
DEFINE_COLOR(name,38;5;code)

constexpr const char *_reset = "\e[0m";
// opaque data structure which from the outside should act mostly like it is just the "fmt" string
struct fmt  {
	const char *data;
	const char *name;
	constexpr fmt(const char* fmt, const char* name) : name(name), data(fmt) {}

	friend std::ostringstream &operator<<(std::ostringstream& os, const fmt& obj) {
		os << obj.data;
		return os;
	}


	friend std::ostream &operator<<(std::ostream& os, const fmt& obj) {
		os << obj.data;
		return os;
	}


	std::string_view view() const noexcept {
		return data;
	}

	// single implicit conversion:
	operator std::string_view() const noexcept {
		return view();
	}

	// single implicit conversion:
	operator std::string() const noexcept {
		return std::string(view());
	}


	inline std::string test_bg() const;
	inline std::string test_fg() const;

};

// BASIC COLORS - FG
DEFINE_COLOR(reset, 0);

DEFINE_COLOR(fg_black	, 30);
DEFINE_COLOR(fg_red	, 31);
DEFINE_COLOR(fg_green	, 32);
DEFINE_COLOR(fg_yellow	, 33);
DEFINE_COLOR(fg_blue	, 34);
DEFINE_COLOR(fg_magenta	, 35);
DEFINE_COLOR(fg_cyan	, 36);
DEFINE_COLOR(fg_white	, 37);

// BASIC COLORS - BG
DEFINE_COLOR(bg_black	, 40);
DEFINE_COLOR(bg_red	, 41);
DEFINE_COLOR(bg_green	, 42);
DEFINE_COLOR(bg_yellow	, 43);
DEFINE_COLOR(bg_blue	, 44);
DEFINE_COLOR(bg_magenta	, 45);
DEFINE_COLOR(bg_cyan	, 46);
DEFINE_COLOR(bg_white	, 47);

DEFINE_BGCOLOR(bg_grey0, 232);
DEFINE_BGCOLOR(bg_grey1, 233);
DEFINE_BGCOLOR(bg_grey2, 234);
DEFINE_BGCOLOR(bg_grey3, 235);
DEFINE_BGCOLOR(bg_grey4, 236);
DEFINE_BGCOLOR(bg_grey5, 237);
DEFINE_BGCOLOR(bg_grey6, 238);
DEFINE_BGCOLOR(bg_grey7, 239);
DEFINE_BGCOLOR(bg_grey8, 240);
DEFINE_BGCOLOR(bg_grey9, 241);
DEFINE_BGCOLOR(bg_grey10, 242);
DEFINE_BGCOLOR(bg_grey11, 243);
DEFINE_BGCOLOR(bg_grey12, 244);
DEFINE_BGCOLOR(bg_grey13, 245);
DEFINE_BGCOLOR(bg_grey14, 246);
DEFINE_BGCOLOR(bg_grey15, 247);
DEFINE_BGCOLOR(bg_grey16, 248);
DEFINE_BGCOLOR(bg_grey17, 249);
DEFINE_BGCOLOR(bg_grey18, 250);
DEFINE_BGCOLOR(bg_grey19, 251);
DEFINE_BGCOLOR(bg_grey20, 252);
DEFINE_BGCOLOR(bg_grey21, 253);
DEFINE_BGCOLOR(bg_grey22, 254);
DEFINE_BGCOLOR(bg_grey23, 255);
DEFINE_BGCOLOR(bg_grey24, 231);

DEFINE_FGCOLOR(fg_red0, 9);
DEFINE_FGCOLOR(fg_lime, 10);
DEFINE_FGCOLOR(fg_yellow0, 11);
DEFINE_FGCOLOR(fg_blue0, 12);
DEFINE_FGCOLOR(fg_fuchsia, 13);
DEFINE_FGCOLOR(fg_aqua, 14);
DEFINE_FGCOLOR(fg_white0, 15);
DEFINE_FGCOLOR(fg_navyblue, 17);
DEFINE_FGCOLOR(fg_darkblue, 18);
DEFINE_FGCOLOR(fg_blue2, 19);
DEFINE_FGCOLOR(fg_blue3, 20);
DEFINE_FGCOLOR(fg_blue1, 21);
DEFINE_FGCOLOR(fg_darkgreen, 22);
DEFINE_FGCOLOR(fg_deepskyblue5, 23);
DEFINE_FGCOLOR(fg_deepskyblue6, 24);
DEFINE_FGCOLOR(fg_deepskyblue7, 25);
DEFINE_FGCOLOR(fg_dodgerblue2, 26);
DEFINE_FGCOLOR(fg_dodgerblue1, 27);
DEFINE_FGCOLOR(fg_green4, 28);
DEFINE_FGCOLOR(fg_springgreen6, 29);
DEFINE_FGCOLOR(fg_turquoise4, 30);
DEFINE_FGCOLOR(fg_deepskyblue3, 31);
DEFINE_FGCOLOR(fg_deepskyblue4, 32);
DEFINE_FGCOLOR(fg_dodgerblue, 33);
DEFINE_FGCOLOR(fg_green2, 34);
DEFINE_FGCOLOR(fg_springgreen4, 35);
DEFINE_FGCOLOR(fg_darkcyan, 36);
DEFINE_FGCOLOR(fg_lightseagreen, 37);
DEFINE_FGCOLOR(fg_deepskyblue2, 38);
DEFINE_FGCOLOR(fg_deepskyblue1, 39);
DEFINE_FGCOLOR(fg_green3, 40);
DEFINE_FGCOLOR(fg_springgreen5, 41);
DEFINE_FGCOLOR(fg_springgreen2, 42);
DEFINE_FGCOLOR(fg_cyan3, 43);
DEFINE_FGCOLOR(fg_darkturquoise, 44);
DEFINE_FGCOLOR(fg_turquoise2, 45);
DEFINE_FGCOLOR(fg_green1, 46);
DEFINE_FGCOLOR(fg_springgreen3, 47);
DEFINE_FGCOLOR(fg_springgreen1, 48);
DEFINE_FGCOLOR(fg_mediumspringgreen, 49);
DEFINE_FGCOLOR(fg_cyan2, 50);
DEFINE_FGCOLOR(fg_cyan1, 51);
DEFINE_FGCOLOR(fg_darkred, 52);
DEFINE_FGCOLOR(fg_deeppink5, 53);
DEFINE_FGCOLOR(fg_purple4, 54);
DEFINE_FGCOLOR(fg_purple5, 55);
DEFINE_FGCOLOR(fg_purple3, 56);
DEFINE_FGCOLOR(fg_blueviolet, 57);
DEFINE_FGCOLOR(fg_orange, 58);
DEFINE_FGCOLOR(fg_mediumpurple6, 60);
DEFINE_FGCOLOR(fg_slateblue2, 61);
DEFINE_FGCOLOR(fg_slateblue3, 62);
DEFINE_FGCOLOR(fg_royalblue1, 63);
DEFINE_FGCOLOR(fg_chartreuse6, 64);
DEFINE_FGCOLOR(fg_darkseagreen7, 65);
DEFINE_FGCOLOR(fg_paleturquoise4, 66);
DEFINE_FGCOLOR(fg_steelblue, 67);
DEFINE_FGCOLOR(fg_steelblue3, 68);
DEFINE_FGCOLOR(fg_cornflowerblue, 69);
DEFINE_FGCOLOR(fg_chartreuse4, 70);
DEFINE_FGCOLOR(fg_darkseagreen8, 71);
DEFINE_FGCOLOR(fg_cadetblue, 72);
DEFINE_FGCOLOR(fg_cadetblue1, 73);
DEFINE_FGCOLOR(fg_skyblue3, 74);
DEFINE_FGCOLOR(fg_steelblue1, 75);
DEFINE_FGCOLOR(fg_chartreuse5, 76);
DEFINE_FGCOLOR(fg_palegreen2, 77);
DEFINE_FGCOLOR(fg_seagreen3, 78);
DEFINE_FGCOLOR(fg_aquamarine2, 79);
DEFINE_FGCOLOR(fg_mediumturquoise, 80);
DEFINE_FGCOLOR(fg_steelblue2, 81);
DEFINE_FGCOLOR(fg_chartreuse2, 82);
DEFINE_FGCOLOR(fg_seagreen2, 83);
DEFINE_FGCOLOR(fg_seagreen, 84);
DEFINE_FGCOLOR(fg_seagreen1, 85);
DEFINE_FGCOLOR(fg_aquamarine, 86);
DEFINE_FGCOLOR(fg_darkslategray2, 87);
DEFINE_FGCOLOR(fg_darkred1, 88);
DEFINE_FGCOLOR(fg_deeppink6, 89);
DEFINE_FGCOLOR(fg_darkmagenta, 90);
DEFINE_FGCOLOR(fg_darkmagenta1, 91);
DEFINE_FGCOLOR(fg_darkviolet, 92);
DEFINE_FGCOLOR(fg_purple1, 93);
DEFINE_FGCOLOR(fg_orange4, 94);
DEFINE_FGCOLOR(fg_lightpink4, 95);
DEFINE_FGCOLOR(fg_plum4, 96);
DEFINE_FGCOLOR(fg_mediumpurple4, 97);
DEFINE_FGCOLOR(fg_mediumpurple5, 98);
DEFINE_FGCOLOR(fg_slateblue1, 99);
DEFINE_FGCOLOR(fg_yellow5, 100);
DEFINE_FGCOLOR(fg_wheat4, 101);
DEFINE_FGCOLOR(fg_lightslategrey, 103);
DEFINE_FGCOLOR(fg_mediumpurple, 104);
DEFINE_FGCOLOR(fg_lightslateblue, 105);
DEFINE_FGCOLOR(fg_yellow6, 106);
DEFINE_FGCOLOR(fg_darkolivegreen4, 107);
DEFINE_FGCOLOR(fg_darkseagreen, 108);
DEFINE_FGCOLOR(fg_lightskyblue2, 109);
DEFINE_FGCOLOR(fg_lightskyblue3, 110);
DEFINE_FGCOLOR(fg_skyblue2, 111);
DEFINE_FGCOLOR(fg_chartreuse3, 112);
DEFINE_FGCOLOR(fg_darkolivegreen5, 113);
DEFINE_FGCOLOR(fg_palegreen3, 114);
DEFINE_FGCOLOR(fg_darkseagreen5, 115);
DEFINE_FGCOLOR(fg_darkslategray3, 116);
DEFINE_FGCOLOR(fg_skyblue1, 117);
DEFINE_FGCOLOR(fg_chartreuse1, 118);
DEFINE_FGCOLOR(fg_lightgreen, 119);
DEFINE_FGCOLOR(fg_lightgreen1, 120);
DEFINE_FGCOLOR(fg_palegreen, 121);
DEFINE_FGCOLOR(fg_aquamarine1, 122);
DEFINE_FGCOLOR(fg_darkslategray1, 123);
DEFINE_FGCOLOR(fg_red2, 124);
DEFINE_FGCOLOR(fg_deeppink7, 125);
DEFINE_FGCOLOR(fg_mediumvioletred, 126);
DEFINE_FGCOLOR(fg_magenta3, 127);
DEFINE_FGCOLOR(fg_darkviolet1, 128);
DEFINE_FGCOLOR(fg_purple2, 129);
DEFINE_FGCOLOR(fg_darkorange1, 130);
DEFINE_FGCOLOR(fg_indianred, 131);
DEFINE_FGCOLOR(fg_hotpink3, 132);
DEFINE_FGCOLOR(fg_mediumorchid3, 133);
DEFINE_FGCOLOR(fg_mediumorchid, 134);
DEFINE_FGCOLOR(fg_mediumpurple2, 135);
DEFINE_FGCOLOR(fg_darkgoldenrod, 136);
DEFINE_FGCOLOR(fg_lightsalmon1, 137);
DEFINE_FGCOLOR(fg_rosybrown, 138);
DEFINE_FGCOLOR(fg_mediumpurple3, 140);
DEFINE_FGCOLOR(fg_mediumpurple1, 141);
DEFINE_FGCOLOR(fg_gold1, 142);
DEFINE_FGCOLOR(fg_darkkhaki, 143);
DEFINE_FGCOLOR(fg_navajowhite3, 144);
DEFINE_FGCOLOR(fg_lightsteelblue3, 146);
DEFINE_FGCOLOR(fg_lightsteelblue, 147);
DEFINE_FGCOLOR(fg_yellow3, 148);
DEFINE_FGCOLOR(fg_darkolivegreen6, 149);
DEFINE_FGCOLOR(fg_darkseagreen6, 150);
DEFINE_FGCOLOR(fg_darkseagreen3, 151);
DEFINE_FGCOLOR(fg_lightcyan3, 152);
DEFINE_FGCOLOR(fg_lightskyblue1, 153);
DEFINE_FGCOLOR(fg_greenyellow, 154);
DEFINE_FGCOLOR(fg_darkolivegreen2, 155);
DEFINE_FGCOLOR(fg_palegreen1, 156);
DEFINE_FGCOLOR(fg_darkseagreen4, 157);
DEFINE_FGCOLOR(fg_darkseagreen1, 158);
DEFINE_FGCOLOR(fg_paleturquoise1, 159);
DEFINE_FGCOLOR(fg_red3, 160);
DEFINE_FGCOLOR(fg_deeppink3, 161);
DEFINE_FGCOLOR(fg_deeppink4, 162);
DEFINE_FGCOLOR(fg_magenta4, 163);
DEFINE_FGCOLOR(fg_magenta5, 164);
DEFINE_FGCOLOR(fg_magenta1, 165);
DEFINE_FGCOLOR(fg_darkorange2, 166);
DEFINE_FGCOLOR(fg_indianred1, 167);
DEFINE_FGCOLOR(fg_hotpink4, 168);
DEFINE_FGCOLOR(fg_hotpink2, 169);
DEFINE_FGCOLOR(fg_orchid, 170);
DEFINE_FGCOLOR(fg_mediumorchid1, 171);
DEFINE_FGCOLOR(fg_orange3, 172);
DEFINE_FGCOLOR(fg_lightsalmon2, 173);
DEFINE_FGCOLOR(fg_lightpink3, 174);
DEFINE_FGCOLOR(fg_pink3, 175);
DEFINE_FGCOLOR(fg_plum3, 176);
DEFINE_FGCOLOR(fg_violet, 177);
DEFINE_FGCOLOR(fg_gold2, 178);
DEFINE_FGCOLOR(fg_lightgoldenrod5, 179);
DEFINE_FGCOLOR(fg_tan, 180);
DEFINE_FGCOLOR(fg_mistyrose3, 181);
DEFINE_FGCOLOR(fg_thistle3, 182);
DEFINE_FGCOLOR(fg_plum2, 183);
DEFINE_FGCOLOR(fg_yellow4, 184);
DEFINE_FGCOLOR(fg_khaki3, 185);
DEFINE_FGCOLOR(fg_lightgoldenrod2, 186);
DEFINE_FGCOLOR(fg_lightyellow3, 187);
DEFINE_FGCOLOR(fg_lightsteelblue1, 189);
DEFINE_FGCOLOR(fg_yellow2, 190);
DEFINE_FGCOLOR(fg_darkolivegreen, 191);
DEFINE_FGCOLOR(fg_darkolivegreen1, 192);
DEFINE_FGCOLOR(fg_darkseagreen2, 193);
DEFINE_FGCOLOR(fg_honeydew2, 194);
DEFINE_FGCOLOR(fg_lightcyan1, 195);
DEFINE_FGCOLOR(fg_red1, 196);
DEFINE_FGCOLOR(fg_deeppink2, 197);
DEFINE_FGCOLOR(fg_deeppink, 198);
DEFINE_FGCOLOR(fg_deeppink1, 199);
DEFINE_FGCOLOR(fg_magenta2, 200);
DEFINE_FGCOLOR(fg_magenta0, 201);
DEFINE_FGCOLOR(fg_orangered1, 202);
DEFINE_FGCOLOR(fg_indianred2, 203);
DEFINE_FGCOLOR(fg_indianred3, 204);
DEFINE_FGCOLOR(fg_hotpink, 205);
DEFINE_FGCOLOR(fg_hotpink1, 206);
DEFINE_FGCOLOR(fg_mediumorchid2, 207);
DEFINE_FGCOLOR(fg_darkorange, 208);
DEFINE_FGCOLOR(fg_salmon1, 209);
DEFINE_FGCOLOR(fg_lightcoral, 210);
DEFINE_FGCOLOR(fg_palevioletred1, 211);
DEFINE_FGCOLOR(fg_orchid2, 212);
DEFINE_FGCOLOR(fg_orchid1, 213);
DEFINE_FGCOLOR(fg_orange1, 214);
DEFINE_FGCOLOR(fg_sandybrown, 215);
DEFINE_FGCOLOR(fg_lightsalmon, 216);
DEFINE_FGCOLOR(fg_lightpink1, 217);
DEFINE_FGCOLOR(fg_pink1, 218);
DEFINE_FGCOLOR(fg_plum1, 219);
DEFINE_FGCOLOR(fg_gold, 220);
DEFINE_FGCOLOR(fg_lightgoldenrod3, 221);
DEFINE_FGCOLOR(fg_lightgoldenrod4, 222);
DEFINE_FGCOLOR(fg_navajowhite1, 223);
DEFINE_FGCOLOR(fg_mistyrose1, 224);
DEFINE_FGCOLOR(fg_thistle1, 225);
DEFINE_FGCOLOR(fg_yellow1, 226);
DEFINE_FGCOLOR(fg_lightgoldenrod1, 227);
DEFINE_FGCOLOR(fg_khaki1, 228);
DEFINE_FGCOLOR(fg_wheat1, 229);
DEFINE_FGCOLOR(fg_cornsilk1, 230);

DEFINE_BGCOLOR(bg_red0, 9);
DEFINE_BGCOLOR(bg_lime, 10);
DEFINE_BGCOLOR(bg_yellow0, 11);
DEFINE_BGCOLOR(bg_blue0, 12);
DEFINE_BGCOLOR(bg_fuchsia, 13);
DEFINE_BGCOLOR(bg_aqua, 14);
DEFINE_BGCOLOR(bg_white0, 15);
DEFINE_BGCOLOR(bg_navyblue, 17);
DEFINE_BGCOLOR(bg_darkblue, 18);
DEFINE_BGCOLOR(bg_blue2, 19);
DEFINE_BGCOLOR(bg_blue3, 20);
DEFINE_BGCOLOR(bg_blue1, 21);
DEFINE_BGCOLOR(bg_darkgreen, 22);
DEFINE_BGCOLOR(bg_deepskyblue5, 23);
DEFINE_BGCOLOR(bg_deepskyblue6, 24);
DEFINE_BGCOLOR(bg_deepskyblue7, 25);
DEFINE_BGCOLOR(bg_dodgerblue2, 26);
DEFINE_BGCOLOR(bg_dodgerblue1, 27);
DEFINE_BGCOLOR(bg_green4, 28);
DEFINE_BGCOLOR(bg_springgreen6, 29);
DEFINE_BGCOLOR(bg_turquoise4, 30);
DEFINE_BGCOLOR(bg_deepskyblue3, 31);
DEFINE_BGCOLOR(bg_deepskyblue4, 32);
DEFINE_BGCOLOR(bg_dodgerblue, 33);
DEFINE_BGCOLOR(bg_green2, 34);
DEFINE_BGCOLOR(bg_springgreen4, 35);
DEFINE_BGCOLOR(bg_darkcyan, 36);
DEFINE_BGCOLOR(bg_lightseagreen, 37);
DEFINE_BGCOLOR(bg_deepskyblue2, 38);
DEFINE_BGCOLOR(bg_deepskyblue1, 39);
DEFINE_BGCOLOR(bg_green3, 40);
DEFINE_BGCOLOR(bg_springgreen5, 41);
DEFINE_BGCOLOR(bg_springgreen2, 42);
DEFINE_BGCOLOR(bg_cyan3, 43);
DEFINE_BGCOLOR(bg_darkturquoise, 44);
DEFINE_BGCOLOR(bg_turquoise2, 45);
DEFINE_BGCOLOR(bg_green1, 46);
DEFINE_BGCOLOR(bg_springgreen3, 47);
DEFINE_BGCOLOR(bg_springgreen1, 48);
DEFINE_BGCOLOR(bg_mediumspringgreen, 49);
DEFINE_BGCOLOR(bg_cyan2, 50);
DEFINE_BGCOLOR(bg_cyan1, 51);
DEFINE_BGCOLOR(bg_darkred, 52);
DEFINE_BGCOLOR(bg_deeppink5, 53);
DEFINE_BGCOLOR(bg_purple4, 54);
DEFINE_BGCOLOR(bg_purple5, 55);
DEFINE_BGCOLOR(bg_purple3, 56);
DEFINE_BGCOLOR(bg_blueviolet, 57);
DEFINE_BGCOLOR(bg_orange, 58);
DEFINE_BGCOLOR(bg_mediumpurple6, 60);
DEFINE_BGCOLOR(bg_slateblue2, 61);
DEFINE_BGCOLOR(bg_slateblue3, 62);
DEFINE_BGCOLOR(bg_royalblue1, 63);
DEFINE_BGCOLOR(bg_chartreuse6, 64);
DEFINE_BGCOLOR(bg_darkseagreen7, 65);
DEFINE_BGCOLOR(bg_paleturquoise4, 66);
DEFINE_BGCOLOR(bg_steelblue, 67);
DEFINE_BGCOLOR(bg_steelblue3, 68);
DEFINE_BGCOLOR(bg_cornflowerblue, 69);
DEFINE_BGCOLOR(bg_chartreuse4, 70);
DEFINE_BGCOLOR(bg_darkseagreen8, 71);
DEFINE_BGCOLOR(bg_cadetblue, 72);
DEFINE_BGCOLOR(bg_cadetblue1, 73);
DEFINE_BGCOLOR(bg_skyblue3, 74);
DEFINE_BGCOLOR(bg_steelblue1, 75);
DEFINE_BGCOLOR(bg_chartreuse5, 76);
DEFINE_BGCOLOR(bg_palegreen2, 77);
DEFINE_BGCOLOR(bg_seagreen3, 78);
DEFINE_BGCOLOR(bg_aquamarine2, 79);
DEFINE_BGCOLOR(bg_mediumturquoise, 80);
DEFINE_BGCOLOR(bg_steelblue2, 81);
DEFINE_BGCOLOR(bg_chartreuse2, 82);
DEFINE_BGCOLOR(bg_seagreen2, 83);
DEFINE_BGCOLOR(bg_seagreen, 84);
DEFINE_BGCOLOR(bg_seagreen1, 85);
DEFINE_BGCOLOR(bg_aquamarine, 86);
DEFINE_BGCOLOR(bg_darkslategray2, 87);
DEFINE_BGCOLOR(bg_darkred1, 88);
DEFINE_BGCOLOR(bg_deeppink6, 89);
DEFINE_BGCOLOR(bg_darkmagenta, 90);
DEFINE_BGCOLOR(bg_darkmagenta1, 91);
DEFINE_BGCOLOR(bg_darkviolet, 92);
DEFINE_BGCOLOR(bg_purple1, 93);
DEFINE_BGCOLOR(bg_orange4, 94);
DEFINE_BGCOLOR(bg_lightpink4, 95);
DEFINE_BGCOLOR(bg_plum4, 96);
DEFINE_BGCOLOR(bg_mediumpurple4, 97);
DEFINE_BGCOLOR(bg_mediumpurple5, 98);
DEFINE_BGCOLOR(bg_slateblue1, 99);
DEFINE_BGCOLOR(bg_yellow5, 100);
DEFINE_BGCOLOR(bg_wheat4, 101);
DEFINE_BGCOLOR(bg_lightslategrey, 103);
DEFINE_BGCOLOR(bg_mediumpurple, 104);
DEFINE_BGCOLOR(bg_lightslateblue, 105);
DEFINE_BGCOLOR(bg_yellow6, 106);
DEFINE_BGCOLOR(bg_darkolivegreen4, 107);
DEFINE_BGCOLOR(bg_darkseagreen, 108);
DEFINE_BGCOLOR(bg_lightskyblue2, 109);
DEFINE_BGCOLOR(bg_lightskyblue3, 110);
DEFINE_BGCOLOR(bg_skyblue2, 111);
DEFINE_BGCOLOR(bg_chartreuse3, 112);
DEFINE_BGCOLOR(bg_darkolivegreen5, 113);
DEFINE_BGCOLOR(bg_palegreen3, 114);
DEFINE_BGCOLOR(bg_darkseagreen5, 115);
DEFINE_BGCOLOR(bg_darkslategray3, 116);
DEFINE_BGCOLOR(bg_skyblue1, 117);
DEFINE_BGCOLOR(bg_chartreuse1, 118);
DEFINE_BGCOLOR(bg_lightgreen, 119);
DEFINE_BGCOLOR(bg_lightgreen1, 120);
DEFINE_BGCOLOR(bg_palegreen, 121);
DEFINE_BGCOLOR(bg_aquamarine1, 122);
DEFINE_BGCOLOR(bg_darkslategray1, 123);
DEFINE_BGCOLOR(bg_red2, 124);
DEFINE_BGCOLOR(bg_deeppink7, 125);
DEFINE_BGCOLOR(bg_mediumvioletred, 126);
DEFINE_BGCOLOR(bg_magenta3, 127);
DEFINE_BGCOLOR(bg_darkviolet1, 128);
DEFINE_BGCOLOR(bg_purple2, 129);
DEFINE_BGCOLOR(bg_darkorange1, 130);
DEFINE_BGCOLOR(bg_indianred, 131);
DEFINE_BGCOLOR(bg_hotpink3, 132);
DEFINE_BGCOLOR(bg_mediumorchid3, 133);
DEFINE_BGCOLOR(bg_mediumorchid, 134);
DEFINE_BGCOLOR(bg_mediumpurple2, 135);
DEFINE_BGCOLOR(bg_darkgoldenrod, 136);
DEFINE_BGCOLOR(bg_lightsalmon1, 137);
DEFINE_BGCOLOR(bg_rosybrown, 138);
DEFINE_BGCOLOR(bg_mediumpurple3, 140);
DEFINE_BGCOLOR(bg_mediumpurple1, 141);
DEFINE_BGCOLOR(bg_gold1, 142);
DEFINE_BGCOLOR(bg_darkkhaki, 143);
DEFINE_BGCOLOR(bg_navajowhite3, 144);
DEFINE_BGCOLOR(bg_lightsteelblue3, 146);
DEFINE_BGCOLOR(bg_lightsteelblue, 147);
DEFINE_BGCOLOR(bg_yellow3, 148);
DEFINE_BGCOLOR(bg_darkolivegreen6, 149);
DEFINE_BGCOLOR(bg_darkseagreen6, 150);
DEFINE_BGCOLOR(bg_darkseagreen3, 151);
DEFINE_BGCOLOR(bg_lightcyan3, 152);
DEFINE_BGCOLOR(bg_lightskyblue1, 153);
DEFINE_BGCOLOR(bg_greenyellow, 154);
DEFINE_BGCOLOR(bg_darkolivegreen2, 155);
DEFINE_BGCOLOR(bg_palegreen1, 156);
DEFINE_BGCOLOR(bg_darkseagreen4, 157);
DEFINE_BGCOLOR(bg_darkseagreen1, 158);
DEFINE_BGCOLOR(bg_paleturquoise1, 159);
DEFINE_BGCOLOR(bg_red3, 160);
DEFINE_BGCOLOR(bg_deeppink3, 161);
DEFINE_BGCOLOR(bg_deeppink4, 162);
DEFINE_BGCOLOR(bg_magenta4, 163);
DEFINE_BGCOLOR(bg_magenta5, 164);
DEFINE_BGCOLOR(bg_magenta1, 165);
DEFINE_BGCOLOR(bg_darkorange2, 166);
DEFINE_BGCOLOR(bg_indianred1, 167);
DEFINE_BGCOLOR(bg_hotpink4, 168);
DEFINE_BGCOLOR(bg_hotpink2, 169);
DEFINE_BGCOLOR(bg_orchid, 170);
DEFINE_BGCOLOR(bg_mediumorchid1, 171);
DEFINE_BGCOLOR(bg_orange3, 172);
DEFINE_BGCOLOR(bg_lightsalmon2, 173);
DEFINE_BGCOLOR(bg_lightpink3, 174);
DEFINE_BGCOLOR(bg_pink3, 175);
DEFINE_BGCOLOR(bg_plum3, 176);
DEFINE_BGCOLOR(bg_violet, 177);
DEFINE_BGCOLOR(bg_gold2, 178);
DEFINE_BGCOLOR(bg_lightgoldenrod5, 179);
DEFINE_BGCOLOR(bg_tan, 180);
DEFINE_BGCOLOR(bg_mistyrose3, 181);
DEFINE_BGCOLOR(bg_thistle3, 182);
DEFINE_BGCOLOR(bg_plum2, 183);
DEFINE_BGCOLOR(bg_yellow4, 184);
DEFINE_BGCOLOR(bg_khaki3, 185);
DEFINE_BGCOLOR(bg_lightgoldenrod2, 186);
DEFINE_BGCOLOR(bg_lightyellow3, 187);
DEFINE_BGCOLOR(bg_lightsteelblue1, 189);
DEFINE_BGCOLOR(bg_yellow2, 190);
DEFINE_BGCOLOR(bg_darkolivegreen, 191);
DEFINE_BGCOLOR(bg_darkolivegreen1, 192);
DEFINE_BGCOLOR(bg_darkseagreen2, 193);
DEFINE_BGCOLOR(bg_honeydew2, 194);
DEFINE_BGCOLOR(bg_lightcyan1, 195);
DEFINE_BGCOLOR(bg_red1, 196);
DEFINE_BGCOLOR(bg_deeppink2, 197);
DEFINE_BGCOLOR(bg_deeppink, 198);
DEFINE_BGCOLOR(bg_deeppink1, 199);
DEFINE_BGCOLOR(bg_magenta2, 200);
DEFINE_BGCOLOR(bg_magenta0, 201);
DEFINE_BGCOLOR(bg_orangered1, 202);
DEFINE_BGCOLOR(bg_indianred2, 203);
DEFINE_BGCOLOR(bg_indianred3, 204);
DEFINE_BGCOLOR(bg_hotpink, 205);
DEFINE_BGCOLOR(bg_hotpink1, 206);
DEFINE_BGCOLOR(bg_mediumorchid2, 207);
DEFINE_BGCOLOR(bg_darkorange, 208);
DEFINE_BGCOLOR(bg_salmon1, 209);
DEFINE_BGCOLOR(bg_lightcoral, 210);
DEFINE_BGCOLOR(bg_palevioletred1, 211);
DEFINE_BGCOLOR(bg_orchid2, 212);
DEFINE_BGCOLOR(bg_orchid1, 213);
DEFINE_BGCOLOR(bg_orange1, 214);
DEFINE_BGCOLOR(bg_sandybrown, 215);
DEFINE_BGCOLOR(bg_lightsalmon, 216);
DEFINE_BGCOLOR(bg_lightpink1, 217);
DEFINE_BGCOLOR(bg_pink1, 218);
DEFINE_BGCOLOR(bg_plum1, 219);
DEFINE_BGCOLOR(bg_gold, 220);
DEFINE_BGCOLOR(bg_lightgoldenrod3, 221);
DEFINE_BGCOLOR(bg_lightgoldenrod4, 222);
DEFINE_BGCOLOR(bg_navajowhite1, 223);
DEFINE_BGCOLOR(bg_mistyrose1, 224);
DEFINE_BGCOLOR(bg_thistle1, 225);
DEFINE_BGCOLOR(bg_yellow1, 226);
DEFINE_BGCOLOR(bg_lightgoldenrod1, 227);
DEFINE_BGCOLOR(bg_khaki1, 228);
DEFINE_BGCOLOR(bg_wheat1, 229);
DEFINE_BGCOLOR(bg_cornsilk1, 230);

static constexpr std::array<fmt, 25> greys = {{
		bg_grey0, bg_grey1, bg_grey2, bg_grey3,
		bg_grey4, bg_grey5, bg_grey6, bg_grey7,
		bg_grey8, bg_grey9, bg_grey10, bg_grey11,
		bg_grey12, bg_grey13, bg_grey14, bg_grey15,
		bg_grey16, bg_grey17, bg_grey18, bg_grey19,
		bg_grey20, bg_grey21, bg_grey22, bg_grey23,
		bg_grey24,
	}
};

static constexpr std::array<std::pair<char, const char *>, 100> matches = {
	{
		{'\r', "\\r"},
		{'\n', "\\n"},
		{'\t', "\\t"},
		{' ', "␣"},
		{'\e', "\\e"},
	}
};
static inline std::string escapeEscapes(const std::string& s, bool prettyPrint = true) {
	std::ostringstream oss;
	for (unsigned char ch : s) {
		bool replaced = false;
		for (const auto&[match, replacement] : matches) {
			if (ch == match) {
				replaced = true;
				oss << replacement;
				break;
			}
		}

		if (!replaced) oss << ch;

	}
	return oss.str();
}

inline std::string fmt::test_fg() const {
	// "\t\t\t name"
	std::ostringstream res;

	std::string prefix1{}, prefix2{};
	prefix1 += "[";
	prefix1 += this->name;

	prefix2 += "->'";
	prefix2 += escapeEscapes(this->data, false);
	prefix2 += "']: ";
	res << std::setw(25) << std::left << prefix1;
	res << std::setw(25) << std::right << prefix2;

	for (size_t i = 0; i < greys.size() / 2; i++) {
		res << greys[i * 2];
		res << this->data;
		res << "test";
		res << reset;
		res << " ";
	}
	// test with a range of backgrounds
	return res.str() + "\n";
};

inline	std::string fmt::test_bg() const {
	// "\t\t\t name"
	std::ostringstream res;

	std::string prefix1{}, prefix2{};
	prefix1 += "[";
	prefix1 += this->name;

	prefix2 += "->'";
	prefix2 += escapeEscapes(this->data, false);
	prefix2 += "']: ";
	res << std::setw(25) << std::left << prefix1;

	res << std::setw(25) << std::right << prefix2;

	res << this->data;
	res << "\t\t";
	res << _reset;
	// test with a range of backgrounds
	return res.str() + "\n";
};


static void debug_print_colors(std::ostream& os) {

	os  << ansi::fg_black	.test_fg();
	os  << ansi::fg_red	.test_fg();
	os  << ansi::fg_green	.test_fg();
	os  << ansi::fg_yellow	.test_fg();
	os  << ansi::fg_blue  	.test_fg();
	os  << ansi::fg_magenta	.test_fg();
	os  << ansi::fg_cyan   	.test_fg();
	os  << ansi::fg_white  	.test_fg();

	os  << ansi::bg_black	.test_bg();
	os  << ansi::bg_red	.test_bg();
	os  << ansi::bg_green	.test_bg();
	os  << ansi::bg_yellow	.test_bg();
	os  << ansi::bg_blue  	.test_bg();
	os  << ansi::bg_magenta	.test_bg();
	os  << ansi::bg_cyan   	.test_bg();
	os  << ansi::bg_white  	.test_bg();

	os  << ansi::bg_grey0.test_bg();
	os  << ansi::bg_grey1.test_bg();
	os  << ansi::bg_grey2.test_bg();
	os  << ansi::bg_grey3.test_bg();
	os  << ansi::bg_grey4.test_bg();
	os  << ansi::bg_grey5.test_bg();
	os  << ansi::bg_grey6.test_bg();
	os  << ansi::bg_grey7.test_bg();
	os  << ansi::bg_grey8.test_bg();
	os  << ansi::bg_grey9.test_bg();
	os  << ansi::bg_grey10.test_bg();
	os  << ansi::bg_grey11.test_bg();
	os  << ansi::bg_grey12.test_bg();
	os  << ansi::bg_grey13.test_bg();
	os  << ansi::bg_grey14.test_bg();
	os  << ansi::bg_grey15.test_bg();
	os  << ansi::bg_grey16.test_bg();
	os  << ansi::bg_grey17.test_bg();
	os  << ansi::bg_grey18.test_bg();
	os  << ansi::bg_grey19.test_bg();
	os  << ansi::bg_grey20.test_bg();
	os  << ansi::bg_grey21.test_bg();
	os  << ansi::bg_grey22.test_bg();
	os  << ansi::bg_grey23.test_bg();
	os  << ansi::bg_grey24.test_bg();
	os  << ansi::fg_black.test_fg();

	os  << ansi::fg_red1.test_fg();
	os  << ansi::fg_lime.test_fg();
	os  << ansi::fg_yellow0.test_fg();
	os  << ansi::fg_blue0.test_fg();
	os  << ansi::fg_fuchsia.test_fg();
	os  << ansi::fg_aqua.test_fg();
	os  << ansi::fg_white.test_fg();
	os  << ansi::fg_navyblue.test_fg();
	os  << ansi::fg_darkblue.test_fg();
	os  << ansi::fg_blue2.test_fg();
	os  << ansi::fg_blue3.test_fg();
	os  << ansi::fg_blue1.test_fg();
	os  << ansi::fg_darkgreen.test_fg();
	os  << ansi::fg_deepskyblue5.test_fg();
	os  << ansi::fg_deepskyblue6.test_fg();
	os  << ansi::fg_deepskyblue7.test_fg();
	os  << ansi::fg_dodgerblue2.test_fg();
	os  << ansi::fg_dodgerblue1.test_fg();
	os  << ansi::fg_green4.test_fg();
	os  << ansi::fg_springgreen6.test_fg();
	os  << ansi::fg_turquoise4.test_fg();
	os  << ansi::fg_deepskyblue3.test_fg();
	os  << ansi::fg_deepskyblue4.test_fg();
	os  << ansi::fg_dodgerblue.test_fg();
	os  << ansi::fg_green2.test_fg();
	os  << ansi::fg_springgreen4.test_fg();
	os  << ansi::fg_darkcyan.test_fg();
	os  << ansi::fg_lightseagreen.test_fg();
	os  << ansi::fg_deepskyblue2.test_fg();
	os  << ansi::fg_deepskyblue1.test_fg();
	os  << ansi::fg_green3.test_fg();
	os  << ansi::fg_springgreen5.test_fg();
	os  << ansi::fg_springgreen2.test_fg();
	os  << ansi::fg_cyan3.test_fg();
	os  << ansi::fg_darkturquoise.test_fg();
	os  << ansi::fg_turquoise2.test_fg();
	os  << ansi::fg_green1.test_fg();
	os  << ansi::fg_springgreen3.test_fg();
	os  << ansi::fg_springgreen1.test_fg();
	os  << ansi::fg_mediumspringgreen.test_fg();
	os  << ansi::fg_cyan2.test_fg();
	os  << ansi::fg_cyan1.test_fg();
	os  << ansi::fg_darkred.test_fg();
	os  << ansi::fg_deeppink5.test_fg();
	os  << ansi::fg_purple4.test_fg();
	os  << ansi::fg_purple5.test_fg();
	os  << ansi::fg_purple3.test_fg();
	os  << ansi::fg_blueviolet.test_fg();
	os  << ansi::fg_orange.test_fg();
	os  << ansi::fg_mediumpurple6.test_fg();
	os  << ansi::fg_slateblue2.test_fg();
	os  << ansi::fg_slateblue3.test_fg();
	os  << ansi::fg_royalblue1.test_fg();
	os  << ansi::fg_chartreuse6.test_fg();
	os  << ansi::fg_darkseagreen7.test_fg();
	os  << ansi::fg_paleturquoise4.test_fg();
	os  << ansi::fg_steelblue.test_fg();
	os  << ansi::fg_steelblue3.test_fg();
	os  << ansi::fg_cornflowerblue.test_fg();
	os  << ansi::fg_chartreuse4.test_fg();
	os  << ansi::fg_darkseagreen8.test_fg();
	os  << ansi::fg_cadetblue.test_fg();
	os  << ansi::fg_cadetblue1.test_fg();
	os  << ansi::fg_skyblue3.test_fg();
	os  << ansi::fg_steelblue1.test_fg();
	os  << ansi::fg_chartreuse5.test_fg();
	os  << ansi::fg_palegreen2.test_fg();
	os  << ansi::fg_seagreen3.test_fg();
	os  << ansi::fg_aquamarine2.test_fg();
	os  << ansi::fg_mediumturquoise.test_fg();
	os  << ansi::fg_steelblue2.test_fg();
	os  << ansi::fg_chartreuse2.test_fg();
	os  << ansi::fg_seagreen2.test_fg();
	os  << ansi::fg_seagreen.test_fg();
	os  << ansi::fg_seagreen1.test_fg();
	os  << ansi::fg_aquamarine.test_fg();
	os  << ansi::fg_darkslategray2.test_fg();
	os  << ansi::fg_darkred1.test_fg();
	os  << ansi::fg_deeppink6.test_fg();
	os  << ansi::fg_darkmagenta.test_fg();
	os  << ansi::fg_darkmagenta1.test_fg();
	os  << ansi::fg_darkviolet.test_fg();
	os  << ansi::fg_purple1.test_fg();
	os  << ansi::fg_orange4.test_fg();
	os  << ansi::fg_lightpink4.test_fg();
	os  << ansi::fg_plum4.test_fg();
	os  << ansi::fg_mediumpurple4.test_fg();
	os  << ansi::fg_mediumpurple5.test_fg();
	os  << ansi::fg_slateblue1.test_fg();
	os  << ansi::fg_yellow5.test_fg();
	os  << ansi::fg_wheat4.test_fg();
	os  << ansi::fg_lightslategrey.test_fg();
	os  << ansi::fg_mediumpurple.test_fg();
	os  << ansi::fg_lightslateblue.test_fg();
	os  << ansi::fg_yellow6.test_fg();
	os  << ansi::fg_darkolivegreen4.test_fg();
	os  << ansi::fg_darkseagreen.test_fg();
	os  << ansi::fg_lightskyblue2.test_fg();
	os  << ansi::fg_lightskyblue3.test_fg();
	os  << ansi::fg_skyblue2.test_fg();
	os  << ansi::fg_chartreuse3.test_fg();
	os  << ansi::fg_darkolivegreen5.test_fg();
	os  << ansi::fg_palegreen3.test_fg();
	os  << ansi::fg_darkseagreen5.test_fg();
	os  << ansi::fg_darkslategray3.test_fg();
	os  << ansi::fg_skyblue1.test_fg();
	os  << ansi::fg_chartreuse1.test_fg();
	os  << ansi::fg_lightgreen.test_fg();
	os  << ansi::fg_lightgreen1.test_fg();
	os  << ansi::fg_palegreen.test_fg();
	os  << ansi::fg_aquamarine1.test_fg();
	os  << ansi::fg_darkslategray1.test_fg();
	os  << ansi::fg_red2.test_fg();
	os  << ansi::fg_deeppink7.test_fg();
	os  << ansi::fg_mediumvioletred.test_fg();
	os  << ansi::fg_magenta3.test_fg();
	os  << ansi::fg_darkviolet1.test_fg();
	os  << ansi::fg_purple2.test_fg();
	os  << ansi::fg_darkorange1.test_fg();
	os  << ansi::fg_indianred.test_fg();
	os  << ansi::fg_hotpink3.test_fg();
	os  << ansi::fg_mediumorchid3.test_fg();
	os  << ansi::fg_mediumorchid.test_fg();
	os  << ansi::fg_mediumpurple2.test_fg();
	os  << ansi::fg_darkgoldenrod.test_fg();
	os  << ansi::fg_lightsalmon1.test_fg();
	os  << ansi::fg_rosybrown.test_fg();
	os  << ansi::fg_mediumpurple3.test_fg();
	os  << ansi::fg_mediumpurple1.test_fg();
	os  << ansi::fg_gold1.test_fg();
	os  << ansi::fg_darkkhaki.test_fg();
	os  << ansi::fg_navajowhite3.test_fg();
	os  << ansi::fg_lightsteelblue3.test_fg();
	os  << ansi::fg_lightsteelblue.test_fg();
	os  << ansi::fg_yellow3.test_fg();
	os  << ansi::fg_darkolivegreen6.test_fg();
	os  << ansi::fg_darkseagreen6.test_fg();
	os  << ansi::fg_darkseagreen3.test_fg();
	os  << ansi::fg_lightcyan3.test_fg();
	os  << ansi::fg_lightskyblue1.test_fg();
	os  << ansi::fg_greenyellow.test_fg();
	os  << ansi::fg_darkolivegreen2.test_fg();
	os  << ansi::fg_palegreen1.test_fg();
	os  << ansi::fg_darkseagreen4.test_fg();
	os  << ansi::fg_darkseagreen1.test_fg();
	os  << ansi::fg_paleturquoise1.test_fg();
	os  << ansi::fg_red3.test_fg();
	os  << ansi::fg_deeppink3.test_fg();
	os  << ansi::fg_deeppink4.test_fg();
	os  << ansi::fg_magenta4.test_fg();
	os  << ansi::fg_magenta5.test_fg();
	os  << ansi::fg_magenta1.test_fg();
	os  << ansi::fg_darkorange2.test_fg();
	os  << ansi::fg_indianred1.test_fg();
	os  << ansi::fg_hotpink4.test_fg();
	os  << ansi::fg_hotpink2.test_fg();
	os  << ansi::fg_orchid.test_fg();
	os  << ansi::fg_mediumorchid1.test_fg();
	os  << ansi::fg_orange3.test_fg();
	os  << ansi::fg_lightsalmon2.test_fg();
	os  << ansi::fg_lightpink3.test_fg();
	os  << ansi::fg_pink3.test_fg();
	os  << ansi::fg_plum3.test_fg();
	os  << ansi::fg_violet.test_fg();
	os  << ansi::fg_gold2.test_fg();
	os  << ansi::fg_lightgoldenrod5.test_fg();
	os  << ansi::fg_tan.test_fg();
	os  << ansi::fg_mistyrose3.test_fg();
	os  << ansi::fg_thistle3.test_fg();
	os  << ansi::fg_plum2.test_fg();
	os  << ansi::fg_yellow4.test_fg();
	os  << ansi::fg_khaki3.test_fg();
	os  << ansi::fg_lightgoldenrod2.test_fg();
	os  << ansi::fg_lightyellow3.test_fg();
	os  << ansi::fg_lightsteelblue1.test_fg();
	os  << ansi::fg_yellow2.test_fg();
	os  << ansi::fg_darkolivegreen.test_fg();
	os  << ansi::fg_darkolivegreen1.test_fg();
	os  << ansi::fg_darkseagreen2.test_fg();
	os  << ansi::fg_honeydew2.test_fg();
	os  << ansi::fg_lightcyan1.test_fg();
	os  << ansi::fg_red1.test_fg();
	os  << ansi::fg_deeppink2.test_fg();
	os  << ansi::fg_deeppink.test_fg();
	os  << ansi::fg_deeppink1.test_fg();
	os  << ansi::fg_magenta2.test_fg();
	os  << ansi::fg_magenta.test_fg();
	os  << ansi::fg_orangered1.test_fg();
	os  << ansi::fg_indianred2.test_fg();
	os  << ansi::fg_indianred3.test_fg();
	os  << ansi::fg_hotpink.test_fg();
	os  << ansi::fg_hotpink1.test_fg();
	os  << ansi::fg_mediumorchid2.test_fg();
	os  << ansi::fg_darkorange.test_fg();
	os  << ansi::fg_salmon1.test_fg();
	os  << ansi::fg_lightcoral.test_fg();
	os  << ansi::fg_palevioletred1.test_fg();
	os  << ansi::fg_orchid2.test_fg();
	os  << ansi::fg_orchid1.test_fg();
	os  << ansi::fg_orange1.test_fg();
	os  << ansi::fg_sandybrown.test_fg();
	os  << ansi::fg_lightsalmon.test_fg();
	os  << ansi::fg_lightpink1.test_fg();
	os  << ansi::fg_pink1.test_fg();
	os  << ansi::fg_plum1.test_fg();
	os  << ansi::fg_gold.test_fg();
	os  << ansi::fg_lightgoldenrod3.test_fg();
	os  << ansi::fg_lightgoldenrod4.test_fg();
	os  << ansi::fg_navajowhite1.test_fg();
	os  << ansi::fg_mistyrose1.test_fg();
	os  << ansi::fg_thistle1.test_fg();
	os  << ansi::fg_yellow1.test_fg();
	os  << ansi::fg_lightgoldenrod1.test_fg();
	os  << ansi::fg_khaki1.test_fg();
	os  << ansi::fg_wheat1.test_fg();
	os  << ansi::fg_cornsilk1.test_fg();
	os  << ansi::bg_red1.test_bg();
	os  << ansi::bg_lime.test_bg();
	os  << ansi::bg_yellow0.test_bg();
	os  << ansi::bg_blue0.test_bg();
	os  << ansi::bg_fuchsia.test_bg();
	os  << ansi::bg_aqua.test_bg();
	os  << ansi::bg_white.test_bg();
	os  << ansi::bg_navyblue.test_bg();
	os  << ansi::bg_darkblue.test_bg();
	os  << ansi::bg_blue2.test_bg();
	os  << ansi::bg_blue3.test_bg();
	os  << ansi::bg_blue1.test_bg();
	os  << ansi::bg_darkgreen.test_bg();
	os  << ansi::bg_deepskyblue5.test_bg();
	os  << ansi::bg_deepskyblue6.test_bg();
	os  << ansi::bg_deepskyblue7.test_bg();
	os  << ansi::bg_dodgerblue2.test_bg();
	os  << ansi::bg_dodgerblue1.test_bg();
	os  << ansi::bg_green4.test_bg();
	os  << ansi::bg_springgreen6.test_bg();
	os  << ansi::bg_turquoise4.test_bg();
	os  << ansi::bg_deepskyblue3.test_bg();
	os  << ansi::bg_deepskyblue4.test_bg();
	os  << ansi::bg_dodgerblue.test_bg();
	os  << ansi::bg_green2.test_bg();
	os  << ansi::bg_springgreen4.test_bg();
	os  << ansi::bg_darkcyan.test_bg();
	os  << ansi::bg_lightseagreen.test_bg();
	os  << ansi::bg_deepskyblue2.test_bg();
	os  << ansi::bg_deepskyblue1.test_bg();
	os  << ansi::bg_green3.test_bg();
	os  << ansi::bg_springgreen5.test_bg();
	os  << ansi::bg_springgreen2.test_bg();
	os  << ansi::bg_cyan3.test_bg();
	os  << ansi::bg_darkturquoise.test_bg();
	os  << ansi::bg_turquoise2.test_bg();
	os  << ansi::bg_green1.test_bg();
	os  << ansi::bg_springgreen3.test_bg();
	os  << ansi::bg_springgreen1.test_bg();
	os  << ansi::bg_mediumspringgreen.test_bg();
	os  << ansi::bg_cyan2.test_bg();
	os  << ansi::bg_cyan1.test_bg();
	os  << ansi::bg_darkred.test_bg();
	os  << ansi::bg_deeppink5.test_bg();
	os  << ansi::bg_purple4.test_bg();
	os  << ansi::bg_purple5.test_bg();
	os  << ansi::bg_purple3.test_bg();
	os  << ansi::bg_blueviolet.test_bg();
	os  << ansi::bg_orange.test_bg();
	os  << ansi::bg_mediumpurple6.test_bg();
	os  << ansi::bg_slateblue2.test_bg();
	os  << ansi::bg_slateblue3.test_bg();
	os  << ansi::bg_royalblue1.test_bg();
	os  << ansi::bg_chartreuse6.test_bg();
	os  << ansi::bg_darkseagreen7.test_bg();
	os  << ansi::bg_paleturquoise4.test_bg();
	os  << ansi::bg_steelblue.test_bg();
	os  << ansi::bg_steelblue3.test_bg();
	os  << ansi::bg_cornflowerblue.test_bg();
	os  << ansi::bg_chartreuse4.test_bg();
	os  << ansi::bg_darkseagreen8.test_bg();
	os  << ansi::bg_cadetblue.test_bg();
	os  << ansi::bg_cadetblue1.test_bg();
	os  << ansi::bg_skyblue3.test_bg();
	os  << ansi::bg_steelblue1.test_bg();
	os  << ansi::bg_chartreuse5.test_bg();
	os  << ansi::bg_palegreen2.test_bg();
	os  << ansi::bg_seagreen3.test_bg();
	os  << ansi::bg_aquamarine2.test_bg();
	os  << ansi::bg_mediumturquoise.test_bg();
	os  << ansi::bg_steelblue2.test_bg();
	os  << ansi::bg_chartreuse2.test_bg();
	os  << ansi::bg_seagreen2.test_bg();
	os  << ansi::bg_seagreen.test_bg();
	os  << ansi::bg_seagreen1.test_bg();
	os  << ansi::bg_aquamarine.test_bg();
	os  << ansi::bg_darkslategray2.test_bg();
	os  << ansi::bg_darkred1.test_bg();
	os  << ansi::bg_deeppink6.test_bg();
	os  << ansi::bg_darkmagenta.test_bg();
	os  << ansi::bg_darkmagenta1.test_bg();
	os  << ansi::bg_darkviolet.test_bg();
	os  << ansi::bg_purple1.test_bg();
	os  << ansi::bg_orange4.test_bg();
	os  << ansi::bg_lightpink4.test_bg();
	os  << ansi::bg_plum4.test_bg();
	os  << ansi::bg_mediumpurple4.test_bg();
	os  << ansi::bg_mediumpurple5.test_bg();
	os  << ansi::bg_slateblue1.test_bg();
	os  << ansi::bg_yellow5.test_bg();
	os  << ansi::bg_wheat4.test_bg();
	os  << ansi::bg_lightslategrey.test_bg();
	os  << ansi::bg_mediumpurple.test_bg();
	os  << ansi::bg_lightslateblue.test_bg();
	os  << ansi::bg_yellow6.test_bg();
	os  << ansi::bg_darkolivegreen4.test_bg();
	os  << ansi::bg_darkseagreen.test_bg();
	os  << ansi::bg_lightskyblue2.test_bg();
	os  << ansi::bg_lightskyblue3.test_bg();
	os  << ansi::bg_skyblue2.test_bg();
	os  << ansi::bg_chartreuse3.test_bg();
	os  << ansi::bg_darkolivegreen5.test_bg();
	os  << ansi::bg_palegreen3.test_bg();
	os  << ansi::bg_darkseagreen5.test_bg();
	os  << ansi::bg_darkslategray3.test_bg();
	os  << ansi::bg_skyblue1.test_bg();
	os  << ansi::bg_chartreuse1.test_bg();
	os  << ansi::bg_lightgreen.test_bg();
	os  << ansi::bg_lightgreen1.test_bg();
	os  << ansi::bg_palegreen.test_bg();
	os  << ansi::bg_aquamarine1.test_bg();
	os  << ansi::bg_darkslategray1.test_bg();
	os  << ansi::bg_red2.test_bg();
	os  << ansi::bg_deeppink7.test_bg();
	os  << ansi::bg_mediumvioletred.test_bg();
	os  << ansi::bg_magenta3.test_bg();
	os  << ansi::bg_darkviolet1.test_bg();
	os  << ansi::bg_purple2.test_bg();
	os  << ansi::bg_darkorange1.test_bg();
	os  << ansi::bg_indianred.test_bg();
	os  << ansi::bg_hotpink3.test_bg();
	os  << ansi::bg_mediumorchid3.test_bg();
	os  << ansi::bg_mediumorchid.test_bg();
	os  << ansi::bg_mediumpurple2.test_bg();
	os  << ansi::bg_darkgoldenrod.test_bg();
	os  << ansi::bg_lightsalmon1.test_bg();
	os  << ansi::bg_rosybrown.test_bg();
	os  << ansi::bg_mediumpurple3.test_bg();
	os  << ansi::bg_mediumpurple1.test_bg();
	os  << ansi::bg_gold1.test_bg();
	os  << ansi::bg_darkkhaki.test_bg();
	os  << ansi::bg_navajowhite3.test_bg();
	os  << ansi::bg_lightsteelblue3.test_bg();
	os  << ansi::bg_lightsteelblue.test_bg();
	os  << ansi::bg_yellow3.test_bg();
	os  << ansi::bg_darkolivegreen6.test_bg();
	os  << ansi::bg_darkseagreen6.test_bg();
	os  << ansi::bg_darkseagreen3.test_bg();
	os  << ansi::bg_lightcyan3.test_bg();
	os  << ansi::bg_lightskyblue1.test_bg();
	os  << ansi::bg_greenyellow.test_bg();
	os  << ansi::bg_darkolivegreen2.test_bg();
	os  << ansi::bg_palegreen1.test_bg();
	os  << ansi::bg_darkseagreen4.test_bg();
	os  << ansi::bg_darkseagreen1.test_bg();
	os  << ansi::bg_paleturquoise1.test_bg();
	os  << ansi::bg_red3.test_bg();
	os  << ansi::bg_deeppink3.test_bg();
	os  << ansi::bg_deeppink4.test_bg();
	os  << ansi::bg_magenta4.test_bg();
	os  << ansi::bg_magenta5.test_bg();
	os  << ansi::bg_magenta1.test_bg();
	os  << ansi::bg_darkorange2.test_bg();
	os  << ansi::bg_indianred1.test_bg();
	os  << ansi::bg_hotpink4.test_bg();
	os  << ansi::bg_hotpink2.test_bg();
	os  << ansi::bg_orchid.test_bg();
	os  << ansi::bg_mediumorchid1.test_bg();
	os  << ansi::bg_orange3.test_bg();
	os  << ansi::bg_lightsalmon2.test_bg();
	os  << ansi::bg_lightpink3.test_bg();
	os  << ansi::bg_pink3.test_bg();
	os  << ansi::bg_plum3.test_bg();
	os  << ansi::bg_violet.test_bg();
	os  << ansi::bg_gold2.test_bg();
	os  << ansi::bg_lightgoldenrod5.test_bg();
	os  << ansi::bg_tan.test_bg();
	os  << ansi::bg_mistyrose3.test_bg();
	os  << ansi::bg_thistle3.test_bg();
	os  << ansi::bg_plum2.test_bg();
	os  << ansi::bg_yellow4.test_bg();
	os  << ansi::bg_khaki3.test_bg();
	os  << ansi::bg_lightgoldenrod2.test_bg();
	os  << ansi::bg_lightyellow3.test_bg();
	os  << ansi::bg_lightsteelblue1.test_bg();
	os  << ansi::bg_yellow2.test_bg();
	os  << ansi::bg_darkolivegreen.test_bg();
	os  << ansi::bg_darkolivegreen1.test_bg();
	os  << ansi::bg_darkseagreen2.test_bg();
	os  << ansi::bg_honeydew2.test_bg();
	os  << ansi::bg_lightcyan1.test_bg();
	os  << ansi::bg_red1.test_bg();
	os  << ansi::bg_deeppink2.test_bg();
	os  << ansi::bg_deeppink.test_bg();
	os  << ansi::bg_deeppink1.test_bg();
	os  << ansi::bg_magenta2.test_bg();
	os  << ansi::bg_magenta.test_bg();
	os  << ansi::bg_orangered1.test_bg();
	os  << ansi::bg_indianred2.test_bg();
	os  << ansi::bg_indianred3.test_bg();
	os  << ansi::bg_hotpink.test_bg();
	os  << ansi::bg_hotpink1.test_bg();
	os  << ansi::bg_mediumorchid2.test_bg();
	os  << ansi::bg_darkorange.test_bg();
	os  << ansi::bg_salmon1.test_bg();
	os  << ansi::bg_lightcoral.test_bg();
	os  << ansi::bg_palevioletred1.test_bg();
	os  << ansi::bg_orchid2.test_bg();
	os  << ansi::bg_orchid1.test_bg();
	os  << ansi::bg_orange1.test_bg();
	os  << ansi::bg_sandybrown.test_bg();
	os  << ansi::bg_lightsalmon.test_bg();
	os  << ansi::bg_lightpink1.test_bg();
	os  << ansi::bg_pink1.test_bg();
	os  << ansi::bg_plum1.test_bg();
	os  << ansi::bg_gold.test_bg();
	os  << ansi::bg_lightgoldenrod3.test_bg();
	os  << ansi::bg_lightgoldenrod4.test_bg();
	os  << ansi::bg_navajowhite1.test_bg();
	os  << ansi::bg_mistyrose1.test_bg();
	os  << ansi::bg_thistle1.test_bg();
	os  << ansi::bg_yellow1.test_bg();
	os  << ansi::bg_lightgoldenrod1.test_bg();
	os  << ansi::bg_khaki1.test_bg();
	os  << ansi::bg_wheat1.test_bg();
	os  << ansi::bg_cornsilk1.test_bg();
}
} // namespace ansi
