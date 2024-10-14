#pragma once


class Color
{
public:


	Color(int r, int g, int b, int a)
	{
		iR = r;
		iG = g;
		iB = b;
		iA = a;
	}

	int iR, iG, iB, iA{};
};