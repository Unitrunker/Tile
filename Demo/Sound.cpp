#include "stdafx.h"
#include "Sound.h"
#include "../Tile/Combo.h"

/*
Copyright © 2012 Rick Parrish
*/

using namespace Tiles;

// Audio recording device
SoundSet::SoundSet(Theme &theme) : 
	SetT<Sound>(NULL),
	_port(*this, &Sound::_port),
	_rate(*this, &Sound::_rate),
	_channels(*this, &Sound::_channels),
	_channel(*this, &Sound::_channel),
	_bits(*this, &Sound::_bits),
	Rate(&_rate),
	Channels(&_channels),
	Bits(&_bits)
{
	Section *section = NULL;
	Property *prop = NULL;
	Theme::Font textFont = {Theme::eText, theme.Text};
	std::vector<Combo::Item> list;

	static Combo::Item listRates[] = 
	{
		{_T("96000"), 96000L},
		{_T("48000"), 48000L},
		{_T("44100"), 44100L}
	};

	static Combo::Item listChannels[] = 
	{
		{_T("Mono"), 1},
		{_T("Stereo"), 2}
	};

	static Combo::Item listBits[] = 
	{
		{_T("8"), 8L},
		{_T("16"), 16L},
		{_T("24"), 24L},
		{_T("32"), 32L}
	};

	section = new Section(_T("Audio Input"), _T("Audio recording device"));

	for (size_t i = 0; i < _countof(listRates); i++)
		list.push_back(listRates[i]);
	prop = new Property(_T("Rate"), _T("Sample rate"), new Combo(0, theme, textFont, list, &Rate) );
	section->Items.push_back(prop);
	list.clear();

	for (size_t i = 0; i < _countof(listChannels); i++)
		list.push_back(listChannels[i]);
	prop = new Property(_T("Channels"), _T("Number of channels"), new Combo(0, theme, textFont, list, &Channels) );
	section->Items.push_back(prop);
	list.clear();

	for (size_t i = 0; i < _countof(listBits); i++)
		list.push_back(listBits[i]);
	prop = new Property(_T("Bits"), _T("Bits per audio sample"), new Combo(0, theme, textFont, list, &Bits) );
	section->Items.push_back(prop);
	list.clear();

	Add(section);
}
