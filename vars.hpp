#pragma once
namespace Globals {
	struct myGlobals {
		float aimbotFOV = 2.0f;
		float aimbotSmooth = 2.5f;
		float aimbotSmoothRand = 2.5f;
		float aimbotOver = 0.05f;
		float RCSSmoothRand = 0.1f;
		bool bRadar = false, bESP = false, bMenu = true, bRCSAimbot = true, bScoreWall = false;
		int boneIndex = 7;

		void restoreValues() {
		aimbotFOV = 2.0f;
		aimbotSmooth = 2.5f;
		aimbotSmoothRand = 2.5f;
		aimbotOver = 0.05f;
		RCSSmoothRand = 0.1f;
		bRadar = false;
		bESP = false;
		bMenu = true;
		bRCSAimbot = true;
		bScoreWall = false;
		boneIndex = 7;
		}
	};
	
}