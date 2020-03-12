#pragma once
namespace Globals {
	struct myGlobals {
		float aimbotFOV = 3.0f;
		float aimbotSmooth = 4.0f;
		float aimbotSmoothRand = 2.5f;
		float aimbotOver = 0.14f;
		float RCSSmoothRand = 0.1f;
		bool bRadar = false, bESP = false, bMenu = true, bRCSAimbot = true;
		int boneIndex = 7;

		void restoreValues() {
		aimbotFOV = 3.0f;
		aimbotSmooth = 4.0f;
		aimbotSmoothRand = 2.5f;
		aimbotOver = 0.14f;
		RCSSmoothRand = 0.1f;
		bRadar = false;
		bESP = false;
		bMenu = true;
		bRCSAimbot = true;
		boneIndex = 7;
		}
	};
	
}