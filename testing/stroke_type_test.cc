#include "common.hh"

#include "game/guitar/stroke_type.hh"

TEST(UnitTest_StrokeType, parseStrokes_empty) {
	EXPECT_THAT(parseStrokes(""), ElementsAre());
}

TEST(UnitTest_StrokeType, parseStrokes_up_down) {
	EXPECT_THAT(parseStrokes("u d u d"), ElementsAre(StrokeType::Up, StrokeType::Down, StrokeType::Up, StrokeType::Down));
}

TEST(UnitTest_StrokeType, parseStrokes_up_down_pause) {
	EXPECT_THAT(parseStrokes("u d - d"), ElementsAre(StrokeType::Up, StrokeType::Down, StrokeType::Pause, StrokeType::Down));
}

TEST(UnitTest_StrokeType, parseStrokes_up_down_pause_and) {
	EXPECT_THAT(parseStrokes("u & d & - & d"), 
		ElementsAre(StrokeType::Up, StrokeType::Pause, StrokeType::Down, StrokeType::Pause, StrokeType::Pause, StrokeType::Pause, StrokeType::Down));
}

TEST(UnitTest_StrokeType, parseStrokes_strings) {
	EXPECT_THAT(parseStrokes("E A D G B e"), 
		ElementsAre(StrokeType::StringE, StrokeType::StringA, StrokeType::StringD, StrokeType::StringG, StrokeType::StringB, StrokeType::StringHighE));
}

TEST(UnitTest_StrokeType, parseStrokes_strings_up_down) {
	EXPECT_THAT(parseStrokes("E A D G B e u d"), 
		ElementsAre(StrokeType::StringE, StrokeType::StringA, StrokeType::StringD, StrokeType::StringG, StrokeType::StringB, StrokeType::StringHighE, StrokeType::Up, StrokeType::Down));
}

TEST(UnitTest_StrokeType, parseStrokes_ghost_note) {
	EXPECT_THAT(parseStrokes("x"), ElementsAre(StrokeType::Ghost));
}

TEST(UnitTest_StrokeType, parseStrokes_palm_mute) {
	EXPECT_THAT(parseStrokes("p"), ElementsAre(StrokeType::PalmMute));
}

TEST(UnitTest_StrokeType, parseStrokes_strings_compact) {
	EXPECT_THAT(parseStrokes("EADGBe"), 
		ElementsAre(StrokeType::StringE, StrokeType::StringA, StrokeType::StringD, StrokeType::StringG, StrokeType::StringB, StrokeType::StringHighE));
}

TEST(UnitTest_StrokeType, parseStrokes_ignore_unknown) {
	EXPECT_THAT(parseStrokes("agbhyz"),  ElementsAre());
}
