#include "Assets.h"
#include "Ember.h"
#include "Logger.h"

namespace Ember {
	bool InitializeImageLoader() {
		int imgFlags = IMG_INIT_PNG;

		if (!(IMG_Init(imgFlags) & imgFlags))
			return false;

		return true;
	}

	bool InitializeSoundLoader() {
		Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

		return true;
	}

	bool InitializeAssets() {
		return (InitializeImageLoader() && InitializeSoundLoader());
	}

	void AssetCleanUp() {
		IMG_Quit();
		Mix_Quit();
	}
}