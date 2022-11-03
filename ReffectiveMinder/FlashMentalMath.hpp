#pragma once

namespace Games {
namespace FlashMentalMath {
const Font *biggerFont, *bigFont, *mediumFont;
double transition;
Array<int> numbers;
std::u32string s, t;
uint8 correct, i, j, scene_id;
Game game = {
	U"フラッシュ\n     暗算",
	U"画面に一瞬だけ数が表示されます。その数を足していって、最後に足した数をキーボードで入力します。\n一番最初は3回数が表示されます。5回成功すると4回、10回成功すると5回になります。",
	U"flash-mental-math",
	std::make_pair(
		[] {
			game.result = U"正解した回数: 0/15";
			game.score = 0;
			transition = 0.0;
			numbers = {Random(9) + 1, Random(9) + 1, Random(9) + 1};
			s = Format(numbers.reduce1([](int a, int b) { return a + b; }));
			t = U"";
			correct = 0;
			i = 0;
			j = 0;
			scene_id = 0;
			return true;
		},
		[] {
			Print << game.result;
			if (i == 14) return false;
			switch (scene_id) {
				case 0: {
					(*mediumFont)(U"数が表示されるまで...").drawAt(Scene::Center() - Point{0, 150});
					if ((transition += Scene::DeltaTime()) <= 0.75) (*bigFont)(U"3").drawAt(Scene::Center());
					else if (transition <= 1.5) (*bigFont)(U"2").drawAt(Scene::Center());
					else if (transition <= 2.25) (*bigFont)(U"1").drawAt(Scene::Center());
					else if (transition >= 2.5) {
						transition = 0.0;
						scene_id++;
					}
					break;
				}
				case 1: {
					if (j < numbers.size()) {
						if ((transition += Scene::DeltaTime()) <= 0.9 - (0.06 * correct))
							(*biggerFont)(Format(numbers[j])).drawAt(Scene::Center());
						else if (transition >= 0.9 - (0.06 * correct) + 0.3) {
							transition = 0.0;
							j++;
						}
						break;
					}
					j = 0;
					scene_id++;
					break;
				}
				case 2: {
					(*mediumFont)(U"足した数はいくつ？").drawAt(Scene::Center() - Point{0, 150});
					(*bigFont)(t).drawAt(Scene::Center());
					if (t.size() < s.size()) {
						if (Key0.down() && t.size() != 0) t += U'0';
						else if (Key1.down()) t += U'1';
						else if (Key2.down()) t += U'2';
						else if (Key3.down()) t += U'3';
						else if (Key4.down()) t += U'4';
						else if (Key5.down()) t += U'5';
						else if (Key6.down()) t += U'6';
						else if (Key7.down()) t += U'7';
						else if (Key8.down()) t += U'8';
						else if (Key9.down()) t += U'9';
						else if (KeyBackspace.down() && t.size() != 0) t = t.substr(0, t.size() - 1);
						if (s == t) {
							correct++;
							game.result = Format(U"正解した回数: ", correct, U"/15");
							game.score += correct * 1000;
						}
					} else {
						if ((transition += Scene::DeltaTime()) <= 0.5) {
							if (s == t) Circle{Scene::Center(), 175}.drawFrame(50, Palette::Red);
							else Shape2D::Cross(225, 50, Scene::Center()).draw(Palette::Blue);
							break;
						}
						i++;
						transition = 0.0;
						scene_id++;
					}
					break;
				}
				default: {
					numbers = {Random(9) + 1, Random(9) + 1, Random(9) + 1};
					if (i > 5) numbers.push_back(Random(9) + 1);
					if (i > 10) numbers.push_back(Random(9) + 1);
					s = Format(numbers.reduce1([](int a, int b) { return a + b; }));
					t = U"";
					scene_id = 0;
				}
			}
			return true;
		}),
	90s,
	true};
}  // namespace FlashMentalMath
}  // namespace Games
