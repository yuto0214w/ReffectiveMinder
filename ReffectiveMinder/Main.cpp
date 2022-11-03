#include <Siv3D.hpp>  // OpenSiv3D v0.6.5

struct Game {
	String title;
	const char32 *description, *id;
	std::pair<bool (*)(), bool (*)()> game;
	Duration time;
	bool darkMode;
	int score;
	String result;
	const Texture *preview;
};
#include "KazuateGame.hpp"
#include "QuizGame.hpp"
#include "FlashMentalMath.hpp"

Array<Game *> games;

void Main() {
	Window::SetTitle(U"ReffectiveMinder");
	System::SetTerminationTriggers(UserAction::CloseButtonClicked);
	ScreenCapture::SetShortcutKeys({});
	Scene::SetBackground(Color{0xfa, 0xfa, 0xfa});

	const Font biggestFont{200};
	const Font biggerFont{100};
	const Font bigFont{60};
	const Font biggerMediumFont{50};
	const Font mediumFont{40};
	const Font smallerMediumFont{30};
	const Font smallFont{20};
	const Font smallEmojiFont{20, Typeface::ColorEmoji};
	smallFont.addFallback(smallEmojiFont);

	Games::Kazuate::font = &mediumFont;
	games << &Games::Kazuate::game;
	Games::Quiz::font = &smallerMediumFont;
	games << &Games::Quiz::game;
	Games::FlashMentalMath::biggerFont = &biggestFont;
	Games::FlashMentalMath::bigFont = &biggerFont;
	Games::FlashMentalMath::mediumFont = &mediumFont;
	games << &Games::FlashMentalMath::game;

	for (auto game : games) {
		game->preview = new const Texture{Format(U"screenshots/", game->id, U".png")};
	}

	// Splash screen
#ifdef NDEBUG
	{
		Stopwatch splashStopwatch{StartImmediately::Yes};
		while (System::Update()) {
			if (splashStopwatch <= 3.5s) {
				if (splashStopwatch <= 1s) bigFont(U"ReMinder").drawAt(Scene::Center(), Palette::Black);
				else if (splashStopwatch <= 1.4s) bigFont(U"RefMinder").drawAt(Scene::Center(), Palette::Black);
				else if (splashStopwatch <= 1.5s) bigFont(U"ReffMinder").drawAt(Scene::Center(), Palette::Black);
				else if (splashStopwatch <= 1.6s) bigFont(U"ReffeMinder").drawAt(Scene::Center(), Palette::Black);
				else if (splashStopwatch <= 1.7s) bigFont(U"ReffecMinder").drawAt(Scene::Center(), Palette::Black);
				else if (splashStopwatch <= 1.8s) bigFont(U"ReffectMinder").drawAt(Scene::Center(), Palette::Black);
				else if (splashStopwatch <= 1.9s) bigFont(U"ReffectiMinder").drawAt(Scene::Center(), Palette::Black);
				else if (splashStopwatch <= 2s) bigFont(U"ReffectivMinder").drawAt(Scene::Center(), Palette::Black);
				else if (splashStopwatch <= 3s) bigFont(U"ReffectiveMinder").drawAt(Scene::Center(), Palette::Black);
			} else {
				splashStopwatch.reset();
				break;
			}
		}
	}
#endif

	// S
	const Triangle arrowLeft{25, 300, 50, 250, 50, 350};
	const RoundRect gameLeft{75, 100, 200, 450, 10};
	const RoundRect gameMiddle{300, 100, 200, 450, 10};
	const RoundRect gameRight{525, 100, 200, 450, 10};
	const Triangle arrowRight{775, 300, 750, 250, 750, 350};

	// ER
	const RoundRect gameBack{50, 470, 325, 80, 10};
	const RoundRect gameStart{425, 470, 325, 80, 10};

	// M
	const RoundRect gameContinue{240, 190, 320, 80, 10};
	const RoundRect gameRetry{240, 290, 320, 80, 10};
	const RoundRect gameCancel{240, 390, 320, 80, 10};

	const Color materialBlue{0x44, 0x8a, 0xff};
	const Color materialGreen{0x00, 0xc8, 0x53};
	const Color materialRed{0xff, 0x52, 0x52};
	const Color materialYellow{0xff, 0xea, 0x00};
	const Color materialDarkblue{0x05, 0x4b, 0xc0};
	const Color materialDarkgreen{0x00, 0x89, 0x14};
	const Color materialDarkred{0xc0, 0x13, 0x13};
	const Color materialDarkyellow{0xc0, 0xab, 0x00};

	Stopwatch countdownStopwatch{StartImmediately::No};
	Stopwatch gameStopwatch{StartImmediately::No};
	double darkModeTransitionTime = 0.0, resultSceneTransitionTime = 0.0;

	/*
	  scene_id = 0; game select
		  scene_id = 1; game explanation
		  scene_id = 2; game start countdown
		  scene_id = 3; game
		  scene_id = 4; game result
	*/

	uint8 scene_id = 0;
	size_t gameleft_idx = 0, game_id = 0;
	bool game_start = false;
	while (System::Update()) {
		switch (scene_id) {
			case 0: {
				bigFont(U"ゲーム選択").draw(50, 10, Palette::Black);

				arrowLeft.mouseOver() ? arrowLeft.draw(Palette::Black) : arrowLeft.drawFrame(1.0, Palette::Black);
				gameLeft.mouseOver()
					? gameLeft.draw((MouseL.pressed() || KeyEnter.pressed()) ? materialDarkblue : materialBlue)
					: gameLeft.drawFrame(1.0, Palette::Blue);
				gameMiddle.mouseOver()
					? gameMiddle.draw((MouseL.pressed() || KeyEnter.pressed()) ? materialDarkgreen : materialGreen)
					: gameMiddle.drawFrame(1.0, Palette::Green);
				gameRight.mouseOver()
					? gameRight.draw((MouseL.pressed() || KeyEnter.pressed()) ? materialDarkred : materialRed)
					: gameRight.drawFrame(1.0, Palette::Red);
				arrowRight.mouseOver() ? arrowRight.draw(Palette::Black) : arrowRight.drawFrame(1.0, Palette::Black);

				mediumFont(games[gameleft_idx]->title)
					.drawAt(gameLeft.center(), gameLeft.mouseOver() ? Palette::White : Palette::Black);
				mediumFont(games[gameleft_idx + 1]->title)
					.drawAt(gameMiddle.center(), gameMiddle.mouseOver() ? Palette::White : Palette::Black);
				mediumFont(games[gameleft_idx + 2]->title)
					.drawAt(gameRight.center(), gameRight.mouseOver() ? Palette::White : Palette::Black);

				if (arrowLeft.leftClicked() && gameleft_idx != 0) {
					gameleft_idx--;
				} else if (gameLeft.leftReleased() || (gameLeft.mouseOver() && KeyEnter.up())) {
					scene_id = 1;
					game_id = 0 + gameleft_idx;
				} else if (gameMiddle.leftReleased() || (gameMiddle.mouseOver() && KeyEnter.up())) {
					scene_id = 1;
					game_id = 1 + gameleft_idx;
				} else if (gameRight.leftReleased() || (gameRight.mouseOver() && KeyEnter.up())) {
					scene_id = 1;
					game_id = 2 + gameleft_idx;
				} else if (arrowRight.leftClicked() && games.size() - 3 > gameleft_idx) {
					gameleft_idx++;
				}
				break;
			}
			case 1: {
				if (!game_start) {
					games[game_id]->preview->draw(0, 0, ColorF{Palette::White, 0.5});
					bigFont(games[game_id]->title.replaced(U" ", U"").replaced(U"\n", U""))
						.draw(50, 10, Palette::Black);
					RoundRect{50, 100, 700, 325, 10}.draw(Palette::White).drawFrame(2, Palette::Black);
					smallFont(Format(games[game_id]->description, U"\n\n制限時間: ", games[game_id]->time))
						.draw(Rect{65, 115, 670, 295}, Palette::Black);

					gameBack.mouseOver() ? gameBack.draw(Palette::Black)
										 : gameBack.draw(Palette::White).drawFrame(2, Palette::Black);
					gameStart.mouseOver()
						? gameStart.draw((MouseL.pressed() || KeyEnter.pressed()) ? materialDarkgreen : materialGreen)
						: gameStart.draw(Palette::White).drawFrame(2, materialGreen);

					smallerMediumFont(U"メニューに戻る")
						.drawAt(gameBack.center(), gameBack.mouseOver() ? Palette::White : Palette::Black);
					smallerMediumFont(U"ゲームスタート")
						.drawAt(gameStart.center(), gameStart.mouseOver() ? Palette::White : Palette::Black);

					if (gameBack.leftReleased() || (gameBack.mouseOver() && KeyEnter.up())) scene_id = 0;
					else if (gameStart.leftReleased() || (gameStart.mouseOver() && KeyEnter.up())) {
						if (games[game_id]->darkMode) darkModeTransitionTime = 0.25;
						game_start = true;
					}
				} else {
					if (games[game_id]->darkMode) {
						if (darkModeTransitionTime == 0.25) {
							darkModeTransitionTime -= Scene::DeltaTime();
							Scene::SetBackground(Color{0x21, 0x21, 0x21});
							Rect{0, 0, Scene::Size()}.draw(Color{0xfa, 0xfa, 0xfa});
							break;
						} else if ((darkModeTransitionTime -= Scene::DeltaTime()) >= 0) {
							Rect{0, 0, Scene::Size()}.draw(ColorF{Color{0xfa, 0xfa, 0xfa}, darkModeTransitionTime * 4});
							break;
						}
					}
					games[game_id]->game.first();
#ifdef NDEBUG
					countdownStopwatch.start();
#endif
					game_start = false;
					scene_id = 2;
				}
				break;
			}
			case 2: {
#ifndef NDEBUG
				gameStopwatch.start();
				scene_id = 3;
#else
				if (countdownStopwatch <= 1s)
					biggerFont(U"3").drawAt(Scene::Center(),
											games[game_id]->darkMode ? Palette::White : Palette::Black);
				else if (countdownStopwatch <= 2s)
					biggerFont(U"2").drawAt(Scene::Center(),
											games[game_id]->darkMode ? Palette::White : Palette::Black);
				else if (countdownStopwatch <= 3s)
					biggerFont(U"1").drawAt(Scene::Center(),
											games[game_id]->darkMode ? Palette::White : Palette::Black);
				else {
					countdownStopwatch.reset();
					gameStopwatch.start();
					scene_id = 3;
				}
#endif
				break;
			}
			case 3: {
				if (gameStopwatch <= games[game_id]->time) {
#ifndef NDEBUG
				}
				{
#endif
					if (KeyEscape.down() ||
						SimpleGUI::Button(U"ポーズ", Vec2{675, 15}, unspecified, !gameStopwatch.isPaused()))
						gameStopwatch.isPaused() ? gameStopwatch.resume() : gameStopwatch.pause();
					if (gameStopwatch.isPaused()) {
						Rect{0, 0, Scene::Size()}.draw(Color{0, 0, 0, 127});
						RoundRect{200, 100, 400, 400, 10}.draw();
						mediumFont(U"ポーズ中").drawAt(Scene::Center() - Point{0, 150}, Palette::Black);

						gameContinue.mouseOver()
							? gameContinue.draw((MouseL.pressed() || KeyEnter.pressed()) ? materialDarkgreen
																						 : materialGreen)
							: gameContinue.drawFrame(2, materialGreen);
						gameRetry.mouseOver()
							? gameRetry.draw((MouseL.pressed() || KeyEnter.pressed()) ? materialDarkyellow
																					  : materialYellow)
							: gameRetry.drawFrame(2, materialYellow);
						gameCancel.mouseOver()
							? gameCancel.draw((MouseL.pressed() || KeyEnter.pressed()) ? materialDarkred : materialRed)
							: gameCancel.drawFrame(2, materialRed);

						smallerMediumFont(U"続ける").drawAt(gameContinue.center(),
															gameContinue.mouseOver() ? Palette::White : Palette::Black);
						smallerMediumFont(U"リトライ")
							.drawAt(gameRetry.center(), gameRetry.mouseOver() ? Palette::White : Palette::Black);
						smallerMediumFont(U"メニューに戻る")
							.drawAt(gameCancel.center(), gameCancel.mouseOver() ? Palette::White : Palette::Black);

						if (gameContinue.leftReleased() || (gameContinue.mouseOver() && KeyEnter.up())) {
#ifdef NDEBUG
							countdownStopwatch.start();
#endif
							scene_id = 2;
						} else if (gameRetry.leftReleased() || (gameRetry.mouseOver() && KeyEnter.up())) {
							ClearPrint();
							gameStopwatch.reset();
							games[game_id]->game.first();
#ifdef NDEBUG
							countdownStopwatch.start();
#endif
							scene_id = 2;
						} else if (gameCancel.leftReleased() || (gameCancel.mouseOver() && KeyEnter.up())) {
							ClearPrint();
							gameStopwatch.reset();
							if (games[game_id]->darkMode) Scene::SetBackground(Color{0xfa, 0xfa, 0xfa});
							scene_id = 0;
						}
						break;
					} else {
						ClearPrint();
						Print << gameStopwatch;
						if (games[game_id]->game.second()) break;
					}
				}
				ClearPrint();
				gameStopwatch.reset();
				if (games[game_id]->darkMode) Scene::SetBackground(Color{0xfa, 0xfa, 0xfa});
				resultSceneTransitionTime = 1.25;
				scene_id = 4;
				break;
			}
			case 4: {
				if ((resultSceneTransitionTime -= Scene::DeltaTime()) >= 0.25) {
					biggerFont(U"終了！").drawAt(Scene::Center(), Palette::Black);
					break;
				}
				bigFont(U"結果発表").draw(50, 10, Palette::Black);

				biggerMediumFont(Format(U"点数は ", games[game_id]->score, U" 点でした！"))
					.draw(50, 150, Palette::Black);
				smallerMediumFont(Format(games[game_id]->result, U"\n遊んだ日: ", Date::Today()))
					.draw(50, 300, Palette::Black);

				gameBack.mouseOver() ? gameBack.draw(Palette::Black)
									 : gameBack.draw(Palette::White).drawFrame(2, Palette::Black);
				gameStart.mouseOver()
					? gameStart.draw((MouseL.pressed() || KeyEnter.pressed()) ? materialDarkgreen : materialGreen)
					: gameStart.draw(Palette::White).drawFrame(2, materialGreen);

				smallerMediumFont(U"メニューに戻る")
					.drawAt(gameBack.center(), gameBack.mouseOver() ? Palette::White : Palette::Black);
				smallerMediumFont(U"もう一回遊ぶ")
					.drawAt(gameStart.center(), gameStart.mouseOver() ? Palette::White : Palette::Black);

				if (gameBack.leftReleased() || (gameBack.mouseOver() && KeyEnter.up())) scene_id = 0;
				else if (gameStart.leftReleased() || (gameStart.mouseOver() && KeyEnter.up())) {
					if (games[game_id]->darkMode) darkModeTransitionTime = 0.25;
					game_start = true;
					scene_id = 1;
				}

				if ((resultSceneTransitionTime -= Scene::DeltaTime()) > 0) {
					Rect{0, 0, Scene::Size()}.draw(ColorF{Color{0xfa, 0xfa, 0xfa}, resultSceneTransitionTime * 4});
				}
				break;
			}
			default: {
				scene_id = 0;
			}
		}
	}

	for (auto game : games) {
		delete game->preview;
	}
	if (Games::Quiz::buttons) delete Games::Quiz::buttons;
}
