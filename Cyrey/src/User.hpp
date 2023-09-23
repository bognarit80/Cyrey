#ifndef _CYREY_USER_HEADER
#define _CYREY_USER_HEADER

namespace Cyrey
{
	//User preferences. Changes in settings or through keyboard shortcuts, if I decide to keep them.
	class User
	{
	public:
		float mMusicVolume;
		float mSoundVolume;
		float mSwapDeadZone; //percentage of tile size before the drag turns into a swap
		bool mWantBoardSwerve;
		float mQueueSwapTolerance;

		//Returns a user with defaults initialized
		User() :
			mMusicVolume(cMusicVolume),
			mSoundVolume(cSoundVolume),
			mSwapDeadZone(cSwapDeadZone),
			mWantBoardSwerve(cWantBoardSwerve),
			mQueueSwapTolerance(cQueueSwapTolerance) {};

	private:
		//default consts
		static constexpr float cMusicVolume = 0.80f;
		static constexpr float cSoundVolume = 0.90f;
		static constexpr float cSwapDeadZone = 0.33f;
		static constexpr bool cWantBoardSwerve = true;
		static constexpr float cQueueSwapTolerance = 0.15f;
	};
}

#endif // !_CYREY_USER_HEADER
