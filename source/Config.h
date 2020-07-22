#pragma once

namespace Falcon {
	struct Config {
		Vec2<UINT>	Resolution;
		bool		MSAA_Enabled;
		ScreenMode	SM;
		ApiType		API;
	};
}