#pragma once

namespace Games {
namespace Kazuate {
const Font *font;
int index, fail, flag, y, x;
struct Balloon {
	int index;
	Vec2 pos;
	double red_time = 0.0;
};
Grid<Optional<Balloon>> balloons;
Game game = {
	U"数当て", U"画面に出てくるバルーンを、書いてある数順にクリックしていきます。", U"kazuate",
	std::make_pair(
		[] {
			game.result = U"成功した数: 0/60\n失敗した回数: 0";
			game.score = 0;
			index = 0;
			fail = 0;
			flag = 0;
			balloons = {{{}, {}, {}, {}, {}, {}, {}, {}},
						{{}, {}, {}, {}, {}, {}, {}, {}},
						{{}, {}, {}, {}, {}, {}, {}, {}},
						{{}, {}, {}, {}, {}, {}, {}, {}},
						{{}, {}, {}, {}, {}, {}, {}, {}}};
			for (int i = 0; i < 3; i++) {
			random:
				y = Random(balloons.height() - 1);
				x = Random(balloons.width() - 1);
				if (balloons[y][x]) goto random;
				else balloons[y][x] = Balloon{i, Vec2{(x * 100) + 50, (y * 100) + 150}};
			}
			return true;
		},
		[] {
			Print << game.result.substr(0, game.result.indexOf(U'\n'));
			for (auto by : step(balloons.height())) {
				for (auto bx : step(balloons.width())) {
					auto &field = balloons[by][bx];
					if (!field.has_value()) continue;
					const Circle balloon{field.value().pos, 50};
					if (field.value().red_time <= 0)
						balloon.mouseOver() ? balloon.draw(Palette::Black) : balloon.drawFrame(2, Palette::Black);
					else {
						balloon.draw(Palette::Red);
						field.value().red_time -= Scene::DeltaTime();
					}
					(*font)(Format(field.value().index + 1))
						.drawAt(field.value().pos,
								field.value().red_time > 0 || balloon.mouseOver() ? Palette::White : Palette::Black);

					if (balloon.leftReleased()) {
						if (field.value().index == index) {
							if (index == 59) {
								game.score += 2000;
								game.result = Format(U"成功した数: 60/60\n失敗した回数: ", fail);
								return false;
							}
							index++;
							game.result = Format(U"成功した数: ", index, U"/60\n失敗した回数: ", fail);
							balloons[by][bx] = none;
						random:
							y = Random(balloons.height() - 1);
							x = Random(balloons.width() - 1);
							if (balloons[y][x] || (by == y && bx == x)) goto random;
							else balloons[y][x] = Balloon{index + 2 + flag, Vec2{(x * 100) + 50, (y * 100) + 150}};
							game.score += 1000;
							if (index != 0 && index % 10 == 0 && index / 10 == flag + 1) {
								flag++;
								goto random;
							}
						} else {
							fail++;
							field.value().red_time = 1.0;
						}
					}
				}
			}
			return true;
		}),
	30s};
}  // namespace Kazuate
}  // namespace Games
