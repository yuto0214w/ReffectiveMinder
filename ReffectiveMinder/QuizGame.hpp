#pragma once

namespace Games {
namespace Quiz {
const Font *font;
bool is_correct, special_flag;
double time, transition;
uint8 correct, i;
struct Question {
	const char32 *sentence, *question;
	Array<const char32 *> answers;
	const char32 *answer;
};
Array<Question> questions = {
	{U"Windows パソコンのアプリには、「exe」という拡張子が使われています。Mac では「app」という拡張子が使われており、Linux のプログラムには拡張子がありません。",
	 U"Mac での拡張子は？",
	 {U"exe", U"app", U"com", U"拡張子", U"そもそもない"},
	 U"app"},
	{U"ユナは「そろそろ行かなきゃ。またね」と言うと、スーツケースとスマホを手に持ち、少し急ぎ足で駅の改札を抜けていった。赤いマフラーを身に着けていた。",
	 U"スーツケースの色は？",
	 {U"赤", U"黒", U"駅の改札", U"スマホの色", U"分からない"},
	 U"分からない"},
	{U"日本からフランスまでは約 9,844km の距離があると言われています。フランスの標準時と日本標準時では8時間の時差があります。夏時間に入ると時差は7時間になります。",
	 U"夏時間と標準時では何時間の時差がありますか？\n               (フランス)",
	 {U"1時間", U"7時間", U"8時間", U"-7時間", U"-8時間"},
	 U"1時間"},
	{U"日本語と英語の大きな違いは語順です。日本語では「私は　あなたが　好きです」のように人名が先に来ますが、英語では「I　love　you」のように動詞が先に来ています。",
	 U"人名が先に来るのはどちらの言語ですか？",
	 {U"日本語", U"英語", U"中国語", U"北京語", U"標準語"},
	 U"日本語"},
	{U"くまさんがアイスを5つ食べました。そのあとに、友達のりすくんがやってきて、くまさんにアイスを2つあげました。",
	 U"さて、登場人物はくまさんと誰ですか？",
	 {U"くまさん", U"りすくん", U"アイス", U"5つと2つ", U"この中にいない"},
	 U"りすくん"},
	{U"私は紺色の半袖、茶色のズボンを着ています。白いスマホを持っていて、黒いフチの眼鏡をかけています。髪の毛は黒で、赤い腕時計をしています。",
	 U"私は何色の服を着ていますか？",
	 {U"紺", U"茶", U"白", U"黒", U"赤"},
	 U"紺"},
	{U"そして僕は街に出た。街には人がたくさんいた。犬の散歩をしている人や、集団で話しながら歩いている高校生がいる。ああいう集団に入るのは苦手なほうだ。",
	 U"僕はどこに来ていますか？",
	 {U"街", U"広場", U"公園", U"学校", U"都会"},
	 U"街"},
	{U"今日は大翔と春花の3年目の結婚記念日です。大翔は、春花と付き合っていた時によく遊んでいたゲームの続編をプレゼントすることにしました。",
	 U"大翔がプレゼントする予定の品は？",
	 {U"新作のゲーム", U"ゲームの続編", U"結婚指輪", U"プレゼント", U"ぬいぐるみ"},
	 U"ゲームの続編"},
	{U"目が覚めた。時計を見るとまだ5時で、日も上ってないようだ。いい朝だ。スマホを見る。17時だった。夕方だ。なんてこったい。僕は死を悟った。",
	 U"今何時？",
	 {U"17時", U"15時", U"7時", U"5時", U"一大事"},
	 U"17時"},
	{U"CPS = Clicks Per Second\nDPS = Damages Per Second\nFPS = Frames Per Second\nGPS = Global Positioning System",
	 U"上にないものは？",
	 {U"CPS", U"DPS", U"FPS", U"GPS", U"VPS"},
	 U"VPS"}};
std::array<RoundRect, 5> *buttons;
Game game = {U"クイズ",
			 U"画面上に問題文、質問文と、回答のボタンが表示されます。質問文の答えだと思うボタンをクリックしましょう。",
			 U"quiz",
			 std::make_pair(
				 [] {
					 game.result = U"正解した回数: 0/10";
					 game.score = 0;
					 is_correct = false;
					 special_flag = false;
					 time = 0.0;
					 transition = 0.0;
					 correct = 0;
					 i = 0;
					 questions.shuffle();
					 questions[0].answers.shuffle();
					 if (!buttons)
						 buttons = new std::array{RoundRect{50, 375, 223, 80, 10}, RoundRect{288, 375, 224, 80, 10},
												  RoundRect{527, 375, 223, 80, 10}, RoundRect{175, 470, 218, 80, 10},
												  RoundRect{407, 470, 218, 80, 10}};
					 return true;
				 },
				 [] {
					 Print << game.result;

					 (*font)(U"S. ").draw(50, 85, Palette::Black);
					 (*font)(questions[i].sentence).draw(Rect{91, 85, 630, 172}, Palette::Black);
					 (*font)(U"Q. ", questions[i].question).draw(50, 275, Palette::Black);

					 {
						 RoundRect button;
						 for (auto x : step(buttons->size())) {
							 button = (*buttons)[x];
						  // if  user hasn't answered yet
							 if (transition <= 0) {
								 button.mouseOver() ? button.draw(Palette::Black) : button.drawFrame(2, Palette::Black);
								 (*font)(questions[i].answers[x])
									 .drawAt(button.center(), button.mouseOver() ? Palette::White : Palette::Black);
								 if (button.leftReleased()) {
									 is_correct = questions[i].answers[x] == questions[i].answer;
									 if (is_correct) {
										 correct++;
										 game.score += time > 0.2221 ? 2221 / time : 10000;
									 } else if (questions[i].answers[x] == U"一大事") {
										 special_flag = true;
										 game.score += time > 0.2221 ? 1110.5 / time : 5000;
									 }
									 game.result = Format(U"正解した回数: ", correct, U"/10");
									 transition = 0.5;
								 }
							 } else {
								 button.drawFrame(2, Palette::Black);
								 (*font)(questions[i].answers[x]).drawAt(button.center(), Palette::Black);
							 }
						 }
					 }

					 if (transition <= 0) time += Scene::DeltaTime();
					 else if ((transition -= Scene::DeltaTime()) <= 0) {
						 if (i == 9) return false;
						 if (special_flag) special_flag = false;
						 i++;
						 questions[i].answers.shuffle();
						 time = 0.0;
					 } else {
						 if (is_correct) Circle{Scene::Center(), 175}.drawFrame(50, Palette::Red);
						 else if (special_flag)
							 Triangle{Scene::Center() + Point{0, 25}, 325}.drawFrame(50, Palette::Green);
						 else Shape2D::Cross(225, 50, Scene::Center()).draw(Palette::Blue);
					 }

					 return true;
				 }),
			 60s};
}  // namespace Quiz
}  // namespace Games
